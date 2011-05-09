# Project: Linwarrior 3D
# Makefile with auto-dependency generation

# Add Include directories here.
INCLUDES = -I . 

# Automatic searching for source files.
# Objects to compile are all sources (cpp) and put the .o below build-dir.
OBJECTS=$(addprefix build/, $(subst .cpp,.o, $(wildcard source/*.cpp source/*/*.cpp source/*/*/*.cpp source/*/*/*/*.cpp source/*/*/*/*/*.cpp source/*/*/*/*/*/*.cpp) ) )

# Different Parameters and Programms for different OSes.
ifneq (,$(findstring Win,$(OS)))
	LIBRARIES= -Wl,-subsystem,console -lmingw32 -lSDLmain -lSDL -lOpenGL32  -lglew32 -lGLU32 -lopenal32 -lalut
	TARGET=dist\linwarrior.exe
	MKDIR=mkdir
	CP=copy
	RM=cmd /C del /Q
	RMREC=cmd /C del /Q /S
	CPP=c++
	LIMITER=$(dir \file)
	CFLAGS+= -static-libgcc
	QUOTE='
else
	LIBRARIES= -lGLEW -lGLU -lSDL -lopenal -lalut
	TARGET=dist/linwarrior
	MKDIR=mkdir -p
	RM=rm -f
	RMREC=rm -f -r
	CP=cp
	CPP=c++
	LIMITER=/
	QUOTE=
endif

# Creation of dependency information when compiling.
CFLAGS += -Wp,-M,-MP,-MT,$@,-MF,dep/$(subst /,-,$@).d

# Enable c++0x standard which gives extended initializer lists
CFLAGS += -std=c++0x
#CFLAGS += -std=gnu++0x

# Print warnings when compiling.
CFLAGS += -Wall

# Use the given includepathes.
CFLAGS += $(INCLUDES)

# Optimizations.
CFLAGS += -O1 -funroll-loops

# More Optimizations.
#CFLAGS += -O3 -funroll-loops

# Even More optimizations (architecture depending).
#CFLAGS += -O3 -funroll-loops -msse3 -ftree-vectorizer-verbose=0 -ftree-vectorize

# Fast-Math doesn't work because of NaN usage.

# Flags for profiling, instructions:
# 1. Recompile everything with these flags un-commented.
# 2. Run the binaray to collect data.
# 3. Call gprof with the binary as a parameter to analyse the collected data.
# 4. Disable/Comment these flags again and recompile everything.
#CFLAGS += -pg
#LFLAGS += -pg

# Default makefile Target.
all: $(TARGET)

# For executable we need all sources compiled to objects.
$(TARGET): $(OBJECTS)
	$(MKDIR) dist
	$(CPP) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBRARIES)

# Compile all Source files, creates output directories as necessary.
build/%.o: %.cpp
	$(shell $(MKDIR) dep )
	$(shell $(MKDIR) build )
	$(shell $(MKDIR) $(dir $@) )
	$(CPP) $(CFLAGS) -c $< -o $@

#	$(shell $(MKDIR) build 2>/dev/null)
#	$(shell $(MKDIR) $(dir $@) 2>/dev/null)

# IDE may call makefile with target "build" instead of "all".
build: all

# May call clean before build to delete previous relics.
.PHONY: clean
clean:
	$(RMREC) $(QUOTE)dist$(QUOTE)
	$(RMREC) $(QUOTE)dep$(QUOTE)
	$(RMREC) $(QUOTE)build$(QUOTE)
	$(MKDIR) dep
	$(MKDIR) build
	$(MKDIR) dist

#	$(RM) $(QUOTE)dep$(LIMITER)*.o.d$(QUOTE)
#	$(RM) $(QUOTE)build$(LIMITER)source$(LIMITER)*.o$(QUOTE)

# Target to compile doxygen helpfiles from code and comments see conf file.
doxygen:
	doxygen doxygen.conf

# Include autogenerated dependency files (silently makedir if not existing).
-include $(shell $(MKDIR) dep 2>/dev/null) $(wildcard dep/*.o.d)

