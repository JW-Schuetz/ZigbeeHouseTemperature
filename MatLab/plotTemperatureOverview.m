function plotTemperatureOverview( t, tab1, tab2, states, minT, maxT, both )

    t1 = double( tab1.Temperatur ) / 100;
    t2 = double( tab2.Temperatur ) / 100;

    withHeatingTimes = false;

    if( ~both )
        % Mittelwert der beiden Sensoren bilden
        temp( 1, : ) = ( t1 + t2 ) / 2;
    else
        temp( 1, : ) = t1;
        temp( 2, : ) = t2;
    end

    % Anzahl der zu plottenden Temperaturkurven
    S = size( temp, 1 );

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Tagesmittelwerte
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    dt            = day( t );               % Kalendertage der Zeitmarken
    availableDays = union( [], dt );        % Menge der in t vorhandenen Kalendertage

    M = length( t );
    N = length( availableDays );

    meanTemp = zeros( S, M );
    for n = 1 : N
        % Auswahlindex für Array t und den Kalendertag n
        ndx = ( dt == availableDays( n ) );

        for s = 1 : S
            meanTemp( s, ndx ) = mean( temp( s, ndx ) );
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Plotten
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	fig = figure( 'Name', 'Temperaturen', 'WindowState', 'maximized' );

    hold on
    grid on

    yyaxis left
    ylim( [ minT, maxT ] )
    yticks( minT : 1 : maxT )

    xlabel( 'Zeit', 'FontSize', 12, 'FontWeight', 'normal' )
    ylabel( 'Temperatur [°]', 'FontSize', 12, 'FontWeight', 'normal' )

    legends = [];
    legendStr = {};

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Heizzeiten: 8:00-9:30 Uhr und 17:30-19:00 Uhr
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    if( withHeatingTimes == true )
        dy = datetime( string( t( 1 ), "dd.MM.yyyy" ) );
    
        for n = 1 : N
            d = dy + days( n - 1 );
    
            dt1 = d + hours( 8 );
            dt2 = d + hours( 9 ) + minutes( 30 );
            dt3 = d + hours( 17 ) + minutes( 30 );
            dt4 = d + hours( 19 );
    
            if( n == 1 )
                id = plot( [ dt1, dt1 ], [ minT, maxT ], 'r', 'linewidth', 1.0, ...
                           'LineStyle', '-.' );
                legendStr = [ legendStr, 'Heizung an' ]; %#ok<AGROW> 
                legends = [ legends, id ]; %#ok<AGROW> 
    
                id = plot( [ dt2, dt2 ], [ minT, maxT ], 'm', 'linewidth', 1.0, ...
                           'LineStyle', '-.' );
                legendStr = [ legendStr, 'Heizung aus' ]; %#ok<AGROW> 
                legends = [ legends, id ]; %#ok<AGROW> 
            else
                plot( [ dt1, dt1 ], [ minT, maxT ], 'r', 'linewidth', 1.0, ...
                      'linestyle', '-.' )
                plot( [ dt2, dt2 ], [ minT, maxT ], 'm', 'linewidth', 1.0, ...
                      'linestyle', '-.' )
            end
            plot( [ dt3, dt3 ], [ minT, maxT ], 'r', 'linewidth', 1.0, ...
                  'linestyle', '-.' )
            plot( [ dt4, dt4 ], [ minT, maxT ], 'm', 'linewidth', 1.0, ...
                  'linestyle', '-.' )
        end
    end

    colors = [ 'k', 'b' ];

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Temperaturen
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    for s = 1 : S
        id = plot( t, temp( s, : ), colors( s ), 'linewidth', 2, 'linestyle', '-' );

        legends = [ legends, id ]; %#ok<AGROW> 
        str = sprintf( 'Temperatur %1d', s );
        legendStr = [ legendStr, str ]; %#ok<AGROW> 
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Tagesmittelwerte
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    yyaxis right
    ylim( [ minT, maxT ] )
    yticks( minT : 1 : maxT )

    for s = 1 : S
        id = plot( t, meanTemp( s, : ), colors( s ), 'linewidth', 0.5, 'linestyle', '-' );

        legends = [ legends, id ]; %#ok<AGROW> 
        str = sprintf( 'Tagesmittelwerte %1d, %2.1f°', s, meanTemp( s, end ) );
        legendStr = [ legendStr, str ]; %#ok<AGROW> 
    end

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Zustandswechsel
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    c = { '#FF0000'; '#FF00A0'; '#FF00F0' };
    for s = 1 : size( states, 1 )
        % zyklisch durch Array c laufen
        cm = CyclicIndex( s, length( c ) );
        t0 = states{ s, 1 };
        id = plot( [ t0, t0 ], [ minT, maxT ], 'r', 'linewidth', 1.5, ...
            'LineStyle', '-.', 'marker', '.', 'color', c{ cm } );
        legends = [ legends, id ]; %#ok<AGROW> 
        legendStr = [ legendStr, states{ s, 2 } ]; %#ok<AGROW> 
    end

    legend( legends, legendStr, 'Location', 'southwest' )

    xtickangle( 45 )
    xtickformat( 'dd.MM HH:mm' )

    % MinorTicks
    ax = gca;
    tickvals = xticks;
    ax.XAxis.MinorTickValues = tickvals( 1 ) : diff( tickvals( 1 : 2 ) ) / 12  : ...
        tickvals( end );
    ax.XAxis.MinorTick = 'on';

    ax.YAxis(1).Color = 'k';
    ax.YAxis(2).Color = 'k';

    % als JPG speichern
	saveas( fig, 'Temperature.jpg' )
end