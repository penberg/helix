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
 * @typedef helix_timestamp_t
 * @abstract Type of a timestamp.
 */
typedef uint64_t helix_timestamp_t;

/*!
 * @typedef helix_price_t
 * @abstract Type of a price.
 */
typedef uint64_t helix_price_t;

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
 * @typedef  helix_trade_t
 * @abstract Type of a trade.
 */
typedef struct helix_opaque_trade *helix_trade_t;

/*!
 * @typedef  helix_event_t
 * @abstract Type of an event.
 */
typedef struct helix_opaque_event *helix_event_t;

/*!
 * @enum     helix_event_mask_t
 * @abstract Event mask.
 */
typedef enum {
    /*! Order book update. */
    HELIX_EVENT_ORDER_BOOK_UPDATE = 1UL << 0,
    /*! Trade. */
    HELIX_EVENT_TRADE = 1UL << 1,
} helix_event_mask_t;

/*!
 * @abstract Returns the event mask.
 */
helix_event_mask_t helix_event_mask(helix_event_t);

/*!
 * @abstract Returns an order book from an event.
 *
 * The function returns an order book object if HELIX_EVENT_ORDER_BOOK_UPDATE
 * bit is set in the event mask.
 */
helix_order_book_t helix_event_order_book(helix_event_t);

/*!
 * @abstract Returns a trade from an event object.
 *
 * The function returns a trade object if HELIX_EVEN_TRADE bit is set in the
 * event mask.
 */
helix_trade_t helix_event_trade(helix_event_t);

/*!
 * @typedef  helix_event_callback_t
 * @abstract Type of an event callback.
 */
typedef void (*helix_event_callback_t)(helix_session_t, helix_event_t);

/*!
 * @enum     helix_trading_state_t
 * @abstract Order book instrument trading state.
 */
typedef enum {
    /*! Order book instrument trading state is unknown. */
    HELIX_TRADING_STATE_UNKNOWN,
    /*! Order book instrument trading is halted. */
    HELIX_TRADING_STATE_HALTED,
    /*! Order book instrument trading is paused. */
    HELIX_TRADING_STATE_PAUSED,
    /*! Order book instrument is in quotation only period. */
    HELIX_TRADING_STATE_QUOTATION_ONLY,
    /*! Order book instrument is trading at the exchange. */
    HELIX_TRADING_STATE_TRADING,
    /*! Order book instrument is traded in intraday auction. */
    HELIX_TRADING_STATE_AUCTION,
} helix_trading_state_t;

/*!
 * @enum     helix_trade_sign_t
 * @abstract Trade sign.
 */
typedef enum {
    /*! Trade was buyer-initiated. */
    HELIX_TRADE_SIGN_BUYER_INITIATED,
    /*! Trade was seller-initiated. */
    HELIX_TRADE_SIGN_SELLER_INITIATED,
    /*! Trade was crossing. */
    HELIX_TRADE_SIGN_CROSSING,
    /*! Trade was matched with a non-displayable order. */
    HELIX_TRADE_SIGN_NON_DISPLAYABLE,
} helix_trade_sign_t;

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
 * @abstract Returns the number of order book bid price levels.
 */
size_t helix_order_book_bid_levels(helix_order_book_t);

/*!
 * @abstract Returns the number of order book ask price levels.
 */
size_t helix_order_book_ask_levels(helix_order_book_t);

/*!
 * @abstract Returns the number of orders in the book.
 */
size_t helix_order_book_order_count(helix_order_book_t);

/*!
 * @abstract Returns the order book bid price for a price level.
 */
helix_price_t helix_order_book_bid_price(helix_order_book_t, size_t);

/*!
 * @abstract Returns the order book bid size for a price level.
 */
uint64_t helix_order_book_bid_size(helix_order_book_t, size_t);

/*!
 * @abstract Returns the order book ask price for a price level.
 */
helix_price_t helix_order_book_ask_price(helix_order_book_t, size_t);

/*!
 * @abstract Returns the order book ask size for a price level.
 */
uint64_t helix_order_book_ask_size(helix_order_book_t, size_t);

/*!
 * @abstract Returns midprice for a price level in the order book.
 */
helix_price_t helix_order_book_midprice(helix_order_book_t, size_t);

/*!
 * @abstract Returns the trade symbol.
 */
const char *helix_trade_symbol(helix_trade_t);

/*!
 * @abstract Returns the trade timestamp.
 */
uint64_t helix_trade_timestamp(helix_trade_t);

/*!
 * @abstract Returns the trade sign.
 */
helix_trade_sign_t helix_trade_sign(helix_trade_t);

/*!
 * @abstract Returns the price of a trade.
 */
helix_price_t helix_trade_price(helix_trade_t);

/*!
 * @abstract Returns the size of a trade.
 */
uint64_t helix_trade_size(helix_trade_t);

/*!
 * @abstract Lookup a market data protocol.
 */
helix_protocol_t helix_protocol_lookup(const char *name);

/*!
 * Create a new session.
 */
helix_session_t helix_session_create(helix_protocol_t, helix_event_callback_t, void *data);

/*!
 * @abstract Returns session opaque context data.
 */
void *helix_session_data(helix_session_t);

/*!
 * @abstract Process a packet for a session.
 *
 * This function is called by the packet I/O code to process raw packet data.
 */
size_t helix_session_process_packet(helix_session_t, const char* buf, size_t len);

/*!
 * @abstract Subscribe to listening to market data updates for a symbol.
 */
void helix_session_subscribe(helix_session_t, const char *symbol, size_t max_orders);

/*!
 * @abstract Unsubscribe a subscription from session.
 */
void helix_session_unsubscribe(helix_subscription_t);

#ifdef __cplusplus
}
#endif

#endif
