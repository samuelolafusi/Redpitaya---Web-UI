# Get the current directory name (without the full path) to use as the app name
APP=$(notdir $(CURDIR:%/=%))
# 'notdir' removes the path, and '%/=%' strips any trailing slash

# Versioning system
BUILD_NUMBER ?= 0  # Set default build number to 0 if not specified
REVISION ?= devbuild  # Set default revision to 'devbuild' if not specified
# Extract the version from info/info.json, removing everything before and after the version string
VER:=$(shell cat info/info.json | grep version | sed -e 's/.*:\ *\"//' | sed -e 's/-.*//')

# Set the installation directory (defaults to /opt/redpitaya/ if not specified)
INSTALL_DIR ?= /opt/redpitaya/

# Define the name of the output shared library
CONTROLLERHF = controllerhf.so

# Add version and revision info to the C compiler flags
CFLAGS += -DVERSION=$(VER)-$(BUILD_NUMBER) -DREVISION=$(REVISION)
# These will be passed as preprocessor definitions (e.g., -DVERSION=1.0-0)
export CFLAGS  # Make CFLAGS available to sub-make processes

# Default target: build the controllerhf.so library
all: $(CONTROLLERHF)

# Rule to build controllerhf.so by running make in the src directory
$(CONTROLLERHF):
    $(MAKE) -C src  # Run the Makefile in the 'src' subdirectory

# Rule to clean up generated files
clean:
    $(MAKE) -C src clean  # Run the clean rule in the 'src' subdirectory
    -$(RM) *.so  # Remove all .so files in the current directory (if they exist)