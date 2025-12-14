ifeq ($(OS), Windows_NT)
	# WINDOWS USE THESE DEFINITIONS (MinGW gcc)
	RM = del /Q
	SLASH = \\
	SWALLOW_OUTPUT = 2>NUL
	TOOLCHAIN =
	TOOL =
	OS_DEFINES = -DECO_WINDOWS -DECO_WIN64
	MKDIR_OUTDIR = if not exist "$(OUT_DIR)" mkdir "$(OUT_DIR)"
else
	# LINUX USE THESE DEFINITIONS
	RM = rm -f
	SLASH = /
	SWALLOW_OUTPUT = 2>/dev/null
	TOOLCHAIN =
	TOOL =
	OS_DEFINES = -DECO_LINUX -DECO_POSIX
	MKDIR_OUTDIR = mkdir -p "$(OUT_DIR)"
endif

ifeq ($(TARGET), 1)
OUT_TARGET = lib902ABA722D34417BB714322CC761620F.a
ifeq ($(DEBUG), 1)
CONFIG_TARGET = StaticDebug
CCFLAGS_TARGET = -Wall -g -DECO_LIB
LDFLAGS_TARGET =
else
CONFIG_TARGET = StaticRelease
CCFLAGS_TARGET = -DECO_LIB
LDFLAGS_TARGET =
endif
else
ifeq ($(OS), Windows_NT)
OUT_TARGET = 902ABA722D34417BB714322CC761620F.dll
else
OUT_TARGET = 902ABA722D34417BB714322CC761620F.so
endif

ifeq ($(DEBUG), 1)
CONFIG_TARGET = DynamicDebug
CCFLAGS_TARGET = -Wall -fPIC -g -DECO_DLL

ifeq ($(OS), Windows_NT)
LDFLAGS_TARGET = -shared -Wl,--out-implib,lib902ABA722D34417BB714322CC761620F.a
else
LDFLAGS_TARGET = -shared
endif
else
CONFIG_TARGET = DynamicRelease
CCFLAGS_TARGET = -fPIC -DECO_DLL
ifeq ($(OS), Windows_NT)
LDFLAGS_TARGET = -shared -Wl,--out-implib,lib902ABA722D34417BB714322CC761620F.a
else
LDFLAGS_TARGET = -shared
endif
endif
endif

# ARCH
ifeq ($(OS), Windows_NT)
ARCH_TARGET = windows_x86_64
else
ARCH_TARGET = x86_64
endif

# include directories
INC = \
-I ..$(SLASH)..$(SLASH)..$(SLASH)HeaderFiles \
-I ..$(SLASH)..$(SLASH)..$(SLASH)SharedFiles \
-I "$(ECO_FRAMEWORK)$(SLASH)Eco.Core1$(SLASH)SharedFiles" \
-I "$(ECO_FRAMEWORK)$(SLASH)Eco.System1$(SLASH)SharedFiles" \
-I "$(ECO_FRAMEWORK)$(SLASH)Eco.Timer1$(SLASH)SharedFiles" \
-I "$(ECO_FRAMEWORK)$(SLASH)Eco.InterfaceBus1$(SLASH)SharedFiles" \
-I "$(ECO_FRAMEWORK)$(SLASH)Eco.MemoryManager1$(SLASH)SharedFiles" \
-I "$(ECO_FRAMEWORK)$(SLASH)Eco.FileSystemManagement1$(SLASH)SharedFiles"

# source files.
SRC = \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoTask1Lab.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoTaskScheduler1Lab.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoTaskScheduler1LabFactory.c

OBJ = $(SRC:.c=.o)

OUT = $(OUT_TARGET)
OUT_DIR = ..$(SLASH)..$(SLASH)..$(SLASH)BuildFiles$(SLASH)EcoOS$(SLASH)$(ARCH_TARGET)$(SLASH)$(CONFIG_TARGET)

# Compiler
GNU =
CC = gcc
AR = gcc-ar

ifeq ($(TARGET), 1)
CMD_TARGET=$(GNU)$(AR) rcs
else
CMD_TARGET=$(GNU)$(CC) -o
endif

# library paths
LIBS = -lc -lm -lgcc

# compile flags
CCFLAGS = $(CCFLAGS_TARGET) -std=gnu89 -Wall -Wextra -O2 $(OS_DEFINES) -DECO_X86_64 -DUGUID_UTILITY -DCECO_TASKSCHEDULER1LAB_EXPORTS
LDFLAGS = $(LDFLAGS_TARGET)

.SUFFIXES: .c

all: $(OUT)

.c.o:
	$(GNU)$(CC) $(INC) $(CCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(MKDIR_OUTDIR)
	$(CMD_TARGET) "$(OUT_DIR)$(SLASH)$(OUT)" $(OBJ) $(LDFLAGS)
	-$(RM) $(OBJ) $(SWALLOW_OUTPUT)

clean:
	-$(RM) $(OBJ) $(SWALLOW_OUTPUT)
	-$(RM) "$(OUT_DIR)$(SLASH)$(OUT)" $(SWALLOW_OUTPUT)