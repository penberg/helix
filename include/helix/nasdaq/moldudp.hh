/*
 * MoldUDP protocol support
 *
 * The implementation is based on the following specifications provided
 * by NASDAQ OMX:
 *
 *   MoldUDP
 *   Version 1.02a
 *   October 19, 2006
 *
 * and
 *
 *   MoldUDP for NASDAQ OMX Nordic
 *   Version 1.0.1
 *   February 10, 2014
 */

#pragma once

#include "helix/nasdaq/moldudp_messages.h"
#include "helix/helix.hh"
#include "helix/net.hh"

#include <string>

namespace helix {

namespace nasdaq {

template<typename Handler>
class moldudp_session : public session {
    Handler _handler;
    uint32_t _seq_num;
public:
    explicit moldudp_session(void* data);

    virtual bool is_rth_timestamp(uint64_t timestamp) override;

    virtual void subscribe(const std::string& symbol, size_t max_orders) override;

    virtual void register_callback(event_callback callback) override;

    virtual void set_send_callback(send_callback callback) override;

    virtual size_t process_packet(const net::packet_view& packet) override;

};

template<typename Handler>
moldudp_session<Handler>::moldudp_session(void* data)
    : session{data}
    , _seq_num{1}
{
}

template<typename Handler>
bool moldudp_session<Handler>::is_rth_timestamp(uint64_t timestamp)
{
    return _handler.is_rth_timestamp(timestamp);
}

template<typename Handler>
void moldudp_session<Handler>::subscribe(const std::string& symbol, size_t max_orders)
{
    _handler.subscribe(symbol, max_orders);
}

template<typename Handler>
void moldudp_session<Handler>::register_callback(event_callback callback)
{
    _handler.register_callback(callback);
}

template<typename Handler>
void moldudp_session<Handler>::set_send_callback(send_callback send_cb)
{
}

template<typename Handler>
size_t moldudp_session<Handler>::process_packet(const net::packet_view& packet)
{
    auto* p = packet.buf();

    assert(packet.len() >= sizeof(moldudp_header));

    auto* header = packet.cast<moldudp_header>();
    if (header->SequenceNumber != _seq_num) {
        throw std::runtime_error(std::string("invalid sequence number: ") + std::to_string(header->SequenceNumber) + ", expected: " + std::to_string(_seq_num));
    }
    p += sizeof(moldudp_header);

    for (int i = 0; i < header->MessageCount; i++) {
        auto* msg_block = reinterpret_cast<const moldudp_message_block*>(p);

        p += sizeof(moldudp_message_block);

        _handler.process_packet(net::packet_view{p, msg_block->MessageLength});

        p += msg_block->MessageLength;

        _seq_num++;
    }

    return p - packet.buf();
}

}

}
