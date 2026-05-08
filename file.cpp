#include "file.h"

File::File()
{
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

    LONG perc = 0;
    for(LONG i = 0; i < FILE_INPUT_UINTS_COUNT; i++) {
        UINT randomValue = rand();
        const size_t sz = fwrite(&randomValue, UINT_SIZE, 1, file );
        if( !sz ) {
            Msg::info(string("Error generating big file on position:") + to_string(i));
            fclose(file);
            return false;
        }

        LONG newPerc = i * 100 / FILE_INPUT_UINTS_COUNT;
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

    LONG perc = 0;
    for(LONG i = 0; i < FILE_INPUT_UINTS_COUNT; i++) {
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

        LONG newPerc = i * 100 / FILE_INPUT_UINTS_COUNT;
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

    LONG bytesAll = 0;

    // process one single HHD record
    auto processRecord = [&](uint32_t& offset ) {
        uint32_t offsetStart = offset;
        while( chunk[offset] && chunk[offset] != '{' ) {
            offset ++;
        };

        if( !chunk[offset] ) {
            return false;
        }

        uint32_t quotesCount = 0;
        uint32_t quotesOffset = offsetStart;
        uint32_t quotesLeft = 0;

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
                modelName[quotesOffset - quotesLeft] = symbol;
            }

            symbol = chunk[quotesOffset++];
        }

        if( quotesCount != 5 ) {
            return false;
        }

        JsonMap::iterator i = mMap.find(modelName);
        if( i != mMap.end() ) {
            i->second ++;
        }else {
            mMap.emplace(modelName, 0);
        }

        return true;
    };

    // Read some big chunk from the file
    // because it's too slow to read byte by byte
    LONG offset = 0;
    while(uint32_t bytesRed =
           fread(&chunk[offset], 1, BIG_JSON_READ_CHUNK - offset, file)) {

        // process all records in that chunk
        bool isInterupted = false;
        LONG newOffset = offset;
        do {
            offset = processRecord(newOffset, isInterupted);
        } while(!isInterupted);

        LONG count = bytesRed - newOffset;
        for( LONG i = 0; i < count; i++) {
            chunk[i] = chunk[i + offset];
        }
        offset += count;


    };

    delete [] chunk;
    return true;
}
