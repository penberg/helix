#include "binaryfile.hh"

#include <stdexcept>

#include <endian.h>

using namespace std;

namespace helix {

namespace nasdaq {

binaryfile_session::binaryfile_session(shared_ptr<net::message_parser> parser)
    : _parser{parser}
{
}

size_t binaryfile_session::parse(const net::packet_view& packet)
{
    uint16_t payload_len = be16toh(*packet.cast<uint16_t>());
    if (!payload_len) {
        // End of session.
        return 0;
    }
    size_t offset = sizeof(uint16_t);
    while (payload_len) {
        size_t nr = _parser->parse(net::packet_view{packet.buf() + offset, payload_len});
        if (nr > payload_len) {
            throw std::runtime_error("payload overflow");
        }
        payload_len -= nr;
        offset += nr;
    }
    return offset;
}

}

}
