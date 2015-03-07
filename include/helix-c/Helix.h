#ifndef HELIX_C_H
#define HELIX_C_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HelixOpaqueProtocol     *HelixProtocolRef;
typedef struct HelixOpaqueVenue        *HelixVenueRef;
typedef struct HelixOpaqueSymbol       *HelixSymbolRef;
typedef struct HelixOpaqueSession      *HelixSessionRef;
typedef struct HelixOpaqueSubscription *HelixSubscriptionRef;
typedef struct HelixOpaqueOrderBook    *HelixOrderBookRef;

typedef void (*HelixCallback)(HelixOrderBookRef);

typedef enum {
    HELIX_TRADING_STATE_UNKNOWN,
    HELIX_TRADING_STATE_HALTED,
    HELIX_TRADING_STATE_TRADING,
    HELIX_TRADING_STATE_AUCTION,
} HelixTradingState;

HelixTradingState HelixOrderBookState(HelixOrderBookRef);

const char *HelixOrderBookSymbol(HelixOrderBookRef);

/**
 * Lookup a market data protocol.
 */
HelixProtocolRef HelixProtocolLookup(const char *name);

/**
 * Create a new session.
 */
HelixSessionRef HelixSessionCreate(HelixProtocolRef, const char *symbol, HelixCallback);

/**
 * Process a packet for a session.
 *
 * This function is called by the packet I/O code to process raw packet data.
 */
void HelixSessionProcessPacket(HelixSessionRef, const char* buf, size_t len);

/**
 * Subscribe to listening to market data updates for a symbol at a specific
 * venue.
 */
HelixSubscriptionRef HelixSessionSubscribe(HelixSessionRef, HelixVenueRef, HelixSymbolRef);

/**
 * Unsubscribe a subscription from session.
 */
void HelixSessionUnsubscribe(HelixSubscriptionRef);

#ifdef __cplusplus
}
#endif

#endif
