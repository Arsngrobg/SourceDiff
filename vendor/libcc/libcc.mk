#      ,──.   ,──.,──.    ,─────. ,─────.
#      │  │   `──'│  │─. '  .──./'  .──./
#      │  │   ,──.│ .─. '│  │    │  │
#      │  '──.│  ││ `─' │'  '──'╲'  '──'╲
#      `─────'`──' `───'  `─────' `─────'
#
#      LibCC - lightweight C compiler invocation library

ifndef LIBCC_MK
LIBCC_MK := $(dir $(lastword $(MAKEFILE_LIST)))

include $(LIBCC_MK)mymk.mk

# RELATIVE FILE PATHING
LIBCC_PREFIX  := $(subst ./,,$(LIBCC_MK))
LIBCC_OBJDUMP := $(OBJDUMP)$(addprefix /,$(notdir $(patsubst %/,%,$(LIBCC_PREFIX))))

# COMPILER CONFIGURATION
override CFLAGS += -I$(LIBCC_PREFIX)

# FILES
LIBCC_C       := $(LIBCC_PREFIX)libcc.c
LIBCC_O       := $(LIBCC_OBJDUMP)/libcc.o
LIBCC_SHARED  := $(LIBDUMP)/libcc.$(SOEXT)
LIBCC_STATIC  := $(LIBDUMP)/libcc.a

# RECIPES
$(LIBCC_STATIC): $(LIBCC_O) | $(LIBDUMP)/
	@$(LOG) Packaging the libcc static library (libcc.a)
	@$(AR) $(ARFLAGS) $@ $<

$(LIBCC_SHARED): $(LIBCC_O) | $(LIBDUMP)/
	@$(LOG) Packaging the libcc shared library (libcc.$(SOEXT))
	@$(CC) $(SOFLAG) -o $@ $<

$(LIBCC_O): $(LIBCC_C) | $(LIBCC_OBJDUMP)/
	@$(LOG) Compiling $<
	@$(CC) $(SOFLAG) $(CFLAGS) -c $< -o $@

endif # LIBCC_MK
