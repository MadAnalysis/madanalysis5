CXX     = `$(MA5_BASE)/tools/SampleAnalyzer/ExternalSymLink/Bin/root-config --cxx`
CFLAGS  = `$(MA5_BASE)/tools/SampleAnalyzer/ExternalSymLink/Bin/root-config --cflags`
LIBS    = `$(MA5_BASE)/tools/SampleAnalyzer/ExternalSymLink/Bin/root-config --libs`
SOURCE  = all.C
PROGRAM = goROOT

$(PROGRAM):
	$(CXX) $(CFLAGS) $(SOURCE) $(LIBS) -o $(PROGRAM)
