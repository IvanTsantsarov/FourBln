#include "includes.h"

// https://student.cs.uwaterloo.ca/~cs452/terminal.html
#define TERM_HEADER "\033[95m";
#define TERM_BLUE "\033[94m";
#define TERM_CYAN "\033[96m";
#define TERM_GREEN "\033[92m"
#define TERM_ERROR "\033[91m"
#define TERM_WARNING "\033[93m"
#define TERM_END "\033[0m"
#define TERM_BOLD "\033[1m"
#define TERM_UNDERLINE = "\033[4m"

void Msg::print(const string& msg, const string& color, bool isNewLine)
{
    string all = color + msg + TERM_END;
    if( isNewLine ) {
        printf("%s\n", all.c_str() );
    }else {
        printf("%s", all.c_str() );
    }
}

void Msg::go(int row, int col)
{
    printf("\033[%d;%dH", row, col);
}

void Msg::clrscr()
{
    printf("\033[2J");
}

void Msg::error(const string& msg, bool isNewLine)
{
    Msg::print(msg, TERM_ERROR, isNewLine);
}

void Msg::warning(const string &msg, bool isNewLine)
{
    Msg::print(msg, TERM_WARNING, isNewLine);
}

void Msg::info(const string& msg, bool isNewLine)
{
    Msg::print(msg, TERM_GREEN, isNewLine);
}

void Msg::push()
{
    printf("\033[s");
}

void Msg::pop()
{
    printf("\033[u");
}

void Msg::flush()
{
    fflush(stdout);
}
