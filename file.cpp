#include "file.h"

File::File()
{
    char buffer[BASE36_MAX_DIGITS+1] = {0};
    ULONG number = File::fromInversedBase36("HGST2048T");
    File::toInversedBase36(number, buffer);
    printf("%s", buffer);
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

    while(count++ < BASE36_MAX_DIGITS) {
        ULONG digit = number / div;
        char ch = digit > 9 ? digit + ('A' - 10) : digit + '0';
        result[resultPos++] = ch;
        number -= digit * div;
        div /= BASE36;
    }

    // and reverse it
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

bool File::countModels(const string &pathToJsonFile)
{
    FILE *file = fopen(FILE_INPUT_PATH, "r");
    if (!file) {
        Msg::error("Error opening JSON file:" + pathToJsonFile);
        return false;
    }

    // No prudent error message, please
    fseek(file, 0, SEEK_END);
    UINT fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* chunk = new char[BIG_JSON_READ_CHUNK + 1];

    ULONG bytesAll = 0;

    // process one single HHD record
    auto processRecord = [&](ULONG& offset) {
        ULONG offsetStart = offset;
        while( chunk[offset] && chunk[offset] != '{' ) {
            offset ++;
        };

        if( !chunk[offset] ) {
            return false;
        }

        UINT quotesCount = 0;
        UINT quotesOffset = offsetStart;
        UINT quotesLeft = 0;

        char modelName[BIG_JSON_MODEL_NAME_SIZE] = {0};
        char symbol = chunk[quotesOffset++];
        while(symbol && quotesCount != 5) {

            if( symbol == '"' ) {
                quotesCount ++;
                if( quotesCount == 4 ) {
                    quotesLeft = quotesOffset;
                }
            }else
            if( quotesLeft ) {
                // copy and make it uppercase
                // (nice try with this "broken" model)
                modelName[quotesOffset - quotesLeft] =
                    symbol >= 'a' ? symbol - 'a' + 'A' : symbol;
            }

            symbol = chunk[quotesOffset++];
        }

        if( quotesCount != 5 ) {
            return false;
        }

        ULONG modelValue = File::fromInversedBase36(modelName);

        JsonMap::iterator i = mMap.find(modelValue);
        if( i != mMap.end() ) {
            i->second ++;
        }else {
            mMap.emplace(modelValue, 0);
        }

        return true;
    };

    // Read some big chunk from the file
    // because it's too slow to read byte by byte
    ULONG offset = 0;
    while(UINT bytesRed =
           fread(&chunk[offset], 1, BIG_JSON_READ_CHUNK - offset, file)) {

        // process all records in that chunk
        bool isInterupted = false;
        ULONG newOffset = offset;
        while(!processRecord(newOffset));

        ULONG count = bytesRed - newOffset;
        for( ULONG i = 0; i < count; i++) {
            chunk[i] = chunk[i + offset];
        }
        offset += count;


    };

    delete [] chunk;
    return true;
}
