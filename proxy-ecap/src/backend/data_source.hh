//
// Created by igor on 31/12/2020.
//

#ifndef BINADOX_ECAP_DATA_SOURCE_HH
#define BINADOX_ECAP_DATA_SOURCE_HH

#include <vector>
#include <string>
#include <map>
#include <type_traits>

namespace binadox
{
    class data_source
    {
    public:
        enum type_t
        {
            eIGNORE_LIST,
            eLOGIN_INCLUDE_SCRIPTS,
            eON_LOAD_SCRIPT,
            eINJECTION_SCRIPT,
            e403_PAGE,
            eBLACK_LIST,
            eADDONS_LIST,

            eMAX_TYPE
        };
    public:
        using list_of_strings = std::vector<std::string>;
        using map_of_strings = std::map<std::string, std::string>;
    public:
        virtual ~data_source();

        virtual list_of_strings get_ignore_list() const = 0;
        virtual void set_ignore_list(const list_of_strings& data);

        virtual list_of_strings get_black_list() const = 0;
        virtual void set_black_list(const list_of_strings& data);

        virtual map_of_strings  get_login_include_scripts() const = 0;
        virtual void set_login_include_scripts(const map_of_strings& data);

        virtual map_of_strings  get_onload_scripts() const = 0;
        virtual void set_onload_scripts(const map_of_strings& data);

        virtual std::string     get_injection_script() const = 0;
        virtual void set_injection_script(const std::string& data);

        virtual std::string     get_403_page() const = 0;
        virtual void set_403_page(const std::string& data);


        virtual list_of_strings get_addons_list() const = 0;
        virtual void set_addons_list(const list_of_strings& data);

        virtual void clear(type_t what);


    };
    // ===================================================================================
    namespace detail
    {
        template <data_source::type_t WHAT>
        struct data_source_setter;

#define DECLARE_BY_TYPE_SETTER(WHAT, TYPE, MTH)                     \
       template<>  struct data_source_setter<WHAT>{                 \
       static void call(data_source* ds, const TYPE& data) {        \
       ds->MTH(data);}                                              \
       }

        DECLARE_BY_TYPE_SETTER(data_source::eBLACK_LIST, data_source::list_of_strings, set_black_list);
        DECLARE_BY_TYPE_SETTER(data_source::eIGNORE_LIST, data_source::list_of_strings, set_ignore_list);
        DECLARE_BY_TYPE_SETTER(data_source::eLOGIN_INCLUDE_SCRIPTS, data_source::map_of_strings, set_login_include_scripts);
        DECLARE_BY_TYPE_SETTER(data_source::eON_LOAD_SCRIPT, data_source::map_of_strings, set_onload_scripts);
        DECLARE_BY_TYPE_SETTER(data_source::eINJECTION_SCRIPT, std::string, set_injection_script);
        DECLARE_BY_TYPE_SETTER(data_source::e403_PAGE, std::string, set_403_page);
        DECLARE_BY_TYPE_SETTER(data_source::eADDONS_LIST, data_source::list_of_strings , set_addons_list);


        template<typename... T>
        struct all_same : std::false_type { };

        template<>
        struct all_same<> : std::true_type { };

        template<typename T>
        struct all_same<T> : std::true_type { };

        template<typename T, typename... Ts>
        struct all_same<T, T, Ts...> : all_same<T, Ts...> { };

    }

    class data_source_observer
    {
    public:
        template<typename... T>
        data_source_observer(data_source::type_t first, T... rest)
        {
            register_all(first, rest...);
        }

        virtual ~data_source_observer();
        virtual void handle_data_source(data_source::type_t type, const data_source* ds) = 0;
    private:
        void register_me (data_source::type_t type);

        template<typename... T>
        typename std::enable_if<detail::all_same<T...>::value, void>::type
        register_all(T... args)
        {
            using expander = int[];
            (void)expander{0,  (void(register_me(args)), 0)...};
        }
    private:
        std::vector<data_source::type_t> m_types;
    };
} // ns binadox

#endif //BINADOX_ECAP_DATA_SOURCE_HH
