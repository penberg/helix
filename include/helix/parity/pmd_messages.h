/*
 * Parity PMD protocol messages
 */

#ifndef HELIX_PARITY_PMD_MESSAGES_H
#define HELIX_PARITY_PMD_MESSAGES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define PMD_INSTRUMENT_LEN 8

struct pmd_message {
    char MessageType;
};

struct pmd_version {
    char     MessageType;
    uint32_t Version;
} __attribute__ ((packed));

struct pmd_second {
    char     MessageType;
    uint32_t Second;
} __attribute__ ((packed));

struct pmd_order_added {
    char     MessageType;
    uint32_t Timestamp;
    uint64_t OrderNumber;
    char     Side;
    char     Instrument[8];
    uint32_t Quantity;
    uint32_t Price;
} __attribute__ ((packed));

struct pmd_order_executed {
    char     MessageType;
    uint32_t Timestamp;
    uint64_t OrderNumber;
    uint32_t Quantity;
    uint32_t MatchNumber;
} __attribute__ ((packed));

struct pmd_order_canceled {
    char     MessageType;
    uint32_t Timestamp;
    uint64_t OrderNumber;
    uint32_t CanceledQuantity;
} __attribute__ ((packed));

struct pmd_order_deleted {
    char     MessageType;
    uint32_t Timestamp;
    uint64_t OrderNumber;
} __attribute__ ((packed));

struct pmd_broken_trade {
    char     MessageType;
    uint32_t Timestamp;
    uint32_t MatchNumber;
} __attribute__ ((packed));

#ifdef __cplusplus
}
#endif

#endif
