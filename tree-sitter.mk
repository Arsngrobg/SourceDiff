#    ,────────.                             ,───.  ,──.  ,──.    ,──.
#    '──.  .──',──.──. ,───.  ,───. ,─────.'   .─' `──',─'  '─.,─'  '─. ,───. ,──.──.
#       │  │   │  .──'│ .─. :│ .─. :'─────'`.  `─. ,──.'─.  .─''─.  .─'│ .─. :│  .──'
#       │  │   │  │   ╲   ──.╲   ──.       .─'    ││  │  │  │    │  │  ╲   ──.│  │
#       `──'   `──'    `────' `────'       `─────' `──'  `──'    `──'   `────'`──'
#    Copyright (c) 2018 Max Brunsfeld
#    Custom build by James Armstrong (2026)

ifndef TREE-SITTER_MK
TREE-SITTER_MK := $(dir $(lastword $(MAKEFILE_LIST)))

include $(TREE-SITTER_MK)mymk.mk

# FILE PATHING
TREE-SITTER_OBJDUMP := $(OBJDUMP)/tree-sitter
TREE-SITTER_PREFIX  := vendor/tree-sitter/lib/

# COMPILER CONFIGURATION
override CFLAGS += -I$(TREE-SITTER_PREFIX)src -I$(TREE-SITTER_PREFIX)include

# FILES
TREE-SITTER_CB      := $(TREE-SITTER_PREFIX)src/lib.c
TREE-SITTER_C       := $(filter-out $(TREE-SITTER_CB),$(wildcard $(TREE-SITTER_PREFIX)src/*.c))
TREE-SITTER_O       := $(addprefix $(OBJDUMP)/tree-sitter/,$(notdir $(TREE-SITTER_C:.c=.o)))
TREE-SITTER_STATIC  := $(LIBDUMP)/libtree-sitter.a

# RECIPES
$(TREE-SITTER_STATIC): $(TREE-SITTER_O) | $(LIBDUMP)/
	@$(LOG) Creating the tree-sitter shared library (libtree-sitter.a)
	@$(AR) $(ARFLAGS) $@ $^

$(TREE-SITTER_OBJDUMP)/%.o: $(TREE-SITTER_PREFIX)src/%.c | $(TREE-SITTER_OBJDUMP)/
	@$(LOG) Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

endif # TREE-SITTER_MK
