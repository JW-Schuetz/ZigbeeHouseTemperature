function systemAnalysis()
    clc
    clear
    
    % read data (only last days)
    days = inf;
    [ t, sensor1, sensor2 ] = readTable( days );
    if( isempty( t ) || isempty( sensor1 ) || isempty( sensor2 ) )
        'No data available!' %#ok<NOPRT>
        return
    end

    % analysis only time interval
    ndx = isbetween( t, datetime( "18.02.2023 0:00 " ), datetime( "23.02.2023 0:00 " ) );
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
    Ts    = 15;     % Samplezeit in Sekunden

    % output (indoor temperature), input (outdoor temperature), sampletime
    data = iddata( temp1, temp2, Ts, 'ExperimentName', 'Mitte Februar' );

    % advice( data )
    sys = idnlarx( 'indoor', 'outdoor', [ 4, 4, 1 ] );
    disp(sys.OutputFcn)
end