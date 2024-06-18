TARGET := main
PRODUCTION := debug

# Change the following three variables to fit your environment
BINDADDR := 127.0.0.1
PORT := 5000
WEBHOOK_PROXY_URL := https://smee.io/oLA7lp7yRRFyB5w


CC := g++
CPPFLAGS := -std=c++20 -pedantic-errors -Wall -Wextra -DCROW_USE_BOOST
LDPATHFLAGS := -Wl,-rpath='./bin' -L./bin
LDFLAGS := -Wl,--start-group -llogger -lhandleton -lcli \
	-levent_checkers -levent_handler -Wl,--end-group

RELFLAG := -O3 -DNDEBUG
DBGFLAG := -g

BIN := bin/
SRC := src/
INCLUDE := include/ 

OBJECTS := $(BIN)main.o $(BIN)liblogger.so $(BIN)libhandleton.so \
	$(BIN)libcli.so $(BIN)libevent_checkers.so $(BIN)libevent_handler.so

ifeq ($(PRODUCTION),debug)
	CPPFLAGS += $(DBGFLAG)
else
	CPPFLAGS += $(RELFLAG)
endif

.PHONY: run runproxy clean clean_all

$(BIN)$(TARGET): $(OBJECTS)
	$(CC) $(LDPATHFLAGS) $< $(LDFLAGS) -o $@

run: $(BIN)$(TARGET)
	./$< $(BINDADDR) $(PORT)

runproxy:
	smee --url $(WEBHOOK_PROXY_URL) --path / --port $(PORT)

$(BIN)%.o: $(SRC)%.cpp
	$(CC) -c $^ -I$(INCLUDE) $(CPPFLAGS) -o $@

$(BIN)lib%.so: $(SRC)%.cpp
	$(CC) $(CPPFLAGS) -fPIC -shared -I$(INCLUDE) $< -o $@

clean:
	rm -f $(BIN)*.o $(BIN)*.out 

clean_all: clean
	rm -f $(BIN)*.so