/*
 * MoldUDP64 protocol messages
 *
 * The implementation is based on the following specifications provided
 * by NASDAQ:
 *
 *   MoldUDP64 Protocol Specification
 *   V 1.00
 *   07/07/2009
 */

#ifndef HELIX_NASDAQ_MOLDUDP64_MESSAGES_H
#define HELIX_NASDAQ_MOLDUDP64_MESSAGES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct moldudp64_header {
    char     Session[10];
    uint64_t SequenceNumber;
    uint16_t MessageCount;
} __attribute__ ((packed));

struct moldudp64_message_block {
    uint16_t MessageLength;
} __attribute__ ((packed));

struct moldudp64_request_packet {
    char     Session[10];
    uint64_t SequenceNumber;
    uint16_t MessageCount;
} __attribute__ ((packed));

#ifdef __cplusplus
}
#endif

#endif
