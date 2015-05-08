#include "soupfile.hh"

#include <stdexcept>

using namespace std;

namespace helix {

namespace nasdaq {

soupfile_session::soupfile_session(shared_ptr<net::message_parser> parser)
    : _parser{parser}
{
}

size_t soupfile_session::parse(const net::packet_view& packet)
{
    size_t nr = _parser->parse(packet);
    size_t remaining = packet.len() - nr;
    static constexpr size_t terminator_size = 2;
    if (remaining < terminator_size) {
        throw runtime_error("packet is truncated");
    }
    const char* terminator = packet.buf() + nr;
    if (*terminator++ != 0x0d || *terminator != 0x0a) {
        throw runtime_error("terminator mismatch");
    }
    return nr + terminator_size;
}

}

}
