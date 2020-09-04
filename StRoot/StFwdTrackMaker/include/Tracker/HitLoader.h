#ifndef HitLoader_h
#define HitLoader_h

#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "TRandom3.h"

#include "StFwdTrackMaker/include/Tracker/FwdHit.h"
#include "StFwdTrackMaker/include/Tracker/loguru.h"
#include "StFwdTrackMaker/include/Tracker/ConfigUtil.h"

#include "StFwdTrackMaker/XmlConfig/XmlConfig.h"

class IHitLoader
{
public:

  virtual unsigned long long nEvents() = 0;
  virtual std::map<int, std::vector<KiTrack::IHit *> > &load( unsigned long long iEvent ) = 0;
  virtual std::map<int, std::vector<KiTrack::IHit *> > &loadSi( unsigned long long iEvent ) = 0;
  virtual std::map<int, shared_ptr<McTrack>> &getMcTrackMap() = 0;
};

#endif
