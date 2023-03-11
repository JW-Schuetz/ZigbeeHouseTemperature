 
CC = g++
LNK = -lc -lcurl
CFLAGS = -Wall -std=c++20 #-g -ggdb

BIN = zigbeesensors
OBJ = ZigbeeSensors.o Manager.o SendFileHandle.o Parser.o Regexp.o Sensor.o WeatherSensor.o

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LNK)

%.o: %.cpp	makefile
			${CC} ${CFLAGS} -c $<

ZigbeeSensors.o: Manager.h
Manager.o: Manager.h Credentials.h SendFileHandle.h WeatherSensor.h
SendFileHandle.o: SendFileHandle.h Credentials.h
Parser.o: Parser.h Regexp.h
Regexp.o: Regexp.h Manager.h
Sensor.o: Sensor.h Parser.h
WeatherSensor.o: WeatherSensor.h Sensor.h

clean:
		rm -f $(OBJ); rm -f $(BIN)