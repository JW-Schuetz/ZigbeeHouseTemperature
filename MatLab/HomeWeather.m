function HomeWeather()

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

    % Verlauf von Luftfeuchte und Luftdruck plotten
    minP = 960;
    maxP = 1026;
    minH = 20;
    maxH = 130;
    both = true;

    plotWeatherOverview( t, tab1, tab2, minP, maxP, minH, maxH, both )
end