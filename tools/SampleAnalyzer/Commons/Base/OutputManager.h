#pragma once

#include <vector>
#include <array>
#include <string>


#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"
#include "SampleAnalyzer/Commons/DataFormat/SampleFormat.h"

//include all output type interfaces here
#include "SampleAnalyzer/Commons/Base/DatabaseManager.h"


using namespace std;
using namespace MA5;



//output manager object initializer takes in a AnalyzerBase pointer and a vector of arrays of size 3
//each array constains [output type name, path to histogram file, path to cutflow file]
class OutputManager {
	private:
		//base pointer type OutputBase can take on any output interface, it's only purpose is to provide 
		//and execute function, new output formats require implementation of the output interface
		vector<array<string, 3> > output_types_;
		AnalyzerBase *analyzer_;
		SampleFormat *samples_;
	
	public:
		OutputManager(vector<array<string, 3> > output_types, AnalyzerBase *analyzer, SampleFormat *samples) : output_types_(output_types), analyzer_(analyzer), samples_(samples) {}
		//for each output type, get output object from factory and call execute on it's interface
		void Execute() {
			
			//implement each individual output type here
			for(int i = 0; i < output_types_.size(); ++i){

				//implementaton for SQLite
				if(output_types_[i][0] == "sqlite"){
					 DatabaseManager cutflow(output_types_[i][1]);
					 DatabaseManager histogram(output_types_[i][2]);

					 histogram.createHistoTables();
					 analyzer_->Manager()->GetPlotManager()->WriteSQL(histogram);
					 samples_->mc()->WriteWeightNames(histogram);
					 histogram.closeDB();

					 cutflow.createCutflowTables();	
					 bool addInitial = true;
					 samples_->mc()->WriteWeightNames(cutflow);
					 for(int i = 0; i < analyzer_->Manager()->Regions().size(); ++i){
						 analyzer_->Manager()->Regions()[i]->WriteSQL(cutflow, addInitial);
						
					 }
					 cutflow.closeDB();
					
				}
			}
		}
};
				
							
		



