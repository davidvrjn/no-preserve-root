# Main Targets:
#   all         - (Default) Compiles the project.
#   run         - Compiles and runs the executable.
#   debug       - Compiles and starts a GDB debugging session.
#   valgrind    - Runs the program under Valgrind to check for memory leaks.
#   coverage    - Runs the program and displays a line-coverage summary.
#   clean       - Removes all built files, reports, and coverage data.
#
# Shortcuts: r, d, v, cv, c, n (clean all)

# Prevents command echoing
.SILENT:
# Suppresses "Entering directory..." messages
MAKEFLAGS += --no-print-directory
# Phony targets prevent conflicts with file names
.PHONY: all clean run debug coverage valgrind r c d cv v n clean_coverage clean_build test clang-tidy format

#########################################################################################################################################

# Project Configuration
# The cpp file name where int main() exists
main = main

# Which C++ standard to use
cstand = 17

# Clear terminal on clean (1=true, 0=false)          
cclr = 1

# Toolchain
cxx = g++

#########################################################################################################################################

# Directory and executable configuration
src_dir = src
include_dir = include
obj_dir = obj
bin_dir = bin
target = $(bin_dir)/$(main)

# Sanity check for the main file
ifeq (,$(wildcard $(src_dir)/$(main).cpp))
$(error "Main file $(src_dir)/$(main).cpp does not exist!")
endif

# Compiler flags and file variables
cpp_flags = -std=c++$(cstand) -I$(include_dir) -Wall -Wextra -g
gcov_flags = -fprofile-arcs -ftest-coverage
cxx_flags = $(cpp_flags) $(gcov_flags)

cpps = $(shell find $(src_dir) -name '*.cpp')
ofiles = $(patsubst $(src_dir)/%.cpp, $(obj_dir)/%.o, $(cpps))
depfiles = $(patsubst $(src_dir)/%.cpp, $(obj_dir)/%.d, $(cpps))

# Test sources (simple pattern under tests/)
test_srcs = $(shell find tests -name '*.cpp' 2>/dev/null)
test_objs = $(patsubst tests/%.cpp, $(obj_dir)/tests/%.o, $(test_srcs))

# Test runner
test_target = $(bin_dir)/test

# Files/directories to be cleaned
coverage_files = *.gcda *.gcno *.gcov
build_files = $(obj_dir) $(bin_dir)

# Default rule
all: $(target)

# Rule to link the executable from object files
$(target): $(ofiles) | $(bin_dir)
	$(cxx) $(cxx_flags) $^ -o $@

# Link test runner from test object files (and any project object files if needed)
# Exclude main.o from project objects to avoid multiple main() definitions
project_objs = $(filter-out $(obj_dir)/main.o, $(ofiles))
$(test_target): $(test_objs) $(project_objs) | $(bin_dir)
	$(cxx) $(cxx_flags) $^ -o $@

# Rule to compile .cpp to .o and generate header dependency files
$(obj_dir)/%.o: $(src_dir)/%.cpp | $(obj_dir)
	mkdir -p $(dir $@)
	$(cxx) $(cxx_flags) -MMD -MP -c $< -o $@

# Rule to compile test sources
$(obj_dir)/tests/%.o: tests/%.cpp | $(obj_dir)
	mkdir -p $(dir $@)
	$(cxx) $(cxx_flags) -I$(include_dir) -MMD -MP -c $< -o $@

# Rule to create output directories
$(bin_dir) $(obj_dir):
	mkdir -p $@

# Rule to run the program
run: $(target)
	./$(target)

# Rule to build and run tests
test: fetch-doctest $(test_target)
	./$(test_target)

# download doctest single header if missing (install into include/doctest.h)
fetch-doctest:
	mkdir -p $(dir include/doctest.h)
	[ -f include/doctest.h ] || \
	  wget -q -O include/doctest.h \
	    https://raw.githubusercontent.com/onqtam/doctest/v2.4.9/doctest/doctest.h

# Rule to launch the debugger
# Dont know if or how it works :)
debug: $(target)
	gdb ./$(target)

# Rule to run the program and display a coverage summary
coverage: $(target)
	./$(target) > /dev/null 2>&1
	@echo "--------------------------------------------------------"
	@echo "  GCOV Coverage Summary Report"
	@echo "--------------------------------------------------------"
	@for file in $(cpps); do \
		gcov $$file -m | grep -A 1 "File '$$file'" | tail -n 1 | awk -v f=$$file '{printf "%-20s | %s\n", f, $$0}'; \
	done
	@echo "--------------------------------------------------------"
	$(MAKE) clean_coverage
#   ^^^^ This isnt going to always work since we'll have user input at some point. I'll deal with it later if I must

# Rule to run Valgrind for memory analysis
valgrind: $(target)
	valgrind --leak-check=full --show-leak-kinds=all ./$(target)

# Cleanup rules
clean_coverage:
	rm -f $(coverage_files) $(depfiles)

clean_build:
	rm -rf $(build_files)

clean: clean_coverage clean_build
	@if [ $(cclr) -eq 1 ]; then clear; fi

# Shortcuts
r: run
c: clean
d: debug
cv: coverage
v: valgrind
n: clean run

# Include all the generated dependency files for correct incremental builds
-include $(depfiles)

clang-tidy: clean_coverage clean_build
	rm -f compile_commands.json
	bear -- make
	clang-tidy -p . $(cpps)


format:
	git ls-files '*.cpp' '*.h' | xargs -r clang-format -i -style=file
