# C compiler
CC = pgc++
CC_FLAGS = -w -mp -acc -ta=tesla,multicore -Minfo=accel -O3 -std=c++11 -Mcuda

CC_FLAGS += -I$(GECKO_HOME)/ -L$(GECKO_HOME)/lib
CC_LIB = -lgecko

needle: 
	$(CC) $(CC_FLAGS) needle.cpp -o needle $(CC_LIB)

clean:
	rm -f needle 
