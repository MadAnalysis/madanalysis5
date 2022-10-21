#include <iostream>
#include <string>
#include <sqlite3.h>

#include "SampleAnalyzer/Interfaces/sqlite/SQLiteDB.h"


using namespace std;

int SQLiteDB::callback(void *NotUsed, int argc, char **argv, char **azColName) {

		    // int argc: holds the number of results
		    // (array) azColName: holds each column returned
		    // (array) argv: holds each value

		    for(int i = 0; i < argc; i++) {
		        
		        // Show column name, value, and newline
		        cout << azColName[i] << ": " << argv[i] << endl;
		    
		    }

		    // Insert a newline
		    cout << endl;

		    // Return successful
		    return 0;
}

bool SQLiteDB::checkDBErrors(string msg){
	if( rc ){
		 // Show an error message
		cout << "DB Error: " << sqlite3_errmsg(db) << " " << msg << endl;
		return false;        
	}
	else {
		cout << " success @ " << msg << endl;
		return true;
	}

}

SQLiteDB::SQLiteDB(string path) : SQLiteBase(path){
	// Save the result of opening the file
    rc = sqlite3_open(path.c_str(), &db);
	// enable foreign key constraint
	sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0 ,0 ,0);
    bool open_success = checkDBErrors("opening DB");
	if(!open_success) {
		sqlite3_close(db);
		cout << "open DB failed!" << endl;
	}
}

void SQLiteDB::createCutflowTables() {

		// Save SQL to create a table
		sql = "CREATE TABLE IF NOT EXISTS Cutflow(" \
				"region_name TEXT NOT NULL,"\
				"cut_name TEXT NOT NULL," \
				"primary key (region_name, cut_name));";
	
		// Run the SQL
	    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
		checkDBErrors("creating cutflow table");

		sql = "CREATE TABLE IF NOT EXISTS Weights(" \
				"r_name TEXT NOT NULL," \
				"c_name TEXT NOT NULL," \
				"id INTEGER NOT NULL," \
				"pos_entries INTEGER NOT NULL," \
				"neg_entries INTEGER NOT NULL," \
				"pos_sum DOUBLE NOT NULL," \
				"neg_sum DOUBLE NOT NULL," \
				"pos_squared_sum DOUBLE NOT NULL,"\
				"neg_squared_sum DOUBLE NOT NULL,"\
				"primary key (r_name, c_name, id)" \
				"foreign key (r_name, c_name) references Cutflow(region_name, cut_name) ON DELETE CASCADE);";

		rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
		checkDBErrors("creating weights table");
}

void SQLiteDB::createHistoTables(){
			sql = "CREATE TABLE IF NOT EXISTS HistoDescription("\
				   "name TEXT NOT NULL,"\
				   "num_of_bins INTEGER NOT NULL,"\
				   "xmin DOUBLE NOT NULL,"\
				   "xmax DOUBLE NOT NULL,"\
				   "regions TEXT NOT NULL,"\
				   "primary key(name) );";
			rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
			checkDBErrors("creating HistoDescription table");

			sql = "CREATE TABLE IF NOT EXISTS Statistics("\
				   "name TEXT NOT NULL,"\
				   "id TEXT NOT NULL,"\
				   "pos_num_events INTEGER NOT NULL,"\
				   "neg_num_events INTEGER NOT NULL,"\
				   "pos_sum_event_weights_over_events DOUBLE NOT NULL,"\
				   "neg_sum_event_weights_over_events DOUBLE NOT NULL,"\
				   "pos_entries INTEGER NOT NULL,"\
				   "neg_entries INTEGER NOT NULL,"\
				   "pos_sum_event_weights_over_entries DOUBLE NOT NULL,"\
				   "neg_sum_event_weights_over_entries DOUBLE NOT NULL,"\
				   "pos_sum_squared_weights DOUBLE NOT NULL,"\
				   "neg_sum_squared_weights DOUBLE NOT NULL,"\
				   "pos_value_times_weight DOUBLE NOT NULL,"\
				   "neg_value_times_weight DOUBLE NOT NULL,"\
				   "pos_value_squared_times_weight DOUBLE NOT NULL,"\
				   "neg_value_squared_times_weight DOUBLE NOT NULL,"\
				   "primary key(name, id) );";
				   
			rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
			checkDBErrors("creating Statistics table");

			sql = "CREATE TABLE IF NOT EXISTS Data("\
				   "name TEXT NOT NULL,"\
				   "id INTERGER NOT NULL,"\
				   "bin TEXT NOT NULL,"\
				   "positive DOUBLE NOT NULL,"\
				   "negative DOUBLE NOT NULL,"\
				   "primary key (name, id, bin) );";
				   
			rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
			checkDBErrors("creating Data table");

		}

		void SQLiteDB::createWeightNamesTable(){
			sql = "CREATE TABLE IF NOT EXISTS WeightDefinition("\
				   "id INTERGER NOT NULL,"\
				   "definition TEXT NOT NULL,"\
				   "primary key (id) );";
			rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
			checkDBErrors("creating Weight Names table");
		}

		void SQLiteDB::addWeightDefinition(int id, string def){
			sql = "INSERT INTO WeightDefinition VALUES ('" + to_string(id) + "','" + def + "')";
			rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
		}

		void SQLiteDB::addHisto(string name, int bins, double xmin, double xmax, string regions){
			sql = "INSERT INTO HistoDescription VALUES ('" + name + "'" + "," + "'" + to_string(bins) + "'" + "," + "'" + to_string(xmin) + "'" + "," + "'" + to_string(xmax) + "'" + "," + "'" + regions + "')";
			rc = sqlite3_exec(db, sql.c_str(), callback, 0 , &zErrMsg);
			//checkDBErrors("inserting into Histo: " + name);
		}

		void SQLiteDB::addStatistic(string name,
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
				double neg_val2_weight){

			sql = "INSERT INTO Statistics VALUES ('" + name + "','" + to_string(id) + "','" + to_string(pos_events) + "','" + to_string(neg_events) + "','" + to_string(pos_sum_events) + "','" + to_string(neg_sum_events) + "','" + to_string(pos_entries) + "','" + to_string(neg_entries) + "','" + to_string(pos_sum_entries) + "','" + to_string(neg_sum_entries) + "','" + to_string(pos_sum_squared) + "','" + to_string(neg_sum_squared) + "','" + to_string(pos_val_weight) + "','" + to_string(neg_val_weight) + "','" + to_string(pos_val2_weight) + "','" + to_string(neg_val2_weight) + "')"; 

			rc = sqlite3_exec(db, sql.c_str(), callback, 0 , &zErrMsg);
			//checkDBErrors("inserting into Statistics: " + name);
			
		}

		void SQLiteDB::addData(string name, int id, string bin, double positive, double negative){
			sql = "INSERT INTO Data VALUES ('" + name + "'" + "," + "'" + to_string(id) + "'" + "," + "'" + bin + "'" + "," + "'" + to_string(positive) + "'" + "," + "'" + to_string(negative) + "')";
			rc = sqlite3_exec(db, sql.c_str(), callback, 0 , &zErrMsg);
			//checkDBErrors("inserting into Data: " + name + " " + to_string(id));

		}
	
		//add cut to databse with primary keys region name and cut name
		void SQLiteDB::addCut(string r_name, string c_name) {

	
			sql = "INSERT INTO Cutflow VALUES ('" + r_name + "'" + "," + "'" + c_name + "')";	
			//cout << sql << endl;
			rc = sqlite3_exec(db, sql.c_str(), callback, 0 , &zErrMsg);
			//checkDBErrors("inserting cutflow: " + r_name + " " + c_name);
				    
		}

		//add weight to database with primary keys region name, cut name, and weight id
		void SQLiteDB::addWeight(string r_name, string c_name, int id, int pos, int neg, double pos_sum, double neg_sum, double pos_2sum, double neg_2sum) {
			sql = "INSERT INTO Weights VALUES ('" + r_name + "'" + ",'" + c_name + "','" + to_string(id) + "','" + to_string(pos) + "','" + to_string(neg) + "','" + to_string(pos_sum) + "','" + to_string(neg_sum) \
				   + "','" + to_string(pos_2sum) + "','" + to_string(neg_2sum) + "')";
			//cout << sql << endl;
			rc = sqlite3_exec(db, sql.c_str(), callback, 0 , &zErrMsg);
			//checkDBErrors("inserting weight values: " + r_name + " " + c_name + " weight ID: " + to_string(id));
		
		}

		void SQLiteDB::closeDB() 
		
		{

			// Close the SQL connection
    		sqlite3_close(db);

		}



