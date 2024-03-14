# Makefile
# Build rules for EECS 280 project 4

# Compiler
CXX ?= g++

# Compiler flags
CXXFLAGS ?= --std=c++11 -Wall -Werror -pedantic -g -Wno-sign-compare -Wno-comment

# Run a regression test
test: test-api List_compile_check.exe List_public_test.exe List_tests.exe
	./List_compile_check.exe
	./List_public_test.exe
	./List_tests.exe

test-api: api.exe
	./api.exe < test01.in > test01.out
	diff -q test01.out test01.out.correct

	./api.exe < test02.in > test02.out
	diff -q test02.out test02.out.correct

	./api.exe < test03.in > test03.out
	diff -q test03.out test03.out.correct

	./api.exe < test04.in > test04.out
	diff -q test04.out test04.out.correct

	./api.exe < test05.in > test05.out
	diff -q test05.out test05.out.correct

	./api.exe < public_error01.in > public_error01.out
	diff -q public_error01.out public_error01.out.correct

api.exe: api.cpp
	$(CXX) $(CXXFLAGS) api.cpp -o $@

List_tests.exe: List_tests.cpp List.h
	$(CXX) $(CXXFLAGS) $< -o $@

List_compile_check.exe: List_compile_check.cpp List.h
	$(CXX) $(CXXFLAGS) List_compile_check.cpp -o $@

List_public_test.exe: List_public_test.cpp List.h
	$(CXX) $(CXXFLAGS) $< -o $@

# disable built-in rules
.SUFFIXES:

# these targets do not create any files
.PHONY: clean
clean:
	rm -vrf *.o *.exe *.gch *.dSYM *.stackdump *.out

# Run style check tools
CPD ?= /usr/um/pmd-6.0.1/bin/run.sh cpd
OCLINT ?= /usr/um/oclint-0.13/bin/oclint
FILES := List.h List_tests.cpp api.cpp
style :
	$(OCLINT) \
    -no-analytics \
    -rule=LongLine \
    -rule=HighNcssMethod \
    -rule=DeepNestedBlock \
    -rule=TooManyParameters \
    -rc=LONG_LINE=90 \
    -rc=NCSS_METHOD=40 \
    -rc=NESTED_BLOCK_DEPTH=4 \
    -rc=TOO_MANY_PARAMETERS=4 \
    -max-priority-1 0 \
    -max-priority-2 0 \
    -max-priority-3 0 \
    $(FILES) \
    -- -xc++ --std=c++11
	$(CPD) \
    --minimum-tokens 100 \
    --language cpp \
    --failOnViolation true \
    --files $(FILES)
	@echo "########################################"
	@echo "EECS 280 style checks PASS"
