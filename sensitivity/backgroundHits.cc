// gemc
#include "backgroundHits.h"

// c++
#include <fstream>

// mlibrary
#include "gstring.h"
using namespace gstring;

// initialize from string
// notice hardcoded order
BackgroundHit::BackgroundHit(vector<string> hitsData, int verbosity)
{
	int identifierSize = stoi(hitsData[2]);

	timeFromEventStart = stod(hitsData[3 + identifierSize]);
	energy             = stod(hitsData[3 + identifierSize + 1]);
	nphe              = stod(hitsData[3 + identifierSize + 2]);

	for(unsigned i=0; i<identifierSize; i++) {
		identifier iden;
		iden.name = hitsData[0];
		iden.id   = stoi(hitsData[3+i]);
		iden.time = timeFromEventStart;
		identity.push_back(iden);
	}

	if(verbosity > 4) {
		cout << " New background hit added for " << hitsData[0] << " event number " << hitsData[1] << ":  identifier: " ;
		for(auto iden: identity) {
			cout << " " << iden.id << " " ;
		}
		cout << "  time: " << timeFromEventStart;
		cout << "[ns]  energy: " << energy;
		cout << "[MeV]  number of photons: " << nphe << endl;

	}
}



ostream &operator<<(ostream &stream, BackgroundHit bgh)
{
	stream << " - identifiers: " ;
	for(unsigned i=0; i<bgh.identity.size(); i++) {
		stream << "#" << i+1 << ": " << bgh.identity[i].id ;
		if(i < bgh.identity.size() - 1) stream << ", " ;
	}
	stream << endl;

	stream << " - time from start of the event: " << bgh.timeFromEventStart/CLHEP::ns << " [ns]" << endl;
	if(bgh.energy > 0) {
		stream << " - energy: " << bgh.energy << endl;
	}
	if(bgh.nphe > 0) {
		stream << " - number of photons: " << bgh.nphe << endl;
	}
	return stream;
}







// initialize map from filename
GBackgroundHits::GBackgroundHits(string filename, int verbosity)
{
	backgroundHitMap = new map<string, vector<BackgroundHit*> >;

	ifstream bgif(filename.c_str());

	if(bgif.good()) {

	} else {
		cout << " Warning: background file " << filename << " could not be opened. No background events will be merged." << endl;
		return;
	}

	if(verbosity > 0) cout << " Loading background hits from " << filename << endl;

	// file is good, loading hits
	while(!bgif.eof()) {
		string bgline;
		getline(bgif, bgline);

		if(!bgline.size())
			continue;

		vector<string> hitsData = getStringVectorFromString(bgline);

		string systemEventNumber = hitsData[0] + "____" + hitsData[1];

		// load hit from string
		(*backgroundHitMap)[systemEventNumber].push_back(new BackgroundHit(hitsData, verbosity));
	}
}


map<int, vector<BackgroundHit*> >* GBackgroundHits::getBackgroundForSystem(string system)
{
	map<int, vector<BackgroundHit*> > *systemBGHits = new map<int, vector<BackgroundHit*> >;

	for(auto allHits: (*backgroundHitMap)) {

		vector<string> systemAndEvents = getStringVectorFromStringWithDelimiter(allHits.first, "____");

		// found system
		if(systemAndEvents[0] == system) {
			int eventN = stoi(systemAndEvents[1]);
			for(auto bghit: allHits.second) {
				(*systemBGHits)[eventN].push_back(bghit);
			}
		}
	}

	if(systemBGHits->size() > 0) {
		return systemBGHits;
	} else {
		return nullptr;
	}
}





