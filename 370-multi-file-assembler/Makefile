# Makefile
# Build rules for EECS 370 P2

# Compiler
CXX = gcc

# Compiler flags (including debug info)
CXXFLAGS = -std=c99 -Wall -Werror -g3
# -std=c99 restricts us to using C and not C++
# -Wall and -Werror catch extra warnings as errors to decrease the chance of undefined behaviors on CAEN
# -g3 or -g includes debug info for gdb

# Uncomment next line and replace "mysystem" with your
# system if you are using our solution to project 1a.
#INST_OBJ = inst_p1a_obj.linux.o

# Compile Assembler
assembler: assembler.c $(INST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile Linker
linker: linker.c
	$(CXX) $(CXXFLAGS) $< -o $@

# Compile Simulator - COPY simulator.c FROM P1
simulator: simulator.c
	$(CXX) $(CXXFLAGS) $< -o $@

# Compile any C program
%.exe: %.c
	$(CXX) $(CXXFLAGS) $< -o $@

# Assemble an LC2K file into an Object file
%.obj: assembler %.as
	./$^ $@

# Assemble an LC2K file into an Object file
%.obj: assembler %.s
	./$^ $@

# Assemble an LC2K file into an Object file
%.obj: assembler %.lc2k
	./$^ $@

# Link the spec. HINT: you may want to rename these to count5_0.obj and count5_1.obj
count5.mc: linker main.obj subone.obj
	./$^ $@

# Assemble a Machine code file from a SINGLE object file of the same basename
# Hint: The output should be the same as p1a's command make %.mc
%.mc: linker %.obj
	./$^ $@

# Assemble a machine code file from SIX object files following the AG naming
%.mc: linker %_0.obj %_1.obj %_2.obj %_3.obj %_4.obj %_5.obj
	./$^ $@

# Assemble a machine code file from FIVE object files following the AG naming
%.mc: linker %_0.obj %_1.obj %_2.obj %_3.obj %_4.obj
	./$^ $@

# Assemble a machine code file from FOUR object files following the AG naming
%.mc: linker %_0.obj %_1.obj %_2.obj %_3.obj
	./$^ $@

# Assemble a machine code file from THREE object files following the AG naming
%.mc: linker %_0.obj %_1.obj %_2.obj
	./$^ $@

# Assemble a machine code file from TWO object files following the AG naming
%.mc: linker %_0.obj %_1.obj
	./$^ $@

# Assemble a machine code file from a SINGLE object file following the AG naming
%.mc: linker %_0.obj
	./$^ $@

# We will not test you on linking >6 object files,
# but you can add dependencies above the SIX file dependency if you wish to link more

# Simulate a machine code program to a file
%.out: simulator %.mc
	./$^ > $@

# Compare output to a *.mc.correct or *.out.correct file
%.diff: % %.correct
	diff $^ > $@

# Compare output to a *.mc.correct or *.out.correct file with full output
%.sdiff: % %.correct
	sdiff $^ > $@

# Remove anything created by a makefile
clean:
	rm -f *.obj *.mc *.out *.exe *.diff *.sdiff assembler simulator linker