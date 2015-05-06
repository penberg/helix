/*
 * MoldUDP protocol messages
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

#ifndef HELIX_NASDAQ_MOLDUDP_PROTO_H
#define HELIX_NASDAQ_MOLDUDP_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct moldudp_header {
    char     Session[10];
    uint32_t SequenceNumber;
    uint16_t MessageCount;
} __attribute__ ((packed));

struct moldudp_message_block {
    uint16_t MessageLength;
} __attribute__ ((packed));

#ifdef __cplusplus
}
#endif

#endif