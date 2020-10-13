#ifndef ST_FWD_TRACK_MAKER_H
#define ST_FWD_TRACK_MAKER_H

#include "StChain/StMaker.h"

#ifndef __CINT__
#include "GenFit/Track.h"
#endif

#include "FwdTrackerConfig.h"

#include <memory>

namespace KiTrack {
class IHit;
};

namespace genfit {
  class Track;
}

class ForwardTracker;
class ForwardHitLoader;
class StarFieldAdaptor;

class StGlobalTrack;
class StRnDHitCollection;
class StTrack;
class StTrackDetectorInfo;
class SiRasterizer;
class McTrack;

// ROOT includes
#include "TNtuple.h"
#include "TTree.h"
// STL includes
#include <vector>
#include <memory>

const size_t MAX_TREE_ELEMENTS = 10000;

class StFwdTrackMaker : public StMaker {

    ClassDef(StFwdTrackMaker, 0);

  public:
    StFwdTrackMaker();
    ~StFwdTrackMaker(){/* nada */};

    int Init();
    int Finish();
    int Make();
    void Clear(const Option_t *opts = "");

    enum { kInnerGeometry,
           kOuterGeometry };

    void SetConfigFile(std::string n) {
        mConfigFile = n;
    }
    void SetGenerateHistograms( bool _genHisto ){ mGenHistograms = _genHisto; }
    void SetGenerateTree(bool _genTree) { mGenTree = _genTree; }

  private:
  protected:

    // Track Seed typdef 
    typedef std::vector<KiTrack::IHit *> Seed_t;

    FwdTrackerConfig mFwdConfig;
    std::unique_ptr<ForwardTracker> mForwardTracker;
    std::unique_ptr<ForwardHitLoader> mForwardHitLoader;
    StarFieldAdaptor *mFieldAdaptor;

    std::unique_ptr<SiRasterizer> mSiRasterizer;

    

    bool mGenHistograms = false;
    std::map<std::string, TH1 *> mHistograms;
    TFile *mTreeFile = 0;
    TTree *mTree = 0;
    bool mGenTree = false;
    std::string mConfigFile;

    // elements used only if the mGenTree = true
    float mTreeX[MAX_TREE_ELEMENTS], mTreeY[MAX_TREE_ELEMENTS], mTreeZ[MAX_TREE_ELEMENTS];
    int mTreeN, mTreeNTracks, mTreeTID[MAX_TREE_ELEMENTS], mTreeVID[MAX_TREE_ELEMENTS], mTreeHPt[MAX_TREE_ELEMENTS], mTreeHSV[MAX_TREE_ELEMENTS];
    float mTreePt[MAX_TREE_ELEMENTS], mTreeEta[MAX_TREE_ELEMENTS], mTreePhi[MAX_TREE_ELEMENTS];
    std::map<string, std::vector<float>> mTreeCrits;
    std::map<string, std::vector<int>> mTreeCritTrackIds;

    // I could not get the library generation to succeed with these.
    // so I have removed them
    #ifndef __CINT__
        void loadMcTracks( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap );
        void loadStgcHits( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
        void loadStgcHitsFromGEANT( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
        void loadStgcHitsFromStEvent( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
        void loadFstHits( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
        void loadFstHitsFromGEANT( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
        void loadFstHitsFromStEvent( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
    #endif
    

    // Fill StEvent
    void FillEvent();
    void FillDetectorInfo(StTrackDetectorInfo *info, genfit::Track *track, bool increment);
    void FillTrack(StTrack *otrack, genfit::Track *itrack, const Seed_t &iseed, StTrackDetectorInfo *info);
    void FillTrackFlags(StTrack *otrack, genfit::Track *itrack);
    void FillTrackGeometry(StTrack *otrack, genfit::Track *itrack, double zplane, int io);
    void FillTrackDcaGeometry ( StGlobalTrack    *otrack, genfit::Track *itrack );
    void FillTrackFitTraits(StTrack *otrack, genfit::Track *itrack);
    void FillTrackMatches(StTrack *otrack, genfit::Track *itrack);
};

#endif
