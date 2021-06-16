//
// Created by igor on 27/08/2020.
//

#ifndef BINADOX_ECAP_KEY_VALUE_STORAGE_HH
#define BINADOX_ECAP_KEY_VALUE_STORAGE_HH

#include <string>

namespace binadox
{
    struct key_value_storage_impl;

    class key_handler
    {
    public:
        virtual ~key_handler();
        virtual void handle(const std::string& key) = 0;
    };

    class key_value_storage
    {
    public:
        explicit key_value_storage(const std::string& path);
        ~key_value_storage();

        void put(const std::string& key, const std::string& meta, const std::string& value, bool async=true);
        bool get(const std::string& key, std::string& meta, std::string& value);
        bool get_with_meta(const std::string& key, const std::string& meta, std::string& value);
        void drop(const std::string& key, const std::string& meta, bool async=true);
        void drop(bool async=true);
        void list_keys(const std::string& meta, key_handler& handler);
    private:
        key_value_storage_impl* pimpl;
    };
}

#endif //BINADOX_ECAP_KEY_VALUE_STORAGE_HH
