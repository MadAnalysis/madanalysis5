#ifndef HTML_REPORT_h
#define HTML_REPORT_h

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>


class HTMLReport
{

 public:
  
  std::vector<std::string> sample_files_;
  std::vector<std::string> nofgenevents_;
  std::vector<std::string> cross_section_;
  std::vector<std::string> datasets_;
  std::vector<std::string> nofrealevents_;
  std::vector<std::string> means_;
  std::vector<std::string> rms_;
  std::vector<std::string> uflow_;
  std::vector<std::string> oflow_;
  std::vector<std::string> datasets2_;
  std::vector<std::string> selected_;
  std::vector<std::string> rejected_;
  std::vector<std::string> sel_selprej_;
  std::vector<std::string> sel_nofevents_;
  std::vector<std::string> cutname_;
  std::vector<std::string> signalevents_;
  std::vector<std::string> bckgdevents_;
  std::vector<std::string> sbratio_;

  
  std::ifstream report;
  std::string version;
  std::string reportname;

  HTMLReport() {}
  ~HTMLReport() {}

  void openReport(std::string name)
  {
    std::string file;
    file = name;
    reportname = name;
    report.open (file.c_str());
  }

};

#endif
