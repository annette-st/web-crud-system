//
// Created by igor on 23/08/2020.
//

#ifndef BINADOX_ECAP_ABSTRACT_PRINTER_HH
#define BINADOX_ECAP_ABSTRACT_PRINTER_HH

#include <ostream>
#include <nonstd/string_view.hpp>

namespace binadox
{
    struct abstract_printer
    {
        template<typename T0>
        static void print(std::ostream& stream, const T0& a0)
        {
            stream << a0;
        }
        template<typename T0, typename T1>
        static void print(std::ostream& stream, const T0& a0, const T1& a1)
        {
            stream << a0 << " " << a1;
        }
        template<typename T0, typename T1, typename T2>
        static void print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2)
        {
            stream << a0 << " " << a1 << " " << a2;
        }
        template<typename T0, typename T1, typename T2, typename T3>
        static void print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4>
        static void print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6, const T7& a7)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6 << " " << a7;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6, const T7& a7, const T8& a8)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6 << " " << a7
                   << " " << a8;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6, const T7& a7, const T8& a8, const T9& a9)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6 << " " << a7
                   << " " << a8 << " " << a9;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6 << " " << a7
                   << " " << a8 << " " << a9 << " " << a10;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10, const T11& a11)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6 << " " << a7
                   << " " << a8 << " " << a9 << " " << a10 << " " << a11;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10, const T11& a11, const T12& a12)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6 << " " << a7
                   << " " << a8 << " " << a9 << " " << a10 << " " << a11 << " " << a12;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10, const T11& a11, const T12& a12,
              const T13& a13)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6 << " " << a7
                   << " " << a8 << " " << a9 << " " << a10 << " " << a11 << " " << a12 << " " << a13;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10, const T11& a11, const T12& a12,
              const T13& a13, const T14& a14)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6 << " " << a7
                   << " " << a8 << " " << a9 << " " << a10 << " " << a11 << " " << a12 << " " << a13 << " " << a14;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10, const T11& a11, const T12& a12,
              const T13& a13, const T14& a14, const T15& a15)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6 << " " << a7
                   << " " << a8 << " " << a9 << " " << a10 << " " << a11 << " " << a12 << " " << a13 << " " << a14
                   << " " << a15;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10, const T11& a11, const T12& a12,
              const T13& a13, const T14& a14, const T15& a15, const T16& a16)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6 << " " << a7
                   << " " << a8 << " " << a9 << " " << a10 << " " << a11 << " " << a12 << " " << a13 << " " << a14
                   << " " << a15 << " " << a16;
        }
        template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17>
        static void
        print(std::ostream& stream, const T0& a0, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5,
              const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10, const T11& a11, const T12& a12,
              const T13& a13, const T14& a14, const T15& a15, const T16& a16, const T17& a17)
        {
            stream << a0 << " " << a1 << " " << a2 << " " << a3 << " " << a4 << " " << a5 << " " << a6 << " " << a7
                   << " " << a8 << " " << a9 << " " << a10 << " " << a11 << " " << a12 << " " << a13 << " " << a14
                   << " " << a15 << " " << a16 << " " << a17;
        }
    };

    struct locus
    {
        locus(nonstd::string_view src, int aline)
        : file(src), line(aline)
        {

        }

        nonstd::string_view file;
        const int line;
    };

    inline
    std::ostream& operator << (std::ostream& os, const locus& loc)
    {
        os << loc.file << ":" << loc.line;
        return os;
    }
} // ns binadox

#endif //BINADOX_ECAP_ABSTRACT_PRINTER_HH
