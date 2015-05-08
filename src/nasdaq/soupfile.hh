/*
 * SoupFILE support
 *
 * Copyright (C) 2015 Bitbot
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

class soupfile_session {
    std::shared_ptr<net::message_parser> _parser;
public:
    soupfile_session(std::shared_ptr<net::message_parser> parser);

    size_t parse(const net::packet_view& packet);
};

}

}
