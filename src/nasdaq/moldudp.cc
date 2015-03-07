#include "moldudp.hh"

#include <cassert>

using namespace std;

moldudp_session::moldudp_session(shared_ptr<message_parser> parser)
    : _parser(parser)
    , _seq_num{1}
{
}

void moldudp_session::parse(const char *buf, size_t len)
{
    auto* end = buf + len;
    auto* p = buf;

    assert(len >= sizeof(moldudp_header));

    auto* header = reinterpret_cast<const moldudp_header*>(p);

    if (header->SequenceNumber != _seq_num) {
        throw runtime_error(string("invalid sequence number: ") + to_string(header->SequenceNumber) + ", expected: " + to_string(_seq_num));
    }
    p += sizeof(moldudp_header);

    for (int i = 0; i < header->MessageCount; i++) {
        auto* msg_block = reinterpret_cast<const moldudp_message_block*>(p);

        p += sizeof(moldudp_message_block);

        _parser->parse(p, msg_block->MessageLength);

        p += msg_block->MessageLength;

        _seq_num++;
    }

    assert(p == end);
}
