 
CC = g++
LNK = -lc -lcurl
CFLAGS = -Wall -g -ggdb -std=c++20

BIN = zigbeesensors
OBJ = ZigbeeSensors.o Manager.o Parser.o Regexp.o Sensor.o WeatherSensor.o

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LNK)

%.o: %.cpp	makefile
			${CC} ${CFLAGS} -c $<

ZigbeeSensors.o: Manager.h
Manager.o: Manager.h Credentials.h WeatherSensor.h
Parser.o: Parser.h Regexp.h
Regexp.o: Regexp.h Manager.h
Sensor.o: Sensor.h Parser.h
WeatherSensor.o: WeatherSensor.h Sensor.h

clean:
		rm -f $(OBJ); rm -f $(BIN)