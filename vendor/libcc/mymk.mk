#                                 ,──.
#    ,──,──,──.,──. ,──.,──,──,──.│  │,─.
#    │        │ ╲  '  ╱ │        ││     ╱
#    │  │  │  │  ╲   '  │  │  │  ││  ╲  ╲
#    `──`──`──'.─'  ╱   `──`──`──'`──'`──'
#              `───'
#    Standardized, cross-platform GNU Make

ifndef MYMK_MK
MYMK_MK := $(lastword $(MAKEFILE_LIST))

# BUILD SYSTEM
PREFIX   ?= /
BUILDDIR := build
OBJDUMP  := $(BUILDDIR)/obj
LIBDUMP  := $(BUILDDIR)/lib
BINDUMP  := $(BUILDDIR)/bin
DESTDIR  ?= $(BUILDDIR)/pkg

# SHELL UTILITIES
ifdef ComSpec
PATHSEP := $()\$()
MKDIR   := mkdir
RMDIR   := rmdir /Q /S 2>nul
RM      := del
else
PATHSEP := /
MKDIR   := mkdir -p
RMDIR   := rm -rf
RM      := rm -f
endif # ComSpec
LOG     := echo make:
CC      ?= cc
AR      ?= ar

# FLAGS
CFLAGS  ?= -Wall -Wextra -Werror -Wpedantic
ARFLAGS := -rcs

# CROSS-PLATFORM DEFINITIONS
ifeq ($(OS),Windows_NT)
EXEXT   := .exe
SOEXT   := dll
SOFLAG  := -shared
else
OS := $(shell uname -s)
ifeq ($(OS),Darwin)
SOEXT   := dylib
SOFLAG  := -dynamiclib
else
SOEXT   := so
SOFLAG  := -shared
override CFLAGS += -fPIC
endif # $(OS),Darwin
EXEXT   := $()
endif # $(OS),Windows_NT

# COMMON RECIPES
%/:
	@$(LOG) Creating new directory: $@
	@$(MKDIR) $(subst /,$(PATHSEP),$@)

endif # MYMK_MK
