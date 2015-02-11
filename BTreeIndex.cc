/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
	treeHeight = 0;
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)//done
{
	RC rc = pf.open(indexname, mode);
	PageId pid;
	if(rc != 0)
	{
		return rc;
	}
	
	char readBuf[PageFile::PAGE_SIZE];

	
	if(pf.endPid() == 0)//empty tree
	{
		rootPid = -1;
		treeHeight = 0;
		pf.write(0,readBuf);
		return 0;
	}
	else
	{
		pf.read(0,readBuf);
		rootPid = *((PageId *)readBuf);
		treeHeight = *((int *)(readBuf+sizeof(PageId)));
	}

    return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()//done//checked against shadow spy
{
	char readBuf[PageFile::PAGE_SIZE];
	*((PageId *)readBuf) = rootPid;
	*((int *)(readBuf+sizeof(PageId))) = treeHeight;
	pf.write(0,readBuf);

	RC rc = pf.close();
    return rc;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
	RC ecode; //error code
	int curHeight = 1;
	PageId xPid;
	int xKey;

	if(treeHeight == 0)
	{
		BTLeafNode ln;
		
		rootPid = pf.endPid();
		ln.insert(key, rid);
		//using pid = 0 for storing meta data
		treeHeight++;
		ecode = ln.write(rootPid, pf);
		
	}
	this->insertHelp2(key, rid, rootPid, xPid, xKey, curHeight);
	if(xKey > 0)
	{
		BTNonLeafNode newRoot;
		newRoot.initializeRoot(rootPid, xKey, xPid);
		rootPid = pf.endPid();
		treeHeight++;
		newRoot.write(rootPid, pf);
	}
	return 0;
}

RC BTreeIndex::insertHelp2(int key, RecordId rid, PageId curPid, PageId& xPid, int& xKey, int curHeight)
{
	RC ecode;
	xKey = -1;
	
	if(curHeight == treeHeight)
	{
		BTLeafNode ln;
		ln.read(curPid, pf);
		
		ecode = ln.insert(key, rid);
		if(ecode == 0)
		{
			ln.write(curPid, pf);
			return ecode;
		}
		else//node full
		{
			BTLeafNode sibn;
			if(ln.insertAndSplit(key,rid,sibn,xKey))
			{
				return 1;
			}
			xPid = pf.endPid();
			sibn.setNextNodePtr(ln.getNextNodePtr());///
			
			ln.setNextNodePtr(xPid);
			
			if(sibn.write(xPid, pf))
			{
				return 1;
			}
			if(ln.write(curPid,pf))
			{
				return 1;
			}
		}
		return 0;
	}
	else//at non leaf node
	{
		BTNonLeafNode nln;
		nln.read(curPid, pf);
		PageId childPid;
		PageId splitPid;
		int splitKey;
		int eid;
		nln.locateChildPtr(key, childPid);
		insertHelp2(key, rid, childPid, xPid, xKey, curHeight+1);
		//end of recursion
		if(xKey > 0)
		{
			if(nln.insert(xKey, xPid))
			{
				int midKey;
				BTNonLeafNode sibn;
				if(nln.insertAndSplit(xKey, xPid, sibn, midKey))
					return 1;
				xKey = midKey;
				xPid = pf.endPid();
				if(sibn.write(xPid,pf))
					return 1;
			}
			else
			{
				xKey = -1;
			}
			nln.write(curPid,pf);
		}
		
	}
	return 0;
	
}

/*
 * Find the leaf-node index entry whose key value is larger than or 
 * equal to searchKey, and output the location of the entry in IndexCursor.
 * IndexCursor is a "pointer" to a B+tree leaf-node entry consisting of
 * the PageId of the node and the SlotID of the index entry.
 * Note that, for range queries, we need to scan the B+tree leaf nodes.
 * For example, if the query is "key > 1000", we should scan the leaf
 * nodes starting with the key value 1000. For this reason,
 * it is better to return the location of the leaf node entry 
 * for a given searchKey, instead of returning the RecordId
 * associated with the searchKey directly.
 * Once the location of the index entry is identified and returned 
 * from this function, you should call readForward() to retrieve the
 * actual (key, rid) pair from the index.
 * @param key[IN] the key to find.
 * @param cursor[OUT] the cursor pointing to the first index entry
 *                    with the key value.
 * @return error code. 0 if no error.
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
	RC ecode;
    BTNonLeafNode nln;
	BTLeafNode ln;
	PageId curPid = rootPid;
	int curEid;
	int curHeight = 1;
	while(curHeight < treeHeight)
	{
		if((ecode = nln.read(curPid, pf)) != 0)
		{
			return ecode;
		}
		if((ecode = nln.locateChildPtr(searchKey, curPid)) != 0)
		{
			return ecode;
		}
		curHeight++;
	}
	//in leaf nodes
	if((ecode = ln.read(curPid, pf)) != 0)
	{
		return ecode;
	}
	if((ecode = ln.locate(searchKey, curEid)) != 0)
	{
		return ecode;
	}
	cursor.eid = curEid;
	cursor.pid = curPid;
	return ecode;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
	BTLeafNode ln;
	RC ecode;

	if(ecode = ln.read(cursor.pid, pf))
	{
		return ecode;
	}
	
	if((ecode = ln.readEntry(cursor.eid, key, rid)) != 0)
	{
		return ecode;
	}
	if(cursor.pid <= 0 || cursor.pid >= pf.endPid())
	{
		return 1;
	}
	
	cursor.eid++;
	if(cursor.eid >= ln.getKeyCount())// or >=
	{
		cursor.pid = ln.getNextNodePtr();
		cursor.eid = 0;
	}
	return 0;
}

/***************************Helper Functions*********************************************/
/****************************************************************************************/

RC BTreeIndex::createRoot(PageId leftPid, int key, PageId rightPid)
{
    BTNonLeafNode nln;
	RC ecode;
    ecode = nln.initializeRoot(leftPid, key, rightPid);
    rootPid = pf.endPid();
    nln.write(rootPid, pf);
	treeHeight++;
    return ecode;
}
