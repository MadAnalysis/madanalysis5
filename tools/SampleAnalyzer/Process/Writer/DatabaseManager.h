#pragma once


#include <iostream>
#include <string>
#include <sqlite3.h>

using namespace std;

class DatabaseManager {

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
		static int callback(void *NotUsed, int argc, char **argv, char **azColName) {

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

		bool checkDBErrors(string msg) {

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


	public:

		DatabaseManager(string path) {
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

		void createTables() {

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
	
		//add cut to databse with primary keys region name and cut name
		void addCut(string r_name, string c_name) {

	
			sql = "INSERT INTO Cutflow VALUES ('" + r_name + "'" + "," + "'" + c_name + "')";	
			//cout << sql << endl;
			rc = sqlite3_exec(db, sql.c_str(), callback, 0 , &zErrMsg);
			checkDBErrors("inserting cutflow: " + r_name + " " + c_name);
				    
		}

		//add weight to database with primary keys region name, cut name, and weight id
		void addWeight(string r_name, string c_name, int id, int pos, int neg, double pos_sum, double neg_sum, double pos_2sum, double neg_2sum) {
			sql = "INSERT INTO Weights VALUES ('" + r_name + "'" + ",'" + c_name + "','" + to_string(id) + "','" + to_string(pos) + "','" + to_string(neg) + "','" + to_string(pos_sum) + "','" + to_string(neg_sum) \
				   + "','" + to_string(pos_2sum) + "','" + to_string(neg_2sum) + "')";
			//cout << sql << endl;
			rc = sqlite3_exec(db, sql.c_str(), callback, 0 , &zErrMsg);
			checkDBErrors("inserting weight values: " + r_name + " " + c_name + " weight ID: " + to_string(id));
		
		}

		void closeDB() {

			// Close the SQL connection
    		sqlite3_close(db);

		}
	


};

/*
int main() {
	DatabaseManager dbManager("test.db");
	dbManager.createTables();
	dbManager.addCut("region1", "cut1");
	dbManager.addWeight("region1", "cut1", 1, 2, 2, 2, -2, 4, 4);
	dbManager.addWeight("region1", "cut1", 10, 2, 3, 4, 5, 6, 7);
	dbManager.closeDB();

}

*/



