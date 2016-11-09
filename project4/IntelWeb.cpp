#include "IntelWeb.h"
#include "DiskMultiMap.h"
#include "BinaryFile.h"
#include "MultiMapTuple.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <string>
#include <queue>
using namespace std;

bool operator < (const InteractionTuple& a, const InteractionTuple& b)
{
	if (a.context < b.context)
		return true;
	else if (a.context > b.context)
		return false;
	else  //a.context == b.context
	{
		if (a.from < b.from)
			return true;
		else if (a.from > b.from)
			return false;
		else  //a.from == b.from
		{
			if (a.to < b.to)
				return true;
			else if (a.to > b.to)
				return false;
			else
				return false;
		}
	}
}
IntelWeb::IntelWeb()
{
	//openFlag = 0;
}
IntelWeb::~IntelWeb()
{
	dmm.close();
	dmm_r.close();
}
bool IntelWeb::createNew(const std::string& filePrefix, unsigned int maxDataItems)
{
	//if any of them are created or open, close first
		dmm.close();
		dmm_r.close();

	if (dmm.createNew(filePrefix + "forward.dat", 2 * maxDataItems + 1 ) && dmm_r.createNew(filePrefix + "reverse.dat", 2 * maxDataItems + 1))
		//if dmm and dmm_r are all created successfully
	{
		return true;
	}
	else
	{
		dmm.close();
		dmm_r.close(); 
		return false;
	}
	
}
bool IntelWeb::openExisting(const std::string& filePrefix)
{
	if (dmm.openExisting(filePrefix + "forward.dat") && dmm_r.openExisting(filePrefix + "reverse.dat"))
		//if dmms are all open successfully
	{
		return true;
	}
	else
	{
		dmm.close(); 
		dmm_r.close();
		return false;
	}
}
void IntelWeb::close()
{
	dmm.close();
	dmm_r.close();
}
bool IntelWeb::ingest(const std::string& telemetryFile)
{
	// Open the file for input
	ifstream inf("telemetryFile");
	// Test for failure to open
	if (!inf)
	{
		cout << "Cannot open  telemetryFile!" << endl;
		return false;
	}
	// Read each line.  The return value of getline is treated
	// as true if a line was read, false otherwise (e.g., because
	// the end of the file was reached).
	string line;
	while (getline(inf, line))
	{
		// To extract the information from the line, we'll
		// create an input stringstream from it, which acts
		// like a source of input for operator>>
		istringstream iss(line);
		string key;
		string value;
		string context;
		// The return value of operator>> acts like false
		// if we can't extract a string followed by a string followed by a string
		if (!(iss >> key >> value >> context))
		{
			cout << "Ignoring badly-formatted input line: " << line << endl;
			continue;
		}
		// If we want to be sure there are no other non-whitespace
		// characters on the line, we can try to continue reading
		// from the stringstream; if it succeeds, extra stuff
		// is after the double.
		char dummy;
		if (iss >> dummy) // succeeds if there a non-whitespace char
			cout << "Ignoring extra data in line: " << line << endl;
		// Add data to dmm and dmm_r
		dmm.insert(key, value, context);
		dmm_r.insert(value, key, context);
	}
}
unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators,
	unsigned int minPrevalenceToBeGood,
	std::vector<std::string>& badEntitiesFound,
	std::vector<InteractionTuple>& interactions)
{
	m_indicators = indicators;
	m_minPrevalenceToBeGood = minPrevalenceToBeGood;
	map<string, int> prevalenceMap;	//a map to store entries whose prevalences are bigger than the minPrevalenceToBeGood
	set<string> badEntitiesSet;	//hold bad entities, take benefits of set to find key and sort
	set<InteractionTuple> interactionsSet;		//hold interactions, take benefits of set to find interaction and sort
	queue<string> badEntitiesQueue;

	for (vector<std::string>::iterator indicatorsIt = m_indicators.begin(); indicatorsIt != m_indicators.end(); indicatorsIt++)
	{
		DiskMultiMap::Iterator keysearchIt = dmm.search(*indicatorsIt);	//find first key in the doc dmm that == current indicator
		while (keysearchIt.isValid())	//if find an indicator
		{
			MultiMapTuple m = *keysearchIt;

			badEntitiesSet.insert(m.key);	//put this key into bad entities set,ignore if duplicate 

			//then we want to count prevalence of the value to see if it is good or bad
			DiskMultiMap::Iterator valuesearchIt = dmm.search(m.value);		//first search the forward doc
			int count = 0;
			while (valuesearchIt.isValid())
			{
				count++;
				++valuesearchIt;
			}
			valuesearchIt = dmm_r.search(m.value);	//then search the reversed doc
			while (valuesearchIt.isValid())
			{
				count++;
				++valuesearchIt;
			}
			if (count < m_minPrevalenceToBeGood)	//it is a bad one
			//then we want to see if this is already in set
			{
				set<string>::iterator findkeyIt = badEntitiesSet.find(m.value);
				if (findkeyIt == badEntitiesSet.end())
				{
					badEntitiesSet.insert(m.value);	//put this value into bad entities set
					badEntitiesQueue.push(m.value);	//put this value into the queue for the search later
				}
			}
			//then we want to insert this interaction into interactionsSet
			InteractionTuple i(m.key, m.value, m.context);
			interactionsSet.insert(i);

			++keysearchIt;	//find next key of this indicator
		}

		//then we try to find indicator in the reverse doc
		keysearchIt = dmm_r.search(*indicatorsIt);
		while (keysearchIt.isValid())	//if find an indicator
		{
			MultiMapTuple m = *keysearchIt;

			badEntitiesSet.insert(m.key);	//put this key into bad entities set,ignore if duplicate 

			//then we want to count prevalence of the value to see if it is good or bad
			DiskMultiMap::Iterator valuesearchIt = dmm.search(m.value);		//first search the forward doc
			int count = 0;
			while (valuesearchIt.isValid())
			{
				count++;
				++valuesearchIt;
			}
			valuesearchIt = dmm_r.search(m.value);	//then search the reversed doc
			while (valuesearchIt.isValid())
			{
				count++;
				++valuesearchIt;
			}
			if (count < m_minPrevalenceToBeGood)	//it is a bad one
													//then we want to see if this is already in set
			{
				set<string>::iterator findkeyIt = badEntitiesSet.find(m.value);
				if (findkeyIt == badEntitiesSet.end())
				{
					badEntitiesSet.insert(m.value);	//put this value into bad entities set
					badEntitiesQueue.push(m.value);	//put this value into the queue for the search later
				}
			}
			//then we want to insert this interaction into interactionsSet
			InteractionTuple i(m.value, m.key, m.context);
			interactionsSet.insert(i);

			++keysearchIt;	//find next key of this indicator
		}

	}

	while (!badEntitiesQueue.empty())	//when queue is not empty
	{
		string s = badEntitiesQueue.front();
		badEntitiesQueue.pop();
		DiskMultiMap::Iterator keysearchIt = dmm.search(s);
		while (keysearchIt.isValid())	//if find an indicator
		{
			MultiMapTuple m = *keysearchIt;
											//then we want to count prevalence of the value to see if it is good or bad
			DiskMultiMap::Iterator valuesearchIt = dmm.search(m.value);		//first search the forward doc
			int count = 0;
			while (valuesearchIt.isValid())
			{
				count++;
				++valuesearchIt;
			}
			valuesearchIt = dmm_r.search(m.value);	//then search the reversed doc
			while (valuesearchIt.isValid())
			{
				count++;
				++valuesearchIt;
			}
			if (count < m_minPrevalenceToBeGood)	//it is a bad one
													//then we want to see if this is already in set
			{
				set<string>::iterator findkeyIt = badEntitiesSet.find(m.value);
				if (findkeyIt == badEntitiesSet.end())
				{
					badEntitiesSet.insert(m.value);	//put this value into bad entities set
					badEntitiesQueue.push(m.value);	//put this value into the queue for the search later
				}
			}
			//then we want to insert this interaction into interactionsSet
			InteractionTuple i(m.key, m.value, m.context);
			interactionsSet.insert(i);

			++keysearchIt;	//find next key of this indicator
		}

		//then we try to find indicator in the reverse doc
		keysearchIt = dmm_r.search(s);
		while (keysearchIt.isValid())	//if find an indicator
		{
			MultiMapTuple m = *keysearchIt;
											//then we want to count prevalence of the value to see if it is good or bad
			DiskMultiMap::Iterator valuesearchIt = dmm.search(m.value);		//first search the forward doc
			int count = 0;
			while (valuesearchIt.isValid())
			{
				count++;
				++valuesearchIt;
			}
			valuesearchIt = dmm_r.search(m.value);	//then search the reversed doc
			while (valuesearchIt.isValid())
			{
				count++;
				++valuesearchIt;
			}
			if (count < m_minPrevalenceToBeGood)	//it is a bad one
													//then we want to see if this is already in set
			{
				set<string>::iterator findkeyIt = badEntitiesSet.find(m.value);
				if (findkeyIt == badEntitiesSet.end())
				{
					badEntitiesSet.insert(m.value);	//put this value into bad entities set
					badEntitiesQueue.push(m.value);	//put this value into the queue for the search later
				}
			}
			//then we want to insert this interaction into interactionsSet
			InteractionTuple i(m.value, m.key, m.context);
			interactionsSet.insert(i);

			++keysearchIt;	//find next key of this indicator
		}
	}
	set<string>::iterator pushIt = badEntitiesSet.begin();
	while (pushIt != badEntitiesSet.end())
	{
		m_badEntitiesFound.push_back(*pushIt);
		pushIt++;
	}
	badEntitiesFound = m_badEntitiesFound;

	set<InteractionTuple>::iterator pushInteractionIt = interactionsSet.begin();
	while (pushInteractionIt != interactionsSet.end())
	{
		m_interactions.push_back(*pushInteractionIt);
		pushInteractionIt++;
	}
	interactions = m_interactions;

	return badEntitiesSet.size();
}
bool IntelWeb::purge(const std::string& entity)
{
	DiskMultiMap::Iterator keysearchIt = dmm.search(entity);
	while (keysearchIt.isValid())
	{
		MultiMapTuple m = *keysearchIt;
		dmm.erase(entity, m.value, m.context);
		dmm_r.erase(m.value, entity, m.context);
		++keysearchIt;
	}
	keysearchIt = dmm_r.search(entity);
	while (keysearchIt.isValid())
	{
		MultiMapTuple m = *keysearchIt;
		dmm.erase(m.value, entity, m.context);
		dmm_r.erase(entity, m.value, m.context);
		++keysearchIt;
	}
	return true;
}