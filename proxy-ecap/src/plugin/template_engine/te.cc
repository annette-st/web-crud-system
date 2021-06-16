//
// Created by igor on 13/08/2020.
//
#include <sstream>
#include "plugin/template_engine/te.hh"
#include "plugin/template_engine/te_scanner.h"

namespace binadox
{
    class abstract_te_parser
    {
    public:
        abstract_te_parser();
        virtual ~abstract_te_parser();
        void parse(const std::string& text);
        void parse(const char* begin, const char* end);
    protected:
        virtual void add_chunk(const std::string& chunk) = 0;
        virtual void add_variable(const std::string& var) = 0;
    };
    // ------------------------------------------------------------------------------------
    abstract_te_parser::abstract_te_parser()
    {

    }
    // ------------------------------------------------------------------------------------
    abstract_te_parser::~abstract_te_parser()
    {
    }
    // ------------------------------------------------------------------------------------
    void abstract_te_parser::parse(const std::string& text)
    {
        if (text.empty())
        {
            return;
        }

        parse(text.c_str(), text.c_str() + text.size());
    }
    // ------------------------------------------------------------------------------------
    void abstract_te_parser::parse(const char* begin, const char* end)
    {
        te_scanner scanner;
        te_scanner_init(&scanner, begin, end);
        std::string current;

        while (te_scan(&scanner) != TE_TOKEN_END)
        {
            if (scanner.token_type == TE_TOKEN_VARIABLE)
            {
                if (!current.empty())
                {
                    add_chunk(current);
                    current = "";
                }
                std::string var_name(scanner.token_begin + 2, scanner.token_end - 2);
                add_variable(var_name);
            } else {
                if (scanner.token_type == TE_TOKEN_TEXT)
                {
                    current += std::string(scanner.token_begin, scanner.token_end);
                }
            }
        }
        if (!current.empty())
        {
            add_chunk(current);
        }
    }

    // ===================================================================================
    class te_parser : public binadox::abstract_te_parser
    {
    public:
        te_parser(te* obj)
        : target(obj)
        {

        }
    private:
        void add_chunk(const std::string& chunk)
        {
            target->chunks.push_back(te::chunk_t(chunk, false));
        }

        void add_variable(const std::string& var)
        {
            target->chunks.push_back(te::chunk_t(var, true));
        }
    private:
        te* target;
    };
    // ====================================================================================
    te::te(const std::string& input)
    {
        if (!input.empty())
        {
            te_parser parser(this);
            parser.parse(input.c_str(), input.c_str() + input.size());
        }
    }
    // ------------------------------------------------------------------------------------
    te::te(const nonstd::string_view& text)
    {
        if (!text.empty())
        {
            te_parser parser(this);
            parser.parse(text.data(), text.data() + text.size());
        }
    }
    // ------------------------------------------------------------------------------------
    te::~te ()
    {
    }
    // ------------------------------------------------------------------------------------
    void te::render(std::ostream& os, const values_map_t& vm) const
    {
        for (std::size_t i = 0; i<chunks.size(); i++)
        {
            const chunk_t& chunk = chunks[i];
            if (!chunk.is_var)
            {
                os << chunk.text;
            } else
            {
                values_map_t ::const_iterator itr = vm.find(chunk.text);
                if (itr != vm.end())
                {
                    os << itr->second;
                }
            }
        }
    }
    // --------------------------------------------------------------------------------
    std::string te::render(const values_map_t& vm) const
    {
        std::ostringstream os;
        render(os, vm);
        return os.str();
    }
} // ns binadox


#if defined(BINADOX_WITH_UNITTESTS)

namespace
{
    struct test_parser : public binadox::abstract_te_parser
    {
        ~test_parser()
        {

        }
        typedef std::multimap<std::string, std::size_t> vars_t;
        std::vector<std::string> chunks;
        vars_t vars;

        void add_chunk(const std::string& chunk)
        {
            chunks.push_back(chunk);
        }
        // -----------------------------------------------------------------------------------
        void add_variable(const std::string& var)
        {
            const std::size_t idx = chunks.size();
            chunks.push_back("");
            vars.insert(vars_t::value_type(var, idx));
        }
        // -----------------------------------------------------------------------------------
        bool chunk_eq(std::size_t idx, const std::string& x)
        {
            if (idx >= chunks.size())
            {
                return false;
            }
            return chunks[idx] == x;
        }
        // -----------------------------------------------------------------------------------
        bool var_exists(const std::string& var, std::size_t index)
        {
            auto itr = vars.equal_range(var);
            for (auto i = itr.first; i != itr.second; i++)
            {
                if (i->second == index)
                {
                    return true;
                }
            }
            return false;
        }
    };
}


#include <doctest.h>

TEST_SUITE("Template Engine")
{
    TEST_CASE("parse 1")
    {
        test_parser parser;
        parser.parse("ZOPA");
        REQUIRE(parser.chunk_eq(0, "ZOPA"));
    }

    TEST_CASE("parse 2")
    {
        test_parser parser;
        parser.parse("ZOPA {{AAA}}");
        REQUIRE(parser.chunk_eq(0, "ZOPA "));
        REQUIRE(parser.var_exists("AAA", 1));

    }

    TEST_CASE("parse 3")
    {
        test_parser parser;
        parser.parse("{{}}ZOPA {{AAA}} BBBB{{AAA}}{{CCC}}{{}}");
        REQUIRE(parser.chunk_eq(0, "{{}}ZOPA "));
        REQUIRE(parser.var_exists("AAA", 1));
        REQUIRE(parser.chunk_eq(2, " BBBB"));
        REQUIRE(parser.var_exists("AAA", 3));
        REQUIRE(parser.var_exists("CCC", 4));
        REQUIRE(parser.chunk_eq(5, "{{}}"));
    }

    TEST_CASE("render 1")
    {
        binadox::te te(std::string("{{}}ZOPA {{AAA}} BBBB{{AAA}}{{CCC}}{{X}}"));
        binadox::te::values_map_t vm;
        vm ["AAA"] = "AAA";
        vm ["CCC"] = "ccc";
        REQUIRE(te.render(vm) == "{{}}ZOPA AAA BBBBAAAccc");
    }

    TEST_CASE("render 2")
    {
        binadox::te te(nonstd::string_view("{{}}ZOPA {{AAA}} BBBB{{AAA}}{{CCC}}{{X}}"));
        binadox::te::values_map_t vm;
        vm ["AAA"] = "AAA";
        vm ["CCC"] = "ccc";
        REQUIRE(te.render(vm) == "{{}}ZOPA AAA BBBBAAAccc");
    }
}
#endif
