CC := gcc
SRCD := src
BLDD := build
BIND := bin
INCD := include
CFLAGS := -g -O2 -Wall -Werror -DMYDEBUG

# creates object/dependency files in build for all the src c files (from my 320 hws makefile)
ALL_SRCF := $(shell find $(SRCD) -type f -name *.c)
ALL_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(ALL_SRCF:.c=.o))

$(BLDD)/%.o: $(SRCD)/%.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

INC := -I $(INCD)

# phony
.PHONY: all clean setup executables

# default target 
all: clean setupfolder $(ALL_OBJF) executables

# makes the build and bin directory
setupfolder:
	mkdir -p $(BLDD)
	mkdir -p $(BIND);

# creates executables from object files
executables:
	$(CC) $(CFLAGS) -o $(BIND)/client $(BLDD)/client.o $(BLDD)/wrapper.o
	$(CC) $(CFLAGS) -o $(BIND)/server $(BLDD)/server.o $(BLDD)/wrapper.o $(BLDD)/job.o 

# removes the build and bin folders
clean:
	-rm -rf $(BLDD) $(BIND) socket_default
	-rm *.out
	
-include $(BLDD)/*.d