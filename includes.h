#pragma once

#include <cstdint>
#include <string>
#include <cstdlib>
#include <cstdio>

using namespace std;

class Msg
{
    static void print(const string& msg, const string& termFlag);
    static void go(int row, int col );

public:
    static void error(const string& msg);
    static void warning(const string& msg);
    static void info(const string& msg);

    static void push();
    static void pop();
    static void flush();

    static void clrscr();
};


using namespace std;