################################################################################
#                 MAKEFILE DEVOTED TO SAMPLE ANALYZER LIBRARY                  #
################################################################################

# Compilators
GCC = g++

# Options for compilation
CXXFLAGS = -Wall -O3 $(shell root-config --cflags) -I./../ -DZIP_USE

# Files
SRCS = $(wildcard */*.cpp)
HDRS = $(wildcard */*.h)
OBJS = $(SRCS:.cpp=.o)

# Name of the library
PROGRAM = SampleAnalyzer

# Defining colours
GREEN  = "\\033[1;32m"
RED    = "\\033[1;31m"
PINK   = "\\033[1;35m"
BLUE   = "\\033[1;34m"
YELLOW = "\\033[1;33m"
CYAN   = "\\033[1;36m"
NORMAL = "\\033[0;39m"

# All target
all: header compile_header compile link_header link

# Header target
header:
	@echo -e $(YELLOW)"--------------------------------------------------"
	@echo -e "         Building SampleAnalyzer library          "
	@echo -e "--------------------------------------------------"$(NORMAL)

# Compile_header target
compile_header:
	@echo -e $(YELLOW)"--------------------------------------------------"
	@echo -e "                   Compilation                    "
	@echo -e "--------------------------------------------------"$(NORMAL)

# Link_header target
link_header:
	@echo -e $(YELLOW)"--------------------------------------------------"
	@echo -e "                  Final linking                   "
	@echo -e "--------------------------------------------------"$(NORMAL)

# clean_header target
clean_header:
	@echo -e $(YELLOW)"--------------------------------------------------"
	@echo -e "    Removing intermediate files from building     "
	@echo -e "--------------------------------------------------"$(NORMAL)

# mrproper_header target
mrproper_header:
	@echo -e $(YELLOW)"--------------------------------------------------"
	@echo -e "             Cleaning all the project             "
	@echo -e "--------------------------------------------------"$(NORMAL)

# Precompile target
precompile:

# Compile target
compile: precompile $(OBJS)

# Link target
link: $(OBJS)
	ar -ruc Lib/lib$(PROGRAM).a $(OBJS)

# Phony target
.PHONY: do_clean header compile_header link_header

# Clean target
clean: clean_header do_clean

# Do clean target
do_clean: 
	@rm -f $(OBJS) Reader/FACdict.*

# Mr Proper target 
mrproper: mrproper_header do_mrproper

# Do Mr Proper target 
do_mrproper: do_clean
	@rm -f Lib/lib$(PROGRAM).a
	@rm -f compilation.log linking.log cleanup.log *~ */*~

