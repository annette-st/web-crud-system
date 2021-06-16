//
// Created by igor on 09/08/2020.
//

#include "plugin/filter/trie.hh"
#include "plugin/string_utils.hh"
#include <string.h>
#include "plugin/logger.hh"

namespace binadox
{

    namespace detail
    {


        string_key::string_key(const std::string& s)
        : owner(true)
        {
            if (s.empty())
            {
                ptr = 0;
                len = 0;
            } else
            {
                len = s.size();
                ptr = new char[s.size()];
                memcpy((char*)ptr, s.c_str(), len);
            }
        }

        string_key::string_key(const char* start, std::size_t length)
        : ptr(start),
        len (length),
        owner(false)
        {

        }

        string_key::string_key(string_key&& s)
        : ptr(s.ptr),
          len (s.len),
          owner(s.owner)
        {
            s.owner = false;
        }

        string_key::~string_key()
        {
            if (owner)
            {
                delete [] ptr;
            }
        }

        void string_key::swap(string_key& x)
        {
            std::swap(ptr, x.ptr);
            std::swap(len, x.len);
            std::swap(owner, x.owner);
        }

        bool string_key::operator<(const string_key& src) const
        {
            if (!ptr)
            {
                return true;
            }
            if (!src.ptr)
            {
                return false;
            }
            if (len < src.len)
            {
                return true;
            }
            if (len > src.len)
            {
                return false;
            }
            return strncmp(ptr, src.ptr, len) < 0;
        }

        bool string_key::operator == (char ch) const
        {
            if (len != 1)
            {
                return false;
            }
            return ptr[0] == ch;
        }

        void swap(string_key& x, string_key& y)
        {
            x.swap(y);
        }
    }


    // ========================================================================
    trie::node::node(node* parent_node)
            : parent(parent_node),
              m_is_final(false),
              m_is_star(false),
              m_ref_count(0)
    {}
    // -----------------------------------------------------------------------
    trie::node::~node()
    {
        for (map_t::iterator i = children.begin(); i != children.end(); i++)
        {
            delete i->second;
        }
    }
    // -----------------------------------------------------------------------
    trie::node* trie::node::get_parent ()
    {
        return parent;
    }
    // -----------------------------------------------------------------------
    const trie::node* trie::node::get_parent () const
    {
        return parent;
    }
    // -----------------------------------------------------------------------
    bool trie::node::is_final () const
    {
        return m_is_final;
    }
    // -----------------------------------------------------------------------
    bool  trie::node::is_star () const
    {
        return m_is_star;
    }
    // -----------------------------------------------------------------------
    int trie::node::ref_count () const
    {
        return m_ref_count;
    }
    // -----------------------------------------------------------------------
    void trie::node::inc_ref_count()
    {
        m_ref_count++;
    }
    // -----------------------------------------------------------------------
    void trie::node::dec_ref_count()
    {
        m_ref_count--;
    }
    // -----------------------------------------------------------------------
    void trie::node::is_final(bool v)
    {
        m_is_final = v;
    }
    // -----------------------------------------------------------------------
    void trie::node::is_star(bool v)
    {
        m_is_star = v;
    }
    // -----------------------------------------------------------------------
    const trie::node* trie::node::find(const detail::string_key& word) const
    {
        map_t::const_iterator i = children.find(word);
        if (i != children.end())
        {
            return i->second;
        }
        return nullptr;
    }
    // -----------------------------------------------------------------------
    trie::node* trie::node::find(const detail::string_key& word)
    {
        map_t::iterator i = children.find(word);
        if (i != children.end())
        {
            return i->second;
        }
        return nullptr;
    }
    // -----------------------------------------------------------------------
    void trie::node::bind(const std::string& word, node* n)
    {
        detail::string_key k (word);
        children.emplace(std::move(k), n);
    }
    // -----------------------------------------------------------------------
    void trie::node::bind(detail::string_key&& word, node* n)
    {
        children.emplace(std::move(word), n);
    }
    // -----------------------------------------------------------------------
    void trie::node::erase(const detail::string_key& word)
    {
        children.erase(word);
    }
    // -----------------------------------------------------------------------
    bool trie::node::empty() const
    {
        return children.empty();
    }
    // =======================================================================
    void trie::split(const std::string& input, tokens_t& toks)
    {
        tokenize(input, toks, '.');
    }
    // ----------------------------------------------------------------------------------
    void trie::split(const nonstd::string_view& input, tokens_t& toks)
    {

        const char delim = '.';
        size_t start;
        size_t end = 0;
        while ((start = input.find_first_not_of(delim, end)) != std::string::npos)
        {
            end = input.find(delim, start);
            if (end == nonstd::string_view::npos)
            {
                end = input.size();
            }
            size_t len = end - start;
            toks.push_back(detail::string_key(input.data() + start, len));
        }
    }
    // =======================================================================
    trie::trie()
            : root(new node(nullptr))
    {

    }
    // -----------------------------------------------------------------------
    trie::~trie()
    {
        delete root;
    }
    // -----------------------------------------------------------------------
    void trie::clear ()
    {
        delete root;
        root = new node(nullptr);
    }
    // -----------------------------------------------------------------------
    void trie::insert(const std::string& domain)
    {
        LOG_ERROR("Inserting ", domain, " to the blacklist");
        tokens_t toks;
        split(domain, toks);
        insert(toks);
    }
    // -----------------------------------------------------------------------
    void trie::insert(tokens_t& tokens)
    {
        if (has_pattern(tokens, NULL))
        {
            return;
        }
        node* curr = root;
        for (std::size_t i = 0; i < tokens.size(); i++)
        {
            detail::string_key& word = tokens[i];

            curr->inc_ref_count();
            node* descend = curr->find(word);
            if (!descend)
            {
                node* new_node = new node(curr);
                if (word == '*')
                {
                    new_node->is_star(true);
                }
                curr->bind(std::move(word), new_node);
                curr = new_node;
            } else
            {
                curr = descend;
            }
        }
        curr->is_final(true);
    }
    // ---------------------------------------------------------------------------
    bool trie::contains(const tokens_t& tokens) const
    {
        static char AST = '*';
        static detail::string_key AST_KEY(&AST, 1);
        const node* curr = root;
        for (std::size_t i = 0; i < tokens.size(); i++)
        {
            auto& word = tokens[i];
            const node* descend = curr->find(word);
            if (descend)
            {
                curr = descend;
            } else
            {
                if (!curr->is_star())
                {
                    const node* link = curr->find(AST_KEY);
                    if (!link)
                    {
                        return false;
                    }
                    curr = link;
                }
            }
        }
        return curr->is_final();
    }
    // -----------------------------------------------------------------------------
    bool trie::contains(const nonstd::string_view& domain) const
    {
        tokens_t toks;
        split(domain, toks);
        return contains(toks);
    }
    // -----------------------------------------------------------------------------
    bool trie::has_pattern(const tokens_t& tokens, std::vector<node*>* path) const
    {
        const node* curr = root;
        for (std::size_t i = 0; i < tokens.size(); i++)
        {
            if (path)
            {
                path->push_back(const_cast<node*>(curr));
            }
            const auto& word = tokens[i];
            const node* link = curr->find(word);
            if (link)
            {
                curr = link;
            } else
            {
                return false;
            }
        }
        if (path)
        {
            path->push_back(const_cast<node*>(curr));
        }
        return curr->is_final();
    }
    // -----------------------------------------------------------------------------
    void trie::remove(const std::string& pattern)
    {
        tokens_t toks;
        split(pattern, toks);
        remove(toks);
    }
    // -----------------------------------------------------------------------------
    void trie::remove(const tokens_t& tokens)
    {
        std::vector<node*> path;
        if (!has_pattern(tokens, &path))
        {
            return;
        }
        if (tokens.size() + 1 != path.size())
        {
            return;
        }
        if (path.empty())
        {
            return;
        }
        std::size_t idx = path.size();
        idx--;
        while (true)
        {
            node* victim = path[idx];
            victim->dec_ref_count();
            if (victim->ref_count() <= 0)
            {
                if (victim->get_parent())
                {
                    const auto& word = tokens[idx-1];
                    victim->get_parent()->erase(word);
                    delete victim;
                }
            }
            if (idx == 0)
            {
                break;
            }
            idx--;
        }
    }
    // ------------------------------------------------------------------------------------------
    bool trie::empty () const
    {
        if (root)
        {
            return root->empty();
        }
        return true;
    }
} // ns binadox
// =================================================================================================
// UNITTEST
// =================================================================================================
#if defined(BINADOX_WITH_UNITTESTS)

#include <doctest.h>

TEST_SUITE ("trie")
{
    TEST_CASE ("empty test")
    {
        binadox::trie tr;
        REQUIRE_FALSE(tr.contains("AAA"));
    }

    TEST_CASE ("exact match - simple")
    {
        binadox::trie tr;
        tr.insert("AAA");
        REQUIRE(tr.contains("AAA"));
    }

    TEST_CASE ("exact match - domain")
    {
        binadox::trie tr;
        tr.insert("AAA.BBB");
        REQUIRE(tr.contains("AAA.BBB"));
        REQUIRE_FALSE(tr.contains("AAA.BB"));
    }

    TEST_CASE ("wildcard - test 1")
    {
        binadox::trie tr;
        tr.insert("google.com.*");

        REQUIRE(tr.contains("google.com.il"));
        REQUIRE(tr.contains("google.com.hk"));
        REQUIRE_FALSE(tr.contains("google.com"));

    }

    TEST_CASE ("wildcard - test 2")
    {
        binadox::trie tr;
        tr.insert("*.google.com.*");
        REQUIRE(tr.contains("a.google.com.il"));
        REQUIRE(tr.contains("b.google.com.hk"));
        REQUIRE_FALSE(tr.contains("google.com"));
    }

    TEST_CASE ("wildcard - test multiple 1")
    {
        binadox::trie tr;
        tr.insert("*.google.com.*");
        tr.insert("google.com.*");

        REQUIRE(tr.contains("a.google.com.il"));
        REQUIRE(tr.contains("b.google.com.hk"));
        REQUIRE(tr.contains("google.com.hk"));
    }

    TEST_CASE ("wildcard - test multiple 2")
    {
        binadox::trie tr;
        tr.insert("*.google.com.*");
        tr.insert("google.com");
        REQUIRE(tr.contains("a.google.com.il"));
        REQUIRE(tr.contains("b.google.com.hk"));
        REQUIRE(tr.contains("google.com"));
        REQUIRE_FALSE(tr.contains("google.com.hk"));
    }

    TEST_CASE ("wildcard - test multiple 3")
    {
        binadox::trie tr;
        tr.insert("*.google.com.*");


        REQUIRE(tr.contains("a.google.com.il"));
        REQUIRE(tr.contains("b.google.com.hk"));
        REQUIRE_FALSE(tr.contains("google.com.hk"));
        tr.insert("google.com.*");
        REQUIRE(tr.contains("google.com.hk"));
    }

    TEST_CASE("removal 1")
    {
        binadox::trie tr;
        tr.insert("google.com.*");

        REQUIRE(tr.contains("google.com.il"));
        tr.remove("google.com.*");
        REQUIRE_FALSE(tr.contains("google.com.il"));
    }


    TEST_CASE ("removal 2")
    {
        binadox::trie tr;
        tr.insert("*.google.com.*");
        tr.insert("google.com.*");
        REQUIRE(tr.contains("a.google.com.il"));
        REQUIRE(tr.contains("b.google.com.hk"));
        REQUIRE(tr.contains("google.com.hk"));

        tr.remove("google.com.*");

        REQUIRE_FALSE(tr.contains("google.com.hk"));
        REQUIRE(tr.contains("a.google.com.il"));
        REQUIRE(tr.contains("b.google.com.hk"));

        tr.remove("*.google.com.*");

        REQUIRE_FALSE(tr.contains("a.google.com.il"));
        REQUIRE_FALSE(tr.contains("b.google.com.hk"));
    }
}

#endif