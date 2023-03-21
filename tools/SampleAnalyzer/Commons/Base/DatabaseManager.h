#pragma once


#include <iostream>

class SQLiteBase;

class DatabaseManager {
	private:

		SQLiteBase *manager;

	public:
		DatabaseManager(std::string path);

		~DatabaseManager();

		void createCutflowTables();

		void createHistoTables();

		void createWeightNamesTable();

		void addWeightDefinition(int id, std::string def);

		void addHisto(std::string name, int bins, double xmin, double xmax, std::string regions);

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
				double neg_val2_weight);

		void addData(std::string name, int id, std::string bin, double positive, double negative);

		void addCut(std::string r_name, std::string c_name);

		void addWeight(std::string r_name, std::string c_name, int id, int pos, int neg, double pos_sum, double neg_sum, double pos_2sum, double neg_2sum);

		void closeDB();
};
