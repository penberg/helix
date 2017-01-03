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

#include <experimental/optional>

namespace helix {

namespace nasdaq {

enum moldudp64_state {
    synchronized,
    gap_fill,
};

template<typename Handler>
class moldudp64_session : public session {
    Handler _handler;
    send_callback _send_cb;
    uint64_t _expected_seq_no = 1;
    moldudp64_state _state = moldudp64_state::synchronized;
    std::experimental::optional<uint64_t> _sync_to_seq_no;
public:
    explicit moldudp64_session(void *data);

    virtual bool is_rth_timestamp(uint64_t timestamp) override;

    virtual void subscribe(const std::string& symbol, size_t max_orders) override;

    virtual void register_callback(event_callback callback) override;

    virtual void set_send_callback(send_callback callback) override;

    virtual size_t process_packet(const net::packet_view& packet) override;

private:
    void retransmit_request(uint64_t seq_no, uint64_t expected_seq_no);
};

template<typename Handler>
moldudp64_session<Handler>::moldudp64_session(void* data)
    : session{data}
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
void moldudp64_session<Handler>::set_send_callback(send_callback send_cb)
{
    _send_cb = send_cb;
}

template<typename Handler>
size_t moldudp64_session<Handler>::process_packet(const net::packet_view& packet)
{
    auto* p = packet.buf();
    if (packet.len() < sizeof(moldudp64_header)) {
        throw std::invalid_argument("truncated packet");
    }
    auto* header = packet.cast<moldudp64_header>();
    auto recv_seq_no = be64toh(header->SequenceNumber);
    if (recv_seq_no < _expected_seq_no) {
        return packet.len();
    }
    if (_expected_seq_no < recv_seq_no) {
        _sync_to_seq_no = recv_seq_no;
        if (_state == moldudp64_state::synchronized) {
            _state = moldudp64_state::gap_fill;
            retransmit_request(recv_seq_no, _expected_seq_no);
        }
        return packet.len();
    }
    bool sync = _state == moldudp64_state::synchronized;
    p += sizeof(moldudp64_header);
    for (int i = 0; i < be16toh(header->MessageCount); i++) {
        auto* msg_block = reinterpret_cast<const moldudp64_message_block*>(p);
        p += sizeof(moldudp64_message_block);
        auto message_length = be16toh(msg_block->MessageLength);
        if (message_length) {
            _handler.process_packet(net::packet_view{p, message_length}, sync);
        }
        p += message_length;
        _expected_seq_no++;
    }
    if (_state == moldudp64_state::gap_fill) {
        if (_expected_seq_no >= _sync_to_seq_no.value()) {
            _state = moldudp64_state::synchronized;
            _sync_to_seq_no = std::experimental::nullopt;
        } else {
            retransmit_request(recv_seq_no, _expected_seq_no);
        }
    }
    return p - packet.buf();
}

template<typename Handler>
void moldudp64_session<Handler>::retransmit_request(uint64_t seq_no, uint64_t expected_seq_no)
{
    if (!bool(_send_cb)) {
         throw std::runtime_error(std::string("invalid sequence number: ") + std::to_string(seq_no) + ", expected: " + std::to_string(expected_seq_no));
    }
    uint64_t message_count = 0xfffe;

    moldudp64_request_packet request_packet;
    request_packet.SequenceNumber = htobe64(expected_seq_no);
    request_packet.MessageCount = htobe16(message_count);

    char *base = reinterpret_cast<char*>(&request_packet);
    size_t len = sizeof(request_packet);

    _send_cb(base, len);
}

}

}
