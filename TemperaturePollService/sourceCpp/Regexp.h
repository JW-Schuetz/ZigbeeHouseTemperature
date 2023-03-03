
#pragma once

#include <string>
#include <regex.h>

using namespace std;


class Regexp
{
    static const int N = 13;

    string regexp[N] =  // regexp's to be translated
    {
        "\"[0-9]{1,2}\":",                      //  0       "start sequence" of each sensor
        "\"name\":",                            //  1
        "\"config\":",                          //  2
        "\"state\":",                           //  3
        "\"battery\":",                         //  4
        "\"humidity\":",                        //  5
        "\"pressure\":",                        //  6
        "\"temperature\":",                     //  7
        "\"lastupdated\":",                     //  8
        "[0-9]{4}-[0-9]{2}-[0-9]{2}",           //  9
        "[0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3}",  // 10
        "[-]*[0-9]{1,}",                        // 11
        "[0-9a-zA-Z_-]{4,}",                    // 12   -> specification of allowed sensornames
    };

    regex_t *regex_compiled;        // translations

public:
    Regexp();
    ~Regexp();

    regex_t getCompiledRegexp( int ndx );
    void printResults( string str, int ret, regex_t regex );
};