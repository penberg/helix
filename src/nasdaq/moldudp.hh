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

#include "message_parser.hh"

#include <cstdlib>
#include <cstdint>
#include <memory>

namespace helix {

namespace nasdaq {

struct moldudp_header {
    char     Session[10];
    uint32_t SequenceNumber;
    uint16_t MessageCount;
} __attribute__ ((packed));

struct moldudp_message_block {
    uint16_t MessageLength;
} __attribute__ ((packed));

class moldudp_session {
private:
    std::shared_ptr<message_parser> _parser;
    uint32_t _seq_num;
public:
    moldudp_session(std::shared_ptr<message_parser> parser);

    void parse(const char *p, size_t size);
};

}

}
