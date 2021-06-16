//
// Created by igor on 23/08/2020.
//

#ifndef BINADOX_ECAP_SNIFFER_DB_HH
#define BINADOX_ECAP_SNIFFER_DB_HH

#include <vector>
#include <memory>
#include <map>
#include <mutex>

#include "plugin/pipeline/zlc.hh"

namespace binadox
{
    class sniffer_meta_data;
    class sniffer_storage;

    class sniffer_context
    {
    public:
        typedef std::shared_ptr<zlc::decompressor> decoder_t;
    public:
        sniffer_context () = default;


        decoder_t get(const sniffer_meta_data& md) const;
        void setup(const sniffer_meta_data& md);
        void finalize (const sniffer_meta_data& md);

    private:
        mutable std::mutex req_mtx;
        std::map<std::string, decoder_t> req;

        mutable std::mutex resp_mtx;
        std::map<std::string, decoder_t> resp;
    };

    class sniffer_db
    {
    public:
        explicit sniffer_db(sniffer_storage* impl);
        ~sniffer_db();

        void put(const sniffer_meta_data& md);
        void put(const sniffer_meta_data& md, int chunk_num, const std::vector<char>& chunk);
        void transaction_finished(const sniffer_meta_data& md);
    private:
        sniffer_context ctx;
        sniffer_storage* pimpl;
    };
} // ns binadox

#endif //BINADOX_ECAP_SNIFFER_DB_HH
