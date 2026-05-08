#pragma once

#include "includes.h"
#include <cassert>
#include <unordered_map>

typedef uint32_t UINT;
typedef uint64_t LONG;
#define UINT_SIZE (sizeof(UINT))

#define FILE_INPUT_PATH "fourbln.bin"

// Count of all 32 bits unsigned integers
#define FILE_INPUT_UINTS_COUNT 1000000000ull

// Count of bytes that contains all integers (multiplied by 4)
#define FILE_INPUT_BYTES_SIZE (FILE_INPUT_UINTS_COUNT<<2)

// We are gone use bits buffer to reduce memory usage
// by allocating a counter for each uint that is only 2 bits long
#define BUFFER_BITS_COUNT (FILE_INPUT_UINTS_COUNT<<1)
#define BUFFER_BYTES_COUNT (BUFFER_BITS_COUNT>>3)

#define TWO_BITS 0x3 //b00000011

#define BIG_JSON_READ_CHUNK 5196
#define BIG_JSON_MODEL_NAME_SIZE 10
class File
{
    FILE *open(bool isReadOnly);

    // buffer with 2 nots counters for each value
    uint8_t* mCounters = nullptr;

    // Internal struct
    typedef struct BufferIndices {
        LONG mByte;     // byte index in the buffer
        uint8_t mBits;  // bits index in the byte, indexed by mByte

        inline BufferIndices(LONG index) {
            // then the whole size of the buffer is shrinked 4 times
            // so to find index of the byte just delete by 4
            mByte = index >> 2;

            // then the index of the 2 bits pair in the byte
            // will be calculated by this:
            mBits = (index - (mByte << 2)) << 1;
        }
    } BufferIndicesStruct;

    // For *3) Analysis*
    // Model names are no more than 9 letters long
    // so we can use base 36 integer to represent it and
    // 9^36 = 1,015599567×10¹⁴ combinations are less
    // than maximum of 64 bits uint (2^64 = 1,844674407×10¹⁹)
    // just to skip using std::string key in the std::map
    // Also std::unordered_map has the ability to reserv memory
    // that will optimize memory alocation on insertion
    typedef std::unordered_map<LONG, uint32_t> JsonMap;
    JsonMap mMap;

public:
    File();
    ~File();
    bool init();

    // Sets a new count in the buffer
    void setBufferCount(LONG index, uint8_t value) {
        // new count must not be bigger the 3
        assert(value <= TWO_BITS);
        // calculate byte&bits indices
        BufferIndices idx(index);
        // create a bit mask with erased bits at specified index
        uint8_t mask = 0xFF ^ (TWO_BITS << idx.mBits);
        // read current byte and mask it
        uint8_t byte = mCounters[idx.mByte] & mask;
        // write new byte with altered bits
        mCounters[idx.mByte] = byte | (value << idx.mBits);
    }

    // Returs a counter from the buffer
    uint8_t getBufferCount(LONG index) const {
        BufferIndices idx(index);
        uint8_t byte = mCounters[idx.mByte];
        return static_cast<uint8_t>((byte >> idx.mBits) & TWO_BITS );
    }

    void allocateCounters();
    bool countAll();
    void freeCounters();

    bool countModels(const string& pathToJsonFile);
};


