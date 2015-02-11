/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "Bruinbase.h"
#include "SqlEngine.h"
/*
#include "PageFile.h"
#include "BTreeNode.h"
#include "BTreeIndex.h"
#include "RecordFile.h"

#include <iostream>

using namespace std;
*/
int main()
{
	
	SqlEngine::run(stdin);
	return 0;

/*
	RC ecode;
	BTLeafNode ln;
	BTLeafNode sln;
	int skey;
	int key=1;
	RecordId rid = {2,3};
	for(int i = 1; i <= 100; i++)
	{
		ecode = ln.insert(key,rid);
		if(ecode != 0)
			ln.insertAndSplit(key, rid, sln, skey);
		key++;
		rid.pid += 1;
		rid.sid += 1;
	}
	int ekey;
	RecordId erid;
	for(int i = 0; i < 100; i++)
	{
		ln.readEntry(i, ekey, erid);
		cout << "pid: " << erid.pid << " sid: " << erid.sid << " key: " << ekey << "\n";	
	}
	return 0;
	*/
	
/*
	BTNonLeafNode node;
	//node.initialize();
	node.initializeRoot(10,2,30);
	node.insert(4,50);
	node.insert(3,90);
	node.insert(9,80);
	node.insert(6,20);
	//node.setFirstPid(99);
	//cout << node.readFirstPid() << endl;
	// node.writeEntry(4,5,1);
	// node.writeEntry(6,7,2);
	// node.writeEntry(8,9,3);
	// node.writeEntry(10,11,4);
	// node.updateNodeKeyCount();
	// node.updateNodeKeyCount();
	// node.updateNodeKeyCount();
	// node.updateNodeKeyCount();
	//int xkey;
	//PageId xpid;
	//node.readEntry(2, xkey,xpid);
	//cout << xkey << "::" << xpid << endl;
	cout << node.gimme(0) << endl;
	cout << node.gimme(1) << endl;
	cout << node.gimme(2) << endl;
	cout << node.gimme(3) << endl;
	cout << node.gimme(4) << endl;
	cout << node.gimme(5) << endl;
	cout << node.gimme(6) << endl;
	cout << node.gimme(7) << endl;
	cout << node.gimme(8) << endl;
	cout << node.gimme(9) << endl;
	cout << node.gimme(10) << endl;
	cout << node.gimme(11) << endl;
	cout << node.gimme(12) << endl;
	cout << node.gimme(13) << endl;
	cout << node.gimme(14) << endl;
	cout << node.gimme(15) << endl;
	cout << node.gimme(16) << endl;
	cout << node.gimme(17) << endl;
	cout << "... \n";
	cout << node.gimme(253) << endl;
	cout << node.gimme(254) << endl;
	cout << node.gimme(255) << endl;
	cout <<"keycount: " << node.getKeyCount() << endl;
	PageId spid;
	cout << node.locateChildPtr(0,spid);
	cout <<"0childptr: " << spid << endl;
	cout << node.locateChildPtr(1,spid);
	cout <<"1childptr: " << spid << endl;
	cout << node.locateChildPtr(2,spid);
	cout <<"2childptr: " << spid << endl;
	cout << node.locateChildPtr(3,spid);
	cout <<"3childptr: " << spid << endl;
	cout << node.locateChildPtr(4,spid);
	cout <<"4childptr: " << spid << endl;
	cout << node.locateChildPtr(5,spid);
	cout <<"5childptr: " << spid << endl;
	cout << node.locateChildPtr(6,spid);
	cout <<"6childptr: " << spid << endl;
	cout << node.locateChildPtr(7,spid);
	cout <<"7childptr: " << spid << endl;
	cout << node.locateChildPtr(8,spid);
	cout <<"8childptr: " << spid << endl;
	cout << node.locateChildPtr(9,spid);
	cout <<"9childptr: " << spid << endl;
	cout << node.locateChildPtr(10,spid);
	cout <<"10childptr: " << spid << endl;
	
	
	
	
	
	
	
	
	
	
	cout << endl;
	cout << endl;
	return 0;
	
*/
	/*
	BTLeafNode node;
	
	RecordId rid1 = {1,2};
	int key1 = 3;
	RecordId rid2 = {4,5};
	int key2 = 6;
	RecordId rid3 = {7,8};
	int key3 = 9;
	RecordId rid4 = {10,11};
	int key4 = 12;
	RecordId rid5 = {13,14};
	int key5 = 15;
	
	//node.initializeLeaf();
	//test one
	int ecode;
	node.writeEntry(rid1, key1, 0);
	node.writeEntry(rid2, key2, 1);
	node.writeEntry(rid3, key3, 2);
	node.writeEntry(rid4, key4, 3);
	node.writeEntry(rid5, key5, 4);
	if((ecode = node.updateNodeKeyCount()) == 0)
		cout << "success \n";
	node.updateNodeKeyCount();
	node.updateNodeKeyCount();
	node.updateNodeKeyCount();
	node.updateNodeKeyCount();
	//cout << node.gimme(-1) << endl;
	cout << node.gimme(0) << endl;
	cout << node.gimme(1) << endl;
	cout << node.gimme(2) << endl;
	cout << node.gimme(3) << endl;
	cout << node.gimme(4) << endl;
	cout << node.gimme(5) << endl;
	cout << node.gimme(6) << endl;
	cout << node.gimme(7) << endl;
	cout << node.gimme(8) << endl;
	cout << node.gimme(9) << endl;
	cout << node.gimme(10) << endl;
	cout << node.gimme(11) << endl;
	cout << node.gimme(12) << endl;
	cout << node.gimme(13) << endl;
	cout << node.gimme(14) << endl;
	cout << node.gimme(15) << endl;
	cout << node.gimme(16) << endl;
	cout << node.gimme(17) << endl;
	cout << node.gimme(18) << endl;
	cout << "... \n";
	cout << node.gimme(252) << endl;
	cout << node.gimme(253) << endl;
	cout << node.gimme(254) << endl;
	cout << node.gimme(255) << endl;
	
	
	//test 2
	
	int resultEid = -1;
	node.locate(0, resultEid);
	
	cout << "0 eid: " << resultEid << endl;
	node.locate(1, resultEid);
	cout << "1 eid: " << resultEid << endl;
	node.locate(2, resultEid);
	cout << "2 eid: " << resultEid << endl;
	node.locate(3, resultEid);
	cout << "3 eid: " << resultEid << endl;
	node.locate(4, resultEid);
	cout << "4 eid: " << resultEid << endl;
	node.locate(5, resultEid);
	cout << "5 eid: " << resultEid << endl;
	node.locate(6, resultEid);
	cout << "6 eid: " << resultEid << endl;
	node.locate(7, resultEid);
	cout << "7 eid: " << resultEid << endl;
	node.locate(8, resultEid);
	cout << "8 eid: " << resultEid << endl;
	node.locate(9, resultEid);
	cout << "9 eid: " << resultEid << endl;
	node.locate(10, resultEid);
	cout << "10 eid: " << resultEid << endl;
	node.locate(11, resultEid);
	cout << "11 eid: " << resultEid << endl;
	node.locate(12, resultEid);
	cout << "12 eid: " << resultEid << endl;
	node.locate(13, resultEid);
	cout << "13 eid: " << resultEid << endl;
	node.locate(14, resultEid);
	cout << "14 eid: " << resultEid << endl;
	node.locate(15, resultEid);
	cout << "15 eid: " << resultEid << endl;
	node.locate(16, resultEid);
	cout << "16 eid: " << resultEid << endl;
	
	//test3
	
	int xkey=-77;
	RecordId xrid = {-77, -77};
	int ykey= -77;
	RecordId yrid = {-77,-77};
	node.readEntry(0, xkey, xrid);
	cout << "::" << xrid.pid << " " << xrid.sid << " " << xkey << endl;
	node.readEntry(83, ykey, yrid);
	cout << yrid.pid << " " << yrid.sid << " " << ykey << endl;
	
	int rcx = node.readEntry(0,xkey,xrid);
	if(rcx != 0)
		cout << "returned error";
	else
		cout << "returned 0";
	
	
	//test4
	node.insert(key5, rid5);
	node.insert(key1, rid1);
	node.insert(key2, rid2);
	node.insert(key3, rid3);
	node.insert(key4, rid4);
	//node.insert(key5, rid5);
	
	fprintf(stdout, "START\n");	
	cout << "count from getKeyCount(): " << node.getKeyCount();	
	fprintf(stdout, "\n");
	fprintf(stdout, "\n");
	int eeid = 0;
	int ekey = -10;
	RecordId erid = {-10,-10};
	node.readEntry(0, ekey, erid);
	cout << "pid: " << erid.pid << " sid: " << erid.sid << " key: " << ekey << "\n";	
	node.readEntry(1, ekey, erid);
	cout << "pid: " << erid.pid << " sid: " << erid.sid << " key: " << ekey << "\n";
	node.readEntry(2, ekey, erid);
	cout << "pid: " << erid.pid << " sid: " << erid.sid << " key: " << ekey << "\n";
	node.readEntry(3, ekey, erid);
	cout << "pid: " << erid.pid << " sid: " << erid.sid << " key: " << ekey << "\n";
	node.readEntry(4, ekey, erid);
	cout << "pid: " << erid.pid << " sid: " << erid.sid << " key: " << ekey << "\n";

	cout << "16: " << node.gimme(16) << endl;
	cout << "-------------------\n";
	cout << "253: " << node.gimme(253)<<endl;
	cout << "254 cout: " << node.gimme(254)<<endl;
	cout << "225 last pid: " << node.gimme(255)<<endl;
	
	fprintf(stdout, "\n");
	
	//test5
	
	int skey = 500;
	int seid = -99;
	int rskey = -20;
	RecordId rsrid = {-20,-20};
	node.locate(skey, seid);
	node.readEntry(seid, rskey, rsrid);
	
	cout << "search for: " << skey << "\n";
	cout << "eid: " << seid << " returns: ";
	cout << "pid: " << rsrid.pid << " sid: " << rsrid.sid << " key: " << rskey << "\n";
	
	RecordId oldrid = {8,7};
	int oldkey = 9;
	node.writeEntry(oldrid, oldkey, 84);
	cout << node.gimme();

	*/
	
	/*
	fprintf(stdout, "\n");
	fprintf(stdout, "\n");
	
	return 0;
	*/
}
