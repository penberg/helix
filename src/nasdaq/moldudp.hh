/*
 * MoldUDP protocol support
 *
 * Copyright (C) 2015 Bitbot
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

#include "helix/net.hh"

#include <cstdint>
#include <memory>

namespace helix {

namespace nasdaq {

class moldudp_session {
private:
    std::shared_ptr<net::message_parser> _parser;
    uint32_t _seq_num;
public:
    moldudp_session(std::shared_ptr<net::message_parser> parser);

    size_t parse(const net::packet_view& packet);
};

}

}
