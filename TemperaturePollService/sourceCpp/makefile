 
CC = g++
CFLAGS = -Wall -g -ggdb -std=c++20
LNK = -lc -lcurl -lpthread

BIN = sensors
OBJ = Main.o Sensors.o Sensor.o Regexp.o Parser.o

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LNK)

%.o: %.cpp	makefile
			${CC} ${CFLAGS} -c $<

Main.o: Sensors.h
Sensors.o: Sensors.h Credentials.h Parser.h
Sensor.o: Sensor.h
Parser.o: Parser.h Regexp.h
Regexp.o: Regexp.h Sensors.h

clean:
		rm -f $(OBJ); rm -f $(BIN)