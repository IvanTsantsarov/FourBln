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



void Msg::print(const string& msg, const string& color)
{
    string all = color + msg + TERM_END;
    printf("%s\n", all.c_str() );
}

void Msg::go(int row, int col)
{
    printf("\033[%d;%dH", row, col);
}

void Msg::clrscr()
{
    printf("\033[2J");
}

void Msg::error(const string& msg)
{
    Msg::print(msg, TERM_ERROR);
}

void Msg::warning(const string &msg)
{
    Msg::print(msg, TERM_WARNING);
}

void Msg::info(const string& msg)
{
    Msg::print(msg, TERM_GREEN);
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
