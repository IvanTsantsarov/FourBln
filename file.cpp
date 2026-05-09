#include <cstring>
#include "file.h"


File::File()
{
    /* Base 36 conversion unit test
    char buffer[BASE36_MAX_DIGITS+1] = {0};
    ULONG number = File::fromInversedBase36("HGST2048T");
    File::toInversedBase36(number, buffer);
    printf("%s", buffer);
    */
}

File::~File()
{
    delete [] mCounters;
}


FILE * File::open(bool isReadOnly)
{
    if (FILE *file = fopen(FILE_INPUT_PATH, isReadOnly ? "r" : "wb")) {
        return file;
    }

    return nullptr;
}

// Inversed version is faster, because
// we skip the cycle to the end of the string
ULONG File::fromInversedBase36(const char *buffer)
{
    ULONG result = 0;
    ULONG degree = 1;
    while(*buffer) {
        char ch = *buffer++;
        ULONG digit = ch >= 'A' ? ch - ('A' - 10) : ch - '0';
        result += digit * degree;
        degree *= BASE36;
    }

    return result;
}


// No need to be super fast - it's just for printing
char* File::toInversedBase36(ULONG number, char* result)
{
    ULONG div = BASE36*BASE36*BASE36*
               BASE36*BASE36*BASE36*
               BASE36*BASE36*BASE36;

    int count = 0;
    int resultPos = 0;

    // skip "seroes" infront
    while(count < BASE36_MAX_DIGITS) {
        if( number > div ) {
            break;
        }
        div /= BASE36;
        count ++;
    }

    // convert it to string
    while(count++ < BASE36_MAX_DIGITS) {
        ULONG digit = number / div;
        char ch = digit > 9 ? digit + ('A' - 10) : digit + '0';
        result[resultPos++] = ch;
        number -= digit * div;
        div /= BASE36;
    }

    // and reverse that string
    count = resultPos / 2;
    for( auto i = 0; i < count; i++) {
        char swap = result[i];
        result[i] = result[--resultPos];
        result[resultPos] = swap;
    }

    return result;
}

bool File::init()
{
    FILE* file = open(true);
    if( file ) {

        if( fseek(file, 0, SEEK_END) ) {
            Msg::error( "Error big file error moving file pointer!");
            return false;
        }

        UINT fsize = ftell(file);
        if( fsize != FILE_INPUT_BYTES_SIZE ) {
            fclose(file);
            Msg::warning( string("Big file is wrong size!") + to_string(fsize) + "!=" + to_string(FILE_INPUT_BYTES_SIZE));
        }else {
            fclose(file);
            Msg::info("Big file OK.");
            return true;
        }
    }else {
        Msg::info("Big file not existing.");
    }

    file = open(false);
    if( !file) {
        Msg::error( "Error opening big file for writing!");
        return false;
    }

    printf("Generating the big file:");

    Msg::push();

    ULONG perc = 0;
    for(ULONG i = 0; i < FILE_INPUT_UINTS_COUNT; i++) {
        UINT randomValue = rand();
        const size_t sz = fwrite(&randomValue, UINT_SIZE, 1, file );
        if( !sz ) {
            Msg::info(string("Error generating big file on position:") + to_string(i));
            fclose(file);
            return false;
        }

        ULONG newPerc = i * 100 / FILE_INPUT_UINTS_COUNT;
        if( newPerc != perc) {
            Msg::pop();
            printf("%lu%%", newPerc);
            Msg::flush();
            perc = newPerc;
        }
    }

    fclose(file);
    Msg::pop();
    printf("100%%");
    Msg::info("\nGeneration done!");

    return true;
}

void File::allocateCounters()
{
    assert(!mCounters);
    mCounters = new uint8_t [BUFFER_BYTES_COUNT];
    for( auto i = 0; i < BUFFER_BYTES_COUNT; i ++) {
        mCounters[i] = 0;
    }
}

bool File::countAll()
{
    FILE* file = open(true);
    if( !file) {
        Msg::error("taskCounting: Error opening file!");
        return false;
    }

    printf("Counting values in the big file:");

    Msg::push();

    ULONG perc = 0;
    for(ULONG i = 0; i < FILE_INPUT_UINTS_COUNT; i++) {
        UINT value = 0;
        const size_t sz = fread(&value, UINT_SIZE, 1, file );
        if( !sz) {
            Msg::info(string("taskCounting:Error reading big file on position:") + to_string(i));
            fclose(file);
            return false;
        }

        uint8_t count = getBufferCount(value);
        // increment the count of the digit if less then two full bits (0x3)
        if( count < TWO_BITS ) {
            count++;
            setBufferCount(i, value);
        }

        ULONG newPerc = i * 100 / FILE_INPUT_UINTS_COUNT;
        if( newPerc != perc) {
            Msg::pop();
            printf("%lu%%", newPerc);
            Msg::flush();
            perc = newPerc;
        }
    }

    Msg::pop();
    printf("100%%");
    fclose(file);
    return true;
}

void File::freeCounters()
{
    assert(mCounters);
    delete [] mCounters;
    mCounters = nullptr;
}

bool File::countModels(const char* pathToJsonFile)
{
    Msg::info("*** Analysis task started. ***");

    FILE *file = fopen(pathToJsonFile, "rb");
    if (!file) {
        Msg::error(string("Error opening JSON file:") + pathToJsonFile);
        Msg::info("JSON file must be in the running directory of this application.");
        return false;
    }

    mMap.clear();

    // Firstly I manage to guess the models count based on the file size
    fseek(file, 0, SEEK_END);
    ULONG fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // But according to chatgpt:
    // Based on industry history, the answer is likely:
    // Tens of thousands of distinct HDD models
    // Probably somewhere around 30,000–100,000+
    // individual model numbers since 1956
    // that's 8KB of memory for the keys
    // and 4KB for the counts (12KB)
    // mMap.reserve(100000);
    // Baaat as we saw there are few models in the file
    // so we can minimize it
    mMap.reserve(1000);

    char* chunk = new char[BIG_JSON_READ_CHUNK + 1];
    memset(chunk, 0, BIG_JSON_READ_CHUNK + 1);

    ULONG bytesAll = 0;


    Msg::info(string("Reding ") + pathToJsonFile + "file:", false);
    Msg::push();

    /////////////////////////////////////////
    // Sigma to process one single HHD record.
    // Returns false if interupted before
    // reaching end of the json
    auto processRecord = [&](ULONG& offset) {
        // fast forward to the first open bracket
        while( chunk[offset] && chunk[offset] != '{' ) {
            offset ++;
        };

        ULONG offsetChunk = offset;

        if( !chunk[offset] ) {
            return false;
        }

        UINT quotesCount = 0;
        UINT quotesLeft = 0;


        char modelName[BASE36_MAX_DIGITS + 1] = {0};
        char symbol = chunk[++offsetChunk];

        while(symbol && quotesCount != 6) {

            if( symbol == '"' ) {
                quotesCount ++;
                if( quotesCount == 5 ) {
                    quotesLeft = offsetChunk;
                }
            }else
            if( quotesLeft ) {
                // copy and make it uppercase
                // (nice try with this "broken" model)
                modelName[offsetChunk - quotesLeft -1] =
                    symbol >= 'a' ? symbol - 'a' + 'A' : symbol;
            }

            symbol = chunk[++offsetChunk];
        }

        if( quotesCount != 6 ) {
            return false;
        }

        ULONG modelValue = File::fromInversedBase36(modelName);

        JsonMap::iterator i = mMap.find(modelValue);
        if( i != mMap.end() ) {
            i->second ++;
        }else {
            mMap.emplace(modelValue, 1);
        }

        offset = offsetChunk;
        // fast forward to the closing bracket
        while( chunk[offset] && chunk[offset] != '}' ) {
            offset ++;
        };

        return chunk[offset] > 0;
    };

    // Read some big chunk from the file
    // because it's too slow to read byte by byte
    ULONG skippedCount = 0;
    ULONG allBytesRed = 0;
    while(UINT bytesRed =
           fread(&chunk[skippedCount], 1, BIG_JSON_READ_CHUNK - skippedCount, file)) {

        allBytesRed += bytesRed;

        Msg::pop();
        printf("%llu%%", allBytesRed * 100ull/fsize );

        // process all records in that chunk
        ULONG offset = 0;
        while(processRecord(offset));

        // move all the unprocessed chunk content
        // in the begining of the chunk buffer
        skippedCount = BIG_JSON_READ_CHUNK - offset;
        for( ULONG i = 0; i < skippedCount; i++) {
            chunk[i] = chunk[i + offset];
        }
    };

    Msg::info("\nReading done!");
    printf("Models count=%ld\n--------------\n", mMap.size());

    for( auto i = mMap.begin(); i != mMap.end(); i++) {
        char modelName[BASE36_MAX_DIGITS+1] = {0};
        File::toInversedBase36(i->first, modelName);
        printf("%s : %u\n", modelName, i->second);
    }

    Msg::info("---------------\n3) Analysis task finished!");

    delete [] chunk;
    return true;
}
