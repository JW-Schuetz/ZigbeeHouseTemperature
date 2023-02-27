
#pragma once

#include <string>
#include <regex.h>

using namespace std;


class Regexp
{
    static const int N = 11;

    string regexp[N] =  // regexp's to be translated
    {
        "\"[0-9]{1,2}\":",                          // sensor raw data
        "\"name\":",
        "\"config\":",
        "\"state\":",
        "\"battery\":",
        "\"humidity\":",
        "\"pressure\":",
        "\"temperature\":",
        "\"lastupdated\":",
        "[0-9]{4}-[0-9]{2}-[0-9]{2}",
        "[0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3}",
    };

    regex_t *regex_compiled;        // translations

public:
    Regexp();
    ~Regexp();

    regex_t getCompiledRegexp( int ndx );
    void printResults( string str, int ret, regex_t regex );
};