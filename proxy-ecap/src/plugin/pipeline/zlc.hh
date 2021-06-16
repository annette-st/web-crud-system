//
// Created by igor on 14/07/2020.
//

#ifndef BINADOX_ECAP_ZLC_HH
#define BINADOX_ECAP_ZLC_HH

#include <vector>


// TODO: https://github.com/gildor2/fast_zlib
// based on https://github.com/rudi-cilibrasi/zlibcomplete

namespace zlc
{
    class compressor
    {
    public:
        static compressor* gzip(int level = 9,  int window_bits = 15);
        static compressor* zlib(int level = 9,  int window_bits = 15);
        static compressor* raw(int level = 9,  int window_bits = 15);
        static compressor* brotli();
    public:
        virtual ~compressor() {};

        virtual std::size_t compress(const char* input, std::size_t size, std::vector<char>& out) = 0;
        virtual std::size_t finish(std::vector<char>& out) = 0;
    };

    class decompressor
    {
    public:
        static decompressor* gzip();
        static decompressor* zlib();
        static decompressor* raw(int window_bits = 15);
        static decompressor* brotli();
    public:
        virtual ~decompressor() {};
        virtual std::size_t decompress(const char* input, std::size_t size, std::vector<char>& out) = 0;
    };


} // ns zlc


#endif //BINADOX_ECAP_ZLC_HH
