/*
 * NASDAQ US ITCH 5.0 protocol messages
 */

#ifndef HELIX_NASDAQ_US_ITCH_PROTO_H
#define HELIX_NASDAQ_US_ITCH_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ITCH_SYMBOL_LEN 8

struct itch50_message {
    char MessageType;
};

struct itch50_system_event {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    char     EventCode;
} __attribute__ ((packed));

struct itch50_stock_directory {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    char     Stock[8];
    char     MarketCategory;
    char     FinancialStatusIndicator;
    uint32_t RoundLotSize;
    char     RoundLotsOnly;
    char     IssueClassification;
    char     IssueSubType[2];
    char     Authenticity;
    char     ShortSaleThresholdIndicator;
    char     IPOFlag;
    char     LULDReferencePriceTier;
    char     ETPFlag;
    uint32_t ETPLeverageFactor;
    char     InverseIndicator;
} __attribute__ ((packed));

struct itch50_stock_trading_action {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    char     Stock[8];
    char     TradingState;
    char     Reserved;
    char     Reason[4];
} __attribute__ ((packed));

struct itch50_reg_sho_restriction {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    char     Stock[8];
    char     RegSHOAction;
} __attribute__ ((packed));

struct itch50_market_participant_position {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    char     MPID[4];
    char     Stock[8];
    char     PrimaryMarketMaker;
    char     MarketMakerMode;
    char     MarketParticipantState;
} __attribute__ ((packed));

struct itch50_mwcb_decline_level {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t Level1;
    uint64_t Level2;
    uint64_t Level3;
} __attribute__ ((packed));

struct itch50_mwcb_breach {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    char     BeachedLevel;
} __attribute__ ((packed));

struct itch50_ipo_quoting_period_update {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    char     Stock[8];
    uint32_t IPOQuotationReleaseTime;
    char     IPOQuotationReleaseQualifier;
    uint32_t IPOPrice;
} __attribute__ ((packed));

struct itch50_add_order {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t OrderReferenceNumber;
    char     BuySellIndicator;
    uint32_t Shares;
    char     Stock[8];
    uint32_t Price;
} __attribute__ ((packed));

struct itch50_add_order_mpid {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t OrderReferenceNumber;
    char     BuySellIndicator;
    uint32_t Shares;
    char     Stock[8];
    uint32_t Price;
    char     Attribution[4];
} __attribute__ ((packed));

struct itch50_order_executed {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t OrderReferenceNumber;
    uint32_t ExecutedShares;
    uint64_t MatchNumber;
} __attribute__ ((packed));

struct itch50_order_executed_with_price {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t OrderReferenceNumber;
    uint32_t ExecutedShares;
    uint64_t MatchNumber;
    char     Printable;
    uint32_t ExecutionPrice;
} __attribute__ ((packed));

struct itch50_order_cancel {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t OrderReferenceNumber;
    uint32_t CanceledShares;
} __attribute__ ((packed));

struct itch50_order_delete {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t OrderReferenceNumber;
} __attribute__ ((packed));

struct itch50_order_replace {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t OriginalOrderReferenceNumber;
    uint64_t NewOrderReferenceNumber;
    uint32_t Shares;
    uint32_t Price;
} __attribute__ ((packed));

struct itch50_trade {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t OrderReferenceNumber;
    char     BuySellIndicator;
    uint32_t Shares;
    char     Stock[8];
    uint32_t Price;
    uint64_t MatchNumber;
} __attribute__ ((packed));

struct itch50_cross_trade {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t Shares;
    char     Stock[8];
    uint32_t CrossPrice;
    uint64_t MatchNumber;
    char     CrossType;
} __attribute__ ((packed));

struct itch50_broken_trade {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t MatchNumber;
} __attribute__ ((packed));

struct itch50_noii {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    uint64_t PairedShares;
    uint64_t ImbalanceShares;
    char     ImbalanceDirection;
    char     Stock[8];
    uint32_t FarPrice;
    uint32_t NearPrice;
    uint32_t CurrentReferencePrice;
    char     CrossType;
    char     PriceVariationIndicator;
} __attribute__ ((packed));

struct itch50_rpii {
    char     MessageType;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp:48;
    char     Stock[8];
    char     InterestFlag;
} __attribute__ ((packed));

#ifdef __cplusplus
}
#endif

#endif
