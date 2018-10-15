# executables names
DAE_PROGRAM = dae
CLI_PROGRAM = cli
PROGRAMS = $(CLI_PROGRAM) $(DAE_PROGRAM)

# name of main source(the one with main func) for client
CLI_SRC = cli.c
CLI_FUNCS = 
CLI_SRCS = $(CLI_FUNCS) $(CLI_SRC)
CLI_OBJS = $(CLI_SRCS:.c=.o)

DAE_SRC = dae.c
DAE_FUNCS = daef.c
DAE_SRCS = $(DAE_SRC) $(DAE_FUNCS)
DAE_OBJS = $(DAE_SRCS:.c=.o)
# shared headers for all PROGRAMs
HEADERS = shared.h                        
OBJS = $(DAE_OBJS) $(CLI_OBJS)

# compiler used
CC = gcc
# compiler flags
CFLAGS = -O0 -ggdb -Wall -Werror
#
# 'make' or 'make all' compiles executables
# 'make clean' removes files produced when compiling

.PHONY: all clean cls
#====NO TOUCHIE TOUCHIE BELOW THIS LINE====#

all: $(DAE_PROGRAM) $(CLI_PROGRAM)
	
$(DAE_PROGRAM): $(DAE_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(DAE_OBJS) -o $@

$(CLI_PROGRAM): $(CLI_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(CLI_OBJS) -o $@

# cleaning
clean cls:
	rm -f $(PROGRAMS) $(OBJS)
