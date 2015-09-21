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

#include "helix/net.hh"

#include <memory>

namespace helix {

namespace nasdaq {

class soupfile_session : public net::message_parser {
    std::shared_ptr<net::message_parser> _parser;
public:
    explicit soupfile_session(std::shared_ptr<net::message_parser> parser);

    virtual size_t parse(const net::packet_view& packet) override;
};

}

}
