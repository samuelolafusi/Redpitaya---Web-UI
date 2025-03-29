# Compiler flags: Set the C standard to GNU11 (C11 with GNU extensions) and enable all warnings
CFLAGS  = -std=gnu11 -Wall

# Append include paths to CFLAGS for Red Pitaya headers
# CFLAGS: Compiler options, including the C standard, warnings, and include paths for headers.
# -I/opt/redpitaya/include: Main Red Pitaya API headers (e.g., rp.h)
# -I/opt/redpitaya/include/api250-12: Additional headers for SIGNALlab 250-12 specific features
CFLAGS += -I/opt/redpitaya/include -I/opt/redpitaya/include/api250-12

# Linker flags: Specify the library directory for Red Pitaya libraries
LDFLAGS = -L/opt/redpitaya/lib

# Libraries to link against, List of libraries to link, with each -l flag explained for clarity
LDLIBS = -static -lrp-hw-can -lrp -lrp-hw-calib -lrp-hw-profiles -lrp-gpio -lrp-i2c -lrp-hw -lm -lstdc++ -lpthread -li2c -lsocketcan

# Define the program to compile
# PRGS is a variable holding the name of the executable to be built
PRGS = 1_acq_sig_no-trig

# Default target: Build all programs listed in PRGS
all: $(PRGS) 

# Rule to build each program from its corresponding .c file
# %: Pattern matching; for each PRGS entry, find its .c file
# $<: Refers to the source file 
# $@: Refers to the target executable
# $(CC): The C compiler (e.g., gcc or cc, set by Make by default)
# $(PRGS) Rule: A pattern rule that compiles each .c file into an executable
$(PRGS): %: %.c
	$(CC) $< $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@

# Clean target: Remove the compiled executables
# $(RM): Remove command (typically 'rm', set by Make)
# $(PRGS): Expands to the list of programs
clean:
	$(RM) $(PRGS)

# Declare 'all' and 'clean' as phony targets
# Phony targets are not associated with files, preventing conflicts if files named 'all' or 'clean' exist
.PHONY: all clean