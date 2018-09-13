# name of client executable
DAE_PROGRAM = cli
# name of daemon executable
CLI_PROGRAM = dae
PROGRAMS = $(DAE_PROGRAM) $(CLI_PROGRAM)  

# name of main source(the one with main func) for client
CLI_SRC = cli.c
# name of main source(the one with main func) for daemon
DAE_SRC = dae.c
# shared functions
FUNC_SRCS =
SRCS = $(CLI_SRC) $(DAE_SRC) $(FUNC_SRCS)
# shared headers for all PROGRAMs
HEADERS = shared.h                        

# compiler used
CC = gcc
# compiler flags
CFLAGS = -O0 -ggdb -Wall -Werror
#
# 'make' or 'make all' compiles executables
# 'make clean' removes files produced when compiling

.PHONY: all clean cls
#====NO TOUCHIE TOUCHIE BELOW THIS LINE====#
OBJS = $(SRCS:.c=.o)

all: $(PROGRAMS)

# cleaning
clean cls:
	rm -f $(PROGRAMS) $(OBJS)
