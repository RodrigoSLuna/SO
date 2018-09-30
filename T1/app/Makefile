
VERSION = 1.0
#-------------------------------------------------------------
# the rest of the file should not change
INC = ../include
SRC = ../src
OBJ = ../obj
BIN = ../bin
APP = ../app



PROCESS 	=	Process.cpp
MONITOR 	=	Monitor.cpp
SCHEDULER	=	Scheduler.cpp
MANAGER 	=	Manager.cpp

PRO  = process
#MON  = monitor
#SCHE = Scheduler
MAN = manager

CC = g++ -std=c++11
CFLAGS = -I$(INC)
LFLAGS = -lpthread

RM = rm


#all:  $(OBJ)/mysocket.o $(BIN)/$(SRV) $(BIN)/$(PAS)

#$(OBJ)/mysocket.o: $(SRC)/mysocket.cpp $(INC)/mysocket.h $(INC)/structs.h
#	$(CC) -c $(CFLAGS) -o $@ $(SRC)/mysocket.cpp

# make all


# $(BIN)/$(SRV): $(OBJ)/mysocket.o $(SERVER)
# 	$(CC) $(CFLAGS) -o $@ $(OBJ)/mysocket.o $(SERVER) $(LFLAGS)
# all: manager
# manager: $(OBJ)/scheduler.o $(OBJ)/monitor.o $(OBJ)/process.o: #$(BIN)/$(MAN)
	# g++ -std=c++11 -o saida 

# $(BIN)/$(MAN): $(OBJ)/process.o $(MANAGER)
	# $(cc) $(CFLAGS) -o $@ $(OBJ)/process.o $(MANAGER) $(LFLAGS)
all: $(BIN)/scheduler 

$(BIN)/scheduler: $(SRC)/Scheduler.cpp 
	g++  -std=c++11  -o $(BIN)/scheduler $(SRC)/Scheduler.cpp $(LFLAGS)

version:
	@echo $(VERSION) "(Draft Version!)"
# clean
clean:
	$(RM) -rf $(OBJ)/*.o  $(BIN)/*
