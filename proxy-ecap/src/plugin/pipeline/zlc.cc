//
// Created by igor on 01/08/2020.
//

#include <stdexcept>
#include <zlib.h>
#include <brotli/encode.h>
#include <brotli/decode.h>
#include <brotli/types.h>

#include "plugin/pipeline/zlc.hh"



namespace zlc
{
    static std::size_t ZLIB_COMPLETE_CHUNK = 16384;

    class zlib_compressor : public compressor
    {
        friend class compressor;
    private:
        zlib_compressor(int level,  int windowBits);
        ~zlib_compressor();

        std::size_t compress(const char* input, std::size_t size, std::vector<char>& out);
        std::size_t finish(std::vector<char>& out);
    private:
        bool finished_;
        z_stream strm_;
    };

    class zlib_decompressor : public decompressor
    {
        friend class decompressor;
    private:
        zlib_decompressor(int windowBits);
        ~zlib_decompressor();
        std::size_t decompress(const char* input, std::size_t size, std::vector<char>& out);
    private:
        z_stream strm_{};
    };

    class brotli_compressor : public compressor
    {
        friend class compressor;
    private:
        brotli_compressor();
        ~brotli_compressor();

        std::size_t compress(const char* input, std::size_t size, std::vector<char>& out);
        std::size_t finish(std::vector<char>& out);

    private:
        BrotliEncoderState* state;
    };

    class brotli_decompressor : public decompressor
    {
        friend class decompressor;
    private:
        brotli_decompressor();
        ~brotli_decompressor();
        std::size_t decompress(const char* input, std::size_t size, std::vector<char>& out);
    private:
        BrotliDecoderState* state;
    };

    
    brotli_compressor::brotli_compressor()
    {
        state = BrotliEncoderCreateInstance(NULL, NULL, NULL);
        if (!state)
        {
            throw std::runtime_error("Failed to create BROTLI encoder");
        }
    }
    // --------------------------------------------------------------------------------------
    
    brotli_compressor::~brotli_compressor()
    {
        BrotliEncoderDestroyInstance(state);
    }
    // --------------------------------------------------------------------------------------
    
    std::size_t brotli_compressor::compress(const char* input, std::size_t size, std::vector<char>& out)
    {
        std::size_t out_ptr = out.size();
        std::size_t out_size = out.size();
        std::size_t size_of_vec = out.size();
        for (std::size_t i = 0; i < size; i += ZLIB_COMPLETE_CHUNK)
        {
            const uInt howManyLeft = static_cast<uInt>(size - i);
            const uInt howManyWanted = (howManyLeft > ZLIB_COMPLETE_CHUNK) ?
                                       ZLIB_COMPLETE_CHUNK : howManyLeft;
            std::size_t avail_in = howManyWanted;
            const uint8_t* next_in = (const uint8_t*)(input + i);
            std::size_t avail_out = ZLIB_COMPLETE_CHUNK;
            do
            {

                out.resize(out.size() + ZLIB_COMPLETE_CHUNK);
                avail_out = ZLIB_COMPLETE_CHUNK;
                uint8_t* next_out = (uint8_t*)out.data() + out_ptr;

                if (!BrotliEncoderCompressStream(state,
                                                 BROTLI_OPERATION_PROCESS,
                                                 &avail_in, &next_in,
                                                 &avail_out, &next_out, NULL))
                {
                    throw std::bad_alloc();
                }

                std::size_t have = ZLIB_COMPLETE_CHUNK - avail_out;
                out_ptr += have;
                out_size += have;

            }
            while (avail_out == 0);
        }
        out.resize(out_size);
        return out_size - size_of_vec;
    }
    // --------------------------------------------------------------------------------------
    
    std::size_t brotli_compressor::finish(std::vector<char>& out)
    {
        std::size_t avail_in = 0;
        const uint8_t* next_in = Z_NULL;
        std::size_t out_ptr = out.size();
        std::size_t out_size = out.size();
        std::size_t size_of_vec = 0;
        std::size_t avail_out = ZLIB_COMPLETE_CHUNK;
        do
        {
            out.resize(out.size() + ZLIB_COMPLETE_CHUNK);
            avail_out = ZLIB_COMPLETE_CHUNK;
            uint8_t * next_out = (uint8_t *) out.data() + out_ptr;
            if (!BrotliEncoderCompressStream(state,
                                             BROTLI_OPERATION_FINISH,
                                             &avail_in, &next_in,
                                             &avail_out, &next_out, NULL)) {
                throw std::bad_alloc();
            }

            std::size_t have = ZLIB_COMPLETE_CHUNK - avail_out;
            out_ptr += have;
            out_size += have;
            size_of_vec += have;
        }
        while (avail_out == 0);
        out.resize(out_size);
        return size_of_vec;
    }
    // ======================================================================================
    
    brotli_decompressor::brotli_decompressor()
    {
        state = BrotliDecoderCreateInstance(NULL, NULL, NULL);
        if (!state)
        {
            throw std::runtime_error("Failed to create BROTLI decoder");
        }
    }
    // --------------------------------------------------------------------------------------
    
    brotli_decompressor::~brotli_decompressor()
    {
        BrotliDecoderDestroyInstance(state);
    }
    // --------------------------------------------------------------------------------------
    
    std::size_t brotli_decompressor::decompress(const char* input, std::size_t size, std::vector<char>& out)
    {
        std::size_t out_ptr = out.size();
        std::size_t out_size = out.size();
        std::size_t total_out_size = 0;
        for (std::size_t i = 0; i < size; i += ZLIB_COMPLETE_CHUNK)
        {
            const uInt howManyLeft = static_cast<uInt>(size - i);
            const uInt howManyWanted = (howManyLeft > ZLIB_COMPLETE_CHUNK) ?
                                       ZLIB_COMPLETE_CHUNK : howManyLeft;

            size_t avail_in = howManyWanted;
            const uint8_t* next_in = (const uint8_t*) input + i;
            if (avail_in == 0)
            {
                break;
            }
            std::size_t avail_out = ZLIB_COMPLETE_CHUNK;
            do
            {
                out.resize(out.size() + ZLIB_COMPLETE_CHUNK);
                avail_out = ZLIB_COMPLETE_CHUNK;
                uint8_t* next_out = (uint8_t *) out.data() + out_ptr;
                BrotliDecoderResult retval = BrotliDecoderDecompressStream(state,
                                                                           &avail_in, &next_in, &avail_out, &next_out, 0);

                if (retval == BROTLI_DECODER_RESULT_ERROR)
                {
                    throw std::bad_alloc();
                }
                std::size_t have = ZLIB_COMPLETE_CHUNK - avail_out;
                out_ptr += have;
                out_size += have;
                total_out_size += have;
            }
            while (avail_out == 0);
            out.resize(out_size);

        }
        return total_out_size;
    }
    // ======================================================================================
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    zlib_compressor::zlib_compressor(int level,
                                     int windowBits) : strm_{0}

    {
        finished_ = false;
        strm_.zalloc = Z_NULL;
        strm_.zfree = Z_NULL;
        strm_.opaque = Z_NULL;
        if (level < 1 || level > 9)
        {
            level = 9;
        }
        const int memLevel = 9;
        const int retval = deflateInit2(&strm_, level, Z_DEFLATED, windowBits,
                                        memLevel, Z_DEFAULT_STRATEGY);
        if (retval != Z_OK)
        {
            throw std::bad_alloc();
        }
    }
#pragma GCC diagnostic pop
    // -------------------------------------------------------------------------------------------------
    
    std::size_t zlib_compressor::compress(const char* input, std::size_t size, std::vector<char>& out)
    {
        if (finished_)
        {
            throw std::runtime_error("compress data after finish");
        }
        std::size_t out_ptr = out.size();
        std::size_t out_size = out.size();
        std::size_t size_of_vec = out.size();
        for (std::size_t i = 0; i < size; i += ZLIB_COMPLETE_CHUNK)
        {
            const uInt howManyLeft = static_cast<uInt>(size - i);
            const uInt howManyWanted = (howManyLeft > ZLIB_COMPLETE_CHUNK) ?
                                       ZLIB_COMPLETE_CHUNK : howManyLeft;
            strm_.avail_in = howManyWanted;
            strm_.next_in = (Bytef*) (input + i);
            do
            {
                std::size_t have;
                out.resize(out.size() + ZLIB_COMPLETE_CHUNK);
                strm_.avail_out = ZLIB_COMPLETE_CHUNK;
                strm_.next_out = (Bytef*) out.data() + out_ptr;
                int retval = deflate(&strm_, Z_NO_FLUSH);
                if (retval == Z_STREAM_ERROR)
                {
                    throw std::bad_alloc();
                }
                have = ZLIB_COMPLETE_CHUNK - strm_.avail_out;
                out_ptr += have;
                out_size += have;
            }
            while (strm_.avail_out == 0);
        }
        out.resize(out_size);
        return out_size - size_of_vec;
    }
    // -------------------------------------------------------------------------------------------------
    
    std::size_t zlib_compressor::finish(std::vector<char>& out)
    {

        if (finished_)
        {
            throw std::runtime_error("already finished");
        }
        finished_ = true;
        strm_.avail_in = 0;
        strm_.next_in = Z_NULL;
        std::size_t out_ptr = out.size();
        std::size_t out_size = out.size();
        std::size_t size_of_vec = 0;
        do
        {
            out.resize(out.size() + ZLIB_COMPLETE_CHUNK);
            strm_.avail_out = ZLIB_COMPLETE_CHUNK;
            strm_.next_out = (Bytef*) out.data() + out_ptr;
            int retval = deflate(&strm_, Z_FINISH);
            if (retval == Z_STREAM_ERROR)
            {
                throw std::bad_alloc();
            }
            std::size_t have = ZLIB_COMPLETE_CHUNK - strm_.avail_out;
            out_ptr += have;
            out_size += have;
            size_of_vec += have;
        }
        while (strm_.avail_out == 0);
        out.resize(out_size);
        return size_of_vec;
    }
    // -------------------------------------------------------------------------------------------------
    
    zlib_compressor::~zlib_compressor()
    {
        deflateEnd(&strm_);
    }
    // ---------------------------------------------------------------------------------------------------------
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    zlib_decompressor::zlib_decompressor(int windowBits)
    : strm_{0}

    {
        int retval;
        strm_.zalloc = Z_NULL;
        strm_.zfree = Z_NULL;
        strm_.opaque = Z_NULL;
        strm_.avail_in = 0;
        strm_.next_in = Z_NULL;
        retval = inflateInit2(&strm_, windowBits);
        if (retval != Z_OK)
        {
            throw std::bad_alloc();
        }
    }
#pragma GCC diagnostic pop
    // ---------------------------------------------------------------------------------------------------------
    
    zlib_decompressor::~zlib_decompressor()
    {
        inflateEnd(&strm_);
    }
    // ---------------------------------------------------------------------------------------------------------
    
    std::size_t zlib_decompressor::decompress(const char* input, std::size_t size, std::vector<char>& out)
    {
        std::size_t out_ptr = out.size();
        std::size_t out_size = out.size();
        std::size_t total_out_size = 0;
        for (std::size_t i = 0; i < size; i += ZLIB_COMPLETE_CHUNK)
        {
            const uInt howManyLeft = static_cast<uInt>(size - i);
            const uInt howManyWanted = (howManyLeft > ZLIB_COMPLETE_CHUNK) ?
                                       ZLIB_COMPLETE_CHUNK : howManyLeft;

            strm_.avail_in = howManyWanted;
            strm_.next_in = (Bytef*) input + i;
            if (strm_.avail_in == 0)
            {
                break;
            }
            do
            {
                out.resize(out.size() + ZLIB_COMPLETE_CHUNK);
                strm_.avail_out = ZLIB_COMPLETE_CHUNK;
                strm_.next_out = (Bytef*) out.data() + out_ptr;

                int retval = inflate(&strm_, Z_NO_FLUSH);
                if (retval == Z_STREAM_ERROR)
                {
                    throw std::bad_alloc();
                }
                std::size_t have = ZLIB_COMPLETE_CHUNK - strm_.avail_out;
                out_ptr += have;
                out_size += have;
                total_out_size += have;
            }
            while (strm_.avail_out == 0);
            out.resize(out_size);

        }
        return total_out_size;
    }
    // ---------------------------------------------------------------------------------------------------------
    
    compressor* compressor::gzip(int level, int window_bits)
    {
        return new zlib_compressor(level, window_bits + 16);
    }
    // ---------------------------------------------------------------------------------------------------------
    
    compressor* compressor::zlib(int level, int window_bits)
    {
        return new zlib_compressor(level, window_bits);
    }
    // ---------------------------------------------------------------------------------------------------------
    compressor* compressor::raw(int level, int window_bits)
    {
        return new zlib_compressor(level, -window_bits);
    }
    // ---------------------------------------------------------------------------------------------------------
    
    compressor* compressor::brotli()
    {
        return new brotli_compressor;
    }
    // ========================================================================================================
    
    decompressor* decompressor::gzip()
    {
        return new zlib_decompressor(15 + 16);
    }
    // ---------------------------------------------------------------------------------------------------------
    
    decompressor* decompressor::zlib()
    {
        return new zlib_decompressor(15);
    }
    // ---------------------------------------------------------------------------------------------------------
    
    decompressor* decompressor::raw(int window_bits)
    {
        return new zlib_decompressor(-window_bits);
    }
    // ---------------------------------------------------------------------------------------------------------
    
    decompressor* decompressor::brotli()
    {
        return new brotli_decompressor;
    }
} // ns zlc

#if defined(BINADOX_WITH_UNITTESTS)
#include <doctest.h>
#include <string.h>

static std::string create_compression_data()
{
    const int chunkTestSize = 100000;
    std::string str;

    int ival = 0;
    int step = 1;

    while (str.size() < chunkTestSize)
    {
        str += (char) ival;
        ival = (ival + step) % 177;
        step = (step + 1) % 17;
        str += "padding";
    }
    return str;
}

static std::vector<char> compress(const std::string input, zlc::compressor* compressor)
{
    std::vector<char> output;
    std::size_t s1 = compressor->compress(input.c_str(), input.size(), output);
    REQUIRE(s1 == output.size());
    std::size_t s2 = compressor->finish(output);
    REQUIRE(s2 + s1 == output.size());
    delete compressor;
    return output;
}

static std::vector<char> decompress(const std::vector<char>& input, zlc::decompressor* decompressor)
{
    const std::size_t block_size = 1234;
    std::size_t n = input.size();
    std::size_t ptr = 0;
    std::vector<char> data;
    while (ptr < n)
    {
        std::size_t take = block_size;
        if (ptr + block_size >= n)
        {
            take = n - ptr;
        }
        std::size_t s1 = data.size();
        std::size_t s2 = decompressor->decompress(input.data() + ptr, take, data);
        REQUIRE (s1+s2 == data.size());
        ptr+=take;
    }
    delete decompressor;
    return data;
}

TEST_SUITE("zlc")
{
    TEST_CASE ("zlib")
    {
        std::string str = create_compression_data();
        std::vector<char> decompressed = decompress(compress(str, zlc::compressor::zlib(9)),
                                                    zlc::decompressor::zlib());

        REQUIRE(decompressed.size() == str.size());
        bool rc = memcmp(decompressed.data(), str.data(), decompressed.size()) == 0;
        REQUIRE(rc);
    }

    TEST_CASE ("gzip")
    {
        std::string str = create_compression_data();
        std::vector<char> decompressed = decompress(compress(str, zlc::compressor::gzip(9)),
                                                    zlc::decompressor::gzip());

        REQUIRE(decompressed.size() == str.size());
        bool rc = memcmp(decompressed.data(), str.data(), decompressed.size()) == 0;
        REQUIRE(rc);
    }

    TEST_CASE ("raw")
    {
        std::string str = create_compression_data();
        std::vector<char> decompressed = decompress(compress(str, zlc::compressor::zlib(9)),
                                                    zlc::decompressor::zlib());

        REQUIRE(decompressed.size() == str.size());
        bool rc = memcmp(decompressed.data(), str.data(), decompressed.size()) == 0;
        REQUIRE(rc);
    }

    TEST_CASE ("brotli")
    {
        std::string str = create_compression_data();
        std::vector<char> decompressed = decompress(compress(str, zlc::compressor::brotli()),
                                                    zlc::decompressor::brotli());

        REQUIRE(decompressed.size() == str.size());
        bool rc = memcmp(decompressed.data(), str.data(), decompressed.size()) == 0;
        REQUIRE(rc);
    }
}


#endif
