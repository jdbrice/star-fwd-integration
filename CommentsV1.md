## In StRoot/StFwdTrackMaker/StFwdTrackMaker.h:

```diff
> 
+    void GenerateTree(bool _genTree) { mGenTree = _genTree; }
+
+  private:
+  protected:
+    ForwardTracker *mForwardTracker;
+    ForwardHitLoader *mForwardHitLoader;
+    StarFieldAdaptor *mFieldAdaptor;
+
+    SiRasterizer *mSiRasterizer;
+
+    typedef std::vector<KiTrack::IHit *> Seed_t;
+
+    std::map<std::string, TH1 *> histograms;
+    TFile *mlFile;
+    TTree *mlTree;
+    bool mGenTree;
```
As far as I can tell, this flag creates and fills a root tree and histograms when enabled. Make sure it is set to false by default. I don't see where it is initialized.
### Answer
initialized to false in the ctor initializer list now.


## In StRoot/StFwdTrackMaker/StFwdTrackMaker.h:
```diff
In StRoot/StFwdTrackMaker/StFwdTrackMaker.h:

> 
+    void SetConfigFile(std::string n) {
+        mConfigFile = n;
+    }
+    void GenerateTree(bool _genTree) { mGenTree = _genTree; }
+
+  private:
+  protected:
+    ForwardTracker *mForwardTracker;
+    ForwardHitLoader *mForwardHitLoader;
+    StarFieldAdaptor *mFieldAdaptor;
+
+    SiRasterizer *mSiRasterizer;
+
+    typedef std::vector<KiTrack::IHit *> Seed_t;
+
+    std::map<std::string, TH1 *> histograms;
```
Are these histograms required for the tracking algorithm to work or just here for debugging? If the latter, don't fill them by default
### Answer: 
all histograms are for additional info. Now they are wrapped in a flag `mGenHistograms` which is set to `false` by default (in ctor init list) see: https://github.com/jdbrice/star-fwd-integration/commit/6fce8e2e90e9e8425a52cef237536f3bb154f839


## In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> 
+  double mPti  = ptinv;
+  double mTan  = tanl;
+  double mCurv = curv;
+
+  double p[] = { mImp, mZ, mPsi, mPti, mTan, mCurv };
+
+  // TODO: fill in errors... (do this numerically?)
+  double e[15] = {};
+
+  StDcaGeometry *dca = new StDcaGeometry;
+  otrack->setDcaGeometry(dca);
+  dca->set(p, e);
+}
+
+
+void StFwdTrackMaker::FillDetectorInfo( StTrackDetectorInfo *info, genfit::Track *track, bool increment )
```
What is the input data for this routine? If it is genfit::Track, you should make it const
### Answer 
added const to all, see commit: https://github.com/jdbrice/star-fwd-integration/commit/7389ac39d4fb8ee1ef516662663e96839b22f66b

In StRoot/StFwdTrackMaker/StFwdTrackMaker.h:
```diff
> 
+        void loadStgcHitsFromStEvent( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
+        void loadFstHits( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
+        void loadFstHitsFromGEANT( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
+        void loadFstHitsFromStEvent( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
+    #endif
+    
+
+    // Fill StEvent
+    void FillEvent();
+    void FillDetectorInfo(StTrackDetectorInfo *info, genfit::Track *track, bool increment);
+    void FillTrack(StTrack *otrack, genfit::Track *itrack, const Seed_t &iseed, StTrackDetectorInfo *info);
+    void FillTrackFlags(StTrack *otrack, genfit::Track *itrack);
+    void FillTrackGeometry(StTrack *otrack, genfit::Track *itrack, double zplane, int io);
+    void FillTrackDcaGeometry ( StGlobalTrack    *otrack, genfit::Track *itrack );
+    void FillTrackFitTraits(StTrack *otrack, genfit::Track *itrack);
+    void FillTrackMatches(StTrack *otrack, genfit::Track *itrack);
```
In the above methods itrack should be const if it represent the input data that is not supposed to change. The methods should be declared const if the don't modify the Maker state (which is my guess on their purpose based on their names)
### Answer,
added const, see above and commit here: https://github.com/jdbrice/star-fwd-integration/commit/7389ac39d4fb8ee1ef516662663e96839b22f66b



## In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> 
+    std::string loggerFile = SAttr("logfile"); // user can changed before Init
+    loguru::add_file( loggerFile.c_str(), loguru::Truncate, loguru::Verbosity_2);
+    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
+
```
Unfortunately, I don't think we should introduce another logging library into the STAR library collection. At least I don't think it can be done now as a dependency for this library. The benefit of using loguru is not clear but any additional resource will require someone to maintain it even if it does not look like it needs maintenance now.
### Answer 
loguru has been removed see commit: https://github.com/jdbrice/star-fwd-integration/commit/c81c63d7298622f3f25b1abd86f6135fe37cd54a


## In StRoot/StFwdTrackMaker/XmlConfig/HistoBins.h:
```diff
> 
+{
+
+enum class BinEdge : int {
+   lower,
+   upper,
+   underflow = -1,
+   overflow = -2,
+   undefined = -3,
+   ambiguous = -4		// only used by LUT to signal that it needs fallback
+};
+
+/* HistoBins provides a container for storing and using bins
+ *
+ * Provides a stand-alone binning container. Can be create directly from configs.
+ */
+class HistoBins : public IObject
```
Why do we need this HistoBins class? If it is not used for tracking it should be removed
### Answer
Removed

## In StRoot/StFwdTrackMaker/XmlConfig/XmlConfig.h:
```diff
> 
+
+#include <sys/stat.h>
+
+#include "StFwdTrackMaker/XmlConfig/Utils.h"
+#include "StFwdTrackMaker/XmlConfig/IObject.h"
+#include "StFwdTrackMaker/XmlConfig/RapidXmlWrapper.h"
+
+
+namespace jdb
+{
+
+class XmlString;
+
+using namespace std;
+
+class XmlConfig : public IObject
```
How is this class XmlConfig used by the library? I see only one .xml file defining five parameters. Do we really need to add thousands of lines of code just to read the five parameters?
### Answer
Removed



## In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> 
+    // // Add the track to its track node
+    // trackNode->addTrack( globalTrack );
+    // trackNodes.push_back( trackNode );
+
+    // NOTE: could we qcall here mForwardTracker->fitTrack( seed, vertex ) ?
+
+  } // end of loop over tracks
+
+  LOG_INFO << "  number visited  = " <<   track_count_total  << endm;
+  LOG_INFO << "  number accepted = " <<   track_count_accept << endm;
+}
+
+
+void StFwdTrackMaker::FillTrack( StTrack *otrack, genfit::Track *itrack, const Seed_t &iseed, StTrackDetectorInfo *info )
+{
+  const double z_stgc[] = { 280.9, 303.7, 326.6, 349.4 };
```
Are these values supposed to come from the STAR geometry?
### Answer
Updated to read from geometry, see commit : https://github.com/jdbrice/star-fwd-integration/commit/1d8fa5ab66b8141182c33d05aad051018d5fc184

## In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> 
+            if (mGenTree) {
+                LOG_F(INFO, "mlt_nt = %d == track_id = %d, is_shower = %d, start_vtx = %d", mlt_nt, track_id, track->is_shower, track->start_vertex_p);
+                mlt_pt[mlt_nt] = pt;
+                mlt_eta[mlt_nt] = eta;
+                mlt_phi[mlt_nt] = phi;
+                mlt_nt++;
+            }
+
+        } // loop on track (irow)
+    } // if g2t_track
+} // loadMcTracks
+
+
+//________________________________________________________________________
+int StFwdTrackMaker::Make() {
+    LOG_INFO << "StFwdTrackMaker::Make()   " << endm;
```
Please try to minimize the output from the maker. In general, makers should log only warnings and errors. The current log files already have too much of unnecessary messages.
### Answer
Removed almost all log statements, also as part of the removal of loguru.
The remaining statements are meant to provide important context.
See: https://github.com/jdbrice/star-fwd-integration/commit/c81c63d7298622f3f25b1abd86f6135fe37cd54a

### In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> 
+        const StTrackNode *node = stEvent->trackNodes()[i];
+        StGlobalTrack *track = (StGlobalTrack *)node->track(global);
+        StTrackGeometry *geometry = track->geometry();
+
+        StThreeVectorF origin = geometry->origin();
+        StThreeVectorF momentum = geometry->momentum();
+
+        LOG_INFO << "-------------------------------------------------------------------------------" << endm;
+        LOG_INFO << "Track # " << i << endm;
+        LOG_INFO << "inner: Track origin: " << origin << " momentum: " << momentum << " pt=" << momentum.perp() << " eta=" << momentum.pseudoRapidity() << endm;
+
+        StDcaGeometry *dca = track->dcaGeometry();
+        if ( dca ) {
+            origin = dca->origin();
+            momentum = dca->momentum();
+            LOG_INFO << "d c a: Track origin: " << origin << " momentum: " << momentum << " pt=" << momentum.perp() << " eta=" << momentum.pseudoRapidity() << endm;
```
This is an example of unnecessary excessive print out
### Answer
Removed along with other similar print out, see above.


## In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> 
+
+        StThreeVectorF origin = geometry->origin();
+        StThreeVectorF momentum = geometry->momentum();
+
+        LOG_INFO << "-------------------------------------------------------------------------------" << endm;
+        LOG_INFO << "Track # " << i << endm;
+        LOG_INFO << "inner: Track origin: " << origin << " momentum: " << momentum << " pt=" << momentum.perp() << " eta=" << momentum.pseudoRapidity() << endm;
+
+        StDcaGeometry *dca = track->dcaGeometry();
+        if ( dca ) {
+            origin = dca->origin();
+            momentum = dca->momentum();
+            LOG_INFO << "d c a: Track origin: " << origin << " momentum: " << momentum << " pt=" << momentum.perp() << " eta=" << momentum.pseudoRapidity() << endm;
+        }
+        else {
+            LOG_INFO << "d c a geometry missing" << endm;
```
And this one is acceptable if it helps to identify the problem
### Answer
Thanks for clarification

## In StRoot/StFwdTrackMaker/include/Tracker/CriteriaKeeper.h:
```diff
> @@ -0,0 +1,72 @@
+#ifndef CRITERIA_KEEPER_H
+#define CRITERIA_KEEPER_H
+
+namespace KiTrack {
```
Hm... I am not sure using the namespace from another library for your own code is a good idea. It can cause some confusion but I don't see any benefits.
### Answer
removed KiTrack namespace see commit: https://github.com/jdbrice/star-fwd-integration/commit/97019fbb98a1946599d7b39457a31f826f39a484


## In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> 
+            FwdHit *hit = new FwdHit(count++, x, y, z, -plane_id, track_id, hitCov3, mcTrackMap[track_id]);
+
+            // Add the hit to the hit map
+            hitMap[hit->getSector()].push_back(hit);
```
I don't see where you deallocate the requested memory for the hits. If you don't delete the hits the memory will leak.
### Answer
Hits are now deleted at the end of the Make() function, see commit : https://github.com/jdbrice/star-fwd-integration/commit/17d5ae01418676b3d99f4d20ca4eccdce520ca9d

## In StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:
```diff
> 
+        _covmat = covmat;
+
+        int _map[] = {0, 0, 0, 0, 0, 1, 2, 0, 0, 3, 4, 5, 6}; // ftsref6a
+
+        if (vid > 0)
+            _sector = _map[vid];
+        else {
+            _sector = abs(vid); // set directly if you want
+            // now set vid back so we retain info on the tru origin of the hit
+            _vid = abs(vid) + 9; // we only use this for sTGC only.  Needs to be
+                                 // cleaner in future.
+        }
+    };
+
+    const KiTrack::ISectorSystem *getSectorSystem() const {
+        return gFwdSystem;
+        
```
This is very odd. You are returning a global pointer

### Answer

Agree. Modified to be an instance managed by StFwdTrackMaker that can be reused by the `FwdConnector` as needed. See commit : https://github.com/jdbrice/star-fwd-integration/commit/fe6d9fd3bcf812b1c4a340b936ddbdee2c7b0767

## In StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:
```diff
> +        _covmat.ResizeTo( 3, 3 );
+        _covmat = covmat;
+
+        int _map[] = {0, 0, 0, 0, 0, 1, 2, 0, 0, 3, 4, 5, 6}; // ftsref6a
+
+        if (vid > 0)
+            _sector = _map[vid];
+        else {
+            _sector = abs(vid); // set directly if you want
+            // now set vid back so we retain info on the tru origin of the hit
+            _vid = abs(vid) + 9; // we only use this for sTGC only.  Needs to be
+                                 // cleaner in future.
+        }
+    };
+
+    const KiTrack::ISectorSystem *getSectorSystem() const {
```
The function seems to be unused. Remove it.

## Answer

This is used internally by the `FwdConnector`, to it is key but updated with the above changes. 

## In StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:
```diff
> 
+        if (vid > 0)
+            _sector = _map[vid];
+        else {
+            _sector = abs(vid); // set directly if you want
+            // now set vid back so we retain info on the tru origin of the hit
+            _vid = abs(vid) + 9; // we only use this for sTGC only.  Needs to be
+                                 // cleaner in future.
+        }
+    };
+
+    const KiTrack::ISectorSystem *getSectorSystem() const {
+        return gFwdSystem;
+    } // need to implement
+
+    int _tid; // aka ID truth
+    int _vid;
```
Need to correct variable names according to the STAR guidelines. From https://www.star.bnl.gov/public/comp/sofi/guidelines/formatguide.xml#Variable_Names

Data members (also called instance variables or member variables) are prefixed with m. If the data member is static, prefix the variable with s instead.

### Answer
I have corrected the naming in several places throughout the code. However, this is actually one place where it should not/cannot be changed. The underscore prefix is used for member variables in the iLCSoft code. For instance, the `IHIT` interface used throughout defines `_x, _y, _z` - so in order to be consistent with the class I have kept the naming here the same.

Everywhere else that makes sense to adopt the STAR convention is updated in this file, see commit : https://github.com/jdbrice/star-fwd-integration/commit/e3308706eb4fde0e2822a4d56f1bc0b16cf43af3



## In StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:
```diff
> 
+
+        if (disk > 1 && _distance >= 2)
+            r.insert(disk - 2);
+
+        if (disk > 2 && _distance >= 3)
+            r.insert(disk - 3);
+
+        if (disk > 3 && _distance >= 4)
+            r.insert(disk - 4);
+
+        return r;
+    };
+
+  private:
+  protected:
+    const FwdSystem _system; // numbering system
```
variable unused?
### Answer
Yes, removed.


## In StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:
```diff
> +class SeedQual {
+  public:
+    inline double operator()(Seed_t s) { return double(s.size()) / 7.0; }
+};
```
What is 7? The number of layers? Why divide?

### Answer 

7 is the number of z-layers in the forward tracking system. However, track seeds are formed only from the hits in the 4 layers of the sTGC detectors. Updated to refer to the authoritative source for this instead of a hard-coded value, see commit : https://github.com/jdbrice/star-fwd-integration/commit/f06769c262c351103e4ba50accdf31741fab5997

```diff
- class SeedQual {
-  public:
-    inline double operator()(Seed_t s) { return double(s.size()) / 7.0; 
+ struct SeedQual {
+    inline double operator()(Seed_t s) { return double(s.size()) / FwdSystem::sNFttLayers ; } // seeds only use the 4 hits from Ftt
};
```
We divide because the ranking algorithm expects a weight from `0 - 1.0`



## In StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:
```diff
> 
+class SeedCompare {
+  public:
```
Why do you define everything as class and then give public access? Isn't cleaner to just define the types as struct in such cases?
### Answer
Sure, changed to a `struct`
```diff
> 
-class SeedCompare {
-  public:
+ struct SeedCompare
```

## In StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:
```diff
> +    inline bool operator()(Seed_t trackA, Seed_t trackB) {
+        std::map<unsigned int, unsigned int> hit_counts;
+        // we are assuming that the same hit can never be used twice on a single
+        // track!
+
+        for (auto h : trackA) {
+            hit_counts[static_cast<FwdHit *>(h)->_id]++;
+        }
+
+        // now look at the other track and see if it has the same hits in it
+        for (auto h : trackB) {
+            hit_counts[static_cast<FwdHit *>(h)->_id]++;
+
+            // incompatible if they share a single hit
+            if (hit_counts[static_cast<FwdHit *>(h)->_id] >= 2) {
+                return false;
+            }
+        }
+
+        // no hits are shared, they are compatible
+        return true;
+    }
```
Use std::equal()
https://en.cppreference.com/w/cpp/algorithm/equal

### Answer
I dont think the `std::equal()` implementation of this is more compact. Maybe I miss something, but I dont want to know if the ranges are equal, but share one or more elements. If I use `std::equal()` wouldnt I still need loops? Let as is for now, but open to improvements.


## In StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:
```diff
> 
+#ifndef FwdHit_h
+#define FwdHit_h
+
+#include "KiTrack/IHit.h"
+#include "KiTrack/ISectorConnector.h"
+#include "KiTrack/ISectorSystem.h"
+#include "KiTrack/KiTrackExceptions.h"
+
+#include <memory>
+#include <set>
+#include <string.h>
+#include <vector>
+
+class StHit;
+
+class FwdSystem : public KiTrack::ISectorSystem {
```
Is this class really used anywhere?

## Answer
yes it is used internally by the `FwdHit` - and now provides authoritative info about the # of layers in the FWD tracking system. 



## In StRoot/StFwdTrackMaker/include/Tracker/TrackFitter.h:
```diff
> +        }
+
+        // TODO : Load the STAR MagField
+        genfit::AbsBField *bField = nullptr;
+
+        if (0 == _gField) {
+            if (cfg.get<bool>("TrackFitter:constB", false)) {
+                bField = new genfit::ConstField(0., 0., 5.);
+                LOG_F(INFO, "Using a CONST B FIELD");
+            } else {
+                bField = new genfit::STARFieldXYZ();
+                LOG_F(INFO, "Using STAR B FIELD");
+            }
+        } else {
+            LOG_F(INFO, "Using StarMagField interface");
+            bField = _gField;
```
Why do you need to use a global pointer to setup the local variable bField? Can't you just use the StarFieldAdaptor directly?

### ANSWER
Good point, there is no reason to do it this way. Now the `StarFieldAdaptor` is created and owned by the `TrackFitter`, the global pointer is removed both here and in `STARField.h`.

Now implemented as:
```cpp
// Determine which Magnetic field to use
// Either constant field or real field from StarFieldAdaptor
if (mConfig.get<bool>("TrackFitter:constB", false)) {
	mBField = std::unique_ptr<genfit::AbsBField>(new genfit::ConstField(0., 0., 5.)); // 0.5 T Bz
	LOG_INFO << "StFwdTrackMaker: Tracking with constant magnetic field" << endl;
} else {
	mBField = std::unique_ptr<genfit::AbsBField>(new StarFieldAdaptor());
	LOG_INFO << "StFwdTrackMaker: Tracking with StarFieldAdapter" << endl;
}
// we must have one of the two available fields at this point
// note, the pointer is still bound to the lifetime of the TackFitter
genfit::FieldManager::getInstance()->init(mBField.get()); 
```
where `mBfield` is now a `unique_ptr<genfit::AbsBField>` so that the memory is tied to the class's life



## In StRoot/StFwdTrackMaker/include/Tracker/STARField.h:
```diff
> 
+namespace genfit
+{
+
```
I don't think it is a good idea to populate the namespace from the external library with your own types. Why? Didn't you mean to use using namespace genfit?
### Answer
Agree, moved out of `genfit` namespace


## In StRoot/StFwdTrackMaker/include/Tracker/STARField.h:
```diff
> 
+    };
+};
+
+namespace genfit
+{
+
+//_______________________________________________________________________________________
+// Adaptor for STAR magnetic field loaded from a ROOT file with the field in cartesian coords
+class STARFieldXYZ : public genfit::AbsBField
+{
+public:
+
+  STARFieldXYZ()
+  {
+    LOG_F( INFO, "Loading STAR Magnetic Field map" );
+    fField = new TFile( "FieldOnXYZ.root" );
```
Wow.. No, you don't want to read the field from a root file. I don't know how STARFieldXYZ is used but it needs to be reconsidered. Is it supposed to be used in place of the existing StarMagField package?

### ANSWER
Agreed. This is no longer used and is now removed (was only used outside of StRoot environment)


# Comments on the changes since last review

## In StRoot/StFwdTrackMaker/Common.h:
```diff
> @@ -0,0 +1,17 @@
+#ifndef FWD_TRACK_MAKER_COMMON_H
+#define FWD_TRACK_MAKER_COMMON_H
+
```
It might be a good idea to `#include` headers for all used external types. In this case I see TAxis and std::vector
I believe the file compiles for you now but it may not if, for example, a different build system with different setup is used.

### ANSWER
I agree, I've added a few headers

## In StRoot/StFwdTrackMaker/Common.h:
```diff
> @@ -0,0 +1,17 @@
+#ifndef FWD_TRACK_MAKER_COMMON_H
+#define FWD_TRACK_MAKER_COMMON_H
+
+class FwdTrackerUtils {
+    public:
+    static void labelAxis( TAxis *_x, vector<string> _labels )
+    {
+       if ( nullptr == _x ) return;
```
It's a stylistic thing but a universally accepted way to check for valid pointers is if (!x) ...
### Answer 
Taken
```diff
- if ( nullptr == axis ) return;
+ if ( !axis ) return;
```

## In StRoot/StFwdTrackMaker/Common.h:
```diff
> @@ -0,0 +1,17 @@
+#ifndef FWD_TRACK_MAKER_COMMON_H
+#define FWD_TRACK_MAKER_COMMON_H
+
+class FwdTrackerUtils {
+    public:
+    static void labelAxis( TAxis *_x, vector<string> _labels )
```
I don't think the underscore prefix is consistent with the coding guidelines. Why?
### ANSWER
Agree, this was reused code from another project, style has been updated.
It is now:
```cpp
static void labelAxis( TAxis *axis, std::vector<std::string> labels )
{
   if ( !axis ) return;

   for ( int i = 0; i <= axis->GetNbins(); i++ ) {
	  if ( i < labels.size() )
		 axis->SetBinLabel( i + 1, labels[i].c_str() );
   }
}
```

## In StRoot/StFwdTrackMaker/Common.h:
```diff
> @@ -0,0 +1,17 @@
+#ifndef FWD_TRACK_MAKER_COMMON_H
+#define FWD_TRACK_MAKER_COMMON_H
+
+class FwdTrackerUtils {
+    public:
+    static void labelAxis( TAxis *_x, vector<string> _labels )
+    {
+       if ( nullptr == _x ) return;
+
+       for ( unsigned int i = 0; i <= static_cast<unsigned int>(_x->GetNbins()); i++ ) {
```
Don't bother with explicit conversion from signed to unsigned. Not sure what you gain by doing so.
### Answer
I agree in this case it does not make sense/is not needed. 
In some cases I used `static_cast<T>(...)` to avoid compiler warnings on RCF (using cons) - In some cases I am not sure how to handle both 32 and 64 bit without warnings. Removed this but then there are compiler warnings.
	
	
## In StRoot/StFwdTrackMaker/FwdTrackerConfig.cxx:
```diff
> @@ -0,0 +1,47 @@
+#include "FwdTrackerConfig.h"
```
⬇️ Suggested change
```diff
-#include "FwdTrackerConfig.h"
+#include "StFwdTrackMaker/FwdTrackerConfig.h"
```
### Answer
Agree, taken


## In StRoot/StFwdTrackMaker/FwdTrackerConfig.cxx:
```diff
> 
+#include "FwdTrackerConfig.h"
+
+const std::string FwdTrackerConfig::valDNE = std::string( "<DNE/>" );
+const std::string FwdTrackerConfig::pathDelim = std::string( "." );
+const std::string FwdTrackerConfig::attrDelim = std::string( ":" );
+std::stringstream FwdTrackerConfig::sstr;
+
+////
+// template specializations
+////
+
+// Specialization for string to avoid extra conversions
+template <>
+std::string FwdTrackerConfig::get( std::string path, std::string dv ) const {
+    // return default value if path DNE
+    if ( false == exists( path ) )
```
⬇️ Suggested change
```diff
-    if ( false == exists( path ) )
+    if ( !exists( path ) )
```

### Answer
Agree, taken.  Changed in several places in `StRoot/StFwdTrackMaker/FwdTrackerConfig.h` and in one place in`StRoot/StFwdTrackMaker/FwdTrackerConfig.cxx`


## In StRoot/StFwdTrackMaker/FwdTrackerConfig.h:
```diff
> 
+class FwdTrackerConfig {
+protected:
+
+    static const std::string valDNE; // used for nodes that DNE
+    static const std::string pathDelim; // separate node levels
+    static const std::string attrDelim; // separate attributes on nodes
+
+    bool mErrorParsing = false;
+    // read only map of the config, read with get<> functions
+    std::map<std::string, std::string> mNodes;
+    static std::stringstream sstr; // reused for string to numeric conversion
+
+    void mapFile(TXMLEngine &xml, XMLNodePointer_t node, Int_t level, std::string path = "") {
+        using namespace std;
+        // add the path delimeter above top level
+        if ( "" != path) path += FwdTrackerConfig::pathDelim;
```
⬇️ Suggested change
```diff
-        if ( "" != path) path += FwdTrackerConfig::pathDelim;
+        if ( !path.empty() ) path += FwdTrackerConfig::pathDelim;
```
### Answer
Answer: Taken

## In StRoot/StFwdTrackMaker/FwdTrackerConfig.h:
```diff
> 
+        size_t len = 3 * (pos != std::string::npos); // 3 if true, 0 if false
+        pos = pos * (pos != std::string::npos); // pos if true, 0 if false
```
Use ternary operator?
### Answer
Sure, since compilers are pretty good at optimizing the ternary operator it is equivalent.  Taken:

```diff
- size_t len = 3 * (pos != std::string::npos); // 3 if true, 0 if false
- pos = pos * (pos != std::string::npos); // pos if true, 0 if false
+ size_t len = (pos != std::string::npos) ? 3 : 0;
+ pos = (pos != std::string::npos) ? pos : 0;

```


## In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> @@ -193,7 +193,7 @@ class ForwardTracker : public ForwardTrackMaker {
 
         // initialize the track fitter
         trackFitter = new TrackFitter(cfg);
```
Don't you need to delete this object in destructor to avoid memory leaks?
### Answer
Yes you are right that this leaks the memory of the `trackFitter` object. Now deleted in the `finish` member function:

```diff
+ if (mTrackFitter){
+	 delete mTrackFitter;
+	 mTrackFitter= 0;
+ }
```



### In StRoot/StFwdTrackMaker/include/Tracker/TrackFitter.h:
```diff
> @@ -247,6 +242,8 @@ class TrackFitter {
     }
 
     void writeHistograms() {
+        if ( false == mGenHistograms )
```
⬇️ Suggested change
```diff
-        if ( false == mGenHistograms )
+        if ( !mGenHistograms )
```

#### Answer
Agreed, taken.

## In StRoot/StFwdTrackMaker/include/Tracker/TrackFitter.h:

```diff
> @@ -809,7 +806,7 @@ class TrackFitter {
 
             // Clone the cardinal rep for persistency
             fTrackRep = cardinalRep->clone(); // save the result of the fit
-            if (fitTrack.getFitStatus(cardinalRep)->isFitConverged()) {
+            if (fitTrack.getFitStatus(cardinalRep)->isFitConverged() && true == mGenHistograms ) {
```
⬇️ Suggested change
```diff
-            if (fitTrack.getFitStatus(cardinalRep)->isFitConverged() && true == mGenHistograms ) {
+            if (fitTrack.getFitStatus(cardinalRep)->isFitConverged() && mGenHistograms ) {
```

### Answer
Agreed, taken.



## In StRoot/StFwdTrackMaker/FwdTrackerConfig.h:

```diff
> +            attr = xml.GetNextAttr(attr);
+        }
+
+        // recursively get child nodes
+        XMLNodePointer_t child = xml.GetChild(node);
+        while (child != 0) {
+            mapFile(xml, child, level + 1, path);
+            child = xml.GetNext(child);
+        }
+    } // mapFile
+public:
+
+    // sanitizes a path to its canonical form 
+    static void canonize( std::string &path ) {
+        // remove whitespace
+        path.erase(std::remove_if(path.begin(), path.end(), ::isspace), path.end());
```
`::isspace` Are you specifically calling the C function from the global namespace? Why?

⬇️ Suggested change
```diff
-        path.erase(std::remove_if(path.begin(), path.end(), ::isspace), path.end());
+        path.erase(std::remove_if(path.begin(), path.end(), std::isspace), path.end());
```

### Answer
Actually, std::isspace is overloaded and cannot be used without more context provided (see:  https://stackoverflow.com/questions/21578544/stdremove-if-and-stdisspace-compile-time-error). I could use a lmabda, but why when `::isspace` is available - left as is. If you know of a better way to resolve this then let me know.


## In StRoot/StFwdTrackMaker/include/Tracker/TrackFitter.h:
```diff
> 
+    TVector3 fitSpacePoints( vector<genfit::SpacepointMeasurement*> spoints, TVector3 &seedPos, TVector3 &seedMom ){
+        LOG_SCOPE_FUNCTION(INFO);
+        auto trackRepPos = new genfit::RKTrackRep(pdg_mu_plus);
+        auto trackRepNeg = new genfit::RKTrackRep(pdg_mu_minus);
+
+        auto fTrack = new genfit::Track(trackRepPos, seedPos, seedMom);
+        fTrack->addTrackRep(trackRepNeg);
+
+        genfit::Track &fitTrack = *fTrack;
+
+
+        try {
+            for ( size_t i = 0; i < spoints.size(); i++ ){
+                fitTrack.insertPoint(new genfit::TrackPoint(spoints[i], &fitTrack));
+            }
+
+            LOG_SCOPE_F(INFO, "Track Fit with GENFIT2");
+            // do the fit
+            LOG_F(INFO, "Processing Track");
+            fitter->processTrackWithRep(&fitTrack, trackRepPos);
+            fitter->processTrackWithRep(&fitTrack, trackRepNeg);
+
+        } catch (genfit::Exception &e) {
+            LOG_F(INFO, "%s", e.what());
+            LOG_F(INFO, "Exception on track fit");
+            std::cerr << e.what();
+        }
+
+        try {
+        fitTrack.checkConsistency();
+
+        fitTrack.determineCardinalRep();
+        auto cardinalRep = fitTrack.getCardinalRep();
+
+        TVector3 p = cardinalRep->getMom(fitTrack.getFittedState(1, cardinalRep));
+
+        return p;
+        } catch (genfit::Exception &e) {
+            LOG_F(INFO, "%s", e.what());
+            LOG_F(INFO, "Exception on track fit");
+            std::cerr << e.what();
+        }
+        return TVector3(0, 0, 0);
+
+    }
```
This function is not called. Is it necessary to include it?
### Answer
Jason previously has asked me to provide generic functionality for fitting with GenFit. This method provides that functionality in a way that is totally independent of all other aspects of this class, modulo the setup. It is also helpful moving forward with integrating other detectors into the track refitting.


## In StRoot/StFwdTrackMaker/include/Tracker/TrackFitter.h:
```diff
> 
+            LOG_F(WARNING, "Original Track fit did not converge, skipping");
+            return pOrig;
+        }
+
+        auto trackRepPos = new genfit::RKTrackRep(pdg_mu_plus);
+        auto trackRepNeg = new genfit::RKTrackRep(pdg_mu_minus);
+
+        auto trackPoints = originalTrack->getPointsWithMeasurement();
+        LOG_F(INFO, "trackPoints.size() = %lu", trackPoints.size());
+        if ((trackPoints.size() < 5 && includeVertexInFit) || trackPoints.size() < 4) {
+            LOG_F(ERROR, "TrackPoints missing");
+            return pOrig;
+        }
+
+        TVectorD rawCoords = trackPoints[0]->getRawMeasurement()->getRawHitCoords();
+        double z = 280.9; //first Stgc, a hack for now
```
I am a bit confused. I thought you had access to the global geometry from where you can get the actual detector position. If this is the case than you shouldn't be using hard coded constants.

### Answer
Yes and no. The Si is NOT in the geometry that is passed to GenFit at this time. We are dealing with a separate issue that Jason and I have been working on, where GenFit is unable to handle the high-detail silicon geometry. I expect this to be solved at some point, at which point this should be updated. For now the deetector positions must be provided in some way. 

Your point is taken for the sTGC though - since the config requires a default, I use 0 for the sTGC default positions to avoid any hard-coded value that could work - which might lead to confusion.
Now implemented more compactly as:
```cpp
mFTTZLocations = fwdGeoUtils.fttZ( mConfig.getVector<double>("TrackFitter.Geometry:ftt", {0.0f, 0.0f, 0.0f, 0.0f}));
```

## In StRoot/StFwdTrackMaker/include/Tracker/TrackFitter.h:
```diff
> 
+        vector<float> SI_DET_Z = cfg.getFloatVector("TrackFitter.Geometry:si");
+        if (SI_DET_Z.size() < 3) {
+        
+            // try to read from GEOMETRY
+            if ( fwdGeoUtils.siZ( 0 ) > 1.0 ) { // returns 0.0 on failure
+                SI_DET_Z.clear();
+                SI_DET_Z.push_back( fwdGeoUtils.siZ( 0 ) );
+                SI_DET_Z.push_back( fwdGeoUtils.siZ( 1 ) );
+                SI_DET_Z.push_back( fwdGeoUtils.siZ( 2 ) );
+                LOG_F( INFO, "From GEOMETRY : Si Z = %0.2f, %0.2f, %0.2f", SI_DET_Z[0], SI_DET_Z[1], SI_DET_Z[2] );
+            } else {
+                LOG_F(WARNING, "Using Default Si z locations - that means FTSM NOT in Geometry");
+                SI_DET_Z.push_back(140.286011);
+                SI_DET_Z.push_back(154.286011);
+                SI_DET_Z.push_back(168.286011);
+            }
+        } else {
+            LOG_F( WARNING, "Using Si Z location from config - may not match real geometry" );
+        }
+
+        for (auto z : SI_DET_Z) {
+            LOG_F(INFO, "Adding Si Detector Plane at (0, 0, %0.2f)", z);
+            SiDetPlanes.push_back(genfit::SharedPlanePtr(new genfit::DetPlane(TVector3(0, 0, z), TVector3(1, 0, 0), TVector3(0, 1, 0))));
+        }
```
Using hard coded detector positions ( or coming from a config file) is probably not what we want. Can these values be taken from the official STAR geometry? Do we need to confirm with Jason that this information is available?
### Answer
See answer above


## In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> 
+class ForwardHitLoader : public IHitLoader {
+  public:
+    unsigned long long nEvents() { return 1; }
+    std::map<int, std::vector<KiTrack::IHit *>> &load(unsigned long long) {
+        return _hits;
+    };
+    std::map<int, std::vector<KiTrack::IHit *>> &loadSi(unsigned long long) {
+        return _fsi_hits;
+    };
+    std::map<int, shared_ptr<McTrack>> &getMcTrackMap() {
+        return _mctracks;
+    };
+
+    // Cleanup
+    void clear() {
+        _hits.clear();
+        _fsi_hits.clear();
+        _mctracks.clear();
+    }
+
+    // TODO, protect and add interaface for pushing hits / tracks
+    std::map<int, std::vector<KiTrack::IHit *>> _hits;
+    std::map<int, std::vector<KiTrack::IHit *>> _fsi_hits;
+    std::map<int, shared_ptr<McTrack>> _mctracks;
+};
```
I don't quite understand the design of this class. It merely holds the containers with hits which are filled outside of this class. Wouldn't it make sense to move such functionality inside this class? After all the name "HitLoader" suggests that what it supposed to do.

Also, it is not clear why you want to intoduce another level of abstraction (via IHitLoader). Do you plan to introduce another HitLoader conforming to this interface in the future?
### Answer
Yes we have several hit loaders, I did not include all of them in this production code.  It originally worked the way you suggest but was modified to move more functionality into the StFwdTrackMaker class. In order to keep compatibility with the non-production versions that are still used for studies, I'd like to keep it. 

## In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> 
+        return _fsi_hits;
+    };
+    std::map<int, shared_ptr<McTrack>> &getMcTrackMap() {
+        return _mctracks;
+    };
+
+    // Cleanup
+    void clear() {
+        _hits.clear();
+        _fsi_hits.clear();
+        _mctracks.clear();
+    }
+
+    // TODO, protect and add interaface for pushing hits / tracks
+    std::map<int, std::vector<KiTrack::IHit *>> _hits;
+    std::map<int, std::vector<KiTrack::IHit *>> _fsi_hits;
```
Here is an inconsistency in my opinion. You introduce two separate containers for _hits and _fsi_hits but both types of hits are abstracted with the same KiTrack::IHit type. The problem here is that you either use a single container with a common type or two containers with independent types.

### Answer
I understand your point but this was done intentionally as an optimization. Having in separate maps speeds up a lot of the searching done in the internal steps. This is mostly since I am using KiTrack, I dont have a good way of telling the internal code that not all layers are used. 

## In StRoot/StFwdTrackMaker/include/Tracker/TrackFitter.h:
```diff
> 
+    jdb::XmlConfig &cfg;
+    std::map<std::string, TH1 *> hist;
+    bool MAKE_HIST = true;
+    genfit::EventDisplay *display;
+    std::vector<genfit::Track *> event;
+    vector<genfit::Track> displayTracks;
+    bool makeDisplay = false;
+    genfit::AbsKalmanFitter *fitter = nullptr;
+
+    const int pdg_pi_plus = 211;
+    const int pdg_pi_minus = -211;
+    const int pdg_mu_plus = 13;
+    const int pdg_mu_minus = -13;
+
+    genfit::AbsTrackRep *pion_track_rep = nullptr;
+    vector<genfit::SharedPlanePtr> DetPlanes;
+    vector<genfit::SharedPlanePtr> SiDetPlanes;
+
+    TRandom *rand = nullptr;
+
+    // parameter ALIASED from cfg
+    float vertexSigmaXY = 1;
+    float vertexSigmaZ = 30;
+    vector<float> vertexPos;
+    bool includeVertexInFit = false;
+    bool useSi = true;
+    bool skipSi0 = false;
+    bool skipSi1 = false;
+
+    genfit::FitStatus fStatus;
+    genfit::AbsTrackRep *fTrackRep;
+    genfit::Track *fTrack;
+
+    // Fit results
+    TVector3 _p;
+    double _q;
```
Another reminder that you need to update data member names according to the STAR style guidelines. Not just here but in other classes as well.
### Answer
Done and unused variables are removed and names are updated. Mostly completed in this commit with some fixes later : https://github.com/jdbrice/star-fwd-integration/commit/90e5a610bc9d94f83086cbf32cbb2e96c178b8d3#diff-c731c9d706512da383c2f484b0d5c919fed3942f544be651722c86d19d8e3567


In StRoot/StFwdTrackMaker/include/Tracker/TrackFitter.h:
```diff
> 
+    float vertexSigmaXY = 1;
+    float vertexSigmaZ = 30;
+    vector<float> vertexPos;
```
I would use double precision for all transient variables used in calculations unless you have a special reason to define them as float.
### Answer
Agree,
```diff
> 
-    float vertexSigmaXY = 1;
-    float vertexSigmaZ = 30;
-    vector<float> vertexPos;
+    double vertexSigmaXY = 1;
+    double vertexSigmaZ = 30;
+    vector<double> vertexPos;
```

## In StRoot/StFwdTrackMaker/include/Tracker/TrackFitter.h:
```diff
> 
+    const int pdg_pi_plus = 211;
+    const int pdg_pi_minus = -211;
```
Unused variables should be removed from production code. I haven't checked all of them but these two is the case.
### Answer
Agree, unused variables have been removed. Note that I moved to consistently use pi for the track hypothesis instead of muons - so these variables are now used, but the others for muons are removed. 


## In StRoot/StFwdTrackMaker/include/Tracker/TrackFitter.h:
```diff
> 
+        auto trackRepPos = new genfit::RKTrackRep(pdg_mu_plus);
+        auto trackRepNeg = new genfit::RKTrackRep(pdg_mu_minus);
```
You specify muons as underlying particles. Does it really affect genfit tracking in any way if you choose another species? Does genfit require you to specify it and if not what is their default?

### Answer
Updated to use pions as per the rest of STAR fitting (and remove muon pdg vals, per above comment). 
- Yes it must be specified
- Yes it effects the track fitting - require energy loss correction per species later on
- in principle we could simultaneously fit to more particle types but I am not sure if it is benefitial 

## In StRoot/StFwdTrackMaker/include/Tracker/FwdTracker.h:
```diff
> 
+#include "Criteria/Criteria.h"
+#include "Criteria/ICriterion.h"
+#include "KiTrack/Automaton.h"
+#include "KiTrack/SegmentBuilder.h"
+#include "KiTrack/SubsetHopfieldNN.h"
+
+#include "CriteriaKeeper.h"
+
+#include "GenFit/FitStatus.h"
+
+#include "StFwdTrackMaker/XmlConfig/XmlConfig.h"
+
+// Utility class for evaluating ID and QA truth
+struct MCTruthUtils {
+
+    static int domCon(std::vector<KiTrack::IHit *> hits, float &qual) {
```
Please come up with a better name for the domCon function
### Answer
changed to `dominantContribution` since the function determines the `quality` and `idTruth` for the track with the dominant number of hits. 

## In StRoot/StFwdTrackMaker/include/Tracker/FwdTracker.h:
```diff
> 
+#include "CriteriaKeeper.h"
+
+#include "GenFit/FitStatus.h"
+
+#include "StFwdTrackMaker/XmlConfig/XmlConfig.h"
+
+// Utility class for evaluating ID and QA truth
+struct MCTruthUtils {
+
+    static int domCon(std::vector<KiTrack::IHit *> hits, float &qual) {
+        std::map<int, int> count;
+        int total = 0;
+
+        for (auto *h : hits) {
+            auto *hit = dynamic_cast<FwdHit *>(h);
+            if (0 == hit)
```
Isn't your code who adds hits into the container? If so, you can guarantee to have only pointers to valid objects and avoid the if (!hit) checks here and elsewhere. It might be even better if you have copies of the hits in the container.

### Answer
Yes,unneeded  checks are removed.


## In StRoot/StFwdTrackMaker/include/Tracker/FwdTracker.h:
```diff
> 
+        for (auto *h : hits) {
+            auto *hit = dynamic_cast<FwdHit *>(h);
+            if (0 == hit)
+                continue;
+            int idtruth = hit->_tid;
+            count[idtruth]++;
+            total++;
+        }
```
Why don't use functions from the standard library? std::count_if seems to be appropriate here.
### Answer
I tried an implementation, but in order to fill the map in the way needed for the next step you stil need a loop even if std::ccount_if is used. If I am missing something and you see a simpler way to implement this method then feel free to point out my mistake.

## In StRoot/StFwdTrackMaker/include/Tracker/FwdTracker.h:
```diff
> 
+        for (auto &iter : count) {
+            if (iter.second > cmx) {
+                cmx = iter.second;
+                idtruth = iter.first;
+            }
+        }
```
std::max_element?
### Answer
If this were a vector yes, but to use on a map I think I would need something like https://stackoverflow.com/a/9371137
and with the added complexity I dont see the advantage.