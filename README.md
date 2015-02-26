# Helix

Helix is a ultra low latency feed handler written in C++. It normalizes market
data for use in trading applications. Helix supports connecting to multiple
market data feeds from different trading venues using their native protocols.
Helix does not have built-in networking but instead expects application to
provide raw packet data.

## Features

* [x] C API bindings
* [ ] Data normalization
* [ ] Data filtering
* [ ] Order book building
* [ ] Order book aggregation from multiple trading venues
* [ ] Synthetic NBBO
* [ ] Push and pull data delivery
