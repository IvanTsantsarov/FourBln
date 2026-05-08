#include "file.h"

File::File()
{

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

    LONG perc = -1;
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
    Msg::info("\nGeneration done!");

    return true;
}
