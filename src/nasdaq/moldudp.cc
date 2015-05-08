#include "moldudp.hh"

#include <cassert>

using namespace std;

namespace helix {

namespace nasdaq {

moldudp_session::moldudp_session(shared_ptr<net::message_parser> parser)
    : _parser(parser)
    , _seq_num{1}
{
}

void moldudp_session::parse(const net::packet_view& packet)
{
    auto* end = packet.end();
    auto* p = packet.buf();

    assert(packet.len() >= sizeof(moldudp_header));

    auto* header = packet.cast<moldudp_header>();
    if (header->SequenceNumber != _seq_num) {
        throw runtime_error(string("invalid sequence number: ") + to_string(header->SequenceNumber) + ", expected: " + to_string(_seq_num));
    }
    p += sizeof(moldudp_header);

    for (int i = 0; i < header->MessageCount; i++) {
        auto* msg_block = reinterpret_cast<const moldudp_message_block*>(p);

        p += sizeof(moldudp_message_block);

        _parser->parse(net::packet_view{p, msg_block->MessageLength});

        p += msg_block->MessageLength;

        _seq_num++;
    }

    assert(p == end);
}

}

}
