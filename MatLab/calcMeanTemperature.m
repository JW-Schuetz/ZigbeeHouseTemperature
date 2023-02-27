
function [ time, meanTempIndoor, meanTempOutdoor ] = ...
    calcMeanTemperature( t, t1, t2 )

    dt            = day( t );               % Kalendertage der Zeitmarken
    availableDays = union( [], dt );        % Menge der in t vorhandenen Kalendertage

    N = length( availableDays );

    time            = datetime;
    meanTempIndoor  = zeros( N, 1 );
    meanTempOutdoor = zeros( N, 1 );
    for n = 1 : N
        ndx = find( day( t ) == availableDays( n ) );

        meanTempIndoor( n )  = mean( t1( ndx ) );
        meanTempOutdoor( n ) = mean( t2( ndx ) );
        time( n )            = datetime( string( t( ndx( 1 ) ), "dd.MM.yyyy" ) );
    end
end