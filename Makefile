# The MakeFile for SourceDiff
# TODO: add official unix support
# TODO: catch errors from robocopy - currently hiding any errors
# TODO: currently using direct linking (with MinGW) to use libtcc
#    -  with MSVC, this will need to be handled deliberately

CC       := cc
CFLAGS   := -Wall -O3 -march=native
CSTD     := 11
PROGNAME := sd
BUILDDIR := build

ifeq ($(OS),Windows_NT)
ROOT      := $(shell cd)
TCCVENDOR := vendor\tinycc
TSVENDOR  := vendor\tree-sitter
OUT       := .exe
else
ROOT      := $(shell pwd)
TCCVENDOR := vendor/tinycc
TSVENDOR  := vendor/tree-sitter
OUT       :=
endif

SOURCES     := $(wildcard src/*.c)
TCCEVILOBJS  = $(wildcard $(BUILDDIR)\[doc|examples|*.def])

all: clean libtcc sdexe licenses

.SILENT: libtcc sdexe licenses clean run

ifeq ($(OS),Windows_NT)
libtcc:
	echo [SourceDiff] Invoking Windows build script for TinyCC
	cd $(TCCVENDOR)\win32 && (build-tcc -i "$(ROOT)\$(BUILDDIR)" && build-tcc -clean) > nul
	echo ^  ^> [TinyCC] Built TinyCC for Windows
	echo ^  ^> [TinyCC] Removing unnecessary objects...
	cd $(BUILDDIR) && (rd /Q /S doc & rd /Q /S examples & del /Q tcc.exe & del /Q /S *.def) > nul

	echo ^  ^> [TinyCC] Moving libraries...
	if not exist include (mkdir include)
	move $(BUILDDIR)\libtcc include\libtcc > nul
	move $(BUILDDIR)\libtcc.dll $(BUILDDIR) > nul
	echo ^  ^> [TinyCC] Finished!

sdexe:
	echo [SourceDiff] Building sources...
	$(CC) $(CFLAGS) --std=c$(CSTD) -Iinclude\libtcc $(BUILDDIR)\libtcc.dll $(SOURCES) -o $(BUILDDIR)\$(PROGNAME)$(OUT)
	echo ^  ^> [SourceDiff] Finished!

licenses:
	echo [SourceDiff] Copying licenses to $(BUILDDIR)...
	if not exist $(BUILDDIR)\licenses mkdir $(BUILDDIR)\licenses

	echo ^  ^> [SourceDiff] Copying SourceDiff license...
	copy /y LICENSE $(BUILDDIR)\licenses\SourceDiff.txt > nul

	echo ^  ^> [SourceDiff] Copying TinyCC license...
	copy /y $(TCCVENDOR)\COPYING $(BUILDDIR)\licenses\TinyCC.txt > nul

	echo ^  ^> [SourceDiff] Copying Tree Sitter license...
	copy /y $(TSVENDOR)\LICENSE $(BUILDDIR)\licenses\Tree-Sitter.txt > nul

	echo ^  ^> [SourceDiff] Finished!

clean:
	echo [SourceDiff] Reverting repo to initial state...
	if exist $(BUILDDIR) (echo ^  ^> [SourceDiff] Removing $(BUILDDIR) directory... & rd /Q /S $(BUILDDIR) > nul)
	if exist include (echo ^  ^> [SourceDiff] Removing include directory... & rd /Q /S include > nul)
	echo ^  ^> [SourceDiff] Finished!
else
$(error SourceDiff does not currently support Unix systems)
endif
