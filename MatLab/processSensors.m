function [ copyTab1, tab2 ] = processSensors( sensorsNames, tab1, tab2 )
    % Ergebnisse von Sensor 1 von Tabelle 2 in Tabelle 1 kopieren
    copyTab1 = tab1;

    ndx = ( string( tab2.Sensorname ) == sensorsNames{ 1 } );   % Indizes: sensorsNames{ 1 } in tab2
    ndx = find( ndx == 1 );
    copyTab1( ndx, : ) = tab2( ndx, : );

    % Ergebnisse von Sensor 2 von Tabelle 1 in Tabelle 2 kopieren
    ndx = ( string( tab1.Sensorname ) == sensorsNames{ 2 } );   % Indizes: sensorsNames{ 2 } in tab1
    ndx = find( ndx == 1 );
    tab2( ndx, : ) = tab1( ndx, : );
end