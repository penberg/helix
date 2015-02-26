#ifndef LLTP_H
#define LLTP_H

namespace lltp {

struct Venue {
};

struct Symbol {
};

struct Connection {
    virtual Subscription Subscribe();
};

struct Subscription {
    virtual void Unsubsribe();
};

struct Error {
};

struct OrderBook {
};

struct Trade {
};

struct TradingStatus {
};

}

#endif
