
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

# GLOBAL COMPILATION CONFIGURATION
CC          := cc
CSTD        := 11
CCFLAGS     := -O2 -Wall -Wextra -Wpedantic
CCDEFS      := -DNDEBUG
override CCFLAGS := -std=c$(CSTD) $(CCFLAGS)

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
	@$(NOTEXISTS) $(subst /,\,$@) echo [Make] Creating new directory $(subst /,\,\$(patsubst %/,%,$@))
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
TSCCFLAGS   := -I$(TSROOT)/lib/src -I$(TSROOT)/lib/include
TSCCDEFS    := -D_POSIX_C_SOURCE=200112L -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_DARWIN_C_SOURCE # from OG Makefile
override TSCCFLAGS := $(CCFLAGS) $(TSCCFLAGS)
override TSCCDEFS  := $(CCDEFS)  $(TSCCDEFS)

# RECIPES
$(LIBROOT)/libtree-sitter.a: $(TSOBJECTS) | $(LIBROOT)/
	@echo [Make] Creating tree-sitter static library (libtree-sitter.a)
	@ar rcs $@ $^

$(OBJROOT)/tree-sitter/%.o: $(TSROOT)/lib/src/%.c | $(OBJROOT)/tree-sitter/
	@echo [Make] Compiling \$(subst /,\,$@)...
	@$(CC) $(TSCCFLAGS) $(TSCCDEFS) -c $< -o $@

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
SDLIBS      := $(LIBROOT)/lib$(NAME).a $(LIBROOT)/libtree-sitter.a

# COMPILATION
SDCCFLAGS   := -MMD -MP -Isrc -I$(TSROOT)/lib/include
SDCCDEFS    := -DSD_VERSION=\"$(VERSION)\" -DSD_DESCRIPTION="\"$(DESCRIPTION)\"" -DSD_REPO=\"https://github.com/Arsngrobg/SourceDiff\"
override SDCCFLAGS := $(CCFLAGS) $(SDCCFLAGS)
override SDCCDEFS  := $(CCDEFS)  $(SDCCDEFS)
-include $(SDDEPS)

# RECIPES
$(BINROOT)/$(NAME).exe: $(SDLIBS) | $(BINROOT)/
	@echo [Make] Compiling $(NAME).exe
	@$(CC) $(SDLIBS) -o $(BINROOT)/$(NAME).exe

$(LIBROOT)/lib$(NAME).a: $(SDOBJECTS) | $(LIBROOT)/
	@echo [Make] Creating $(NAME) static library (lib$(NAME).a)
	@ar rcs $@ $^

$(OBJROOT)/%.o: src/%.c | $(OBJROOT)/
	@echo [Make] Compiling \$(subst /,\,$@)...
	@$(CC) $(SDCCFLAGS) $(SDCCDEFS) -c $< -o $@

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
	@echo make run RUNARGS=? - Compiles (if neccessary) and runs SourceDiff with the supplied arguments
	@echo make clean         - Delete all MAKE build files

pkg: $(BINROOT)/$(NAME).exe | $(PKGROOT)/licenses/
	@echo [Make] Packaging SourceDiff...
	@$(CP) $(subst /,\,$(BINROOT))\$(NAME).exe $(subst /,\,$(PKGROOT))
	@$(CP) LICENSE                             $(subst /,\,$(PKGROOT))\licenses\SourceDiff.txt
	@$(CP) $(subst /,\,$(TSROOT))\LICENSE      $(subst /,\,$(PKGROOT))\licenses\TreeSitter.txt
	@echo [Make] Done!

run: pkg
	@$(CLEAR)
	@cmd /c "cd $(subst /,\,$(PKGROOT)) && $(NAME).exe $(RUNARGS)"

clean:
	@$(EXISTS)    $(subst /,\,$(BUILDROOT)) echo [Make] Purged build directory
	@$(NOTEXISTS) $(subst /,\,$(BUILDROOT)) echo [Make] No build directory - nothing to do
	@$(EXISTS)    $(subst /,\,$(BUILDROOT)) rmdir /s /q $(subst /,\,$(BUILDROOT))

.DEFAULT_GOAL = pkg
