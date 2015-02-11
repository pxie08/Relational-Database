#include "BTreeNode.h"

using namespace std;


/************************************************************
 * Leaf Node's Member Functions
 ************************************************************/

/************************My Own Member Functions for Help********************************/
/****************************************************************************************/

BTLeafNode::BTLeafNode()//done
{
	bzero(buffer, PageFile::PAGE_SIZE);

	setNodeKeyCount(0);
	setNextNodePtr(-9);
	
}

void BTLeafNode::writeEntry(RecordId rid, int key, int entryid)//done
{
	int* intbuffer = (int*)buffer;
	int first = entryid*3;
	int second = first+1;
	int third = second+1;
	*(intbuffer+first) = rid.pid;
	*(intbuffer+second) = rid.sid;
	*(intbuffer+third) = key;
	char* charbuffer = (char*) intbuffer;
	for(int j = 0; j < ENTRY_SIZE; j++)
	{
		buffer[j]=charbuffer[j];
	}
}

RC BTLeafNode::setNodeKeyCount(int count)//done
{
	int* intbuffer = (int*) buffer;
	*(intbuffer + MAX_TUPLES - 2) = count;
	return 0;
}

RC BTLeafNode::updateNodeKeyCount()//done
{
	int* intbuffer = (int*) buffer;
	*(intbuffer + MAX_TUPLES - 2) += 1;
	return 0;
}


/****************************************************************************************/
/****************************************************************************************/

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)//done
{
	return pf.read(pid, buffer);
}
 
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)//done
{
	return pf.write(pid, buffer);
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()//done
{
	
	int* intbuffer = (int*) buffer;
	return *(intbuffer + MAX_TUPLES -2);
	
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)//done
{
	int totKeys = getKeyCount();
	if(totKeys >= MAX_ENTRY_COUNT)
	{
		return RC_NODE_FULL;
	}
	int eid;
	int oldKey;
	RecordId oldRid;
	RC rc = locate(key,eid);
	if(rc != 0)
	{
		eid = totKeys;

	}
	for(int i = totKeys-1; i >= eid; i--)
	{
		readEntry(i, oldKey, oldRid);
		writeEntry(oldRid, oldKey, i+1);
	}
	writeEntry(rid, key, eid);
	updateNodeKeyCount();
	return 0;
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, BTLeafNode& sibling, int& siblingKey)
{
	int keyCount = getKeyCount();
	int spliteid = (int)(MAX_ENTRY_COUNT/2)-1; //84/2=42-1=>eid=41
	int oldKey;
	RecordId oldRid;
	int emptyKey = -1;
	RecordId emptyRid = {-1,-1};
	int sibCount = 0;
	
	int eid;
	locate(key,eid);
	
	//making two nodes now splitting
	for(int i = spliteid; i <= MAX_ENTRY_COUNT; i++)//<
	{
		readEntry(i, oldKey, oldRid);
		sibling.insert(oldKey, oldRid);
		writeEntry(emptyRid, emptyKey, i);
		setNodeKeyCount(keyCount--);
	}
	RecordId siblingRid;
	sibling.readEntry(0,siblingKey,siblingRid);
	if(key < siblingKey)
	{
		insert(key,rid);
	}
	else
	{
		sibling.insert(key,rid);
	}
	return 0;
}

/*
 * Find the entry whose key value is larger than or equal to searchKey
 * and output the eid (entry number) whose key value >= searchKey.
 * Remeber that all keys inside a B+tree node should be kept sorted.
 * @param searchKey[IN] the key to search for
 * @param eid[OUT] the entry number that contains a key larger than or equalty to searchKey
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::locate(int searchKey, int& eid)//done
{
	eid = 0;
	int nkey;
	int first;
	while(eid <getKeyCount())
	{
		int* intbuffer = (int*) buffer;
		first = eid*3;
		nkey = *(intbuffer + first + 2);
		if(searchKey > nkey)
			eid++;
		else
			break;
	}
	if(eid == getKeyCount())
	{
		eid = -1;
		return 1;
	}
	return 0;
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)//done
{

	int keyCount = getKeyCount();
	if(eid >= keyCount || eid < 0)
	{
		return RC_FILE_SEEK_FAILED;
	}

	int* intbuffer = (int*) buffer;
	int first = eid*3;
	rid.pid = *(intbuffer+first);
	rid.sid = *(intbuffer+first+1);
	key = *(intbuffer+first+2);
	return 0;
	
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()//done
{
	int* intbuffer  = (int*) buffer;
	return *(intbuffer + MAX_TUPLES - 1);
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)//done
{
	int* intbuffer = (int*) buffer;
	*(intbuffer + MAX_TUPLES - 1) = pid;
	return 0;
}

/************************************************************
 * Non Leaf Node's Member Functions
 ************************************************************/

 
/************************My Own Member Functions for Help********************************/
/****************************************************************************************/

RC BTNonLeafNode::setFirstPid(PageId pid)//done
{
	int* intbuffer = (int*) buffer;
	*(intbuffer) = pid;
	return 0;
}

PageId BTNonLeafNode::readFirstPid()//done
{
	int* intbuffer = (int*) buffer;
	return *(intbuffer);
}

RC BTNonLeafNode::setNodeKeyCount(int count)//done
{
	int* intbuffer = (int*) buffer;
	*(intbuffer + MAX_TUPLES - 1) = count;
	return 0;
}

RC BTNonLeafNode::updateNodeKeyCount()//done
{
	int* intbuffer = (int*) buffer;
	*(intbuffer + MAX_TUPLES - 1) += 1;
	return 0;
}

void BTNonLeafNode::writeEntry(int key, PageId pid, int entryid)//done
{
	int* intbuffer = (int*)buffer;
	int first = entryid*2+1;
	int second = first+1;
	*(intbuffer+first) = key;
	*(intbuffer+second) = pid;
	char* charbuffer = (char*) intbuffer;
	for(int j = 0; j < ENTRY_SIZE; j++)
	{
		buffer[j]=charbuffer[j];
	}
}

RC BTNonLeafNode::readEntry(int eid, int& key, PageId& rightPid)//done
{
	int keyCount = getKeyCount();
	if(keyCount == 0 && eid == 0)
	{
		keyCount = 1;
	}
	if(eid >= keyCount || eid < 0)
	{
		return RC_FILE_SEEK_FAILED;
	}
	int* intbuffer = (int*) buffer;
	int first = eid*2+1;
	key = *(intbuffer+first);
	rightPid = *(intbuffer+first+1);
	return 0;
}

RC BTNonLeafNode::locate(int searchKey, int& eid)//done
{
	int keyCount = getKeyCount();
	if(keyCount == 0)
	{
		eid = 0;
		return RC_NO_SUCH_RECORD;
	}
	int nkey;
	int first;
	int* intbuffer = (int*) buffer;
	for(int i = 0; i < keyCount; i++)
	{
		first = i*2+1;
		nkey = *(intbuffer+first);
		if(nkey >= searchKey)
		{
			eid = i;
			return 0;
		}
	}
	eid = keyCount;
	return RC_NO_SUCH_RECORD;
}

/****************************************************************************************/
/****************************************************************************************/

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)//done
{
	return pf.read(pid, buffer);
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)//done
{
	return pf.write(pid, buffer);
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()//done
{
	int* intbuffer = (int*) buffer;
	return *(intbuffer + MAX_TUPLES -1);
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)//done
{
	int totKeys = getKeyCount();
	if(totKeys >= MAX_ENTRY_COUNT)
	{
		return RC_NODE_FULL;
	}
	int eid;
	int oldKey;
	PageId oldPid;
	RC rc = locate(key,eid);

	for(int i = totKeys-1; i >= eid; i--)
	{
		readEntry(i, oldKey, oldPid);
		writeEntry(oldKey, oldPid, i+1);
	}
	writeEntry(key, pid, eid);
	updateNodeKeyCount();
	return 0;
}

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{
	int keyCount = getKeyCount();
	if(keyCount < MAX_ENTRY_COUNT)
	{
		return RC_INVALID_FILE_FORMAT;
	}
	int spliteid = (int)(MAX_ENTRY_COUNT/2)-1;//126/2=63-1=>eid=62
	int midPid;
	readEntry(spliteid, midKey, midPid);
	int oldKey;
	PageId oldPid;
	int emptyKey = -1;
	PageId emptyPid = -1;
	
	sibling.initializeRoot(pid,midKey,midPid);
	
	for(int i = spliteid; i < MAX_ENTRY_COUNT; i++)
	{
		readEntry(i, oldKey, oldPid);
		sibling.insert(oldKey, oldPid);
		writeEntry(emptyKey, emptyPid, i);
		setNodeKeyCount(keyCount--);
	}
	PageId siblingPid;
	int siblingKey;
	sibling.readEntry(0,siblingKey,siblingPid);
	if(key < siblingKey)
	{
		insert(key,pid);
	}
	else
	{
		sibling.insert(key,pid);
	}
	return 0;
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)//done
{
	int keyCount = getKeyCount();
	if(keyCount == 0)
	{
		pid = -1;
		return RC_NO_SUCH_RECORD;
	}
	int nkey;
	int first;
	int lkey;
	int rkey;
	
	PageId leftPid;//get these somehow
	PageId rightPid;
	
	int* intbuffer = (int*) buffer;
	//this searches from the very first tuple, first pid in buffer
	if(keyCount ==1)
	{
		first = 0;
		nkey = *(intbuffer+first+1);
		if(searchKey >= nkey)
		{
			rightPid = *(intbuffer+first+2);
			pid = rightPid;
			return 0;
		}
		else
		{
			leftPid = *(intbuffer+first);
			pid = leftPid;
			return 0;
		}
	}
	for(int i = 0; i < keyCount; i++)
	{
		first = i*2;
		lkey = *(intbuffer+first+1);
		rkey = *(intbuffer+first+3);
		if(lkey <= searchKey && searchKey < rkey)
		{
			rightPid = *(intbuffer+first+2);
			pid = rightPid;
			return 0;
		}
		if(searchKey < lkey)
		{
			leftPid = *(intbuffer+first);
			pid = leftPid;
			return 0;
		}
		if(searchKey >= rkey && i==keyCount-1)
		{
			pid = *(intbuffer+first+2);
			return 0;
		}
	}
	pid = -1;
	return RC_NO_SUCH_RECORD;
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)//done
{
	bzero(buffer, PageFile::PAGE_SIZE);
	PageId npid = -1;
	int nkey = -1;
	int startingEid = 0;
	for(int i = 0; i < MAX_ENTRY_COUNT+1; i++)
	{
		writeEntry(nkey, npid, i);
	}
	setFirstPid(pid1);
	writeEntry(key, pid2, startingEid);
	setNodeKeyCount(1);
}
