# C compiler
CC = pgc++
CC_FLAGS = -w -mp -acc -ta=tesla,multicore -Minfo=accel -O3 -std=c++11 -Mcuda

# CC_FLAGS+=-g 

CC_FLAGS += -I$(GECKO_HOME)/ -L$(GECKO_HOME)/lib
CC_LIB = -lgecko


bfs: bfs.cpp
	$(CC) $(CC_FLAGS) bfs.cpp -o bfs $(CC_LIB)

clean:
	rm -f bfs result.txt
