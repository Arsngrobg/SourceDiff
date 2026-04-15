#
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
#    ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀   ▀▀▀▀▀▀▀▀▀▀▀  ▀            ▀
#
#    the MakeFile for SourceDiff

# PROJECT
NAME       := srcdiff
VERSION    := $(shell type .\VERSION)

# COMPILATION
CC         := cc
CSTD       := 11
CCFLAGS    := -g -O2 -Wall -Wextra -Wpedantic
CCDEFS     := -DSD_VERSION="$(VERSION)" -DNDEBUG
override CCFLAGS := -std=c$(CSTD) $(CCFLAGS)

# BUILD SYSTEM
BUILDROOT  ?= build
INCROOT    := include
OBJROOT    := $(BUILDROOT)/obj
LIBROOT    := $(BUILDROOT)/lib
BINROOT    := $(BUILDROOT)/bin
PKGROOT    := $(BUILDROOT)/pkg
SDSRC      := $(wildcard src/*.c)
SDOBJ      := $(addprefix $(OBJROOT)/,$(notdir $(SDSRC:.c=.o)))

# VENDORED LIBRARIES
VENDOROOT  := vendor
TREESITTER := $(VENDOROOT)/tree-sitter
LIBCC      := $(VENDOROOT)/libcc

# VENDORING TREE-SITTER
TSCCFLAGS  := -I$(TREESITTER)/lib/src/ -I$(TREESITTER)/lib/include/
TSSRC      := $(filter-out lib.c wasm_store.c,$(notdir $(wildcard $(TREESITTER)/lib/src/*.c)))
TSOBJ      := $(addprefix $(OBJROOT)/$(notdir $(TREESITTER))/,$(TSSRC:.c=.o))

# SEARCH PATHS
VPATH      := src $(TREESITTER)/lib/src

# EXECUTABLES
$(BINROOT)/$(NAME).exe: $(LIBROOT)/lib$(NAME).a $(LIBROOT)/libtreesitter.a | $(BINROOT)/
	$(CC) $^ -o $@

# LIBRARIES
$(LIBROOT)/lib$(NAME).a: $(SDOBJ) | $(LIBROOT)/
	ar rcs $@ $^
$(LIBROOT)/libtreesitter.a: $(TSOBJ) | $(LIBROOT)/
	ar rcs $@ $^

# SOURCES
$(OBJROOT)/tree-sitter/%.o: %.c | $(OBJROOT)/tree-sitter/
	$(CC) $(CCFLAGS) $(TSCCFLAGS) $(CCDEFS) -c $< -o $@
$(OBJROOT)/%.o: %.c $(INCROOT)/tree-sitter.h | $(OBJROOT)/
	$(CC) $(CCFLAGS) -I$(INCROOT) $(CCDEFS) -c $< -o $@

# INCLUDES
$(INCROOT)/tree-sitter.h: $(TREESITTER)/lib/include/tree_sitter/api.h | $(INCROOT)/
	copy $(subst /,\,$<) $(subst /,\,$@) > nul
$(INCROOT)/libcc.h: $(LIBCC)/libcc.h | $(INCROOT)/
	copy $(subst /,\,$<) $(subst /,\,$@) > nul

%/:
	if not exist $(subst /,\,$@) mkdir $(subst /,\,$@)

# TASKS
all: pkg

pkg: compile | $(PKGROOT)/licenses/
	copy $(subst /,\,$(BINROOT)/$(NAME).exe) $(subst /,\,$(PKGROOT)/$(NAME).exe) > nul
	copy LICENSE $(subst /,\,$(PKGROOT)/licenses/SourceDiff.txt) > nul
	copy $(subst /,\,$(TREESITTER)/LICENSE) $(subst /,\,$(PKGROOT)/licenses/TreeSitter.txt) > nul
	copy $(subst /,\,$(LIBCC)/LICENSE) $(subst /,\,$(PKGROOT)/licenses/LibCC.txt) > nul

run: compile
	@cls
	@$(BINROOT)/$(NAME).exe

compile: deps min $(BINROOT)/$(NAME).exe

min: $(INCROOT)/tree-sitter.h $(INCROOT)/libcc.h

deps: $(LIBROOT)/libtreesitter.a

clean:
	if exist $(subst /,\,$(BUILDROOT)) rmdir /q $(subst /,\,$(BUILDROOT)) > nul
	if exist $(INCROOT) rmdir /q $(INCROOT) > nul

.PHONY: all pkg run compile min deps clean
.DEFAULT_GOAL := all
