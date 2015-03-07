#ifndef ITCH_MESSAGES_HH
#define ITCH_MESSAGES_HH

#include <cstddef>
#include <cstdint>

static constexpr int itch_symbol_len = 16;

struct itch_message {
    char MsgType;
};

struct itch_seconds {
    char MsgType;
    char Second[5];
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

static inline uint64_t itch_uatoi(const char *p, size_t len)
{
    uint64_t ret = 0;
    auto end = p + len;
    while (p != end) {
        if (*p != ' ')
            break;
        p++;
    }
    while (p != end) {
        auto ch = *p++;
        ret = (ret * 10) + (ch - '0');
    }
    return ret;
}

#endif
