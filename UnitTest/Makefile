ifeq ($(OS), Windows_NT)
	#WINDOWS USE THESE DEFINITIONS
	RM = -rm -f
	SLASH = \\
	SWALLOW_OUTPUT = >nul 2>nul
	TOOLCHAIN = $(ECO_TOOLCHAIN)$(SLASH)xpack-aarch64-none-elf-gcc-13.2.1-1.1$(SLASH)bin$(SLASH)
	TOOL = aarch64-none-elf-
else
	#LINUX USE THESE DEFINITIONS
	RM = -rm -f
	SLASH = /
	SWALLOW_OUTPUT =
	TOOLCHAIN =
	TOOL =
endif

CC := gcc
CFLAGS := -std=gnu89 -Wall -Wextra -O2 \
	-DECO_LINUX -DECO_POSIX -DECO_X86_64 -DUGUID_UTILITY \
	-I ..$(SLASH)HeaderFiles \
	-I ..$(SLASH)SharedFiles \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.Core1$(SLASH)SharedFiles" \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.System1$(SLASH)SharedFiles" \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.Timer1$(SLASH)SharedFiles" \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.InterfaceBus1$(SLASH)SharedFiles" \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.MemoryManager1$(SLASH)SharedFiles" \
	-I "$(ECO_FRAMEWORK)$(SLASH)Eco.FileSystemManagement1$(SLASH)SharedFiles"

LIBDIR := ../BuildFiles/EcoOS/x86_64/StaticDebug
LIBS := -L "$(LIBDIR)" -l902ABA722D34417BB714322CC761620F

SRC := main.c
OBJ := $(SRC:.c=.o)
BIN := lab4_unit_test.exe

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(OBJ) $(BIN)