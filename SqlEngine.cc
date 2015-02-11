/**
* Copyright (C) 2008 by The Regents of the University of California
* Redistribution of this file is permitted under the terms of the GNU
* Public License (GPL).
*
* @author Junghoo "John" Cho <cho AT cs.ucla.edu>
* @date 3/24/2008
*/

#include <cstdio>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"

#include "BTreeIndex.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
	fprintf(stdout, "Bruinbase> ");

	// set the command line input and start parsing user input
	sqlin = commandline;
	sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
	// SqlParser.y by bison (bison is GNU equivalent of yacc)

	return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
	RecordFile rf;   // RecordFile containing the table
	RecordId   rid;  // record cursor for table scanning

	RC     rc;
	int    key;     
	string value;
	int    count;
	int    diff;

	// open the table file
	if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
		fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
		return rc;
	}

	// scan the table file from the beginning
	rid.pid = rid.sid = 0;
	count = 0;
	
	//open btreeindex
	BTreeIndex btree;
	RC ecode = btree.open(table + ".idx", 'r');
	
	if(ecode != 0)
	{
		//no index
		rid.pid = 0;
		rid.sid = 0;
		count = 0;
		while (rid < rf.endRid()) {
			// read the tuple
			if ((rc = rf.read(rid, key, value)) < 0) 
			{
				goto exit_select;
			}

			// check the conditions on the tuple
			for (unsigned i = 0; i < cond.size(); i++) {
				// compute the difference between the tuple and the condition value
				switch (cond[i].attr) {
				case 1:
					diff = key - atoi(cond[i].value);
					break;
				case 2:
					diff = strcmp(value.c_str(), cond[i].value);
					break;
				}

				// skip the tuple if any condition is not met
				switch (cond[i].comp) {
				case SelCond::EQ:
					if (diff != 0) goto next_tuple;
					break;
				case SelCond::NE:
					if (diff == 0) goto next_tuple;
					break;
				case SelCond::GT:
					if (diff <= 0) goto next_tuple;
					break;
				case SelCond::LT:
					if (diff >= 0) goto next_tuple;
					break;
				case SelCond::GE:
					if (diff < 0) goto next_tuple;
					break;
				case SelCond::LE:
					if (diff > 0) goto next_tuple;
					break;
				}
			}

			// the condition is met for the tuple. 
			// increase matching tuple counter
			count++;

			// print the tuple 
			switch (attr) {
			case 1:  // SELECT key
				fprintf(stdout, "%d\n", key);
				break;
			case 2:  // SELECT value
				fprintf(stdout, "%s\n", value.c_str());
				break;
			case 3:  // SELECT *
				fprintf(stdout, "%d '%s'\n", key, value.c_str());
				break;
			}

			// move to the next tuple
			next_tuple:
			++rid;
		}
	}
	else//index
	{
		IndexCursor cursor;
		
		int comp = -1;
		
		
		for (unsigned i = 0; i < cond.size(); i++)
		{
			//skipping values
			if (cond[i].attr != 1)
			{
				continue;
			}
			switch(cond[i].comp)
			{
				// The first EQ condition trumps all other conditions
			case SelCond::EQ:
				{
					comp = i;
					break;
				}
				// Determine what range to start from
			case SelCond::GT:
			case SelCond::GE:
				{
					if (comp == -1 || atoi(cond[i].value) > atoi(cond[comp].value))
					{
						comp = i;
					}	
					break;
				}
			}
		}
		if(comp > -1)
		{
			btree.locate(atoi(cond[comp].value), cursor);
		}
		else
		{
			btree.locate(0,cursor);
		}
		
		while(!btree.readForward(cursor,key,rid))
		{
			if((rc = rf.read(rid,key,value)) < 0)
			{
				goto exit_select;
			}
			
			for (unsigned i = 0; i < cond.size(); i++)
			{
				switch (cond[i].attr)
				{
				case 1:
					{
						diff = key - atoi(cond[i].value);
						break;
					}
				case 2:
					{
						diff = strcmp(value.c_str(), cond[i].value);
						break;
					}
				}

				// Determine if condition has been met
				switch (cond[i].comp)
				{
				case SelCond::EQ:
					{
						if (diff != 0 && cond[i].attr == 1)
						{
							goto exit_index;
						}
						else
						{
							continue;
						}
						break;
					}
				case SelCond::NE:
					{
						if (diff == 0)
						{
							continue;
						}
						break;
					}
				case SelCond::GT:
					{
						if (diff <= 0)
						{
							continue;
						}
						break;
					}
				case SelCond::LT:
					{
						if (diff >= 0 && cond[i].attr == 1)
						{
							goto exit_index;
						}
						else
						{
							continue;
						}
						break;
					}
				case SelCond::GE:
					{
						if(diff < 0)
						continue;
						break;
					}
				case SelCond::LE:
					{
						if (diff > 0 && cond[i].attr == 1)
						{
							goto exit_index;
						}
						else
						{
							continue;
						}
						break;
					}
				}
			}
			count++;
			switch (attr) 
			{
			case 1:  // SELECT key
				fprintf(stdout, "%d\n", key);
				break;
			case 2:  // SELECT value
				fprintf(stdout, "%s\n", value.c_str());
				break;
			case 3:  // SELECT *
				rf.read(rid, key, value);
				fprintf(stdout, "%d '%s'\n", key, value.c_str());
				break;
			}
		}
	}
	exit_index:
	if (attr == 4) {
		fprintf(stdout, "%d\n", count);
	}
	rc = 0;

	// close the table file and return
	exit_select:
	rf.close();
	return rc;
}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
	ifstream file;
	file.open(loadfile.c_str());
	if(file.fail())
	{
		file.close();
		return RC_FILE_OPEN_FAILED;
	}
	
	RecordFile rf;
	rf.open(table + ".tbl", 'w');
	
	BTreeIndex btree;
	if(index)
	{

		btree.open(table + ".idx", 'w');

	}

	
	int key;
	string value;
	string line;
	RecordId rid;

	while(!file.eof()) {
		getline(file, line);
		if(line != "")
		{
			parseLoadLine(line, key, value);		
			rf.append(key, value, rid);
			//insert to b+ tree
			if(index)
			{
				btree.insert(key, rid);
			}
		}
	}
	
	
	if(index)
	{
		btree.close();
	}
	rf.close();
	file.close();
	return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
	const char *s;
	char        c;
	string::size_type loc;
	
	// ignore beginning white spaces
	c = *(s = line.c_str());
	while (c == ' ' || c == '\t') { c = *++s; }

	// get the integer key value
	key = atoi(s);

	// look for comma
	s = strchr(s, ',');
	if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

	// ignore white spaces
	do { c = *++s; } while (c == ' ' || c == '\t');
	
	// if there is nothing left, set the value to empty string
	if (c == 0) { 
		value.erase();
		return 0;
	}

	// is the value field delimited by ' or "?
	if (c == '\'' || c == '"') {
		s++;
	} else {
		c = '\n';
	}

	// get the value string
	value.assign(s);
	loc = value.find(c, 0);
	if (loc != string::npos) { value.erase(loc); }

	return 0;
}
