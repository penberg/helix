#ifndef HELIX_C_H
#define HELIX_C_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *HelixProtocol;
typedef void *HelixVenue;
typedef void *HelixSymbol;
typedef void *HelixSession;
typedef void *HelixSubscription;

/**
 * Lookup a market data protocol.
 */
HelixProtocol HelixProtocolLookup(const char *name);

/**
 * Create a new session.
 */
HelixSession HelixSessionCreate(HelixProtocol);

/**
 * Process a packet for a session.
 *
 * This function is called by the packet I/O code to process raw packet data.
 */
void HelixSessionProcessPacket(HelixSession, void *buf, size_t len);

/**
 * Subscribe to listening to market data updates for a symbol at a specific
 * venue.
 */
HelixSubscription HelixSessionSubscribe(HelixSession, HelixVenue, HelixSymbol);

/**
 * Unsubscribe a subscription from session.
 */
void HelixSessionUnsubscribe(HelixSubscription);

#ifdef __cplusplus
}
#endif

#endif
