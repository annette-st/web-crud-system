//
// Created by igor on 23/08/2020.
//

#include "plugin/sniffer_db/sniffer_db.hh"
#include "plugin/sniffer_db/sniffer_meta_data.hh"
#include "plugin/logger.hh"
#include "backend/unused.hh"

#include "backend/sniffer/sniffer_storage.hh"


namespace binadox
{
    sniffer_context::decoder_t sniffer_context::get(const sniffer_meta_data& md) const
    {
        if (md.is_request())
        {
            std::lock_guard<std::mutex> g(req_mtx);
            auto i = req.find(md.get_transaction_id());
            if (i != req.end())
            {
                return i->second;
            }
        }
        else
        {
            std::lock_guard<std::mutex> g(resp_mtx);
            auto i = resp.find(md.get_transaction_id());
            if (i != resp.end())
            {
                return i->second;
            }
        }
        return nullptr;
    }
    // ----------------------------------------------------------------------------------------
    void sniffer_context::setup(const sniffer_meta_data& md)
    {
        zlc::decompressor* decoder = nullptr;
        switch (md.get_encoding())
        {
            case eNONE:
                decoder = nullptr;
                break;
            case eBR:
                decoder = zlc::decompressor::brotli();
                break;
            case eDEFLATE:
                decoder = zlc::decompressor::raw();
                break;
            case eGZIP:
                decoder = zlc::decompressor::gzip();
                break;
            default:
                decoder = nullptr;
                break;
        }
        decoder_t d(decoder);

        if (md.is_request())
        {
            std::lock_guard<std::mutex> g(req_mtx);
            req[md.get_transaction_id()] = d;
        } else {
            std::lock_guard<std::mutex> g(resp_mtx);
            resp[md.get_transaction_id()] = d;
        }
    }
    // ----------------------------------------------------------------------------------------
    void sniffer_context::finalize (const sniffer_meta_data& md)
    {
        if (md.is_request())
        {
            std::lock_guard<std::mutex> g(req_mtx);
            req.erase(md.get_transaction_id());
        } else {
            std::lock_guard<std::mutex> g(resp_mtx);
            resp.erase(md.get_transaction_id());
        }
    }
    // ================================================================================
    class sniffer_transaction
    {
    public:
        explicit sniffer_transaction(sniffer_storage* impl)
        : pimpl(impl)
        {
            pimpl->enter_transaction();
        }
        ~sniffer_transaction()
        {
            pimpl->exit_transaction();
        }
    private:
        sniffer_storage* pimpl;
    };
    // ===================================================================================
    sniffer_db::sniffer_db(sniffer_storage* impl)
    : pimpl (impl)
    {

    }
    // ----------------------------------------------------------------------------------
    sniffer_db::~sniffer_db()
    {
        delete pimpl;
    }
    // ----------------------------------------------------------------------------------
    void sniffer_db::put(const sniffer_meta_data& md)
    {
        sniffer_transaction tx(pimpl);

        if (md.is_request())
        {
            pimpl->write_request(md.get_transaction_id(),
                    md.get_request().get_method(),
                    md.get_request().get_url(),
                    md.get_client_ip(),
                    md.get_headers(),
                    md.get_timestamp(),
                    md.get_script());
        } else {
            pimpl->write_response(md.get_transaction_id(),
                                 md.get_response().get_code(),
                                 md.get_response().get_reason(),
                                 md.get_client_ip(),
                                 md.get_headers(),
                                 md.get_timestamp(),
                                 md.get_script());
        }

        ctx.setup(md);
    }
    // ----------------------------------------------------------------------------------
    void sniffer_db::put(const sniffer_meta_data& md, int chunk_num, const std::vector<char>& chunk)
    {
        sniffer_transaction tx(pimpl);
        auto decoder = ctx.get(md);

        if (!decoder)
        {
            pimpl->write_chunk(md.get_transaction_id(), md.is_request(), chunk_num, chunk);
        }
        else
        {
            std::vector<char> out;
            decoder->decompress(chunk.data(), chunk.size(), out);
            pimpl->write_chunk(md.get_transaction_id(), md.is_request(), chunk_num, out);
        }
    }
    // ----------------------------------------------------------------------------------
    void sniffer_db::transaction_finished(UNUSED_ARG const sniffer_meta_data& md)
    {
        // TODO
        ctx.finalize(md);
        pimpl->finalize(md.get_transaction_id(), md.is_request());
    }
} // ns binadox
