/*
 * MoldUDP64 protocol support
 *
 * The implementation is based on the following specifications provided
 * by NASDAQ:
 *
 *   MoldUDP64 Protocol Specification
 *   V 1.00
 *   07/07/2009
 */

#pragma once

#include "helix/nasdaq/moldudp64_messages.h"
#include "helix/compat/endian.h"
#include "helix/helix.hh"
#include "helix/net.hh"

namespace helix {

namespace nasdaq {

template<typename Handler>
class moldudp64_session : public session {
    Handler _handler;
    uint64_t _seq_num;
public:
    explicit moldudp64_session(void *data);

    virtual bool is_rth_timestamp(uint64_t timestamp) override;

    virtual void subscribe(const std::string& symbol, size_t max_orders) override;

    virtual void register_callback(event_callback callback) override;

    virtual size_t process_packet(const net::packet_view& packet) override;
};

template<typename Handler>
moldudp64_session<Handler>::moldudp64_session(void* data)
    : session{data}
    , _seq_num{1}
{
}

template<typename Handler>
bool moldudp64_session<Handler>::is_rth_timestamp(uint64_t timestamp)
{
    return _handler.is_rth_timestamp(timestamp);
}

template<typename Handler>
void moldudp64_session<Handler>::subscribe(const std::string& symbol, size_t max_orders)
{
    _handler.subscribe(symbol, max_orders);
}

template<typename Handler>
void moldudp64_session<Handler>::register_callback(event_callback callback)
{
    _handler.register_callback(callback);
}

template<typename Handler>
size_t moldudp64_session<Handler>::process_packet(const net::packet_view& packet)
{
    auto* p = packet.buf();

    assert(packet.len() >= sizeof(moldudp64_header));

    auto* header = packet.cast<moldudp64_header>();
    auto sequence_number = be64toh(header->SequenceNumber);
    if (sequence_number != _seq_num) {
        throw std::runtime_error(std::string("invalid sequence number: ") + std::to_string(sequence_number) + ", expected: " + std::to_string(_seq_num));
    }
    p += sizeof(moldudp64_header);

    for (int i = 0; i < be16toh(header->MessageCount); i++) {
        auto* msg_block = reinterpret_cast<const moldudp64_message_block*>(p);
        p += sizeof(moldudp64_message_block);
        auto message_length = be16toh(msg_block->MessageLength);
        _handler.process_packet(net::packet_view{p, message_length});
        p += message_length;
        _seq_num++;
    }

    return p - packet.buf();
}

}

}
