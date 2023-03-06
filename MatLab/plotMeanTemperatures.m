function plotMeanTemperatures( t, tab1, tab2, minT, maxT, both  )

    t1 = double( tab1.Temperatur ) / 100;
    t2 = double( tab2.Temperatur ) / 100;

    [ time, meanTemp1 ] = calcMeanTemperature( t, t1 );
    [ ~,    meanTemp2 ] = calcMeanTemperature( t, t2 );

    if( ~both )
        meanTemp( 1, : ) = ( meanTemp1 + meanTemp2 ) / 2;
    else
        meanTemp( 1, : ) = meanTemp1;
        meanTemp( 2, : ) = meanTemp2;
    end

    % Anzahl der zu plottenden Temperaturkurven
    S = size( meanTemp, 1 );

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Temperaturmittelwerte
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	fig = figure( 'Name', 'Temperatur-Mittelwerte', 'WindowState', 'maximized' );

    hold on
    grid on

    colors = [ 'k', 'b' ];

    ylim( [ minT, maxT ] )
    xlabel( 'Zeit', 'FontSize', 12, 'FontWeight', 'normal' )
    ylabel( 'Tagesmittelwerte Temperatur [°]', 'FontSize', 12, 'FontWeight', 'normal' )

    % Plotten
    legends   = [];
    legendStr = {};

    for s = 1 : S
        id = plot( time, meanTemp( s, : ), colors( s ), 'linewidth', 1.0, ...
            'Marker','o', 'MarkerFaceColor', 'red' );

        legends = [ legends, id ]; %#ok<AGROW> 
        str = sprintf( 'Temperatur %1d', s );
        legendStr = [ legendStr, str ]; %#ok<AGROW> 
    end

    legend( legends, legendStr, 'Location', 'best' )

    xtickangle( 45 )
    xtickformat( 'dd.MM HH:mm' )
    yticks( minT : 2 : maxT )

    % MinorTicks
    ax = gca;
    ax.XAxis.MinorTick = 'on';
    tickvals = xticks;
    ax.XAxis.MinorTickValues = tickvals( 1 ) : diff( tickvals( 1 : 2 ) ) / 12  : ...
        tickvals( end );

    ax.YAxis.MinorTick = 'on';
    ax.YAxis.MinorTickValues = ( minT + 1 ) : 2 : maxT;

    % als JPG speichern
	saveas( fig, 'Temperatur-Mittelwerte.jpg' )
end