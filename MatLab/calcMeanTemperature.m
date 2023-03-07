function [ time, meanTemp ] = calcMeanTemperature( t, t1, mode )
% Calculates the mean temperature of t1( t ) for each calender day in two
% flavours:
% 1.) one mean for each day together with the days date (mode == 0) 
% 2.) the mean value of the day for each temperature sample (mode ~= 0) 
    d  = day( t );
    m  = month( t );
    y  = year( t );    
    dt = 10000 * y + 100 * m + d;

    availableDays = union( [], dt );        % unique set of days samples

    time = datetime;
    N    = length( availableDays );

    if( mode == 0 )
        meanTemp = zeros( N, 1 );
    else
        M = length( t );
        meanTemp = zeros( M, 1 );
    end

    for n = 1 : N
        ndx = find( dt == availableDays( n ) );

        if( mode == 0 )
            meanTemp( n )   = mean( t1( ndx ) );
            time( n )        = datetime( string( t( ndx( 1 ) ), "dd.MM.yyyy" ) );
        else
            meanTemp( ndx ) = mean( t1( ndx ) );
        end
    end
end