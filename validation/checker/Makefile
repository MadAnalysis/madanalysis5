GCC = g++
#CXXFLAGS = -fPIC  -I./ -I/home/gserret/WorkingZone/Madanalysis/trunk/madanalysis-development/madanalysis/SampleAnalyzer/
#LIBFLAGS = -L/opt/root/lib -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore -lCint -pthread -lm -ldl -rdynami -lboost_iostreams -lz -lSampleAnalyzer

SRCS = Comparator.cpp main.cpp logger.cpp
OBJS = $(SRCS:.cpp=.o)
PROGRAM = MadAnalysisCheck

all:	 compile link

compile:	$(OBJS)

link:	$(OBJS)
		$(GCC) $(OBJS) -o $(PROGRAM)
 
clean:;	@rm -f $(OBJS) $(PROGRAM) src/*~ *~ \#*
