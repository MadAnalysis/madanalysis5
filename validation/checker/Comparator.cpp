//------------------------------------------------------------------------------------
//                       __  __  ______         __                                    
//                      /\ \/\ \/\__  _\/'\_/`\/\ \                                   
//                      \ \ \_\ \/_/\ \/\      \ \ \                                  
//                       \ \  _  \ \ \ \ \ \__\ \ \ \  __                             
//                        \ \ \ \ \ \ \ \ \ \_/\ \ \ \_\ \                            
//                         \ \_\ \_\ \ \_\ \_\\ \_\ \____/                            
//                          \/_/\/_/  \/_/\/_/ \/_/\/___/                             
//                                                            __                      
//                                                           /\ \__                   
//        ___   ___   ___ ___    _____     __     _ __   __  \ \ ,_\   ___   _ __     
//      /'___\ / __`\/' __` __`\/\ '__`\ /'__`\  /\`'__\'__`\ \ \ \/  / __`\/\`' \    
//     /\ \__//\ \_\ \\ \/\ \/\ \ \ \_\ \\ \_\.\_\ \ \/\ \_\.\_\ \ \_/\ \L\ \ \ \/    
//     \ \____\ \____/ \_\ \_\ \_\ \ ,__/ \__/.\_\\ \_\ \__/.\_\\ \__\ \____/\ \_\    
//      \/____/\/___/ \/_/\/_/\/_/\ \ \/ \/__/\/_/ \/_/\/__/\/_/ \/__/\/___/  \/_/    
//                                 \ \_\                                              
//                                  \/_/                                              
//                                                                                    
//____________________________________________________________________________________
// Creator : Guillaume Serret                                                         
// Date    : 03/13/2012                                                               
// Version : 1.0                                                                      
//-------------------------------------------------------------------------------------

#include <iomanip>
#include "Comparator.h"
#include "HTMLReport.h"
#include  "logger.h"
//-------------------------------------------------------------------------------------
//   Displaying Header
//-------------------------------------------------------------------------------------
void HTMLcomparator::displayHeader()
{
  INFO << "**************************************************** \n" 
       << "*                                                  * \n" 
       << "*      M A D A N A L Y S I S 5  C H E C K E R      * \n" 
       << "*                                                  * \n"
       << "**************************************************** \n"
       << std::endl;
}

void HTMLcomparator::readTables(HTMLReport & report)
{
  toRead_ = false;
  inTable_= false;
  bug=false;
  std::string line;
  while (std::getline(report.report,line))
    {
      std::stringstream str;
      str.str("");
      std::string tmp;
      str << line;
      str >> tmp;

      std::string testline;
      testline = line;
      testline.erase(0,22);
      std::stringstream teststr;
      std::string testtmp;
      teststr.str("");
      teststr << testline;
      teststr >> testtmp;
      if (testtmp == "MadAnalysis")
	{
	  testline.erase(26,380);
	  report.version = testline;
	}

      if (tmp=="<TABLE") 
	{
	  inTable_   = true;
	  firstLine_ = true;
	  secondcell_= true;
	  cellnum_=0;
	  continue;
	}
      if (inTable_)
	{
	  if(tmp=="<TR>") 
	    {
	      if (firstLine_)
		{
		  type_=0;
		  continue;
		}
	      else if (!firstLine_)
		{
		  toRead_ = true;
		  continue;
		}
	    }
	  if (tmp == "</TR>") 
	    {
	      if (firstLine_)
		{
		  firstLine_=false;
		}
	      continue;
	    }
	  if (tmp == "</TD>") 
	    {
	      continue;
	    }
	  if (firstLine_)
	    {
	      std::stringstream part;
	      part.str("");
	      std::string cell;
	      std::string secondcell;
	      line.erase(0,44);
	      part << line;
	      part >> cell;
	      if (cell == "Event")
		{
		  type_ = 0;
		}
	      if (cell =="Cuts</FONT></TD>" )
		{
		  type_ = 4 ;
		}
	      part >> secondcell;
	      if (secondcell == "events</FONT></TD>")
		{
		  type_ = 2;
		}
	      else if (secondcell == "selected")
		{
		  if (secondcell_)
		    {
		      type_ = 3;
		    }
		  else
		    {
		      type_=42;
		    }
		}
	    }

	  if (tmp == "<CAPTION") 
	    {
	      inTable_ = false;
	      toRead_  = false;
	    }
	  
	  if (toRead_)
	    {
	      line.erase(0,44);
 	      line.erase(line.size()-13,13);
	      if (type_==0)
		{
		  if (cellnum_==0) 
		    {
		      report.sample_files_.push_back(line);
		      cellnum_++;
		      continue;
		    }
		  if (cellnum_ ==1)
		    {
		      report.nofgenevents_.push_back(line);
		      cellnum_++;
		      continue;
		    }
		  if (cellnum_ ==2)
		    {
		      report.cross_section_.push_back(line);
		      cellnum_=0;
		    }
		}
	      if (type_==2)
		{
		  if (cellnum_==0) 
		    {
		      report.datasets_.push_back(line);
		      cellnum_++;
		      continue;
		    }
		  if (cellnum_ ==1)
		    {
		      report.nofrealevents_.push_back(line);
		      cellnum_++;
		      continue;
		    }
		  if (cellnum_ ==2)
		    {
		      report.means_.push_back(line);
		      cellnum_++;
		      continue;
		    }
		  if (cellnum_ ==3)
		    {
		      report.rms_.push_back(line);
		      cellnum_++;
		      continue;
		    }
		  if (cellnum_ ==4)
		    {
		      report.uflow_.push_back(line);
		      cellnum_++;
		      continue;
		    }
		  if (cellnum_ ==5)
		    {
		      report.oflow_.push_back(line);
		      cellnum_=0;
		    }
		}
	      if (type_==3)
		{
		  secondcell_ = false;
		  if (cellnum_==0)
                    {
                      report.datasets2_.push_back(line);
                      cellnum_++;
                      continue;
                    }
                  if (cellnum_ ==1)
                    {
                      report.selected_.push_back(line);
                      cellnum_++;
                      continue;
                    }
                  if (cellnum_ ==2)
                    {
                      report.rejected_.push_back(line);
                      cellnum_++;
                      continue;
                    }
                  if (cellnum_ ==3)
                    {
                      report.sel_selprej_.push_back(line);
                      cellnum_++;
                      continue;
                    }
                  if (cellnum_ ==4)
                    {
                      report.sel_nofevents_.push_back(line);
                      cellnum_=0;
                    }
		}
	      if (type_==4)
		{
		  if (cellnum_==0)
                    {
                      report.cutname_.push_back(line);
                      cellnum_++;
                      continue;
                    }
                  if (cellnum_ ==1)
                    {
                      report.signalevents_.push_back(line);
                      cellnum_++;
                      continue;
                    }
                  if (cellnum_ ==2)
                    {
                      report.bckgdevents_.push_back(line);
                      cellnum_++;
                      continue;
                    }
                  if (cellnum_ ==3)
                    {
                      report.sbratio_.push_back(line);
                      cellnum_++;
		      cellnum_=0;
                    }
		}

	    }
	}	
    }
}

void HTMLcomparator::compareReports(HTMLReport & report1, HTMLReport & report2)
{
  reports_.push_back(&report1);
  reports_.push_back(&report2);
  

  //Checking if versions are different
  if(report1.version == report2.version)
    {
      WARNING << "The two reports have the same MadAnalysis version : \n" 
	      << report1.version << "\n" 
	      << std::endl;
    }
  else
    {
      INFO << "Check for Madanalysis Upgrade :\n" 
	   << "\033[0;34;40m"
	   << report1.version
	   << "\033[0m"
	   << " ---> " 
	   << "\033[0;34;40m"
	   << report2.version 
	   << "\033[0m"
	   << "\n" << std::endl;
    }
  
  //Compare the events table
  INFO << "Checking Datasets Table \n"
       << "------------------------ \n" 
       << std::endl;
  if(report1.sample_files_.size() != report2.sample_files_.size())
    {
      bug = true;
      ERROR << "The Datasets Tables are different! \n " << "Identification : number of sample files different.\n" << std::endl;
    }
  
  for (int i=0; i<report1.sample_files_.size();i++)
    {
      /*if (report1.sample_files_[i] != report2.sample_files_[i])
	{
	  bug = true;
	  ERROR << "The Datasets Tables are different! \n " << "Identification : name of the sample files are not the same.\n"
		<<  "Type return to continue. \n" << std::endl;
	  std::cin.ignore() ;
	  }*/
  
      if (report1.nofgenevents_[i] != report2.nofgenevents_[i])
	{
	  bug = true;
	  ERROR << "The Datasets Tables are different!\n "
		<< "Identification : number of events different" << "\n" 
		<<  "Sample file report 1:\t"<< report1.sample_files_[i]  <<  "\n" 
		<< "\t # events : " << report1.nofgenevents_[i] << "\n"
		<<  "Sample file report 2:\t"<< report2.sample_files_[i]  << "\n"
		<< "\t # events : " << report2.nofgenevents_[i] << "\n"
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
      else if (report1.cross_section_[i] != report2.cross_section_[i])
	{
	  bug = true;
	  ERROR << "The Datasets Tables are different! " << "\n"
		<< "Identification : cross sections are different" << "\n"
		<<  "Sample file report 1:\t"<< report1.sample_files_[i]  << "\n"
		<< "\t cross section  : " << report1.cross_section_[i] << "\n"
		<<  "Sample file report 2:\t"<< report2.sample_files_[i]  << "\n"
		<< "\t cross section : " <<  report2.cross_section_[i] << "\n" 
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
    }
  if(!bug) 
    {
      DEBUG << "Ok.\n" << std::endl;
    }
  
  //Checking the histogram statistics table
  INFO << "Checking Statistics Tables \n"
       << "--------------------------- \n"
       << std::endl;

  bug=false;
  
  for (int i=0; i<report1.datasets_.size();i++)
    {
      if(report1.datasets_.size() != report2.datasets_.size())
	{
	  errors_.push_back(i);
	  bug = true;
	  ERROR << "The Histogram "<< i+1 <<" Statistics Tables are different! " << "\n"
		<< "Identification : different number of dataset .\n" 
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
      
      if (report1.nofrealevents_[i] != report2.nofrealevents_[i])
	{
	  errors_.push_back(i);
	  bug = true;
	  ERROR << "The Histogram "<< i+1 <<" Statistics Tables are different! " << "\n"
		<< "Identification : different number of real events." << "\n"
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
      
      if (report1.means_[i] != report2.means_[i])
	{
	  errors_.push_back(i);
	  bug = true;
	  ERROR << "The Histogram "<< i+1 <<" Statistics Tables are different! " << "\n"
		<< "Identification : different means" << "\n"
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
      
      if (report1.rms_[i] != report2.rms_[i])
	{
	  errors_.push_back(i);
	  bug = true;
	  ERROR << "The Histogram "<< i+1 <<" Statistics Tables are different! " << "\n"
		<< "Identification : different rms" << "\n"
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
    }
  if(!bug)
    {
      DEBUG << "Ok.\n" << std::endl;
    }
  else
    {
      INFO << "Displaying datasets tables : \n" 
	   << "---------------------------- \n"
	   << std::endl;
      
      displayStatisticsTable(report1, errors_);
      displayStatisticsTable(report2, errors_);

    }

  //Checking the cuts tables
  INFO << "Checking Cuts Tables \n"
       << "-------------------- \n"
       << std::endl;
  bug=false;
  
  for (int i=0; i<report1.datasets2_.size();i++)
    {
      if(report1.datasets2_.size() != report2.datasets2_.size())
	{
	  errors2_.push_back(i);
	  bug = true;
	  ERROR << "The Cut "<< i+1 <<" Tables are different! " << "\n"
		<< "Identification : different numbers of dataset .\n" 
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
      
      if (report1.selected_[i] != report2.selected_[i])
	{
	  errors2_.push_back(i);
	  bug = true;
	  ERROR << "The Cuts "<< i+1 <<" Tables are different! " << "\n"
		<< "Identification : different numbers of selected events." << "\n"
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
      
      if (report1.rejected_[i] != report2.rejected_[i])
	{
	  errors2_.push_back(i);
	  bug = true;
	  ERROR << "The Cuts "<< i+1 <<" Tables are different! " << "\n"
		<< "Identification : different numbers of rejected events" << "\n"
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
      
      if (report1.sel_selprej_[i] != report2.sel_selprej_[i])
	{
	  errors2_.push_back(i);
	  bug = true;
	  ERROR << "The Cuts "<< i+1 <<" Tables are different! " << "\n"
		<< "Identification : different numbers of sel/sel+rej events" << "\n"
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
      if (report1.sel_nofevents_[i] != report2.sel_nofevents_[i])
	{
	  errors2_.push_back(i);
	  bug = true;
	  ERROR << "The Cuts "<< i+1 <<" Tables are different! " << "\n"
		<< "Identification : different numbers of sel/# events" << "\n"
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
    }
  if(!bug)
    {
      DEBUG << "Ok.\n" << std::endl;
    }
  else
    {
      INFO << "Displaying Cuts Tables : \n" 
	   << "------------------------ \n"
	   << std::endl;
      displayCutsTable(report1, errors2_);
      displayCutsTable(report2, errors2_);

    }
  
  //Checking the signal background tables
  INFO << "Checking Signal/Background Tables \n"
       << "--------------------------------- \n"
       << std::endl;

  bug=false;
  
  for (int i=0; i<report1.cutname_.size();i++)
    {
      if(report1.cutname_.size() != report2.cutname_.size())
	{
	  errors3_.push_back(i);
	  bug = true;
	  ERROR << "The Signal/Background Tables are different! " << "\n"
		<< "Identification : different numbers of cuts .\n" 
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
      
      if (report1.signalevents_[i] != report2.signalevents_[i])
	{
	  errors3_.push_back(i);
	  bug = true;
	  ERROR << "The Signal/Background Tables are different! " << "\n"
		<< "Identification : different numbers of signal events." << "\n"
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
      
      if (report1.bckgdevents_[i] != report2.bckgdevents_[i])
	{
	  errors3_.push_back(i);
	  bug = true;
	  ERROR << "The Signal/Background Tables are different! " << "\n"
		<< "Identification : different numbers of background events" << "\n"
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
      
      if (report1.sbratio_[i] != report2.sbratio_[i])
	{
	  errors3_.push_back(i);
	  bug = true;
	  ERROR << "The Signal/Background Tables are different! " << "\n"
		<< "Identification : different numbers of Signal/Background ratios" << "\n"
		<< "Type return to continue. \n" << std::endl;
    //	  std::cin.ignore() ;
	}
    }
  if(!bug)
    {
      DEBUG << "Ok.\n" << std::endl;
    }
 else
    {
      INFO << "Displaying S/B Tables : \n" 
	   << "----------------------- \n"
	   << std::endl;

      displaySBTable(report1, errors3_);
      displaySBTable(report2, errors3_);

    }
 
}

void HTMLcomparator::displayStatisticsTable(HTMLReport & Report, std::vector<int> Error)
{
  int muf =0;
  std::cout << std::endl;
  std::cout << "Table of " << Report.reportname << " :" << std::endl;
  std::cout << std::setfill('-') << std::setw(82) << "-" << std::setfill(' ') << std::endl;
  std::cout << std::setw(35) << std::left << "|Datasets" 
	    << std::setw(18) << std::left << "|# events" 
	    << std::setw(18) << std::left << "|Mean " 
	    << std::setw(10) << std::left << "|RMS" << "|" 
	    << std::endl;
  std::cout << std::setfill('-') << std::setw(82) << "-" << std::setfill(' ') << std::endl;
  
  for (int j=0; j<Report.datasets_.size();j++)
    {
      muf=0;
      for (int k=0; k<Error.size(); k++)
	{
	  if (Error[k]==j)
	    {
	      muf=1;
	      std::cout << "\033[0;31;40m" 
			<< "|" << std::setw(34) << std::left << Report.datasets_[j] 
			<< "|" << std::setw(17) << std::left << Report.nofrealevents_[j] 
			<< "|" << std::setw(17) << std::left << Report.means_[j] 
			<< "|" << std::setw(9) << std::left << Report.rms_[j]
			<< "|" << "\033[0m"
			<< std::endl;
	    }
	}
      if (muf==0)
	{
	  std::cout << "|" << std::setw(34) << std::left << Report.datasets_[j] 
		    << "|" << std::setw(17) << std::left << Report.nofrealevents_[j] 
		    << "|" << std::setw(17) << std::left << Report.means_[j] 
		    << "|" << std::setw(9) << std::left << Report.rms_[j]
		    << "|" << std::endl;  
	}

    }
  std::cout << std::setfill('-') << std::setw(82) << "-" << std::setfill(' ') << std::endl;
  std::cout << std::endl;
}

void HTMLcomparator::displayCutsTable(HTMLReport & Report, std::vector<int> Error)
{
  int muf =0;
  std::cout << std::endl;
  std::cout << "Table of " << Report.reportname << " :" << std::endl;
  std::cout << std::setfill('-') << std::setw(120) << "-" << std::setfill(' ') << std::endl;
  std::cout << std::setw(25) << std::left << "|Datasets" 
	    << std::setw(28) << std::left << "|# sel. events" 
	    << std::setw(18) << std::left << "|# rej. events" 
	    << std::setw(10) << std::left << "|# sel. / # sel. + rej." 
	    << std::setw(10) << std::left << "|# sel. / # sample events" << "|" 
	    << std::endl;
  std::cout << std::setfill('-') << std::setw(120) << "-" << std::setfill(' ') << std::endl;
  
  for (int j=0; j<Report.datasets2_.size();j++)
    {
      muf=0;
      for (int k=0; k<Error.size(); k++)
	{
	  if (Error[k]==j)
	    {
	      muf=1;
	      std::cout << "\033[0;31;40m" 
			<< "|" << std::setw(24) << std::left << Report.datasets2_[j] 
			<< "|" << std::setw(27) << std::left << Report.selected_[j] 
			<< "|" << std::setw(17) << std::left << Report.rejected_[j] 
			<< "|" << std::setw(22) << std::left << Report.sel_selprej_[j]
			<< "|" << std::setw(24) << std::left << Report.sel_nofevents_[j]
			<< "|" << "\033[0m"
			<< std::endl;
	    }
	}
      if (muf==0)
	{
	  std::cout << "|" << std::setw(24) << std::left << Report.datasets2_[j] 
		    << "|" << std::setw(27) << std::left << Report.selected_[j] 
		    << "|" << std::setw(17) << std::left << Report.rejected_[j] 
		    << "|" << std::setw(22) << std::left << Report.sel_selprej_[j]
		    << "|" << std::setw(24) << std::left << Report.sel_nofevents_[j]
		    << "|" << std::endl;  
	}

    }
  std::cout << std::setfill('-') << std::setw(120) << "-" << std::setfill(' ') << std::endl;
  std::cout << std::endl;
}

void HTMLcomparator::displaySBTable(HTMLReport & Report, std::vector<int> Error)
{
  int muf =0;
  std::cout << std::endl;
  std::cout << "Table of " << Report.reportname << " :" << std::endl;
  std::cout << std::setfill('-') << std::setw(82) << "-" << std::setfill(' ') << std::endl;
  std::cout << std::setw(25) << std::left << "|Cuts" 
	    << std::setw(28) << std::left << "|# signal events" 
	    << std::setw(18) << std::left << "|# bckgd events" 
	    << std::setw(10) << std::left << "|# S/B"  << "|"
	    << std::endl;
  std::cout << std::setfill('-') << std::setw(82) << "-" << std::setfill(' ') << std::endl;
  
  for (int j=0; j<Report.cutname_.size();j++)
    {
      muf=0;
      for (int k=0; k<Error.size(); k++)
	{
	  if (Error[k]==j)
	    {
	      muf=1;
	      std::cout << "\033[0;31;40m" 
			<< "|" << std::setw(24) << std::left << Report.cutname_[j] 
			<< "|" << std::setw(27) << std::left << Report.signalevents_[j] 
			<< "|" << std::setw(17) << std::left << Report.bckgdevents_[j] 
			<< "|" << std::setw(9) << std::left << Report.sbratio_[j]
			<< "|" << "\033[0m"
			<< std::endl;
	    }
	}
      if (muf==0)
	{
	  std::cout << "|" << std::setw(24) << std::left << Report.cutname_[j] 
		    << "|" << std::setw(27) << std::left << Report.signalevents_[j] 
		    << "|" << std::setw(17) << std::left << Report.bckgdevents_[j] 
		    << "|" << std::setw(9) << std::left << Report.sbratio_[j]
		    << "|" << std::endl;  
	}

    }
  std::cout << std::setfill('-') << std::setw(82) << "-" << std::setfill(' ') << std::endl;
  std::cout << std::endl;
}
