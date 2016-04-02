#ifndef HELIX_ENDIAN_H
#define HELIX_ENDIAN_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __APPLE__

#include <libkern/OSByteOrder.h>

#define be16toh(x) OSSwapBigToHostInt16(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define htobe16(x) OSSwapHostToBigInt16(x)
#define htobe32(x) OSSwapHostToBigInt32(x)
#define htobe64(x) OSSwapHostToBigInt64(x)

#else

#include <endian.h>

#endif

#ifdef __cplusplus
}
#endif

#endif
