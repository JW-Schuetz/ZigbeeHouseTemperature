function systemAnalysis()
    clc
    clear

    % time range to investigate
    start = datetime( "18.02.2023 0:00 " );
    stop  = datetime( "23.02.2023 0:00 " );

    % read data (only last days)
    days = inf;
    [ t, sensor1, sensor2 ] = readTable( days );
    if( isempty( t ) || isempty( sensor1 ) || isempty( sensor2 ) )
        'No data available!' %#ok<NOPRT>
        return
    end

    % analysis only time interval
    ndx     = isbetween( t, start, stop );
    sensor1 = sensor1( ndx, : );
    sensor2 = sensor2( ndx, : );
    t       = t( ndx, : );

    % sensor names (must be identical to Zigbee configuration)
    sensorsNames = { 'Sensor-1', 'Sensor-2' };

    % sort for sensor names
    [ sensor1, sensor2 ] = processSensors( sensorsNames, sensor1, sensor2 );

    % temperature in degres should be analysed
    temp1 = double( sensor1.Temperatur ) / 100;
    temp2 = double( sensor2.Temperatur ) / 100;
    Ts    = 15 * 60;     % Samplezeit in Sekunden

    % calculate sample time in duration s in seconds from start time t(1)
    % samples are maybe not equidistant!
    T = size( t, 1 );
    s = zeros( T, 1 );
    for m = 1 : T
        s( m ) = seconds( t( m ) - t( 1 ) );
    end

    % calculate equidistant time raster
    % duration d in seconds from first sample time to last sample time
    d      = seconds( t( end ) - t( 1 ) );
    raster = ( 0 : Ts : d )';
    temp1  = interp1( s, temp1, raster, 'linear', 'extrap' );
    temp2  = interp1( s, temp2, raster, 'linear', 'extrap' );

    % output (indoor temperature), input (outdoor temperature), sampletime
    data = iddata( temp1, temp2, Ts, 'ExperimentName', 'Mitte Februar' );

    sysL = arx( data,[ 4, 5, 2 ] );
    sysN = nlarx( data, [ 4, 5, 2 ] );
    compare( data, sysL, sysN )
end