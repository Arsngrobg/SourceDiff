
#     ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄   ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄
#    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
#    ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀
#    ▐░▌          ▐░▌       ▐░▌▐░▌          ▐░▌       ▐░▌     ▐░▌     ▐░▌          ▐░▌
#    ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌▐░▌          ▐░▌       ▐░▌     ▐░▌     ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄
#    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌          ▐░▌       ▐░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
#     ▀▀▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀█░█▀▀ ▐░▌          ▐░▌       ▐░▌     ▐░▌     ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀
#              ▐░▌▐░▌     ▐░▌  ▐░▌          ▐░▌       ▐░▌     ▐░▌     ▐░▌          ▐░▌
#     ▄▄▄▄▄▄▄▄▄█░▌▐░▌      ▐░▌ ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌ ▄▄▄▄█░█▄▄▄▄ ▐░▌          ▐░▌
#    ▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░░░░░░░░░░░▌▐░▌          ▐░▌
#     ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀   ▀▀▀▀▀▀▀▀▀▀▀  ▀            ▀
#
#    the MakeFile for SourceDiff

# PROJECT
NAME        := srcdiff
VERSION     := 1.0a
DESCRIPTION := A tool for analysing codebases using parse trees
DOCS        := https://github.com/Arsngrobg/SourceDiff\#building-from-source

# GLOBAL COMPILATION CONFIGURATION
CC          := cc
CSTD        := 11
CFLAGS      := -O2 -Wall -Wextra -Wpedantic -DNDEBUG
override CFLAGS := -std=c$(CSTD) $(CFLAGS)

# BUILD SYSTEM
BUILDROOT   ?= build
OBJROOT     := $(BUILDROOT)/obj
LIBROOT     := $(BUILDROOT)/lib
BINROOT     := $(BUILDROOT)/bin
PKGROOT     ?= $(BUILDROOT)/pkg

# SHELL UTILITIES
CP          := copy > nul
CLEAR       := cls
EXISTS      := if exist
NOTEXISTS   := if not exist

# GLOBAL DIRECTORY RECIPE
%/:
	@$(NOTEXISTS) $(subst /,\,$@) echo make: Creating new directory $(subst /,\,\$(patsubst %/,%,$@))
	@$(NOTEXISTS) $(subst /,\,$@) mkdir $(subst /,\,$@)

#     ,────────.                          ,───.  ,──.  ,──.    ,──.
#     '──.  .──',──.──. ,───.  ,───.     '   .─' `──',─'  '─.,─'  '─. ,───. ,──.──.
#        │  │   │  .──'│ .─. :│ .─. :    `.  `─. ,──.'─.  .─''─.  .─'│ .─. :│  .──'
#        │  │   │  │   ╲   ──.╲   ──.    .─'    ││  │  │  │    │  │  ╲   ──.│  │
#        `──'   `──'    `────' `────'    `─────' `──'  `──'    `──'   `────'`──'
#     Copyright (c) 2018 Max Brunsfeld
#     Custom build by James Armstrong

# SOURCES
TSROOT      := vendor/tree-sitter
TSBLACKLIST := $(TSROOT)/lib/src/lib.c $(TSROOT)/lib/src/wasm_store.c
TSSOURCES   := $(filter-out $(TSBLACKLIST),$(wildcard $(TSROOT)/lib/src/*.c))
TSOBJECTS   := $(addprefix $(OBJROOT)/tree-sitter/,$(notdir $(TSSOURCES:.c=.o)))

# COMPILATION
TSCFLAGS   := -I$(TSROOT)/lib/src -I$(TSROOT)/lib/include
TSCDEFS    := -D_POSIX_C_SOURCE=200112L -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_DARWIN_C_SOURCE # from OG Makefile
override TSCFLAGS := $(CFLAGS) $(TSCFLAGS)
override TSCDEFS  := $(CDEFS)  $(TSCDEFS)

# RECIPES
$(LIBROOT)/libtree-sitter.a: $(TSOBJECTS) | $(LIBROOT)/
	@echo make: Creating tree-sitter static library (libtree-sitter.a)
	@ar rcs $@ $^

$(OBJROOT)/tree-sitter/%.o: $(TSROOT)/lib/src/%.c | $(OBJROOT)/tree-sitter/
	@echo make: Compiling \$(subst /,\,$@)...
	@$(CC) $(TSCFLAGS) $(TSCDEFS) -c $< -o $@

#      ,──.   ,──.,──.    ,─────. ,─────.
#      │  │   `──'│  │─. '  .──./'  .──./
#      │  │   ,──.│ .─. '│  │    │  │
#      │  '──.│  ││ `─' │'  '──'╲'  '──'╲
#      `─────'`──' `───'  `─────' `─────'
#      Copyright (c) 2026 James Armstrong

include vendor/libcc/libcc.mk

#      ,───.                                     ,──────.  ,──. ,───. ,───.
#     '   .─'  ,───. ,──.,──.,──.──. ,───. ,───. │  .─.  ╲ `──'╱  .─'╱  .─'
#     `.  `─. │ .─. ││  ││  ││  .──'│ .──'│ .─. :│  │  ╲  :,──.│  `─,│  `─,
#     .─'    │' '─' ''  ''  '│  │   ╲ `──.╲   ──.│  '──'  ╱│  ││  .─'│  .─'
#     `─────'  `───'  `────' `──'    `───' `────'`───────' `──'`──'  `──'
#     Copyright (c) 2025 James Armstrong

# SOURCES
SDSOURCES   := $(wildcard src/*.c)
SDOBJECTS   := $(addprefix $(OBJROOT)/,$(notdir $(SDSOURCES:.c=.o)))
SDDEPS      := $(SDOBJECTS:.o=.d)
SDLIBS      := $(LIBROOT)/lib$(NAME).a $(LIBROOT)/libtree-sitter.a $(LIBROOT)/libcc.a

# COMPILATION
SDCFLAGS   := -MMD -MP -Isrc -I$(TSROOT)/lib/include -Ivendor/libcc
SDCDEFS    := -DSD_VERSION=\"$(VERSION)\" -DSD_DESCRIPTION="\"$(DESCRIPTION)\"" -DSD_DOCS=\"$(DOCS)\"
override SDCFLAGS := $(CFLAGS) $(SDCFLAGS)
override SDCDEFS  := $(CDEFS)  $(SDCDEFS)

# RECIPES
-include $(SDDEPS)

$(BINROOT)/$(NAME).exe: $(SDLIBS) | $(BINROOT)/
	@echo make: Compiling $(NAME).exe
	@$(CC) $(SDLIBS) -o $(BINROOT)/$(NAME).exe

$(LIBROOT)/lib$(NAME).a: $(SDOBJECTS) | $(LIBROOT)/
	@echo make: Creating $(NAME) static library (lib$(NAME).a)
	@ar rcs $@ $^

$(OBJROOT)/%.o: src/%.c | $(OBJROOT)/
	@echo make: Compiling \$(subst /,\,$@)...
	@$(CC) $(SDCFLAGS) $(SDCDEFS) -c $< -o $@

#     ,────────.              ,──.
#     '──.  .──',──,──. ,───. │  │,─.  ,───.
#        │  │  ' ,─.  │(  .─' │     ╱ (  .─'
#        │  │  ╲ '─'  │.─'  `)│  ╲  ╲ .─'  `)
#        `──'   `──`──'`────' `──'`──'`────'
#     Phony targets

.PHONY:
help:
	@echo make help          - Displays all phony targets
	@echo make pkg           - Packages SourceDiff into a distribution format
	@echo make pkg PKGROOT=? - Packages SourceDiff into the [PKGROOT] directory
	@echo make run           - Compiles (if neccessary) and runs SourceDiff
	@echo make run ARGV=?    - Compiles (if neccessary) and runs SourceDiff with the supplied arguments
	@echo make clean         - Delete all MAKE build files

pkg: $(BINROOT)/$(NAME).exe | $(PKGROOT)/licenses/
	@echo make: Packaging SourceDiff...
	@$(CP) $(subst /,\,$(BINROOT))\$(NAME).exe $(subst /,\,$(PKGROOT))
	@$(CP) LICENSE                             $(subst /,\,$(PKGROOT))\licenses\SourceDiff.txt
	@$(CP) $(subst /,\,$(TSROOT))\LICENSE      $(subst /,\,$(PKGROOT))\licenses\TreeSitter.txt
	@echo make: Done!

run: pkg
	@$(CLEAR)
	@cmd /c "cd $(subst /,\,$(PKGROOT)) && $(NAME).exe $(ARGV)"

clean:
	@$(EXISTS)    $(subst /,\,$(BUILDROOT)) echo make: Purged build directory
	@$(NOTEXISTS) $(subst /,\,$(BUILDROOT)) echo make: No build directory - nothing to do
	@$(EXISTS)    $(subst /,\,$(BUILDROOT)) rmdir /s /q $(subst /,\,$(BUILDROOT))

.DEFAULT_GOAL = pkg
