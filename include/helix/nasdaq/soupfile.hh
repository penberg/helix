/*
 * SoupFILE support
 *
 * This implementation is based on the following specification provided
 * by NASDAQ:
 *
 *   SoupFILE
 *   Version 1.00
 *   3/30/2010
 */

#pragma once

#include "helix/helix.hh"

namespace helix {

namespace nasdaq {

template<typename Handler>
class soupfile_session : public session {
    Handler _handler;
public:
    explicit soupfile_session(void* data);

    virtual bool is_rth_timestamp(uint64_t timestamp) override;

    virtual void subscribe(const std::string& symbol, size_t max_orders) override;

    virtual void register_callback(event_callback callback) override;

    virtual size_t process_packet(const net::packet_view& packet) override;
};

template<typename Handler>
soupfile_session<Handler>::soupfile_session(void* data)
    : session{data}
{
}

template<typename Handler>
bool soupfile_session<Handler>::is_rth_timestamp(uint64_t timestamp)
{
    return _handler.is_rth_timestamp(timestamp);
}

template<typename Handler>
void soupfile_session<Handler>::subscribe(const std::string& symbol, size_t max_orders)
{
    _handler.subscribe(symbol, max_orders);
}

template<typename Handler>
void soupfile_session<Handler>::register_callback(event_callback callback)
{
    _handler.register_callback(callback);
}

template<typename Handler>
size_t soupfile_session<Handler>::process_packet(const net::packet_view& packet)
{
    static std::string terminator = "\r\n";
    auto view = packet.as_string_view();
    auto terminator_start = view.find_first_of(terminator);
    if (terminator_start == std::experimental::string_view::npos) {
        throw std::runtime_error("packet is truncated");
    }
    if (!terminator_start) {
        return 0;
    }
    auto nr = _handler.process_packet(packet);
    if (nr > terminator_start) {
        throw std::runtime_error("parsed message is larger than the framing");
    }
    return terminator_start + terminator.size();
}

}

}
