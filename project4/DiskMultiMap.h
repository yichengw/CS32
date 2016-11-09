#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "MultiMapTuple.h"
#include "BinaryFile.h"

class DiskMultiMap
{
public:

	class Iterator
	{
	public:
		Iterator();
		Iterator(DiskMultiMap* thismap, bool isValid, BinaryFile::Offset pos);
		bool isValid() const;
		Iterator& operator++();
		MultiMapTuple operator*();

	private:
		// Your private member declarations will go here
		bool m_valid;
		BinaryFile::Offset m_pos;
		DiskMultiMap* m_map;
	};

	DiskMultiMap();
	~DiskMultiMap();
	bool createNew(const std::string& filename, unsigned int numBuckets);
	bool openExisting(const std::string& filename);
	void close();
	bool insert(const std::string& key, const std::string& value, const std::string& context);
	Iterator search(const std::string& key);
	int erase(const std::string& key, const std::string& value, const std::string& context);

	//void print();	//ONLY FOR TEST. DELETE AFTER TEST!!!!!
private:
	// Your private member declarations will go here
	int createNewFlag;
	int openFlag;

	BinaryFile bf;
	BinaryFile::Offset next;	//store the position where to put next node
	struct Header	
	{
		unsigned int numOfBuckects;	//store the number of buckets in the hash table
		BinaryFile::Offset freelist;//head of the list of nodes available for reuse
	};
	Header header;

	struct Bucket
	{
		BinaryFile::Offset nodelist;	//head of the list of the node this bucket has
	};
	Bucket bucket;

	struct DiskNode	//hold the association
	{
		char key[120 + 1];
		char value[120 + 1];
		char context[120 + 1];
		BinaryFile::Offset nextNode;	//store the next node in the list
		BinaryFile::Offset currNode;	//store the position of current node
	};
	DiskNode n;

	BinaryFile::Offset searchNext;
	string s;
};

#endif // DISKMULTIMAP_H_