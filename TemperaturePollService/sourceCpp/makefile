 
CC = g++
LNK = -lc -lcurl
CFLAGS = -Wall -std=c++20 #-g -ggdb

BIN = zigbeesensors
OBJ = ZigbeeSensors.o Manager.o Curl.o NAS.o ZigbeeBridge.o\
		Parser.o Regexp.o ZigbeeSensor.o ZigbeeWeatherSensor.o

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LNK)

%.o: %.cpp	makefile
			${CC} ${CFLAGS} -c $<

ZigbeeSensors.o: Manager.h
Manager.o: Manager.h Curl.h Credentials.h NAS.h ZigbeeWeatherSensor.h
Curl.o: Curl.h CurlStaticLib.h
NAS.o: NAS.h Credentials.h CurlStaticLib.h Curl.h
ZigbeeBridge.o: ZigbeeBridge.h Credentials.h CurlStaticLib.h Curl.h
Parser.o: Parser.h Regexp.h
Regexp.o: Regexp.h Manager.h
ZigbeeSensor.o: ZigbeeSensor.h Parser.h
ZigbeeWeatherSensor.o: ZigbeeWeatherSensor.h ZigbeeSensor.h

clean:
		rm -f $(OBJ); rm -f $(BIN)

remake:
		make clean
		make

valgrind:
		valgrind --leak-check=full ./zigbeesensors