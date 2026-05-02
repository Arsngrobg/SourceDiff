#       ▄▄▄▄                        ▄▄▄▄▄        ██        ▄▄▄▄      ▄▄▄▄
#     ▄█▀▀▀▀█                       ██▀▀▀██      ▀▀       ██▀▀▀     ██▀▀▀
#     ██▄        ██▄████   ▄█████▄  ██    ██   ████     ███████   ███████
#      ▀████▄    ██▀      ██▀    ▀  ██    ██     ██       ██        ██
#          ▀██   ██       ██        ██    ██     ██       ██        ██
#     █▄▄▄▄▄█▀   ██       ▀██▄▄▄▄█  ██▄▄▄██   ▄▄▄██▄▄▄    ██        ██
#     ▀▀▀▀▀     ▀▀         ▀▀▀▀▀   ▀▀▀▀▀     ▀▀▀▀▀▀▀▀    ▀▀        ▀▀
#
#    The Makefile for SourceDiff.

include mymk.mk
include tree-sitter.mk
include vendor/libcc/libcc.mk

# PROJECT
NAME           := srcdiff
VERSION        := 1.0a
DESCRIPTION    := a tool for analysing codebases using parse trees
DOCS           := https://github.com/Arsngrobg/SourceDiff\#building-from-source

# FILES
SRCDIFF_PREFIX := src/
SRCDIFF_C      := $(wildcard $(SRCDIFF_PREFIX)*.c)
SRCDIFF_O      := $(addprefix $(OBJDUMP)/,$(notdir $(SRCDIFF_C:.c=.o)))
SRCDIFF_D      := $(SRCDIFF_O:.o=.d)
SRCDIFF_STATIC := $(LIBDUMP)/lib$(NAME).a
SRCDIFF_EXEC   := $(BINDUMP)/$(NAME)$(EXEXT)

# COMPILER CONFIGURATION
override CFLAGS += #-DNDEBUG
override CFLAGS += -O2
override CFLAGS += -MMD -MP
override CFLAGS += -DSD_VERSION=\"$(VERSION)\" -DSD_DESCRIPTION="\"$(DESCRIPTION)\"" -DSD_DOCS="\"$(DOCS)\""

# RECIPES
-include $(SRCDIFF_D)

$(SRCDIFF_EXEC): $(SRCDIFF_STATIC) $(TREE-SITTER_STATIC) $(LIBCC_STATIC) | $(BINDUMP)/
	@$(LOG) Bundling $(NAME)$(EXEXT)
	@$(CC) $^ -o $@

$(SRCDIFF_STATIC): $(SRCDIFF_O) | $(LIBDUMP)/
	@$(LOG) Creating the $(NAME) static library (lib$(NAME).a)
	@$(AR) $(ARFLAGS) $@ $^

$(OBJDUMP)/%.o: $(SRCDIFF_PREFIX)%.c | $(OBJDUMP)/
	@$(LOG) Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

#     ,────────.              ,──.
#     '──.  .──',──,──. ,───. │  │,─.  ,───.
#        │  │  ' ,─.  │(  .─' │     ╱ (  .─'
#        │  │  ╲ '─'  │.─'  `)│  ╲  ╲ .─'  `)
#        `──'   `──`──'`────' `──'`──'`────'
#     Phony targets

help:
	@echo make help                       - Displays all phony targets
	@echo make install                    - Packages SourceDiff into a packaged format
	@echo make install PREFIX=? DESTDIR=? - Packages SourceDiff into a packaged format
	@echo make run                        - Compiles (if neccessary) and runs SourceDiff
	@echo make run ARGV=?                 - Compiles (if neccessary) and runs SourceDiff with the supplied arguments
	@echo make clean                      - Delete all MAKE build files

install: $(SRCDIFF_EXEC) | $(DESTDIR)/licenses/
	@$(LOG) Packaging SourceDiff
	@$(CP) $(subst /,$(PATHSEP),$(BINDUMP)/$(NAME)$(EXEXT)) $(subst /,$(PATHSEP),$(DESTDIR)/$(NAME)$(EXEXT))
	@$(CP) LICENSE                                          $(subst /,$(PATHSEP),$(DESTDIR)/licenses/SourceDiff.txt)
	@$(CP) $(subst /,$(PATHSEP),vendor/tree-sitter/LICENSE) $(subst /,$(PATHSEP),$(DESTDIR)/licenses/Tree-Sitter.txt)
	@$(CP) $(subst /,$(PATHSEP),vendor/libcc/LICENSE)       $(subst /,$(PATHSEP),$(DESTDIR)/licenses/LibCC.txt)
	@$(LOG) Done!

run: install
	@$(CLEAR)
	-@cd $(DESTDIR) && $(NAME) $(ARGV)

clean:
	@$(LOG) Purging $(BUILDDIR) directory
	@$(RMDIR) $(subst /,$(PATHSEP),$(BUILDDIR))

.PHONY: help install run clean
.DEFAULT_GOAL = install
