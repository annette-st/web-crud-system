//
// Created by igor on 27/08/2020.
//

#include <map>
#include <future>
#include "plugin/conf/key_value_storage.hh"
#include "backend/sqlite/sqlite.hh"
#include "backend/active_object.hh"
#include "plugin/logger.hh"

namespace binadox
{
    typedef std::unique_lock<std::mutex> lock_t;
    typedef std::pair<std::string, std::string> cache_key_t;

    struct key_value_storage_impl
    {

        key_value_storage_impl(const std::string& path);
        void put(const std::string& key, const std::string& meta, const std::string& value);
        bool get(const std::string& key, std::string& meta, std::string& value);
        bool get_with_meta(const std::string& key, const std::string& meta, std::string& value);
        void drop(const std::string& key, const std::string& meta);
        void drop();
        void list_keys(const std::string& meta, key_handler& handler);

        sqlite3pp::database database;
        std::mutex db_mutex;
        std::unique_ptr<active_object> worker;

        std::mutex cache_mutex_t;
        typedef std::map<cache_key_t, std::string> cache_t;
        cache_t cache;
    };

    key_value_storage_impl::key_value_storage_impl(const std::string& path)
            : database(path.c_str())
    {
        LOG_ERROR("Using key/value storage from ", path);
        database.execute("CREATE TABLE IF NOT EXISTS keyval ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "timestamp INTEGER,"
                         "key TEXT,"
                         "meta TEXT,"
                         "value TEXT"
                         ")");
        database.execute("DROP INDEX IF EXISTS keyval_key;");
        database.execute("DROP INDEX IF EXISTS keyval_meta;");
        database.execute("CREATE UNIQUE INDEX IF NOT EXISTS keyval_key_meta ON keyval (key, meta);");

        worker = std::move(active_object::create());
    }
    // -----------------------------------------------------------------------------------------------------------
    void key_value_storage_impl::put(const std::string& key, const std::string& meta, const std::string& value)
    {
        lock_t guard(db_mutex);
        sqlite3pp::command insert_cmd(database, "REPLACE INTO keyval "
                                                "(timestamp, key, meta, value) VALUES (?,?,?,?)");

        insert_cmd.bind(1, (int) time(NULL));
        insert_cmd.bind(2, key, sqlite3pp::copy);
        insert_cmd.bind(3, meta, sqlite3pp::copy);
        insert_cmd.bind(4, value, sqlite3pp::copy);

        insert_cmd.execute();
    }
    // -----------------------------------------------------------------------------------------------------------
    bool key_value_storage_impl::get(const std::string& key, std::string& meta, std::string& value)
    {
        lock_t guard(db_mutex);
        sqlite3pp::query qry(database, "SELECT meta, value FROM keyval WHERE key = ?");
        qry.bind(1, key, sqlite3pp::nocopy);
        for (sqlite3pp::query::query_iterator i = qry.begin(); i != qry.end(); ++i)
        {
            std::tie(meta, value) = (*i).get_columns<std::string, std::string>(0, 1);
            return true;
        }
        return false;
    }
    // ----------------------------------------------------------------------------------------------------------
    void key_value_storage_impl::list_keys(const std::string& meta, key_handler& handler) {
        lock_t guard(db_mutex);
        sqlite3pp::query qry(database, "SELECT key FROM keyval WHERE meta = ?");
        qry.bind(1, meta, sqlite3pp::nocopy);
        for (sqlite3pp::query::query_iterator i = qry.begin(); i != qry.end(); ++i)
        {
            handler.handle(std::get<0>((*i).get_columns<std::string>(0)));
        }
    }
    // ----------------------------------------------------------------------------------------------------------
    bool key_value_storage_impl::get_with_meta(const std::string& key, const std::string& meta, std::string& value)
    {
        lock_t guard(db_mutex);
        sqlite3pp::query qry(database, "SELECT value FROM keyval WHERE key = ? and meta = ?");
        qry.bind(1, key, sqlite3pp::nocopy);
        qry.bind(2, meta, sqlite3pp::nocopy);
        for (sqlite3pp::query::query_iterator i = qry.begin(); i != qry.end(); ++i)
        {
            std::tie(value) = (*i).get_columns<std::string>(0);
            return true;
        }
        return false;
    }
    // -----------------------------------------------------------------------------------------------------------
    void key_value_storage_impl::drop(const std::string& key, const std::string& meta)
    {
        lock_t guard(db_mutex);
        sqlite3pp::command drop_cmd(database, "DELETE FROM keyval WHERE key = ? and meta = ?");
        drop_cmd.bind(1, key, sqlite3pp::nocopy);
        drop_cmd.bind(2, meta, sqlite3pp::nocopy);
        drop_cmd.execute();
    }
    // -----------------------------------------------------------------------------------------------------------
    void key_value_storage_impl::drop()
    {
        lock_t guard(db_mutex);
        database.execute("DELETE FROM keyval;");
    }
    // =============================================================================================================
    key_value_storage::key_value_storage(const std::string& path)
    {
        pimpl = new key_value_storage_impl(path);
    }
    // -----------------------------------------------------------------------------------------------------------
    key_value_storage::~key_value_storage()
    {
        delete pimpl;
    }
    // -----------------------------------------------------------------------------------------------------------
    void key_value_storage::put(const std::string& key, const std::string& meta, const std::string& value, bool async)
    {
        bool do_update = false;
        {
            lock_t guard(pimpl->cache_mutex_t);

            cache_key_t k(key, meta);
            auto i = pimpl->cache.find(k);
            if (i == pimpl->cache.end())
            {
                do_update = true;
                pimpl->cache.insert(key_value_storage_impl::cache_t::value_type(k, value));
            } else
            {
                if (i->second != value)
                {
                    do_update = true;
                    i->second = value;
                }
            }
        }
        if (do_update)
        {
            if (async)
            {
                pimpl->worker->post([this, key, meta, value]() { pimpl->put(key, meta, value); });
            } else
            {
                std::promise<bool> promise;
                pimpl->worker->post([this, key, meta, value, &promise]() {
                    try
                    {
                        pimpl->put(key, meta, value);
                        promise.set_value(true);
                    }
                    catch (...)
                    {
                        promise.set_exception(std::current_exception());
                    }
                });
                promise.get_future().wait();
            }
        }
    }
    // -----------------------------------------------------------------------------------------------------------
    bool key_value_storage::get(const std::string& key, std::string& meta, std::string& value)
    {
        return pimpl->get(key, meta, value);
    }
    // -----------------------------------------------------------------------------------------------------------
    bool key_value_storage::get_with_meta(const std::string& key, const std::string& meta, std::string& value)
    {
        cache_key_t k(key, meta);
        {
            lock_t guard(pimpl->cache_mutex_t);
            auto i = pimpl->cache.find(k);
            if (i != pimpl->cache.end())
            {
                value = i->second;
                return true;
            }
        }
        if (pimpl->get_with_meta(key, meta, value))
        {
            lock_t guard(pimpl->cache_mutex_t);
            pimpl->cache.insert(key_value_storage_impl::cache_t::value_type(k, value));
            return true;
        }
        return false;
    }
    // -----------------------------------------------------------------------------------------------------------
    void key_value_storage::drop(const std::string& key, const std::string& meta, bool async)
    {
        {
            lock_t guard(pimpl->cache_mutex_t);
            cache_key_t k(key, meta);
            pimpl->cache.erase(k);
        }
        if (async)
        {
            pimpl->worker->post([this, key, meta]() { pimpl->drop(key, meta); });
        } else
        {
            std::promise<bool> promise;
            pimpl->worker->post([this, key, meta, &promise]() {
                try
                {
                    pimpl->drop(key, meta);
                    promise.set_value(true);
                }
                catch (...)
                {
                    promise.set_exception(std::current_exception());
                }
            });
            promise.get_future().wait();
        }

    }
    // -----------------------------------------------------------------------------------------------------------
    void key_value_storage::drop(bool async)
    {
        {
            lock_t guard(pimpl->cache_mutex_t);
            pimpl->cache.clear();
        }
        if (async)
        {
            pimpl->worker->post([this]() { pimpl->drop(); });
        } else
        {
            std::promise<bool> promise;
            pimpl->worker->post([this, &promise]() {
                try
                {
                    pimpl->drop();
                    promise.set_value(true);
                }
                catch (...)
                {
                    promise.set_exception(std::current_exception());
                }
            });
            promise.get_future().wait();
        }
    }

    void key_value_storage::list_keys(const std::string& meta, key_handler& handler) {
        pimpl->list_keys(meta, handler);
    }
    // ------------------------------------------------------------------------------------------------------------
    key_handler::~key_handler() = default;
} // ns binadox
// ================================================================================================================
#if defined(BINADOX_WITH_UNITTESTS)

#include <doctest.h>
#include "backend/work_dir.hh"

TEST_SUITE ("key-value storage")
{

    TEST_CASE ("simple test")
    {
        auto path = binadox::get_tmp_dir() / "kv_test.db";

        binadox::key_value_storage kvs(path);
        kvs.drop(false);

        kvs.put("k1", "m1", "v1", false);
        kvs.put("k2", "m2", "v2", false);

        std::string m, v;
        REQUIRE(kvs.get("k1", m, v));
        REQUIRE(m == "m1");
        REQUIRE(v == "v1");
        REQUIRE(kvs.get("k2", m, v));
        REQUIRE(m == "m2");
        REQUIRE(v == "v2");

        kvs.drop("k1", "m1", false);
        REQUIRE_FALSE(kvs.get("k1", m, v));

        REQUIRE(kvs.get("k2", m, v));
        REQUIRE(m == "m2");
        REQUIRE(v == "v2");
    }


    TEST_CASE ("with meta")
    {
        auto path = binadox::get_tmp_dir() / "kv_test.db";

        binadox::key_value_storage kvs(path);
        kvs.drop(false);

        kvs.put("k1", "m1", "v1", false);
        kvs.put("k2", "m2", "v2", false);

        std::string m, v;
        REQUIRE(kvs.get("k1", m, v));
        REQUIRE(m == "m1");
        REQUIRE(v == "v1");

        REQUIRE(kvs.get("k2", m, v));
        REQUIRE(m == "m2");
        REQUIRE(v == "v2");

        REQUIRE_FALSE(kvs.get("k3", m, v));


        REQUIRE(kvs.get_with_meta("k1", "m1", v));
        REQUIRE(v == "v1");

        REQUIRE(kvs.get_with_meta("k2", "m2", v));
        REQUIRE(v == "v2");
    }

    TEST_CASE ("test replace")
    {
        auto path = binadox::get_tmp_dir() / "kv_test.db";

        binadox::key_value_storage kvs(path);
        kvs.drop(false);

        kvs.put("k1", "m1", "v1", false);
        kvs.put("k2", "m2", "v2", false);

        std::string m, v;
        REQUIRE(kvs.get("k1", m, v));
        REQUIRE(m == "m1");
        REQUIRE(v == "v1");

        REQUIRE(kvs.get("k2", m, v));
        REQUIRE(m == "m2");
        REQUIRE(v == "v2");

        kvs.put("k2", "m2", "v3", false);
        REQUIRE(kvs.get("k2", m, v));
        REQUIRE(m == "m2");
        REQUIRE(v == "v3");
    }
}
#endif
