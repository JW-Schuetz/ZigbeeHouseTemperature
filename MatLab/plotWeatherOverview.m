function plotWeatherOverview( t, tab1, tab2, pMin, pMax, hMin, hMax, both )

    p1 = double( tab1.Luftdruck );
    p2 = double( tab2.Luftdruck );
    h1 = double( tab1.Luftfeuchtigkeit ) / 100;
    h2 = double( tab2.Luftfeuchtigkeit ) / 100;

    if( ~both )
        % Mittelwert der beiden Sensoren bilden
        d( 1, : ) = ( p1 + p2 ) / 2;
        f( 1, : ) = ( h1 + h2 ) / 2;
    else
%         d( 1, : ) = p1;
%         d( 2, : ) = p2;
        d( 1, : ) = ( p1 + p2 ) / 2;

        f( 1, : ) = h1;
        f( 2, : ) = h2;
    end

    % Anzahl der zu plottenden Luftdruckkurven
    dS = size( d, 1 );
    fS = size( f, 1 );

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % plotten
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	fig = figure( 'Name', 'Wetterdaten', 'WindowState', 'maximized' );

    hold on
    grid on

    colorsD = [ 'k', 'b' ];
    colorsF = [ 'r', 'm' ];

    legends = [];
    legendStr = {};

    xlabel( 'Zeit', 'FontSize', 12, 'FontWeight', 'normal' )

    yyaxis left

    ylim( [ pMin, pMax ] )
    ylabel( 'Luftdruck [hPa]', 'FontSize', 12, 'FontWeight', 'normal' )

    for s = 1 : dS
        id = plot( t, d( s, : ), colorsD( s ), 'linewidth', 2, 'linestyle', '-' );

        if( dS == 1 )
            str = sprintf( 'Gemittelter Luftdruck' );
        else
            str = sprintf( 'Luftdruck %1d', s );
        end
        legendStr = [ legendStr, str ]; %#ok<AGROW> 
        legends = [ legends, id ]; %#ok<AGROW> 
    end

	yyaxis right

    ylim( [ hMin, hMax ] )
    ylabel( 'Luftfeuchtigkeit [°]', 'FontSize', 12, 'FontWeight', 'normal' )

    for s = 1 : fS
        id = plot( t, f( s, : ), colorsF( s ), 'linewidth', 2, 'linestyle', '-' );

        if( fS == 1 )
            str = sprintf( 'Gemittelte Luftfeuchtigkeit' );
        else
            str = sprintf( 'Luftfeuchtigkeit %1d', s );
        end
        legendStr = [ legendStr, str ]; %#ok<AGROW> 
        legends = [ legends, id ]; %#ok<AGROW> 
    end

    legend( legends, legendStr, 'Location', 'north' )

    xtickangle( 45 )
    xtickformat( 'dd.MM HH:mm' )

    % Farben und Ticks der beiden Axen
    ax = gca;
    ax.XAxis.MinorTick = 'on';
    tickvals = xticks;
    ax.XAxis.MinorTickValues = tickvals( 1 ) : diff( tickvals( 1 : 2 ) ) / 12  : ...
        tickvals( end );

    ax.YAxis( 1 ).Color = 'k';
    ax.YAxis( 1 ).TickValues = pMin : 10 : pMax;
    ax.YAxis( 1 ).MinorTick = 'on';

    ax.YAxis( 2 ).Color = 'r';
    ax.YAxis( 2 ).MinorTick = 'on';
    ax.YAxis( 2 ).TickValues = hMin : 10 : hMax;
    tickvals = yticks;
    ax.YAxis(2).MinorTickValues = tickvals( 1 ) : diff( tickvals( 1 : 2 ) ) / 5  : ...
        tickvals( end );

    % als JPG speichern
	saveas( fig, 'Wetterdaten.jpg' )
end