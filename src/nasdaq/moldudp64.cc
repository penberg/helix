#include "helix/nasdaq/moldudp64.hh"

#include "helix/nasdaq/moldudp64_messages.h"
#include "helix/compat/endian.h"
#include <cassert>
#include <cstdlib>
#include <string>

using namespace std;

namespace helix {

namespace nasdaq {

moldudp64_session::moldudp64_session(shared_ptr<net::message_parser> parser)
    : _parser(parser)
    , _seq_num{1}
{
}

size_t moldudp64_session::parse(const net::packet_view& packet)
{
    auto* p = packet.buf();

    assert(packet.len() >= sizeof(moldudp64_header));

    auto* header = packet.cast<moldudp64_header>();
    auto sequence_number = be64toh(header->SequenceNumber);
    if (sequence_number != _seq_num) {
        throw runtime_error(string("invalid sequence number: ") + to_string(sequence_number) + ", expected: " + to_string(_seq_num));
    }
    p += sizeof(moldudp64_header);

    for (int i = 0; i < be16toh(header->MessageCount); i++) {
        auto* msg_block = reinterpret_cast<const moldudp64_message_block*>(p);
        p += sizeof(moldudp64_message_block);
        auto message_length = be16toh(msg_block->MessageLength);
        _parser->parse(net::packet_view{p, message_length});
        p += message_length;
        _seq_num++;
    }

    return p - packet.buf();
}

}

}
