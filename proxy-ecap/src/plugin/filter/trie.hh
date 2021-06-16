//
// Created by igor on 09/08/2020.
//

#ifndef BINADOX_ECAP_TRIE_HH
#define BINADOX_ECAP_TRIE_HH

#include <map>
#include <string>
#include <vector>
#include <nonstd/string_view.hpp>

namespace binadox
{
    namespace detail
    {
        class string_key
        {
        public:
            explicit string_key(const std::string& s);
            string_key(const char* start, std::size_t length);

            string_key(string_key&& s);

            ~string_key();
            bool operator<(const string_key& src)const;

            bool operator == (char ch) const;

            void swap(string_key& x);
        private:
            string_key(const string_key&);
            string_key& operator = (const string_key&);
        private:
            const char* ptr;
            std::size_t len;
            bool owner;
        };
    }
    class trie
    {
    public:
        typedef std::vector<detail::string_key> tokens_t;
    public:
        trie();
        ~trie();



        void insert(const std::string& pattern);
        bool contains (const nonstd::string_view& domain) const;
        void remove(const std::string& pattern);

        bool empty () const;

        void clear ();
    private:
        static void split(const std::string& input, tokens_t& toks);
        static void split(const nonstd::string_view& input, tokens_t& toks);

        bool contains (const tokens_t& tokens) const;
        void insert(tokens_t& tokens);
        void remove(const tokens_t& tokens);
    private:
        trie(const trie&);
        trie& operator = (const trie&);
    private:
        class node
        {
        public:
            explicit node(node* parent_node);
            ~node();

            node* get_parent ();
            const node* get_parent () const;

            const node* find(const detail::string_key& word) const;
            node* find(const detail::string_key& word);

            void bind(const std::string& word, node* n);
            void bind(detail::string_key&& word, node* n);
            void erase(const detail::string_key& word);

            bool empty() const;

            bool  is_final () const;
            void  is_final(bool v);
            bool  is_star () const;
            void  is_star(bool v);

            int   ref_count () const;
            void inc_ref_count();
            void dec_ref_count();

        private:
            typedef std::map<detail::string_key, node*> map_t;

            node* parent;
            map_t children;
            bool  m_is_final;
            bool  m_is_star;
            int   m_ref_count;
        };
    private:
        bool has_pattern(const tokens_t& tokens, std::vector<node*>* path) const;
    private:
        node* root;
    };
} // ns binadox

#endif //BINADOX_ECAP_TRIE_HH
