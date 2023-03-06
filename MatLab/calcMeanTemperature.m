
function [ time, meanTemp ] = calcMeanTemperature( t, t1 )

    d  = day( t );
    m  = month( t );
    y  = year( t );    
    dt = 10000 * y + 100 * m + d;

    availableDays = union( [], dt );        % Menge der in t vorhandenen Kalendertage

    N = length( availableDays );

    time     = datetime;
    meanTemp = zeros( N, 1 );

    for n = 1 : N
        ndx = find( dt == availableDays( n ) );

        meanTemp( n ) = mean( t1( ndx ) );
        time( n )     = datetime( string( t( ndx( 1 ) ), "dd.MM.yyyy" ) );
    end
end