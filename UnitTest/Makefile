ifeq ($(OS), Windows_NT)
	RM = del /Q
	SLASH = \\
	NULLDEV = NUL
	EXEEXT = .exe
else
	RM = rm -f
	SLASH = /
	NULLDEV = /dev/null
	EXEEXT =
endif

CC := gcc
CFLAGS := -std=gnu89 -Wall -Wextra -O2 \
	-DECO_WINDOWS -DECO_WIN32 -DECO_X86 -DUGUID_UTILITY -m32 \
	-I ..$(SLASH)HeaderFiles \
	-I ..$(SLASH)SharedFiles \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.Core1$(SLASH)SharedFiles" \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.System1$(SLASH)SharedFiles" \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.Timer1$(SLASH)SharedFiles" \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.InterfaceBus1$(SLASH)SharedFiles" \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.MemoryManager1$(SLASH)SharedFiles" \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.FileSystemManagement1$(SLASH)SharedFiles"

OUTDIR := ..$(SLASH)BuildFiles$(SLASH)EcoOS$(SLASH)windows_x86$(SLASH)DynamicDebug
LIBS := -L "$(OUTDIR)" -l902ABA722D34417BB714322CC761620F

SRC := main.c
OBJ := $(SRC:.c=.o)
BIN := lab4_unit_test$(EXEEXT)

.PHONY: all clean

all: $(OUTDIR)$(SLASH)$(BIN)

$(OUTDIR)$(SLASH)$(BIN): $(OBJ)
	$(CC) $(OBJ) -m32 -o "$(OUTDIR)$(SLASH)$(BIN)" $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) $(OBJ) 2>$(NULLDEV)
	-$(RM) "$(OUTDIR)$(SLASH)$(BIN)" 2>$(NULLDEV)