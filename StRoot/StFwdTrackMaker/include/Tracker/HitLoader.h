#ifndef FWD_HIT_LOADER_H
#define FWD_HIT_LOADER_H

#include <map>
#include "StFwdTrackMaker/include/Tracker/FwdHit.h"

class IHitLoader
{
public:

  virtual unsigned long long nEvents() = 0;
  virtual std::map<int, std::vector<KiTrack::IHit *> > &load( unsigned long long iEvent ) = 0;
  virtual std::map<int, std::vector<KiTrack::IHit *> > &loadSi( unsigned long long iEvent ) = 0;
  virtual std::map<int, shared_ptr<McTrack>> &getMcTrackMap() = 0;
};

#endif
