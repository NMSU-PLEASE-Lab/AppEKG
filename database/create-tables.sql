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
   id INTEGER PRIMARY KEY NOT NULL,
   user INTEGER,
   beginTime INTEGER, /* should this be a text? */
   endTime INTEGER,
   numNodes INTEGER,
   numProcesses INTEGER,
   appId INTEGER
);

CREATE TABLE IF NOT EXISTS Process (
   pid INTEGER,
   nodeId INTEGER,
   jobId INTEGER,
   rank INTEGER,
   numThreads INTEGER,
   PRIMARY KEY(pid, nodeId, jobId) /* iKey int */
);

CREATE TABLE IF NOT EXISTS EKGSample (
   time INTEGER,
   threadId INTEGER,
   processKey INTEGER,
   PRIMARY KEY(threadId, processKey) /* iKey int */
);

CREATE TABLE IF NOT EXISTS Heartbeat (
   id INTEGER,
   name TEXT,
   appId INTEGER,
   PRIMARY KEY(id, appId) /* iKey int */
);

CREATE TABLE IF NOT EXISTS HBSample (
   id INTEGER PRIMARY KEY AUTOINCREMENT,
   numHeartbeats INTEGER,
   avgDuration REAL,
   hbKey INTEGER,
   sampleKey INTEGER
);

