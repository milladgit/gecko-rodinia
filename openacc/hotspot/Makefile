
# GECKO_HOME?=$(HOME)/gecko

# C compiler
CC = pgc++
CC_FLAGS = -mp -acc -ta=tesla,multicore -Minfo=accel -O3 -std=c++11 -Mcuda

CC_FLAGS += -I$(GECKO_HOME)/ -L$(GECKO_HOME)/lib
CC_LIB = -lgecko


# CC_FLAGS += -g -O0

hotspot: hotspot_openacc.cpp
	$(CC) $(CC_FLAGS) $(CC_FLAGS) hotspot_openacc.cpp -o hotspot $(CC_LIB)

clean:
	rm -f hotspot
