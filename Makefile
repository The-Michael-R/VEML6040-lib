CC         := /usr/bin/gcc
CFLAGS     := -I./inc -Wall -O3 -pthread
LDFLAGS    := -I./inc
DEPENDFILE := .depend

SRC := ./src/rgbw-logger.c ./inc/veml6040.c
OBJ := $(SRC:.c=.o)
BIN := rgbw-logger

all:	veml6040_inc rgbw-logger

debug:	CFLAGS += -DDEBUG -ggdb
debug:	veml6040_inc_dbg rgbw-logger

rgbw-logger:	$(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

veml6040_inc:
	cd inc && make veml6040.o

veml6040_inc_dbg:
	cd inc && make debug

doc: doxygen_conf.dox $(OBJ)
	doxygen doxygen_conf.dox

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	cd inc && make clean
	rm -rf $(BIN) $(OBJ)

.PHONY:	clean all
