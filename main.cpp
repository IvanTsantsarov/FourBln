#include "file.h"


int main(int argc, char *argv[])
{
    Msg::clrscr();

    File file;

    if( !file.init() ) {
        Msg::error("Big file error!");
        return -1;
    }

    file.setCount(0, 3);
    file.setCount(1, 3);
    file.setCount(2, 3);
    file.setCount(3, 3);

    file.setCount(5, 3);
    file.setCount(6, 3);
    file.setCount(7, 3);
    file.setCount(8, 3);

    file.setCount(6, 2);


//     Msg::info("Hello big info challenge!");
//     Msg::warning("This is a warning!");
//    Msg::error("This is an error!");
}
