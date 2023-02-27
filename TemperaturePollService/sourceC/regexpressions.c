
#include "sensors.h"


regex_t regex_compiled[10];


void constructRegexp()
{
  int ret;        // Ergebnisstatus
  char *reg;
  regex_t regex;  // compilierter Regulärer Ausdruck

  // sensors
  reg = "\"[0-9]{1,2}\":";
  ret = regcomp( &regex, reg, REG_EXTENDED );
  printRegExResults( "regcomp sensors", ret, regex );
  regex_compiled[0] = regex;

  // name
  reg = "\"name\":";
  ret = regcomp( &regex, reg, REG_EXTENDED );
  printRegExResults( "regcomp name", ret, regex );
  regex_compiled[1] = regex;

  // config
  reg = "\"config\":";
  ret = regcomp( &regex, reg, REG_EXTENDED );
  printRegExResults( "regcomp config", ret, regex );
  regex_compiled[2] = regex;

  // state
  reg = "\"state\":";
  ret = regcomp( &regex, reg, REG_EXTENDED );
  printRegExResults( "regcomp state", ret, regex );
  regex_compiled[3] = regex;

  // battery
  reg = "\"battery\":";
  ret = regcomp( &regex, reg, REG_EXTENDED );
  printRegExResults( "regcomp batt", ret, regex );
  regex_compiled[4] = regex;

  // humidity
  reg = "\"humidity\":";
  ret = regcomp( &regex, reg, REG_EXTENDED );
  printRegExResults( "regcomp humidity", ret, regex );
  regex_compiled[5] = regex;

  // pressure
  reg = "\"pressure\":";
  ret = regcomp( &regex, reg, REG_EXTENDED );
  printRegExResults( "regcomp pressure", ret, regex );
  regex_compiled[6] = regex;

  // temperature
  reg = "\"temperature\":";
  ret = regcomp( &regex, reg, REG_EXTENDED );
  printRegExResults( "regcomp temperature", ret, regex );
  regex_compiled[7] = regex;

  // lastupdated (Beispiel: 2023-02-06T14:20:13.697)
  reg = "\"lastupdated\":";
  ret = regcomp( &regex, reg, REG_EXTENDED );
  printRegExResults( "regcomp lastupdated", ret, regex );
  regex_compiled[8] = regex;

  // date
  reg = "[0-9]{4}-[0-9]{2}-[0-9]{2}";
  ret = regcomp( &regex, reg, REG_EXTENDED );
  printRegExResults( "regcomp date", ret, regex );
  regex_compiled[9] = regex;

  // time
  reg = "[0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3}";
  ret = regcomp( &regex, reg, REG_EXTENDED );
  printRegExResults( "regcomp time", ret, regex );
  regex_compiled[10] = regex;
}

void destructRegexp()
{
  regfree( &regex_compiled[0] );
  regfree( &regex_compiled[1] );
  regfree( &regex_compiled[2] );
  regfree( &regex_compiled[3] );
  regfree( &regex_compiled[4] );
  regfree( &regex_compiled[5] );
  regfree( &regex_compiled[6] );
  regfree( &regex_compiled[7] );
  regfree( &regex_compiled[8] );
  regfree( &regex_compiled[9] );
  regfree( &regex_compiled[10] );
}