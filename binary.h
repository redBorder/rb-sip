#ifndef BINARY_READER_H
#define BINARY_READER_H

#include <stdint.h>
#include <stddef.h>

uint8_t readUint8(const uint8_t* payload, size_t offset);

uint16_t readUint16BE(const uint8_t* payload, size_t offset);

uint32_t readUint32BE(const uint8_t* payload, size_t offset);

#endif // BINARY_READER_H