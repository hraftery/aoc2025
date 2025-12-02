## Settings

BLD_DIR = build
SRC_DIR = src

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
# Finally, the first patsubst produces all the "BLD_DIR/dayN" targets for the static pattern rule.
.SECONDEXPANSION:
pc := %
$(patsubst %, $(BLD_DIR)/%, $(DAYS)): \
$(BLD_DIR)/day%: $$(patsubst $$(pc), $(BLD_DIR)/$$(pc).o, $$(DAY$$*_REQS))
	$(CC) $^ -o $@ $(LDFLAGS)

# All c files are compiled to object files the same way
$(BLD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Generate prereqs pregammatically. Now even header files get included as a pre-req for each
# target, so modifying any source related to the build will mark the build dirty. The sed line
# turns "file.o: src/path/src.c src/path/inc.h"
# into  "build/path/file.o build/path/file.d: src/path/src.c src/path/inc.h"
$(BLD_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,$(notdir $*)\.o[ :]*,$(BLD_DIR)/$*.o $@: ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# This whole malarky just to ensure all the .d files are read, so headers become pre-reqs.
# Haven't found a way to use the recursive targets to get all the src files, so start a new
# recursive find. Regex the find results (prepend BLD_DIR and append .d) and we have a list
# of .d files to include. Suppress not found error with "-" because the first build after a
# clean has no build directory.
SRCS := $(shell find $(SRC_DIR) -name '*.c')
DEPS := $(patsubst $(SRC_DIR)/%.c, $(BLD_DIR)/%.d, $(SRCS))
-include $(DEPS)


.PHONY: clean
clean:
	rm -R $(BLD_DIR)/*
