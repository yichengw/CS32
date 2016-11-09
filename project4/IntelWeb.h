#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include "DiskMultiMap.h"
#include <string>
#include <vector>

class IntelWeb
{
public:
	IntelWeb();
	~IntelWeb();
	bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
	bool openExisting(const std::string& filePrefix);
	void close();
	bool ingest(const std::string& telemetryFile);
	unsigned int crawl(const std::vector<std::string>& indicators,
		unsigned int minPrevalenceToBeGood,
		std::vector<std::string>& badEntitiesFound,
		std::vector<InteractionTuple>& interactions
		);
	bool purge(const std::string& entity);

private:
	// Your private member declarations will go here
	DiskMultiMap dmm;	//hold data
	DiskMultiMap dmm_r;//hold data with reversed "from" and "to".
	vector<string> m_indicators;
	unsigned int m_minPrevalenceToBeGood;
	vector<string> m_badEntitiesFound;
	vector<InteractionTuple> m_interactions;
	//int openFlag;

};

#endif // INTELWEB_H_
