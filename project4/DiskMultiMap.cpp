#include "DiskMultiMap.h"
#include "BinaryFile.h"
#include <functional>
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

DiskMultiMap::Iterator::Iterator()
{
	m_valid = false;
	m_pos = 0;
}
DiskMultiMap::Iterator::Iterator(DiskMultiMap* thismap, bool isValid, BinaryFile::Offset pos)
	:m_map(thismap),m_valid(isValid),m_pos(pos)
{

}
bool DiskMultiMap::Iterator::isValid() const
{
	return m_valid;
}
DiskMultiMap::Iterator& DiskMultiMap::Iterator:: operator++()
{
	return *this = m_map->search(m_map->s);
}
MultiMapTuple DiskMultiMap::Iterator::operator*()
{
	MultiMapTuple mmt;
	m_map->bf.read(m_map->n,m_pos);
	mmt.key = m_map->n.key;
	mmt.value = m_map->n.value;
	mmt.context = m_map->n.context;
	return mmt;
}

DiskMultiMap::DiskMultiMap()
{
	createNewFlag = 0;
	openFlag = 0;
	next = 12; //first 12 bytes are used to store offsets(next, numOfBuckets and freelist)
	header.numOfBuckects = 0;	//no buckets
	header.freelist = 0;	//no freelist
	
	searchNext = -1;	//initial
	s = "0";
}
DiskMultiMap::~DiskMultiMap()
{
	close();
}
bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBucket)
{
	if (openFlag == 1 || createNewFlag == 1)	//already open or create a file
	{
		bf.close();
		openFlag = 0;
		createNewFlag = 0;
	}
	bf.createNew(filename);
	bf.write(next, 0);	//write next at offset(0-3)
	header.numOfBuckects = numBucket;
	bf.write(header.numOfBuckects, 4);
	bf.write(header.freelist, 8);	//write freelist at offset(8-11)
	
	for (int i = 0; i < header.numOfBuckects; i++)
	{
		bucket.nodelist = 0;	//no nodelist
		bf.write(bucket, next);
		next = bf.fileLength();
	}
	createNewFlag = 1;
	openFlag = 1;
	bf.write(next, 0);
	return true;

}

bool DiskMultiMap::openExisting(const std::string& filename)
{
	if (openFlag == 1 || createNewFlag == 1)	//already open or create a file
	{
		bf.close();
		openFlag = 0;
		createNewFlag = 0;
	}
	if (bf.openExisting(filename))
	{
		openFlag = 1;
		return true;
	}
	else
		return false;
}
void DiskMultiMap::close()
{
	if (openFlag == 1 || createNewFlag == 1)
	{
		bf.close();
	}
}
bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context)
{
	if (key.size() > 120 || value.size() > 120 || context.size() > 120)
		return false;
	hash<string> str_hash;
	unsigned int hashvalue = str_hash(key);
	bf.read(header.numOfBuckects, 4);
	unsigned int atbucket = hashvalue % header.numOfBuckects;
	cout << "insert atbucket: " << atbucket << endl;	//FOR TEST;
	bf.read(bucket, 12 + 4 * atbucket);
	
	if (bucket.nodelist == 0)	//no nodelist
	{
		bf.read(header.freelist, 8);
		if (header.freelist == 0)	//no freelist
		{
			bf.read(next, 0);	//read next from disk
			bucket.nodelist = next;
			strcpy_s(n.key, key.c_str());
			strcpy_s(n.value, value.c_str());
			strcpy_s(n.context, context.c_str());
			n.nextNode = 0;	//0 is the same as nullptr
			n.currNode = next;
			bf.write(n, next);
			next = bf.fileLength();
			bf.write(next, 0);	//write next after inserting
			bf.write(bucket, 12 + 4 * atbucket);
		}
		else   //freelist exists, so the next node is the head of freelist
		{
			BinaryFile::Offset temp;	//hold the node from freelist to reuse in this bucket
			temp = header.freelist;
			bf.read(n, header.freelist);
			header.freelist = n.nextNode;
			bf.write(header.freelist, 8);

			bucket.nodelist = temp;
			strcpy_s(n.key, key.c_str());
			strcpy_s(n.value, value.c_str());
			strcpy_s(n.context, context.c_str());
			n.nextNode = 0;	//0 is the same as nullptr
			n.currNode = temp;
			bf.write(n, temp);
			bf.write(bucket, 12 + 4 * atbucket);
		}

	}
	else
	{
		bf.read(header.freelist, 8);
		if (header.freelist == 0)	//no freelist
		{
			bf.read(n, bucket.nodelist);
			while (n.nextNode != 0)	//find the last node linked to this bucket
			{
				bf.read(n, n.nextNode);
			}
			bf.read(next, 0);	//read next from disk
			n.nextNode = next;	//linked n to the new node
			bf.write(n, n.currNode);
			DiskNode m;
			strcpy_s(m.key, key.c_str());
			strcpy_s(m.value, value.c_str());
			strcpy_s(m.context, context.c_str());
			m.nextNode = 0;	//0 is the same as nullptr
			m.currNode = next;
			bf.write(m, next);
			next = bf.fileLength();
			bf.write(next, 0);//write next after inserting
		}
		else
		{
			BinaryFile::Offset temp;	//hold the node from freelist to reuse in this bucket
			temp = header.freelist;
			bf.read(n, header.freelist);
			header.freelist = n.nextNode;
			bf.write(header.freelist, 8);

			bf.read(n, bucket.nodelist);
			while (n.nextNode != 0)	//find the last node linked to this bucket
			{
				bf.read(n, n.nextNode);
			}
			n.nextNode = temp;	//linked temp to n
			bf.write(n, n.currNode);
			DiskNode m;
			strcpy_s(m.key, key.c_str());
			strcpy_s(m.value, value.c_str());
			strcpy_s(m.context, context.c_str());
			m.nextNode = 0;
			m.currNode = temp;
			bf.write(m, temp);
		}

		
	}
	return true;
}
DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key)
{
	if (key.size() > 120)
	{
		Iterator it(this, false, 0);
		return it;
	}
	s = key;	//store key for iterator class use
	hash<string> str_hash;
	unsigned int hashvalue = str_hash(key);
	bf.read(header.numOfBuckects, 4);
	unsigned int atbucket = hashvalue % header.numOfBuckects;
	//cout << "search atbucket: " << atbucket << endl;	//FOR TEST
	bf.read(bucket, 12 + 4 * atbucket);
	if (bucket.nodelist == 0)	//no nodelist
	{
		Iterator it(this, false, 0);//invalid
		return it;		
	}
	else
	{
		if (searchNext == 0)//the node found is the last node and it is valid
		{
			searchNext = -1;
			Iterator it(this, false, 0);
			return it;
		}
		else if (searchNext == -1)//back to initial
		{
			bf.read(n, bucket.nodelist);//start from the node at bucket.nodelist
		}
		else {
			bf.read(n, searchNext);	//start from the node at searchNext
		}
		while(n.nextNode != 0)	//this is not the last node
		{
			if (strcmp(n.key, key.c_str()) == 0)
			{			
				searchNext = n.nextNode;
				Iterator it(this, true, n.currNode);
				return it;
			}
			bf.read(n, n.nextNode);
		}
		//last node
		if (strcmp(n.key, key.c_str()) == 0)
		{		
			searchNext = 0; //we have searched all the nodes
			Iterator it(this, true, n.currNode);
			return it;
		}
		else
		{
			searchNext = -1; //back to initial
			Iterator it(this, false, 0);
			return it;
		}


	}

}
int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context)
{
	int numOfRemovals = 0;
	if (key.size() > 120 || value.size() > 120 || context.size() > 120)
		return 0;
	hash<string> str_hash;
	unsigned int hashvalue = str_hash(key);
	bf.read(header.numOfBuckects, 4);
	unsigned int atbucket = hashvalue % header.numOfBuckects;
	cout << "remove atbucket: " << atbucket << endl;	//FOR TEST
	bf.read(bucket, 12 + 4 * atbucket);
	if (bucket.nodelist == 0)	//no nodelist
	{
		//cout << "no list in this bucket!" << endl;
		return 0;
	}

	else
	{
		bf.read(n, bucket.nodelist);
		while (strcmp(n.key, key.c_str()) == 0 && strcmp(n.value, value.c_str()) == 0 && strcmp(n.context, context.c_str()) == 0)
		{
			BinaryFile::Offset temp;
			bf.read(header.freelist, 8);	//read freelist's head from disk
			temp = header.freelist;//temporary store
			header.freelist = bucket.nodelist;
			bucket.nodelist = n.nextNode;
			n.nextNode = temp;	//linked to the next node in the freelist
			bf.write(n, n.currNode);
			bf.write(header.freelist, 8);	//write freelist to the disk
			numOfRemovals++;

			bf.read(n, bucket.nodelist);
		}
		while (n.nextNode != 0)
		{
			DiskNode m;
			bf.read(m, n.nextNode);
			if (strcmp(m.key, key.c_str()) == 0 && strcmp(m.value, value.c_str()) == 0 && strcmp(m.context, context.c_str()) == 0)
			{
				bf.read(header.freelist, 8);	//read freelist's head from disk
				n.nextNode = m.nextNode;
				bf.write(n, n.currNode);

				m.nextNode = header.freelist;
				header.freelist = m.currNode;
				bf.write(m, m.currNode);
				bf.write(header.freelist, 8);	//write freelist to the disk
				numOfRemovals++;
			}
			else
				n = m;
		}
	}
	return numOfRemovals;
}
/*
void DiskMultiMap::print()
{
	bf.read(next, 0);
	cout << next << endl;
	
}
*/