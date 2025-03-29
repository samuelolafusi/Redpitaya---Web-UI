# Define the C++ compiler to use (with a cross-compiler prefix if specified)
CXX=$(CROSS_COMPILE)g++

# Define the command for removing files
RM=rm

# Set the installation directory (defaults to /opt/redpitaya/ if not specified)
INSTALL_DIR ?= /opt/redpitaya/

# List of C++ source files to compile
CXXSOURCES=main.cpp

# Specify include directories for header files
INCLUDE =  -I$(INSTALL_DIR)/include          # General Red Pitaya include directory
INCLUDE += -I$(INSTALL_DIR)/include/api2     # API version 2 headers
INCLUDE += -I$(INSTALL_DIR)/include/apiApp   # API for applications
INCLUDE += -I$(INSTALL_DIR)/rp_sdk           # Red Pitaya SDK headers
INCLUDE += -I$(INSTALL_DIR)/rp_sdk/libjson   # JSON library headers

# Specify library directories for linking
LIBS = -L$(INSTALL_DIR)/lib         # General Red Pitaya library directory
LIBS += -L$(INSTALL_DIR)/rp_sdk     # SDK library directory

# Common compiler flags for warnings, position-independent code, optimization, and stripping
COMMON_FLAGS+=-Wall -fPIC -Os -s

# C++ compiler flags: add common flags, use C++11 standard, and include directories
CXXFLAGS+=$(COMMON_FLAGS) -std=c++11 $(INCLUDE)

# Linker flags: create a shared library, use common flags, and specify library paths
LDFLAGS = -shared $(COMMON_FLAGS) $(LIBS)
LDFLAGS+= -Wl,--whole-archive,--no-as-needed  # Include all symbols from static libraries
LDFLAGS+= -lcryptopp -lrpapp -lrp -lrp_sdk -lrp-hw-calib -lrp-hw-profiles  # Link specific libraries
LDFLAGS+= -Wl,--no-whole-archive              # Stop including all static library symbols

# Convert C++ source files to object files (replace .cpp with .o)
CXXOBJECTS=$(CXXSOURCES:.cpp=.o)
OBJECTS=$(CXXOBJECTS)

# Define the output directory and the name of the compiled shared library
OUT_DIR=../
C_OUT_NAME=$(OUT_DIR)controllerhf.so

# Default target: build the C++ application
all: make_c_app

# Rule to compile .cpp files into .o object files
.cpp.o:
    $(CXX) -c $(CXXFLAGS) $< -o $@  # Compile the source file ($<) into an object file ($@)

# Rule to link object files into the final shared library
make_c_app: $(OBJECTS)
    $(CXX) $(OBJECTS) -o $(C_OUT_NAME) $(LDFLAGS)  # Link objects into the output file

# Rule to clean up generated files
clean:
    -$(RM) -f $(C_OUT_NAME) $(OBJECTS)  # Remove the output file and object files (if they exist)