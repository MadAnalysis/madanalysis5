#ifndef HTML_CREATOR_h
#define HTML_CREATOR_h

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "HTMLReport.h"

class HTMLcomparator
{

 public:

  std::vector<HTMLReport *> reports_;
  std::vector<int> errors_;
  std::vector<int> errors2_;
  std::vector<int> errors3_;

  bool firstOpen_;
  bool toRead_;
  bool inTable_;
  bool firstLine_;
  bool secondcell_;
  bool bug;

  int type_;
  int cellnum_;
  HTMLcomparator() {}
  ~HTMLcomparator(){}

  void displayHeader();
  void readTables(HTMLReport & report);
  void compareReports(HTMLReport & report1, HTMLReport & report2);
  void displayStatisticsTable(HTMLReport & Report, std::vector<int> Error);
  void displayCutsTable(HTMLReport & Report, std::vector<int> Error);
  void displaySBTable(HTMLReport & Report, std::vector<int> Error);
};

#endif
