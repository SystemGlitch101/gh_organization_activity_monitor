TARGET := main
PRODUCTION := debug

CC := g++
CPPFLAGS := -std=c++20 -pedantic-errors -Wall -Wextra -DCROW_USE_BOOST

RELFLAG := -O3 -DNDEBUG
DBGFLAG := -g

BIN := bin/
SRC := src/
INCLUDE := -Iinclude/ 


SRCS := $(wildcard $(SRC)*)
OBJECTS := $(subst $(SRC),$(BIN),$(subst .cpp,.o,$(SRCS)))

ifeq ($(PRODUCTION),debug)
	CPPFLAGS += $(DBGFLAG)
else
	CPPFLAGS += $(RELFLAG)
endif

.PHONY: run

run: $(BIN)$(TARGET).out
	./$<

$(BIN)$(TARGET).out: $(OBJECTS)
	$(CC) $^ -o $@

$(BIN)%.o: $(SRC)%.cpp
	$(CC) -c $^ $(INCLUDE) $(CPPFLAGS) -o $@

clean:
	rm -f $(BIN)*.o $(BIN)*.out 
