# C compiler
CC = pgc++
CC_FLAGS = -w -mp -acc -ta=tesla,multicore -Minfo=accel -O3 -std=c++11 -Mcuda

CC_FLAGS += -I$(GECKO_HOME)/ -L$(GECKO_HOME)/lib
CC_LIB = -lgecko

bfs: 
	$(CC) $(CC_FLAGS) srad.cpp -o srad $(CC_LIB)

clean:
	rm -f srad
