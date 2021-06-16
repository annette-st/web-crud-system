//
// Created by igor on 06/08/2020.
//

#include "plugin/ipc/sender_log.hh"
#include "backend/sqlite/sqlite.hh"
#include "backend/unused.hh"

namespace binadox
{
    struct sender_log_impl
    {
        sender_log_impl(const fs::path& path)
        : database(path.c_str())
        {
            database.execute("CREATE TABLE IF NOT EXISTS sender ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             "timestamp INTEGER,"
                             "type INTEGER,"
                             "message BLOB"
                             ")");
        }

        sqlite3pp::database database;

    };
    // ----------------------------------------------------------------------------------------
    sender_log::sender_log(const fs::path& path)
    : ipc(NULL)
    {
        pimpl = new sender_log_impl(path);
    }
    // ----------------------------------------------------------------------------------------
    sender_log::~sender_log()
    {
        delete pimpl;
    }
    // ----------------------------------------------------------------------------------------
    bool sender_log::on_start ()
    {
        return true;
    }
    // ----------------------------------------------------------------------------------------
    void sender_log::on_stop ()
    {

    }
    // ----------------------------------------------------------------------------------------
    void sender_log::on_send(message_type_t type, message_ptr_t message)
    {
        try
        {
            sqlite3pp::command insert_cmd (pimpl->database, "INSERT INTO sender "
                                                            "(timestamp, type, message) VALUES (?,?,?)");

            insert_cmd.bind(1, (int) time(NULL));
            insert_cmd.bind(2, type);
            insert_cmd.bind(3, *message.get(), sqlite3pp::copy);

            insert_cmd.execute();

        } catch (std::exception& e)
        {

        }
    }
    // -------------------------------------------------------------------------------------------
    void sender_log::start_transaction(UNUSED_ARG const std::string& trid, UNUSED_ARG bool is_request, UNUSED_ARG message_ptr_t message)
    {

    }
    // -------------------------------------------------------------------------------------------
    void sender_log::end_transaction(UNUSED_ARG const std::string& trid, UNUSED_ARG bool is_request)
    {

    }
    // -------------------------------------------------------------------------------------------
    void sender_log::send_data(UNUSED_ARG const std::string& trid, UNUSED_ARG bool is_request, UNUSED_ARG int chunk_num,
                               UNUSED_ARG const std::vector<char>& chunk)
    {

    }
    // -------------------------------------------------------------------------------------------
    void sender_log::empty_log(visitor &v)
    {
        {
            sqlite3pp::transaction xct(pimpl->database);
            sqlite3pp::query qry(pimpl->database, "SELECT timestamp, type, message FROM sender");
            for (sqlite3pp::query::query_iterator i = qry.begin(); i != qry.end(); ++i)
            {
                int timestamp;
                int type;
                std::string message;
                std::tie(timestamp, type, message) = (*i).get_columns<int, int, std::string>(0, 1, 2);
                v.visit(timestamp, (message_type_t) type, message);
            }
        }
        pimpl->database.execute("DELETE FROM sender;");

    }
    // -------------------------------------------------------------------------------------------
    sender_log::visitor::~visitor()
    {

    }
} // ns binadox

// ===================================================================================================
#if defined(BINADOX_WITH_UNITTESTS)

#include <doctest.h>
#include <vector>

#include "backend/work_dir.hh"



namespace
{
    struct test_visitor : public binadox::sender_log::visitor
    {
        void visit(UNUSED_ARG int timestamp, binadox::ipc::message_type_t type, const std::string& message)
        {
            data.push_back(data_t(type, message));
        }
        typedef std::pair<binadox::ipc::message_type_t, std::string> data_t;
        std::vector<data_t> data;
    };
}

TEST_CASE("sender log")
{
    auto path = binadox::get_tmp_dir() / "test.db";
    {
        binadox::sender_log sq(path);
        sq.start();
        sq.send(binadox::ipc::eTRACKING_DATA, std::make_shared<std::string>("A"));
        sq.send(binadox::ipc::eTRACKING_DATA, std::make_shared<std::string>("B"));
        sq.send(binadox::ipc::eTRACKING_DATA, std::make_shared<std::string>("C"));

        test_visitor tv;
        sq.empty_log(tv);

        REQUIRE(tv.data.size() == 3);
        REQUIRE((tv.data[0] == test_visitor::data_t(binadox::ipc::eTRACKING_DATA, "A")));
        REQUIRE((tv.data[1] == test_visitor::data_t(binadox::ipc::eTRACKING_DATA, "B")));
        REQUIRE((tv.data[2] == test_visitor::data_t(binadox::ipc::eTRACKING_DATA, "C")));

        tv.data.resize(0);
        sq.empty_log(tv);

        REQUIRE(tv.data.size() == 0);
    }
    {
        binadox::sender_log sq(path);
        sq.start();
        test_visitor tv;
        sq.empty_log(tv);

        REQUIRE(tv.data.size() == 0);
    }
}

#endif

