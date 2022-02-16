#
# Organization of sources.
#

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)

# Inclusion from the package level.
CCFLAGS += -I..

#
# Makefile rules.
#

# Compilation.
all: $(OBJ)

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@
	
# Clean.
.PHONY: clean

clean:
	@rm -f $(OBJ) $(DEP)

-include $(DEP)

