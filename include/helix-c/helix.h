#ifndef HELIX_C_H
#define HELIX_C_H

/*!
 * @file  helix.h
 * @abstract Helix C bindings.
 */
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @typedef  helix_protocol_t
 * @abstract Type of a protocol descriptor.
 */
typedef struct helix_opaque_protocol *helix_protocol_t;

/*!
 * @typedef  helix_venue_t
 * @abstract Type of a venue.
 */
typedef struct helix_opaque_venue *helix_venue_t;

/*!
 * @typedef  helix_symbol_t
 * @abstract Type of a symbol.
 */
typedef struct helix_opaque_symbol *helix_symbol_t;

/*!
 * @typedef  helix_session_t
 * @abstract Type of a session.
 */
typedef struct helix_opaque_session *helix_session_t;

/*!
 * @typedef  helix_subscription_t
 * @abstract Type of a subscription.
 */
typedef struct helix_opaque_subscription *helix_subscription_t;

/*!
 * @typedef  helix_order_book_t
 * @abstract Type of an order book.
 */
typedef struct helix_opaque_order_book *helix_order_book_t;

/*!
 * @typedef  helix_callback_t
 * @abstract Type of a order book update callback.
 */
typedef void (*helix_callback_t)(helix_order_book_t);

/*!
 * @enum     helix_trading_state_t
 * @abstract Order book instrument trading state.
 */
typedef enum {
    /*! Order book instrument trading state is unknown. */
    HELIX_TRADING_STATE_UNKNOWN,
    /*! Order book instrument trading is halted. */
    HELIX_TRADING_STATE_HALTED,
    /*! Order book instrument is trading at the exchange. */
    HELIX_TRADING_STATE_TRADING,
    /*! Order book instrument is traded in intraday auction. */
    HELIX_TRADING_STATE_AUCTION,
} helix_trading_state_t;

/*!
 * @abstract Returns the order book trading state.
 * @param    ob  Order book.
 */
helix_trading_state_t helix_order_book_state(helix_order_book_t);

/*!
 * @abstract Returns the order book symbol.
 */
const char *helix_order_book_symbol(helix_order_book_t);

/*!
 * @abstract Returns the order book timestamp.
 */
uint64_t helix_order_book_timestamp(helix_order_book_t);

/*!
 * @abstract Returns the order book bid price for a price level.
 */
uint64_t helix_order_book_bid_price(helix_order_book_t, size_t);

/*!
 * @abstract Returns the order book bid size for a price level.
 */
uint64_t helix_order_book_bid_size(helix_order_book_t, size_t);

/*!
 * @abstract Returns the order book ask price for a price level.
 */
uint64_t helix_order_book_ask_price(helix_order_book_t, size_t);

/*!
 * @abstract Returns the order book ask size for a price level.
 */
uint64_t helix_order_book_ask_size(helix_order_book_t, size_t);

/*!
 * @abstract Lookup a market data protocol.
 */
helix_protocol_t helix_protocol_lookup(const char *name);

/*!
 * Create a new session.
 */
helix_session_t helix_session_create(helix_protocol_t, const char *symbol, helix_callback_t);

/*!
 * @abstract Process a packet for a session.
 *
 * This function is called by the packet I/O code to process raw packet data.
 */
void helix_session_process_packet(helix_session_t, const char* buf, size_t len);

/*!
 * @abstract Subscribe to listening to market data updates for a symbol.
 */
helix_subscription_t helix_session_subscribe(helix_session_t, helix_venue_t, helix_symbol_t);

/*!
 * @abstract Unsubscribe a subscription from session.
 */
void helix_session_unsubscribe(helix_subscription_t);

#ifdef __cplusplus
}
#endif

#endif
