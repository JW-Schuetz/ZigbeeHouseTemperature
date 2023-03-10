function HomeTemperature()

    clc
    clear

    % Daten lesen (nur die letzten "days" Tage)
    days = 30;
    [ t, tab1, tab2 ] = readTable( days );
    if( isempty( t ) || isempty( tab1 ) || isempty( tab2 ) )
        'Keine Daten zur Auswertung vorhanden!' %#ok<NOPRT>
        return
    end

    % Namensfestlegung für die Sensoren (muss mit ZigBee-Konfiguration matchen)
    sensorsNames = { 'Sensor-1', 'Sensor-2' };

    % die Sensoren verteilen sich evtl. auf beide Tabellen -> entzerren
    [ tab1, tab2 ] = processSensors( sensorsNames, tab1, tab2 );

    % Heizperioden: 8:00 - 9:30 Uhr und 17:30 - 19:00 Uhr

    % Zustandswechsel der Heizungssteuerung
    states = [ ...
        { datetime( '01.02.2023 11:00' ), '"Sonne":   0, "Mond":  0' };
        { datetime( '01.02.2023 22:26' ), '"Sonne":   0, "Mond":  1' };
        { datetime( '02.02.2023  9:10' ), '"Sonne":   0, "Mond":  2' };
        { datetime( '03.02.2023  1:05' ), '"Sonne":   0, "Mond":  1' };
        { datetime( '05.02.2023 10:42' ), '"Sonne":   0, "Mond":  0' };
        { datetime( '06.02.2023 10:40' ), '"Sonne":   0, "Mond": -1' };
        { datetime( '06.02.2023 15:30' ), '"Sonne":   0, "Mond": -2' };
        { datetime( '07.02.2023 10:34' ), '"Sonne":   0, "Mond": -3' };
        { datetime( '07.02.2023 12:45' ), '"Sonne":   0, "Mond": -4' };
        { datetime( '08.02.2023  8:35' ), '"Sonne":   0, "Mond": -2' };
        { datetime( '08.02.2023 11:00' ), '"Sonne":   0, "Mond": -3' };
        { datetime( '09.02.2023 11:00' ), '"Sonne":  -1, "Mond": -2' };
        { datetime( '10.02.2023 11:00' ), '"Sonne":  -2, "Mond": -2' };
        { datetime( '11.02.2023  9:45' ), '"Sonne":  -2, "Mond": -3' };
        { datetime( '12.02.2023 11:03' ), '"Sonne":  -2, "Mond": -1' };
        { datetime( '13.02.2023  9:46' ), '"Sonne":  -2, "Mond":  0' };
        { datetime( '14.02.2023 11:14' ), '"Sonne":  -2, "Mond": -1' };
        { datetime( '15.02.2023 11:08' ), '"Sonne":  -2, "Mond": -2' };
        { datetime( '16.02.2023 13:08' ), '"Sonne":  -3, "Mond": -3' };
        { datetime( '17.02.2023  9:30' ), '"Sonne":  -2, "Mond": -3' };
        { datetime( '24.02.2023 10:17' ), '"Sonne":  -1, "Mond": -2' };
        { datetime( '03.03.2023  0:35' ), '"Sonne":  -3, "Mond": -3' };
        { datetime( '06.03.2023 10:10' ), '"Sonne":  -2, "Mond": -2' };
        { datetime( '09.03.2023  9:17' ), '"Sonne":  -1, "Mond": -1' };
        { datetime( '10.03.2023  9:22' ), '"Sonne":   0, "Mond":  0' };
    ];

    % zu alte "states" ausblenden
    tm     = states( :, 1 );
    tm     = cellfun( @datetime, tm );
    ndx    = isbetween( tm, t( end ) - days, t( end ) );
    states = states( ndx, : );

    % Verlauf der Temperatur plotten
    minT = -10;
    maxT = 24;
    both = true;

    plotTemperatureOverview( t, tab1, tab2, states, minT, maxT, both )
%     plotMeanTemperatures( t, tab1, tab2, minT, maxT, both )
end