#include "soupfile.hh"

#include <stdexcept>

using namespace std;

namespace helix {

namespace nasdaq {

soupfile_session::soupfile_session(shared_ptr<net::message_parser> parser)
    : _parser{std::move(parser)}
{
}

size_t soupfile_session::parse(const net::packet_view& packet)
{
    static std::string terminator = "\r\n";
    auto view = packet.as_string_view();
    auto terminator_start = view.find_first_of(terminator);
    if (terminator_start == std::experimental::string_view::npos) {
        throw runtime_error("packet is truncated");
    }
    if (!terminator_start) {
        return 0;
    }
    auto nr = _parser->parse(packet);
    if (nr > terminator_start) {
        throw runtime_error("parsed message is larger than the framing");
    }
    return terminator_start + terminator.size();
}

}

}
