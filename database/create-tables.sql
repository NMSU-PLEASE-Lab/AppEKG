/*

To create and/or connect to database, run:
$ sqlite3 appekg.db

After connected to database, run this command to create all tables:
$ .read create-tables.sql

Alternatively, use command 
$ sqlite3 appekgdb.db ".read insert_data.sql" 
to run the script without connecting to database

*/

CREATE TABLE IF NOT EXISTS Application (
   id INTEGER PRIMARY KEY NOT NULL,
   name TEXT,
   version TEXT
);

CREATE TABLE IF NOT EXISTS Job (
   id INTEGER,
   user INTEGER,
   begin_time INTEGER,
   end_time INTEGER,
   num_nodes INTEGER,
   num_processes INTEGER,
   app_id INTEGER,
   PRIMARY KEY(id),
   FOREIGN KEY(app_id) REFERENCES Application
);

CREATE TABLE IF NOT EXISTS Process (
   id INTEGER PRIMARY KEY AUTOINCREMENT,
   pid INTEGER,
   node_id INTEGER,
   job_id INTEGER,
   rank INTEGER,
   num_threads INTEGER,
   FOREIGN KEY(job_id) REFERENCES Job
);

CREATE TABLE IF NOT EXISTS EKGSample (
   id INTEGER PRIMARY KEY AUTOINCREMENT,
   time INTEGER,
   thread_id INTEGER,
   process_id INTEGER,
   FOREIGN KEY(process_id) REFERENCES Process
);

CREATE TABLE IF NOT EXISTS Heartbeat (
   id INTEGER,
   name TEXT,
   app_id INTEGER,
   PRIMARY KEY(id),
   FOREIGN KEY(app_id) REFERENCES Application
);

CREATE TABLE IF NOT EXISTS HBSample (
   id INTEGER PRIMARY KEY AUTOINCREMENT,
   num_heartbeats INTEGER,
   avg_duration REAL,
   hb_id INTEGER,
   sample_id INTEGER,
   FOREIGN KEY(sample_id) REFERENCES EKGSample,
   FOREIGN KEY(hb_id) REFERENCES Heartbeat
);

