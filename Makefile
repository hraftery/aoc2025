## Settings

BLD_DIR = ./build
SRC_DIR = ./src

# List all days once here. If a day does not exist it will just produce a warning.
DAYS = day01 day02

# Instead of including all sources in every build with something like
#COMMON_SRCS = $(wildcard $(SRC_UTIL_DIR)/*.c)
# each day has its own set of source files. Specify them here, relative to SRC_DIR
# and without the .c suffix.
DAY01_REQS = days/day01# util/util

CFLAGS = -Wall -Wextra -pedantic -Isrc/util


## Targets
# Targets are assembled automatically from the settings above. They are defined here in the order
# they're typically run (ie. in the reverse order they're typically required). Individual source
# files are compiled into binary files. Those binary files are linked into executables based on
# DAYn_REQS. And executables are built based on the "dayN" target specified.

all: $(DAYS)

# Provide "dayN" as an alias to "build/dayN".
$(DAYS): day%: $(BLD_DIR)/day%

# Whoa, okay, this is a double triple banger. It produces a target rule for all DAYS in BLD_DIR,
# where the prerequisites are specified by each of the DAYn_REQS variables, prepended with BLD_DIR
# and appended with ".c.o". The .SECONDEXPANSION allow $* (the stem from the static pattern rule)
# to be used as part of the variable name. The pc variable allows the % used by patsubst to also
# be delayed until $* is expanded. Ref: https://stackoverflow.com/a/25592360/3697870
.SECONDEXPANSION:
pc := %
$(BLD_DIR)/$(DAYS): $(BLD_DIR)/day%: $$(patsubst $$(pc), $(BLD_DIR)/$$(pc).o, $$(DAY$$*_REQS))
	$(CC) $^ -o $@ $(LDFLAGS)

# All c files are compiled to object files the same way
$(BLD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -R $(BLD_DIR)/*
