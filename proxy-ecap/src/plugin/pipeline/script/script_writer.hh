//
// Created by igor on 28/07/2020.
//

#ifndef BINADOX_ECAP_SCRIPT_WRITER_HH
#define BINADOX_ECAP_SCRIPT_WRITER_HH

#include <string>
#include <sstream>

namespace binadox
{
    class script_writer
    {
    public:
        script_writer(bool need_head);
        void start_script();
        void write_comment(const std::string& line);
        void start_script(const std::string& nonce);
        void end_script();
        void write_line(const std::string& line);

        template <typename T>
        void write(const T& data)
        {
            os << data;
        }

        std::string finalize();

        std::ostream& stream();
    private:
        std::ostringstream os;
        bool write_head;
    };

    template<typename T>
    script_writer& operator<< (script_writer& wr, const T& data)
    {
        wr.write(data);
        return wr;
    }
} // ns binadox


#endif //BINADOX_ECAP_SCRIPT_WRITER_HH
