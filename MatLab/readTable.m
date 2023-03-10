function [ t, tab1, tab2 ] = readTable( days )
    % define column names and types
	names = {   'Zeitstempel', 'Sensorname1', 'Batteriezustand1', ...
                'Luftfeuchtigkeit1', 'Luftdruck1', 'Temperatur1', ...
                'Sensordatum1', 'Sensorzeit1', 'Sensorname2', ...
                'Batteriezustand2', 'Luftfeuchtigkeit2', 'Luftdruck2', ...
                'Temperatur2', 'Sensordatum2', 'Sensorzeit2'
            };
	types = {   'datetime', 'char', 'uint16', 'uint16', 'uint16', ...
                'int16', 'datetime', 'datetime', 'char', 'uint16', ...
                'uint16', 'uint16', 'int16', 'datetime', 'datetime'
            };

    t = datetime( now, 'ConvertFrom', 'datenum' );
    y = year( t );

    % all *.csv files in directory "y"
    dataDir = [ 'D:\Projekte\ZigbeeHouseTemperature\SensorValues\', sprintf( '%4d\\', y ) ];
    files = dir( [ dataDir, '*.csv' ] );

    if( isempty( files ) )
        return
    end

    opts = delimitedTextImportOptions( 'Delimiter', ',', ...
            'VariableNames', names, 'SelectedVariableNames', names, ...
            'VariableTypes', types, 'Encoding', 'UTF-8' );
    opts = setvaropts( opts, { 'Sensordatum1', 'Sensordatum2' }, ...
            'DatetimeFormat', 'yyyy-MM-dd' );
    opts = setvaropts( opts, { 'Sensorzeit1', 'Sensorzeit2' }, ...
            'DatetimeFormat', 'hh:mm:ss.SSS' );

    tab = [];
    for n = 1 : length( files )
        name = [ dataDir, files(n).name ];
        A = readtable( name, opts );

        tab = [ tab; A ]; %#ok<AGROW> 
    end

    % sort tab, increasing time stamp
    [ t, ndx ] = sort( tab.Zeitstempel );
    tab = tab( ndx, : );

    % use only last "days"
    ndx = isbetween( t, t( end ) - days, t( end ) );
    tab = tab( ndx, : );
    t   = t( ndx, : );

    % integrate date and time
    tab.Sensordatum1.Format = 'dd.MM.uuuu HH:mm';
    tab.Sensorzeit1.Format  = 'dd.MM.uuuu HH:mm';
    tab.Sensordatum2.Format = 'dd.MM.uuuu HH:mm';
    tab.Sensorzeit2.Format  = 'dd.MM.uuuu HH:mm';

    STime1 = tab.Sensordatum1 + timeofday( tab.Sensorzeit1 );
    STime2 = tab.Sensordatum2 + timeofday( tab.Sensorzeit2 );

    tab.Sensordatum1 = [];
    tab.Sensorzeit1  = [];
    tab.Sensordatum2 = [];
    tab.Sensorzeit2  = [];

    STimes = table( STime1, STime2, 'VariableNames', { 'Sensorzeit1', 'Sensorzeit2' } );

    tab = [ tab, STimes ];

    % separate sensors
    tab1 = table( tab.Sensorname1, tab.Batteriezustand1, ...
                  tab.Luftfeuchtigkeit1, tab.Luftdruck1, tab.Temperatur1, ...
                  tab.Sensorzeit1, 'VariableNames', { 'Sensorname', ...
                  'Batteriezustand', 'Luftfeuchtigkeit', 'Luftdruck', ...
                  'Temperatur', 'Sensorzeit' } );

    tab2 = table( tab.Sensorname2, tab.Batteriezustand2, ...
                  tab.Luftfeuchtigkeit2, tab.Luftdruck2, tab.Temperatur2, ...
                  tab.Sensorzeit2, 'VariableNames', { 'Sensorname', ...
                  'Batteriezustand', 'Luftfeuchtigkeit', 'Luftdruck', ...
                  'Temperatur', 'Sensorzeit' } );
end