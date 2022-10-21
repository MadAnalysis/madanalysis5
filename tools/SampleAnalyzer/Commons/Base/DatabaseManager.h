#pragma once


#include <iostream>
#include "SampleAnalyzer/Commons/Base/SQLbase.h"

#ifdef SQLITE3_USE
	#include "SampleAnalyzer/Interfaces/sqlite/SQLiteDB.h"
#endif

class DatabaseManager {
	private:
		SQLiteBase *manager;

	public:
		DatabaseManager(std::string path) {
			#ifdef SQLITE3_USE
				manager = new SQLiteDB(path);
			#else 
				manager = new SQLiteBase(path);
			#endif
		}

		~DatabaseManager() {
			delete manager;
		}

		void createCutflowTables() {
			manager->createCutflowTables();
		}

		void createHistoTables() {
			manager->createHistoTables();
		}

		void createWeightNamesTable() {
			manager->createWeightNamesTable();
		}

		void addWeightDefinition(int id, std::string def) {
			manager->addWeightDefinition(id, def);
		}

		void addHisto(std::string name, int bins, double xmin, double xmax, std::string regions) {
			manager->addHisto(name, bins, xmin, xmax, regions);
		}

		void addStatistic(std::string name,
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
				double neg_val2_weight) {
			manager->addStatistic(name,id,pos_events, neg_events, pos_sum_events, neg_sum_events, pos_entries, neg_entries, pos_sum_entries, neg_sum_entries, pos_sum_squared, neg_sum_squared, pos_val_weight, neg_val_weight, pos_val2_weight, neg_val2_weight);
		}

		void addData(std::string name, int id, std::string bin, double positive, double negative) {
			manager->addData(name, id, bin, positive, negative);
		}

		void addCut(std::string r_name, std::string c_name) {
			manager->addCut(r_name, c_name);
		}

		void addWeight(std::string r_name, std::string c_name, int id, int pos, int neg, double pos_sum, double neg_sum, double pos_2sum, double neg_2sum) {
			manager->addWeight(r_name,c_name, id, pos, neg, pos_sum, neg_sum, pos_2sum, neg_2sum);
		}

		void closeDB() {
			manager->closeDB();
		}


};
