#include "js_addon.hh"

#include <sstream>
#include <streambuf>
#include <stdexcept>
#include <iostream>

#include <duktape.h>
#include "environment.hh"

namespace
{
    void push_arg(duk_context* ctx, double val)
    {
        duk_push_number(ctx, val);
    }
    void push_arg(duk_context* ctx, float val)
    {
        duk_push_number(ctx, val);
    }
    void push_arg(duk_context* ctx, int val)
    {
        duk_push_int(ctx, val);
    }
    void push_arg(duk_context* ctx, unsigned int val)
    {
        duk_push_int(ctx, val);
    }
    void push_arg(duk_context* ctx, void* val)
    {
        duk_push_pointer(ctx, val);
    }
    void push_arg(duk_context* ctx, const std::string& x)
    {
        duk_push_lstring(ctx, x.c_str(), x.size());
    }
    void push_arg(duk_context* ctx, const nonstd::string_view & x)
    {
        duk_push_lstring(ctx, x.data(), x.size());
    }
    void push_arg(duk_context* ctx, const char* x)
    {
        duk_push_string(ctx, x);
    }

    template <typename T>
    void push_arg(duk_context* ctx, const std::map<std::string, T>& x)
    {
        duk_idx_t obj_idx = duk_push_object(ctx);
        for (const auto& kv : x)
        {
               push_arg(ctx, kv.second);
               duk_put_prop_lstring(ctx, obj_idx, kv.first.data(), kv.first.size());
        }
    }

    template<typename Head, typename ... Rest>
    void push_args(duk_context* ctx, Head&& arg, Rest&& ... tail)
    {
        push_arg(ctx, std::forward<Head>(arg));
        using expander = int[];
        (void) expander{0, (void(push_arg(ctx, std::forward<Rest>(tail))), 0)...};
    }

    template<typename ... Args>
    void js_call_method(duk_context* ctx, duk_idx_t idx, const char* method, Args&& ...args)
    {
        push_args(ctx, method, std::forward<Args>(args)...);
        duk_call_prop(ctx, idx, sizeof...(args));
    }

    // ========================================================================================
    struct pop_from_ctx
    {
        explicit pop_from_ctx(duk_context* ctx)
        : m_ctx(ctx)
        {}
        ~pop_from_ctx()
        {
            duk_pop(m_ctx);
        }

        duk_context* m_ctx;
    };
    // ========================================================================================
    template <typename T>
    struct return_by_type;

    template <>
    struct return_by_type<bool>
    {
        static bool call(duk_context* ctx, const char* name)
        {
            pop_from_ctx g(ctx);
            if (!duk_is_boolean(ctx, -1))
            {
                std::ostringstream os;
                os << "function " << name << " should return boolean";
                throw std::runtime_error(os.str());
            }
            return duk_get_boolean(ctx, -1);
        }
    };

    template <>
    struct return_by_type<int>
    {
        static int call(duk_context* ctx, const char* name)
        {
            pop_from_ctx g(ctx);
            if (!duk_is_number(ctx, -1))
            {
                std::ostringstream os;
                os << "function " << name << " should return number";
                throw std::runtime_error(os.str());
            }
            return duk_get_number(ctx, -1);
        }
    };

    template <>
    struct return_by_type<std::string>
    {
        static std::string call(duk_context* ctx, const char* name)
        {
            pop_from_ctx g(ctx);
            if (!duk_is_string(ctx, -1))
            {
                std::ostringstream os;
                os << "function " << name << " should return string";
                throw std::runtime_error(os.str());
            }
            return duk_get_string(ctx, -1);
        }
    };
    // ========================================================================================
    template<typename ReturnType, typename ... Args>
    void js_call_function(duk_context* ctx, const char* name, ReturnType& rt, Args&& ...args)
    {
        if (duk_get_global_string(ctx, name))
        {
            push_args(ctx, std::forward<Args>(args)...);
            if (duk_pcall(ctx, sizeof...(args)) != DUK_EXEC_SUCCESS)
            {
                std::ostringstream os;
                os << "Failed to execute  : " << name << " " << duk_safe_to_stacktrace(ctx, -1);
                throw std::runtime_error(os.str());
            } else
            {
                rt = return_by_type<ReturnType>::call(ctx, name);
            }
        }
        else
        {
            std::ostringstream os;
            os << "function " << name << " is missing";
            throw std::runtime_error(os.str());
        }
    }
} // anon. ns

namespace binadox
{
    namespace appgw
    {
        class js_addon_impl
        {
        public:
            explicit js_addon_impl(const std::string& script);
            ~js_addon_impl();

            const std::string& plugin_name() const;
            bool wants_url(const nonstd::string_view& url);
            bool accept(const nonstd::string_view& method,
                        const nonstd::string_view& url,
                        const std::map<std::string, std::string>& headers);
        private:
            void setup(const std::string& script);
            int check_js_function(const char* method_name);
            void assert_js_function(const char* method_name, int num_args);
        private:
            duk_context* ctx;

            std::string m_plugin_name;
        };

        // -------------------------------------------------------------------------------------------------
        js_addon_impl::js_addon_impl(const std::string& script)
                : ctx(nullptr)
        {
            ctx = duk_create_heap_default();
            if (!ctx)
            {
                throw std::runtime_error("Failed to create js context");
            }
            try
            {
                setup(script);
            }
            catch (std::exception& e)
            {
                duk_destroy_heap(ctx);
                ctx = nullptr;
                throw e;
            }
        }
        // -------------------------------------------------------------------------------------------------
        void js_addon_impl::setup(const std::string& script)
        {
            setup_environment(ctx);
            if (duk_pcompile_string(ctx, 0, script.c_str()) != 0)
            {
                // Error in program code
                std::ostringstream os;
                os << "Compilation failed: " << duk_safe_to_string(ctx, -1);
                throw std::runtime_error(os.str());
            }

            // Actually evaluate it - this will push the compiled code into the global scope
            duk_pcall(ctx, 0);
            duk_pop(ctx);

            if (0 != check_js_function("get_plugin_name"))
            {
                throw std::runtime_error("get_plugin_name() function not found or it accepts more than 0 arguments");
            }

            // Get a reference to the named JS function
            duk_get_global_string(ctx, "get_plugin_name");

            // Use pcall - this lets you catch and handle any errors
            if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS)
            {
                std::ostringstream os;
                os << "Failed to execute get_plugin_name() : " << duk_get_prop_string(ctx, -1, "stack");
                throw std::runtime_error(os.str());
            } else
            {
                if (!duk_is_string(ctx, -1))
                {
                    duk_pop(ctx);
                    throw std::runtime_error("get_plugin_name() should return string");
                }
                m_plugin_name = duk_get_string(ctx, -1);
                duk_pop(ctx);
            }


            assert_js_function("wants_url", 1);
            assert_js_function("accept", 3);

        }
        // -------------------------------------------------------------------------------------------------
        const std::string& js_addon_impl::plugin_name() const
        {
            return m_plugin_name;
        }
        // -------------------------------------------------------------------------------------------------
        bool js_addon_impl::wants_url(const nonstd::string_view& url)
        {
            bool rc;
            js_call_function(ctx, "wants_url", rc, url);
            return rc;
        }
        bool js_addon_impl::accept(const nonstd::string_view& method,
                                   const nonstd::string_view& url,
                                   const std::map<std::string, std::string>& headers)
        {
            bool rc;
            js_call_function(ctx, "accept", rc, method, url, headers);
            return rc;
        }
        // -------------------------------------------------------------------------------------------------
        void js_addon_impl::assert_js_function(const char* method_name, int num_args)
        {
            int rc = check_js_function(method_name);
            if (rc < 0)
            {
                std::ostringstream os;
                os << "function " << method_name << " is missing in js_addon " << m_plugin_name;
                throw std::runtime_error(os.str());
            }
            if (num_args != rc)
            {
                std::ostringstream os;
                os << "function " << method_name << " in js_addon " << m_plugin_name
                   << " requires " << num_args << " parameters but accepts " << rc;
                throw std::runtime_error(os.str());
            }
        }
        // -------------------------------------------------------------------------------------------------
        int js_addon_impl::check_js_function(const char* method_name)
        {
            int rc = -1;
            if (duk_get_global_string(ctx, method_name))
            {
                if (duk_is_function(ctx, -1))
                {
                    duk_push_string(ctx, "length");
                    duk_get_prop(ctx, -2);
                    rc = duk_get_int(ctx, -1);
                    duk_pop(ctx);
                }
            }
            duk_pop(ctx);
            return rc;
        }
        // -------------------------------------------------------------------------------------------------
        js_addon_impl::~js_addon_impl()
        {
            if (ctx)
            {
                duk_destroy_heap(ctx);
            }
        }
        // =================================================================================================
        js_addon js_addon::from_text(const std::string& text)
        {
            return js_addon(text);
        }
        // -------------------------------------------------------------------------------------------------
        js_addon js_addon::from_file(const fs::path& path)
        {
            std::ifstream ifs(path.string(), std::ios::binary | std::ios::in);
            ifs.exceptions(std::ios::failbit);
            return from_text(std::string((std::istreambuf_iterator<char>(ifs)),
                                         std::istreambuf_iterator<char>()));
        }
        // ---------------------------------------------------------------------------------------------------
        js_addon::js_addon(js_addon&& other) noexcept
                : m_pimpl(std::move(other.m_pimpl))
        {

        }
        // ---------------------------------------------------------------------------------------------------
        js_addon::js_addon(const std::string& js_txt)
                : m_pimpl(new js_addon_impl(js_txt))
        {

        }
        // ---------------------------------------------------------------------------------------------------
        js_addon::~js_addon() = default;
        // ---------------------------------------------------------------------------------------------------
        const std::string& js_addon::name() const
        {
            return m_pimpl->plugin_name();
        }
        // ---------------------------------------------------------------------------------------------------
        bool js_addon::wants_url(const nonstd::string_view& url)
        {
            return m_pimpl->wants_url(url);
        }
        // ---------------------------------------------------------------------------------------------------
        bool js_addon::accept(const nonstd::string_view& method,
                    const nonstd::string_view& url,
                    const std::map<std::string, std::string>& headers)
        {
            return m_pimpl->accept(method, url, headers);
        }
    } // ns appgw
} // ns binadox
