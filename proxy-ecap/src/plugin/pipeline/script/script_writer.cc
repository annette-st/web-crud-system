//
// Created by igor on 28/07/2020.
//

#include "plugin/pipeline/script/script_writer.hh"

namespace binadox
{
    script_writer::script_writer(bool need_head)
    : write_head(need_head)
    {
        if (need_head)
        {
            write_line("\n\r<head>");
        }
    }
    // ------------------------------------------------------------------------------------------
    void script_writer::start_script(const std::string& nonce)
    {
        write_line("<script nonce=\"" + nonce + "\">");
    }
    // ------------------------------------------------------------------------------------------
    void script_writer::start_script()
    {
        write_line("<script>");
    }
    // ------------------------------------------------------------------------------------------
    void script_writer::write_comment(const std::string& line)
    {
        write_line("<!-- " + line + " -->");
    }
    // ------------------------------------------------------------------------------------------
    void script_writer::end_script()
    {
        write_line("</script>");
    }
    // ------------------------------------------------------------------------------------------
    void script_writer::write_line(const std::string& line)
    {
        os << line << "\n\r";
    }
    // ------------------------------------------------------------------------------------------
    std::string script_writer::finalize()
    {
        if (write_head)
        {
            write_line("\n\r</head>");
        }
        return os.str();
    }
    // ------------------------------------------------------------------------------------------
    std::ostream& script_writer::stream()
    {
        return os;
    }
}
