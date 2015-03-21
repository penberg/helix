# Helix

Helix is an ultra low latency feed handler written in C++ that
normalizes market data updates for use in trading applications. Helix
supports connecting to multiple market data feeds from different trading
venues using their native protocols. Helix core does not include
networking functionality and expects applications to provide raw packet
data.

## Features

### Core

* [x] C++ API
* [x] C binding
* [x] Order book view
* [x] Data normalization
* [x] Data filtering
* [ ] Order book aggregation
* [ ] Synthetic NBBO
* [ ] Retransmission requests

### Protocols

* [x] MoldUDP
* [x] NASDAQ OMX Nordic Equity Total-View ITCH
* [ ] BATS Chi-X Europe Multicast PITCH
