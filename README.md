# Helix

[![Build Status](https://travis-ci.org/penberg/helix.svg?branch=master)](https://travis-ci.org/penberg/helix)

Helix is an ultra low-latency market data feed handler written in C++. It provides an API to trading applications that normalizes market data updates from multiple feeds.

Helix core does not include networking functionality and expects applications to provide raw packet data.

## Building

### Prerequisites

* libuv 1.0 or later
* Boost libraries

**macOS**:

```
brew install libuv boost
```

### Building and Installing

To build Helix:

```
cmake .
make
```

To install Helix:

```
make install
```

Please note that Helix generates a ``pkg-config`` file so you can use ``pkg-config`` to integrate Helix with your project build system. If you installed Helix in the default location `/usr/local/`, you need to set the `PKG_CONFIG_PATH` environment variable as follows:

```
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/
```

## Usage

To convert a NASDAQ TotalView-ITCH 5.0 file to CSV:

```
./helix-trace -i 07302015.NASDAQ_ITCH50 -s AAPL -c nasdaq-binaryfile-itch50 -f csv -o AAPL.csv
```

Please note that Helix only works with uncompressed files.

## Features

### Core

* [x] C++ API
* [x] C binding
* [x] Order book view
* [x] Data normalization
* [x] Data filtering
* [x] Retransmission requests
* [ ] Order book aggregation
* [ ] Synthetic NBBO

### Protocols

* NASDAQ
  * [x] NASDAQ TotalView-ITCH 5.0
  * [x] Nordic Equity TotalView-ITCH 2.02.2
  * [x] MoldUDP
  * [x] MoldUDP64
  * [x] BinaryFILE
  * [x] SoupFILE
* Parity
  * [x] PMD

## License

Copyright © 2015 Pekka Enberg

Helix is distributed under the 2-clause BSD license. See [LICENSE](https://github.com/penberg/helix/blob/master/LICENSE) for the full license text.
