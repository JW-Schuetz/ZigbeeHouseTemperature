function [ ndxt, ndx1, ndx2 ] = processSensorsHelper( sensorName, tab )
    ndx1  = ( string( tab.Sensorname ) == sensorName );
    ndx2  = ( string( tab2.Sensorname ) == sensorName );
    t     = [ t( ndx1 ); t( ndx2 )];
    [ ~, ndxt ] = sort( t );
end