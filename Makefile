CXX = mpicc
CXXFLAGS = -Wall

# Strict compiler options
CXXFLAGS += -Wfloat-equal
# Directories with source code
SRC_DIR = src
INCLUDE_DIR = include

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
DEP_DIR = $(BUILD_DIR)/deps
LIBS = externals

# Add headers dirs to gcc search path
CXXFLAGS += -I $(INCLUDE_DIR)
# Add path with compiled libraries to gcc search path
CXXFLAGS +=
# Link libraries gcc flag: library will be searched with prefix "lib".
LDFLAGS = -lm

# Helper macros
# subst is sensitive to leading spaces in arguments.
make_path = $(addsuffix $(1), $(basename $(subst $(2), $(3), $(4))))
# Takes path list with source files and returns pathes to related objects.
src_to_obj = $(call make_path,.o, $(SRC_DIR), $(OBJ_DIR), $(1))
# Takes path list with object files and returns pathes to related dep. file.
# Dependency files will be generated with gcc -MM.
src_to_dep = $(call make_path,.d, $(SRC_DIR), $(DEP_DIR), $(1))

# All source files in our project that must be built into movable object code.
CXXFILES := $(wildcard $(SRC_DIR)/*.cpp)

# Default target (make without specified target).
.DEFAULT_GOAL := all

# Alias to make all targets.
.PHONY: all
all: $(BIN_DIR)/solve

# Suppress makefile rebuilding.
Makefile: ;

# Generate and include dependency information.
ifneq ($(MAKECMDGOALS), clean)
-include deps.mk
endif

# deps.mk contains redirect to dependency generation
deps.mk: 
	mkdir -p $(BUILD_DIR) $(OBJ_DIR) $(BIN_DIR) $(DEP_DIR)
	echo '-include $(call src_to_dep, $(CXXFILES))' >deps.mk

# Rules for compiling targets
$(BIN_DIR)/solve: $(OBJ_DIR)/solve.o
	$(CXX) $(CXXFLAGS) $(filter %.o, $^) -o $@ $(LDFLAGS)

# $(BIN_DIR)/matrix_example: $(OBJ_DIR)/matrix_example.o $(OBJ_DIR)/io.o
#	$(CXX) $(CXXFLAGS) $(filter %.o, $^) -o $@ $(LDFLAGS)

# Pattern for generating dependency description files (*.d)
$(DEP_DIR)/%.d: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -E -MM -MT $(call src_to_obj, $<) -MT $@ -MF $@ $<

# Pattern for compiling object files (*.o)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $(call src_to_obj, $<) $<

# Fictive target
.PHONY: clean
# Delete all temprorary and binary files
clean:
	rm -rf $(BUILD_DIR)
	rm -f deps.mk
	rm -rf data/

# Additional targers for testing purposes

.PHONY: debug
debug: $(BIN_DIR)/$(prog)
	gdb $(BIN_DIR)/$(prog)
	
.PHONY: run
run: $(BIN_DIR)/$(prog)
	$(BIN_DIR)/$(prog)

# If you still have "WTF?!" feeling, try reading teaching book
# by Mashechkin & Co. http://unicorn.ejudge.ru/instr.pdf
