#ifndef FWD_FWD_DATA_SOURCE_H
#define FWD_FWD_DATA_SOURCE_H

#include <map>
#include "StFwdTrackMaker/include/Tracker/FwdHit.h"


/* Authoritative Data Source for Fwd tracks + hits
 *
 * Separation here is not great, but the track/hit loading 
 * is tightly bound to the StMaker through the ttree and histogram
 * creation, as well as through the Datasets (StEvent, geant/* )
 * 
 * So while the filling is done elsewhere, this holds that 
 * data and releases the pointers when needed.
 */
class FwdDataSource {
  public:
    std::map<int, std::vector<KiTrack::IHit *>> &getFttHits( ) {
        return mFttHits;
    };
    std::map<int, std::vector<KiTrack::IHit *>> &getFstHits() {
        return mFstHits;
    };
    std::map<int, shared_ptr<McTrack>> &getMcTracks() {
        return mMcTracks;
    };

    // Cleanup
    void clear() {

      // delete the hits from the hitmap
      for ( auto kv : mFttHits ){
          for ( auto h : kv.second ){
              delete h;
          }
          kv.second.clear();
      }

      for ( auto kv : mFstHits ){
          for ( auto h : kv.second ){
              delete h;
          }
          kv.second.clear();
      }

      // the tracks are shared pointers, so they will be taken care of by clearing the map (below)

      mFttHits.clear();
      mFstHits.clear();
      mMcTracks.clear();
    }

    // TODO, protect and add interaface for pushing hits / tracks
    std::map<int, std::vector<KiTrack::IHit *>> mFttHits;
    std::map<int, std::vector<KiTrack::IHit *>> mFstHits;
    std::map<int, shared_ptr<McTrack>> mMcTracks;
};




#endif
