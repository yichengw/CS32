/*
#include "DiskMultiMap.h"
#include "BinaryFile.h"
#include <iostream>

using namespace std;

void main()
{
	DiskMultiMap dmm;
	dmm.createNew("myBuckets.dat", 100);
	dmm.openExisting("myBuckets.dat");
	dmm.insert("hmm.exe", "pfft.ext", "m52902");
	dmm.insert("hmm.exe", "pfft.ext", "m52902");
	dmm.insert("hmm.exe", "pfft.ext", "m10001");
	dmm.insert("blah.exe", "bletch.ext", "m0003");
	//cout << dmm.erase("blah.exe", "bletch.ext", "m0003") << endl;
	//cout << dmm.erase("hmm.exe", "pfft.ext", "m52902") << endl;
	dmm.insert("hmm.exe", "pfft.ext", "m10001");
	dmm.insert("blah.exe", "bletch.ext", "m0003");
	dmm.insert("hmm.exe", "pfft.ext", "m52902");
	DiskMultiMap::Iterator it = dmm.search("hmm.exe");
	if (it.isValid())
	{
		int count = 0;
		cout << "I found at least 1 item with a key of hmm.exe\n";
		while(it.isValid()){
			count++;
			MultiMapTuple m = *it;
			cout << "The key is: " << m.key << endl;
			cout << "The value is: " << m.value << endl;
			cout << "The context is: " << m.context << endl;
			++it;
		}
		cout << count << endl;

	}
	else
		cout << " nothing found..." << endl;
	//dmm.print();
	cin.get();
}
*/