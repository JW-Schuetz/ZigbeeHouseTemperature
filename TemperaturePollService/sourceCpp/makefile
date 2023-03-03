 
CC = g++
CFLAGS = -Wall -g -ggdb -std=c++20
LNK = -lc -lcurl -lpthread

BIN = zigbeesensors
OBJ = ZigbeeSensors.o Manager.o Parser.o Regexp.o Sensor.o

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LNK)

%.o: %.cpp	makefile
			${CC} ${CFLAGS} -c $<

ZigbeeSensors.o: Manager.h
Manager.o: Manager.h Credentials.h Sensor.h
Parser.o: Parser.h Regexp.h
Regexp.o: Regexp.h Manager.h
Sensor.o: Sensor.h Parser.h

clean:
		rm -f $(OBJ); rm -f $(BIN)