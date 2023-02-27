function res = sensorInternalTime( date, time )
    y1 = string( date, "dd.MM.uuuu" );
    y2 = string( time, "hh.mm.ss.SSS" );

    res = datetime( y1 + " " + y2, 'InputFormat', 'dd.MM.uuuu HH.mm.ss.SSS' );
end