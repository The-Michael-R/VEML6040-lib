CC         := /usr/bin/gcc
CFLAGS     := -Wall -Wextra -O3
LDFLAGS    :=
DEPENDFILE := .depend

SRC := veml6040.c
OBJ := $(SRC:.c=.o)

debug:	CFLAGS += -DDEBUG -ggdb
debug:	veml6040.o

%.o:	%.c
	$(CC) -c $(CFLAGS) $^ -o $@

doc: doxygen_conf.dox $(OBJ)
	doxygen doxygen_conf.dox

clean:
	rm -rf $(BIN) $(OBJ)

.PHONY:	clean all doc
