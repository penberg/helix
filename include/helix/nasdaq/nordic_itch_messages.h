/*
 * NASDAQ Nordic ITCH protocol messages
 */

#ifndef HELIX_NASDAQ_NORDIC_ITCH_PROTO_H
#define HELIX_NASDAQ_NORDIC_ITCH_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define ITCH_SYMBOL_LEN 16

struct itch_message {
    char MsgType;
};

struct itch_seconds {
    char MsgType;
    char Second[5];
};

struct itch_market_segment_state {
    char MsgType;
    char MarketSegmentID[3];
    char EventCode;
};

struct itch_system_event {
    char MsgType;
    char EventCode;
};

struct itch_milliseconds {
    char MsgType;
    char Millisecond[3];
};

struct itch_order_book_directory {
    char MsgType;
    char OrderBook[6];
    char Symbol[16];
    char ISIN[12];
    char FinancialProduct[3];
    char TradingCurrency[3];
    char MIC[4];
    char MarketSegmentID[3];
    char NoteCodes[8];
    char RoundLotSize[9];
};

struct itch_order_book_trading_action {
    char MsgType;
    char OrderBook[6];
    char TradingState;
    char Reserved;
    char Reason[4];
};

struct itch_add_order {
    char MsgType;
    char OrderReferenceNumber[9];
    char BuySellIndicator;
    char Quantity[9];
    char OrderBook[6];
    char Price[10];
};

struct itch_add_order_mpid {
    char MsgType;
    char OrderReferenceNumber[9];
    char BuySellIndicator;
    char Quantity[9];
    char OrderBook[6];
    char Price[10];
    char Attribution[4];
};

struct itch_order_executed {
    char MsgType;
    char OrderReferenceNumber[9];
    char ExecutedQuantity[9];
    char MatchNumber[9];
    char OwnerParticipantID[4];
    char CounterPartyParticipantID[4];
};

struct itch_order_executed_with_price {
    char MsgType;
    char OrderReferenceNumber[9];
    char ExecutedQuantity[9];
    char MatchNumber[9];
    char Printable;
    char TradePrice[10];
    char OwnerParticipantID[4];
    char CounterPartyParticipantID[4];
};

struct itch_order_cancel {
    char MsgType;
    char OrderReferenceNumber[9];
    char CanceledQuantity[9];
};

struct itch_order_delete {
    char MsgType;
    char OrderReferenceNumber[9];
};

struct itch_trade {
    char MsgType;
    char OrderReferenceNumber[9];
    char TradeType;
    char Quantity[9];
    char OrderBook[6];
    char MatchNumber[9];
    char TradePrice[10];
    char OwnerParticipantID[4];
    char CounterpartyParticipantID[4];
};

struct itch_cross_trade {
    char MsgType;
    char Quantity[9];
    char OrderBook[6];
    char CrossPrice[10];
    char MatchNumber[9];
    char CrossType;
    char NumberOfTrades[10];
};

struct itch_broken_trade {
    char MsgType;
    char MatchNumber[9];
};

struct itch_noii {
    char MsgType;
    char PairedQuantity[9];
    char ImbalanceQuantity[9];
    char ImbalanceDirection;
    char OrderBook[6];
    char EquilibriumPrice[10];
    char CrossType;
    char BestBidPrice[10];
    char BestBidQuantity[9];
    char BestAskPrice[10];
    char BestAskQuantity[9];
};

static inline uint64_t itch_uatoi(const char *p, size_t len)
{
    uint64_t ret = 0;
    const char *end = p + len;
    while (p != end) {
        if (*p != ' ')
            break;
        p++;
    }
    while (p != end) {
        char ch = *p++;
        ret = (ret * 10) + (ch - '0');
    }
    return ret;
}

#ifdef __cplusplus
}
#endif

#endif
