# change the production to debug to see debug level logging
TARGET := main
PRODUCTION := REL

# Change the following three variables to fit your environment
BINDADDR := 127.0.0.1
PORT := 5000
WEBHOOK_PROXY_URL := https://smee.io/oLA7lp7yRRFyB5w


CC := g++
CPPFLAGS := -std=c++2a -pedantic-errors -Wall -Wextra -DCROW_USE_BOOST
FLAGDBG := -g
FLAGREL := -O3 -DNDEBUG

BINDBG := bin/debug/
BINREL := bin/release/

LDPATHFLAGS := -Wl,-rpath='./$(BIN$(PRODUCTION))' -L./$(BIN$(PRODUCTION))
LDFLAGS := -Wl,--start-group -llogger -lhandleton -lcli \
	-levent_checkers -levent_handler -lpthread -Wl,--end-group
SRC := src/
INCLUDE := include/ 

OBJECTS := $(BIN$(PRODUCTION))$(TARGET).o $(BIN$(PRODUCTION))liblogger.so $(BIN$(PRODUCTION))libhandleton.so \
	$(BIN$(PRODUCTION))libcli.so $(BIN$(PRODUCTION))libevent_checkers.so $(BIN$(PRODUCTION))libevent_handler.so

CPPFLAGS += $(FLAG$(PRODUCTION))

.PHONY: run runproxy rundebug clean clean_all \
	debug release

all: debug release

debug:
	$(MAKE) $(BINDBG)$(TARGET) PRODUCTION=DBG

release:
	$(MAKE) $(BINREL)$(TARGET) PRODUCTION=REL

$(BIN$(PRODUCTION))$(TARGET): $(OBJECTS)
	$(CC) $(LDPATHFLAGS) $< $(LDFLAGS) -o $@

rundebug: debug
	./$(BINDBG)$(TARGET) $(BINDADDR) $(PORT)

run: release
	./$(BINREL)$(TARGET) $(BINDADDR) $(PORT)


runproxy:
	smee --url $(WEBHOOK_PROXY_URL) --path / --port $(PORT)

$(BIN$(PRODUCTION))%.o: $(SRC)%.cpp
	$(CC) -c $^ -I$(INCLUDE) $(CPPFLAGS) -o $@

$(BIN$(PRODUCTION))lib%.so: $(SRC)%.cpp
	$(CC) $(CPPFLAGS) -fPIC -shared -I$(INCLUDE) $< -o $@

clean:
	rm -f $(BINDBG)*.o $(BINREL)*.o \
		$(BINDBG)*.out $(BINREL)*.out

clean_all: clean
	rm -f $(BINDBG)*.o $(BINREL)*.o \
		$(BINDBG)*.out $(BINREL)*.out \
		$(BINDBG)*.so $(BINREL)*.so
