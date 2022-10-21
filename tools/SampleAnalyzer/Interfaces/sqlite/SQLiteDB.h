#pragma once

#include <iostream>
#include <string>
#include <sqlite3.h>

#include "SampleAnalyzer/Commons/Base/SQLbase.h"

using namespace std;

class SQLiteDB : public SQLiteBase {
	private:
		// Pointer to SQLite connection
    	sqlite3 *db;

    	// Save any error messages
    	char *zErrMsg;

    	// Save the result of opening the file
    	int rc;

    	// Saved SQL
    	string sql;

    	// Create a callback function  
		static int callback(void *NotUsed, int argc, char **argv, char **azColName);

		bool checkDBErrors(string msg);

	public:

		SQLiteDB(string path);

		void createCutflowTables();

		void createHistoTables();

		void createWeightNamesTable();

		void addWeightDefinition(int id, string def);
		
		void addHisto(string name, int bins, double xmin, double xmax, string regions);
		
		void addStatistic(string name,
				int id,
				int pos_events, 
				int neg_events,
				double pos_sum_events,
				double neg_sum_events,
				int pos_entries,
				int neg_entries,
				double pos_sum_entries,
				double neg_sum_entries,
				double pos_sum_squared,
				double neg_sum_squared,
				double pos_val_weight,
				double neg_val_weight,
				double pos_val2_weight,
				double neg_val2_weight);

		void addData(string name, int id, string bin, double positive, double negative);
			
		void addCut(string r_name, string c_name);

		void addWeight(string r_name, string c_name, int id, int pos, int neg, double pos_sum, double neg_sum, double pos_2sum, double neg_2sum);
			
		void closeDB();
	
};

