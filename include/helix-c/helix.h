#ifndef HELIX_C_H
#define HELIX_C_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct helix_opaque_protocol     *helix_protocol_t;
typedef struct helix_opaque_venue        *helix_venue_t;
typedef struct helix_opaque_symbol       *helix_symbol_t;
typedef struct helix_opaque_session      *helix_session_t;
typedef struct helix_opaque_subscription *helix_subscription_t;
typedef struct helix_opaque_order_book   *helix_order_book_t;

typedef void (*helix_callback_t)(helix_order_book_t);

typedef enum {
    HELIX_TRADING_STATE_UNKNOWN,
    HELIX_TRADING_STATE_HALTED,
    HELIX_TRADING_STATE_TRADING,
    HELIX_TRADING_STATE_AUCTION,
} helix_trading_state_t;

helix_trading_state_t helix_order_book_state(helix_order_book_t);

const char *helix_order_book_symbol(helix_order_book_t);

uint64_t helix_order_book_timestamp(helix_order_book_t);

uint64_t helix_order_book_bid_price(helix_order_book_t, size_t);

uint64_t helix_order_book_bid_size(helix_order_book_t, size_t);

uint64_t helix_order_book_ask_price(helix_order_book_t, size_t);

uint64_t helix_order_book_ask_size(helix_order_book_t, size_t);

/**
 * Lookup a market data protocol.
 */
helix_protocol_t helix_protocol_lookup(const char *name);

/**
 * Create a new session.
 */
helix_session_t helix_session_create(helix_protocol_t, const char *symbol, helix_callback_t);

/**
 * Process a packet for a session.
 *
 * This function is called by the packet I/O code to process raw packet data.
 */
void helix_session_process_packet(helix_session_t, const char* buf, size_t len);

/**
 * Subscribe to listening to market data updates for a symbol at a specific
 * venue.
 */
helix_subscription_t helix_session_subscribe(helix_session_t, helix_venue_t, helix_symbol_t);

/**
 * Unsubscribe a subscription from session.
 */
void helix_session_unsubscribe(helix_subscription_t);

#ifdef __cplusplus
}
#endif

#endif
