
CC = pgc++
SRC = pathfinder.cpp
EXE = pathfinder
FLAGS =-Mllvm -w -mp -acc -ta=tesla,multicore -Minfo=accel -O3 -std=c++11 -Mcuda

FLAGS += -I$(GECKO_HOME)/ -L$(GECKO_HOME)/lib
CC_LIB = -lgecko


all: release

release:
	$(CC) $(SRC) $(FLAGS) -o $(EXE) $(CC_LIB)

debug:
	$(CC) $(SRC) -g -Wall -o $(EXE) $(CC_LIB)

clean:
	rm -f pathfinder out


