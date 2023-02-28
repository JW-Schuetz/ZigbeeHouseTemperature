#
# LINUX program for polling the actual sensor values.
# Is still running on my RasBerry-Pi.
#

CC  = gcc
LNK = -lc -lcurl -lrt
OBJ = sensors.o dopoll.o parsevirtsensors.o writedata.o timerhandling.o\
      exitfun.o setopt.o signalhandler.o\
      printRegExResults.o sensorsRawDataStrings.o sensorsNames.o\
	  parseHelper.o regexpressions.c

BIN = sensors

%.o: %.c	sensors.h makefile
			${CC} -ggdb -c $<

$(BIN):		$(OBJ)
			${CC} $(OBJ) -o $(BIN) ${LNK}

clean:
		rm -f *.o; rm -f $(BIN)
