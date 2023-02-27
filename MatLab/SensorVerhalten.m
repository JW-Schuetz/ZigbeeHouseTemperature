function SensorVerhalten()

    clc
    clear

    % Daten lesen (nur die letzten "days" Tage)
    days = 30;
    tab  = readTable( days );
    if( isempty( tab ) )
        'Keine Daten zur Auswertung vorhanden!' %#ok<NOPRT> 
        return
    end

    [ s1, s2 ] = sensorCapabilities( tab );
end

function [ s1, s2 ] = sensorCapabilities( tab )

  s1 = [];
  s2 = [];

  lastTime1 = tab( 1, : ).Sensorzeit1;
  lastTime2 = tab( 1, : ).Sensorzeit2;

  for t = 2 : height( tab )
    t1 = tab( t, : ).Sensorzeit1;
    if( lastTime1 ~= t1 )
      s.time = string( t1 );
      s.minutes = minutes( t1 - lastTime1 );
      s.t = tab( t, : ).Temperatur1;
      s.p = tab( t, : ).Luftdruck1;
      s.h = tab( t, : ).Luftfeuchtigkeit1;

      lastTime1 = t1;
      s1 = [ s1, s ]; %#ok<AGROW> 
    end

    t2 = tab( t, : ).Sensorzeit2;
    if( lastTime2 ~= t2 )
      s.time = string( t2 );
      s.minutes = minutes( t2 - lastTime2 );
      s.t = tab( t, : ).Temperatur2;
      s.p = tab( t, : ).Luftdruck2;
      s.h = tab( t, : ).Luftfeuchtigkeit2;

      lastTime2 = t2;
      s2 = [ s2, s ]; %#ok<AGROW> 
    end
  end
end