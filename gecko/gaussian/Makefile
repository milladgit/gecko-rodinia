include ../../common/make.config

# GECKO_HOME?=$(HOME)/gecko

CC := pgc++
CCFLAGS := -Mllvm -std=c++11 -mp -acc -ta=tesla,multicore -Minfo=accel -O3 -Mcuda

# CCFLAGS += -g -O0

CCFLAGS += -I$(GECKO_HOME)/ -L$(GECKO_HOME)/lib
CCLIBS = -lgecko

SRC = gaussian.c
EXE = gaussian

release: $(SRC)
	$(CC) $(CCFLAGS) $(SRC) -o $(EXE) $(CCLIBS)

clean:
	rm gaussian
