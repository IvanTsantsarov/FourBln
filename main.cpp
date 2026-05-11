#include "file.h"

#define FILE_4BLN_PATH "../fourbln.bin"
#define BIG_JSON_FILE_PATH "../bigf.json"

extern "C" {
    void Fizzbuzz();
}

// Not a standart lib sadly, so we need to decompress the big boy
// #include <zlib.h>

/*
*1) Counting*
As input to your program, there is a binary file containing 1 billion 32-bit integer numbers,
stored as uint32_t. The size of the file is 4,000,000,000 bytes.
а) count the unique numbers. For example:
given input -> output
0x100 0x100 0xfff 0xfff -> 2 unique numbers
0x100 0x100 0x100 0x100 -> 1 unique number
0x100 0x100 0x800 0xfff -> 3 unique numbers
*/
void taskCountingA(const File& file)
{
    ULONG uniquesCount = 0;
    for(ULONG i = 0; i < FILE_4BLN_UINTS_COUNT; i++) {
        if( file.getBufferCount(i) > 0 ) {
            uniquesCount ++;
        }
    }

    Msg::info(string("\nTask Counting a) Uniquest count =") + to_string(uniquesCount));
}


/*
 * b) count how many numbers are seen ONLY once. For example:
given input -> output
0x100 0x100 0xfff 0xfff -> 0 numbers seen only once
0x100 0x100 0x100 0x100 -> 0 numbers seen only once
0x100 0x100 0x800 0xfff -> 2 numbers seen only once
*/
void taskCountingB(const File& file)
{
    ULONG onceSeenCount = 0;
    for(ULONG i = 0; i < FILE_4BLN_UINTS_COUNT; i++) {
        if( file.getBufferCount(i) == 1 ) {
            onceSeenCount ++;
        }
    }

    Msg::info(string("Task Counting b) Seen once count =") + to_string(onceSeenCount));
}


/*
Fizzbuzz*
The standard fizzbuzz task, with a catch:
Write a program that outputs the numbers from 1 to 100. If the number is divisible by 3, instead
of it print “A”, if it’s divisible by 5 - “B”, if it’s divisible by 15 - “AB”. The catch is to write it without
any conditionals/ifs, including for/while loops.
*/
void taskFizzbuzz()
{
    Msg::info("*** Fizzbuzz ***");
    Fizzbuzz();
    Msg::info("Fizzbuzz finished!");
}

int main(int argc, char *argv[])
{
    taskFizzbuzz();
    /*
    Msg::clrscr();

    File file;

    /////////////////////////////////
    ///// Task Counting
    /////////////////////////////////
    ///

    Msg::info("*** Task counting ***");

    if( !file.init(FILE_4BLN_PATH) ) {
        Msg::error("4bln file error!");
        return -1;
    }

    file.allocateCounters();

    if( !file.countAll() ) {
        Msg::error("4bln file error!");
        return -1;
    }

    taskCountingA(file);

    taskCountingB(file);

    file.freeCounters();

    /////////////////////////////////
    ///// Task Fuzzbuzz
    /////////////////////////////////

    taskFizzbuzz();

    /////////////////////////////////
    ///// Task Analisys
    /////////////////////////////////


    file.countModels(BIG_JSON_FILE_PATH);
*/
}
