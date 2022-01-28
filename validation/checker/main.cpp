#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "Comparator.h"
#include "HTMLReport.h"
#include "logger.h"

int main(int argc, char *argv[])
{
  if (argc!=3)
    {
      std::cout << "Syntax: ./MadAnalysisCheck report1.html report2.html" << std::endl;
      return 1;
    }

  std::ifstream test1 (argv[1]);
  std::ifstream test2 (argv[2]);

  if (!test1)
    {
      ERROR << "The file " << argv[1] << " does not exist"
	    << std::endl;
      return 1;
    }
  else if (!test2)
    {
      ERROR << "The file " << argv[2] << " does not exist"
	    << std::endl;
      return 1;
    }
  
  HTMLReport report1;
  HTMLReport report2;
  HTMLcomparator muf;

  report1.openReport(argv[1]);
  report2.openReport(argv[2]);

  muf.displayHeader();
  
  muf.readTables(report1);
  muf.readTables(report2);


  muf.compareReports(report1, report2);

  return 0;
}
