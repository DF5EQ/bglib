# name of the program to build
PROGNAME := BGLib_scanner

# lists of files
SRCS := $(shell find ../.. -name "*.c") # list of all C files
OBJS := $(SRCS:.c=.o)                   # list of all object files
DEPS := $(SRCS:.c=.d)                   # list of all dependencies files
DIRS := $(dir $(SRCS))                  # directories of all C files

# command and flags for dependencies
DP      = gcc -MM
DPFLAGS = -MG -MT $(basename $<).o -MT $(basename $<).d
# command and flags for C Compiler
CC      = gcc -c
CFLAGS  =
# command and flags for asembler
AS      =
ASFLAGS =
# command and flags for linker
LD      = gcc
LDFLAGS = -export-dynamic -no-pie

# include and library directories
INCS    = $(foreach dir, $(DIRS), -I$(dir))
LIBS    = -lrt

# search path for targets and prerequisites
VPATH = $(DIRS)

# set only the needed suffixes
.SUFFIXES:
.SUFFIXES: .c .d .o

# implicite rule for making dependencies file from C file
%.d: %.c
	@echo "makefile: "$@
	@$(DP) $(DPFLAGS) $(INCS) -o $@ $< 

# implicite rule for making object file from C file
%.o: %.c
	@echo "makefile: "$@
	@$(CC) $(CFLAGS) $(INCS) -o $@ $<

# rule for making the target program
.PHONY: all
all: $(PROGNAME)
	@echo "makefile: all done"
	
# rule for making the program from object files
$(PROGNAME): $(OBJS)
	@echo "makefile: "$@
	@$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

# target and rule for cleaning up the directories
.PHONY: clean
clean:
	@echo "makefile: remove .d files"
	@rm -f $(DEPS)
	@echo "makefile: remove .o files"
	@rm -f $(OBJS)
	@echo "makefile: remove program"
	@rm -f $(PROGNAME)

# target and rule for running the program
.PHONY: run
run: all
	@mate-terminal -e ./$(PROGNAME)

.PHONY: test
test:
	@echo "makefile: PROGNAME = $(PROGNAME)"
	@echo "makefile: SRCS     = $(SRCS)"
	@echo "makefile: OBJS     = $(OBJS)"
	@echo "makefile: DEPS     = $(DEPS)"
	@echo "makefile: DIRS     = $(DIRS)"
	@echo "makefile: DP       = $(DP)"
	@echo "makefile: DPFLAGS  = $(DPFLAGS)"
	@echo "makefile: CC       = $(CC)"
	@echo "makefile: CFLAGS   = $(CFLAGS)"
	@echo "makefile: LD       = $(LD)"
	@echo "makefile: LDFLAGS  = $(LDFLAGS)"
	@echo "makefile: INCS     = $(INCS)"
	@echo "makefile: LIBS     = $(LIBS)"

# append all dependencies files
include $(DEPS)


