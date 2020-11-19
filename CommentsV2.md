In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:

```diff
> +//_______________________________________________________________________________________
+// For now, accept anything we are passed, no matter what it is or how bad it is
+template<typename T> bool accept( T ) { return true; }
+
+// Basic sanity cuts on genfit tracks
+template<> bool accept( genfit::Track *track )
+{
+    // This also gets rid of failed fits (but may need to explicitly
+    // for fit failure...)
+    if (track->getNumPoints() <= 0 ) return false; // fit may have failed
+
+    auto cardinal = track->getCardinalRep();
+
+    // Check that the track fit converged
+    auto status = track->getFitStatus( cardinal );
+    if ( 0 == status->isFitConverged() ) {
```
⬇️ Suggested change
```diff
-    if ( 0 == status->isFitConverged() ) {
+    if ( !status->isFitConverged() ) {
```

## Answer
taken


In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> +    // Fitted state at the first point
+    // const auto &atFirstPoint = track->getFittedState();
+
+    // Getting the fitted state from a track occasionally fails, because
+    // the first point on the fit doesn't have forward/backward fit
+    // information.  So we want the first point with fit info...
+
+    genfit::TrackPoint* first = 0;
+    unsigned int ipoint = 0;
+    for ( ipoint = 0; ipoint < track->getNumPoints(); ipoint++ ) {
+      first = track->getPointWithFitterInfo( ipoint );
+      if ( first ) break;
+    }
+
+    // No points on the track have fit information
+    if ( 0 == first ) {
```
⬇️ Suggested change
```diff
-    if ( 0 == first ) {
+    if ( !first ) {
```
This is not me being picky. Using appropriate operators is what makes c++ c++ :)

## Answer
taken, ;)


In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> +    }
+
+    // Following line fails with an exception, because some tracks lack 
+    //   forward update, or prediction in fitter info at the first point
+    //
+    // genfit::KalmanFitterInfo::getFittedState(bool) const of 
+    //                         GenFit/fitters/src/KalmanFitterInfo.cc:250
+
+    // Fitted state at the first point
+    // const auto &atFirstPoint = track->getFittedState();
+
+    // Getting the fitted state from a track occasionally fails, because
+    // the first point on the fit doesn't have forward/backward fit
+    // information.  So we want the first point with fit info...
+
+    genfit::TrackPoint* first = 0;
```

⬇️ Suggested change
```diff
-    genfit::TrackPoint* first = 0;
+    genfit::TrackPoint* first = nullptr;
```

## Answer
taken

In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> +  int id = -1;
+  int nmax = -1;
+  for (auto const& it : truth ) {
+    if ( it.second > nmax ) {
+      nmax = it.second;
+      id   = it.first;
+    }
+  }
```
⬇️ Suggested change
```diff
-  int id = -1;
-  int nmax = -1;
-  for (auto const& it : truth ) {
-    if ( it.second > nmax ) {
-      nmax = it.second;
-      id   = it.first;
-    }
-  }
+  using namespace std;
+  using P = decltype(truth)::value_type;
+  auto dom = max_element(begin(truth), end(truth), [](P a, P b){ return a.second < b.second; });
+  // use dom->first and dom->second
```

## Answer
Taken

In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> +    FwdHit* fhit = dynamic_cast<FwdHit*>(hit);
+    if ( 0 == fhit ) continue;
```
It is better to make sure the Seed_t seed container does not contain invalid pointers when filled rather than checking every time when it is used.
In general, avoiding if statements inside loops will increase the chances your code is vectorized by the compiler.

## Answer
Yes, point taken. Needed anyways for the goal to move towards full vectorization using e.g. Vc

In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
+        p.SetPerp(minR + (floor((r - minR) / mRasterR) * mRasterR + mRasterR / 2.0));
+        p.SetPhi(-TMath::Pi() + (floor((phi + TMath::Pi()) / mRasterPhi) * mRasterPhi + mRasterPhi / 2.0));
```
std::floor or using namespace std;

## Answer
```diff
-        p.SetPerp(minR + (floor((r - minR) / mRasterR) * mRasterR + mRasterR / 2.0));
-        p.SetPhi(-TMath::Pi() + (floor((phi + TMath::Pi()) / mRasterPhi) * mRasterPhi + mRasterPhi / 2.0));

+		p.SetPerp(minR + (std::floor((r - minR) / mRasterR) * mRasterR + mRasterR / 2.0));
+		p.SetPhi(-TMath::Pi() + (std::floor((phi + TMath::Pi()) / mRasterPhi) * mRasterPhi + mRasterPhi / 2.0));
```

In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:

```diff
> +    }
+
+    auto& fittedState= track->getFittedState(ipoint);
+
+    TVector3 momentum = fittedState.getMom();
+    double   pt       = momentum.Perp();
+
+    if (pt < 0.10 ) return false; // below this
+
+    return true;
+
+};
+
+//_______________________________________________________________________________________
+// // Truth handlers
+int TheTruth ( const Seed_t& seed, int &qa ) {
```

This function is very similar to dominantContribution(). Are you sure you want to keep both?

## Answer
Indeed, they were the same. I removed `TheTruth` from StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx and moved the implementation changes you suggested over to `MCTruthUtils::dominantContribution`

In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
>
+        float pt = sqrt(pt2);
+        float eta = track->eta;
+        float phi = atan2(track->p[1], track->p[0]); //track->phi;
```

It is recommended to use the C++ functions std::sqrt and std::atan2. Without std:: you may pick up the C functions from the global space which may have different implementation

## Answer
Used `std::` everywhere e.g.
```diff
-        float pt = sqrt(pt2);
-        float eta = track->eta;
-        float phi = atan2(track->p[1], track->p[0]); //track->phi;

+		float pt = std::sqrt(pt2);
+        float eta = track->eta;
+        float phi = std::atan2(track->p[1], track->p[0]); //track->phi;
```



In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:

```diff
>
+    auto &trackNodes         = stEvent->trackNodes();
+    auto &trackDetectorInfos = stEvent->trackDetectorInfo();
+
+    int track_count_total  = 0;
+    int track_count_accept = 0;
+
+    for ( auto *genfitTrack : genfitTracks ) {
+
+        // Get the track seed
+        const auto &seed = seed_tracks[track_count_total];
+
+        // Increment total track count
+        track_count_total++;
+
+        // Check to see if the track passes cuts (it should, for now)
+        if ( 0 == accept(genfitTrack) ) continue;
```

Your accept() function actually returns bool
⬇️ Suggested change
```diff
-        if ( 0 == accept(genfitTrack) ) continue;
+        if ( !accept(genfitTrack) ) continue;
```

## Answer
taken


In StRoot/StFwdTrackMaker/include/Tracker/FwdTracker.h:

```diff
> +    Seed_t::iterator findHitById(Seed_t &track, unsigned int _id) {
+        for (Seed_t::iterator it = track.begin(); it != track.end(); ++it) {
+            KiTrack::IHit *h = (*it);
+
+            if (static_cast<FwdHit *>(h)->_id == _id)
+                return it;
+        }
+
+        return track.end();
+    }
```
Remove this function. I see that you use std::find instead which is good

## Answer
removed:

```diff
-    Seed_t::iterator findHitById(Seed_t &track, unsigned int _id) {
-        for (Seed_t::iterator it = track.begin(); it != track.end(); ++it) {
-            KiTrack::IHit *h = (*it);
-
-            if (static_cast<FwdHit *>(h)->_id == _id)
-                return it;
-        }
-
-        return track.end();
-    }
```


In StRoot/StFwdTrackMaker/include/Tracker/FwdTracker.h:
```diff
> +
+    Seed_t::iterator findHitById(Seed_t &track, unsigned int _id) {
+        for (Seed_t::iterator it = track.begin(); it != track.end(); ++it) {
+            KiTrack::IHit *h = (*it);
+
+            if (static_cast<FwdHit *>(h)->_id == _id)
+                return it;
+        }
+
+        return track.end();
+    }
+
+    void removeHits(std::map<int, std::vector<KiTrack::IHit *>> &hitmap, std::vector<Seed_t> &tracks) {
+
+        for (auto track : tracks) {
+            if (track.size() > 0) {
```
⬇️ Suggested change
```diff
-            if (track.size() > 0) {
```
Remove this unnecessary check

## Answer
taken
```diff
-            if (track.size() > 0) {
```

In StRoot/StFwdTrackMaker/include/Tracker/FwdTracker.h:
```diff
> +
+                    // auto hitit = findHitById( hitmap[sector], static_cast<FwdHit*>(hit)->_id );
+                    auto hitit = std::find(hitmap[sector].begin(), hitmap[sector].end(), hit);
+
+                    if (hitit == hitmap[sector].end()) {
+
+                    } else {
+                        hitmap[sector].erase(hitit);
+                        mTotalHitsRemoved++;
+                    }
+
+                } // loop on hits in track
+            }     // if track has 7 hits
+        }         // loop on track
+
+        return;
```
⬇️ Suggested change
```diff
-        return;
```
Unnecessary return statement

## Answer
Taken


In StRoot/StFwdTrackMaker/include/Tracker/FwdTracker.h:
```diff
>
+                    if (hitit == hitmap[sector].end()) {
+
+                    } else {
```
⬇️ Suggested change
```diff
-                    if (hitit == hitmap[sector].end()) {
-
-                    } else {
+                    if (hitit != hitmap[sector].end()) {
```
Easier to read

## Answer 
taken

In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> +    std::map<int, std::vector<KiTrack::IHit *>> &load(unsigned long long) {
+        return _hits;
+    };
+    std::map<int, std::vector<KiTrack::IHit *>> &loadSi(unsigned long long) {
+        return _fsi_hits;
+    };
```

The functions arguments not used -> remove
The name load.. does not really reflect the purpose. These are just getters

## Answer
Yes, removed as part of the larger reconstruction of the HitLoader class into the FwdDataSource class

In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
+        FwdHit *hit = new FwdHit(count++, x, y, z, -plane_id, track_id, hitCov3, mcTrackMap[track_id]);
+
+        // Add the hit to the hit map
+        hitMap[hit->getSector()].push_back(hit);
+
+        // Add hit pointer to the track
+        if (mcTrackMap[track_id])
+            mcTrackMap[track_id]->addHit(hit);
+    }
+} // loadStgcHits
+
+void StFwdTrackMaker::loadStgcHitsFromStEvent( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count ){
+
+    // Get the StEvent handle
+    StEvent *event = (StEvent *)this->GetDataSet("StEvent");
+    if (0 == event) {
```
⬇️ Suggested change

```diff
-    if (0 == event) {
+    if (!event) {
```

## Answer
removed this bail-out clause since new functionality was added that can run even when `event` is `null`.
```diff

-   if (0 == event) {
-		return
-	}


+	StRnDHitCollection *rndCollection = nullptr;
+    if ( event) {
+        rndCollection = event->rndHitCollection();
+    }
```

In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> +        // Add hit pointer to the track
+        if (mcTrackMap[track_id])
+            mcTrackMap[track_id]->addHit(hit);
+    }
+} // loadStgcHits
+
+void StFwdTrackMaker::loadStgcHitsFromStEvent( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count ){
+
+    // Get the StEvent handle
+    StEvent *event = (StEvent *)this->GetDataSet("StEvent");
+    if (0 == event) {
+        return;
+    }
+
+    StRnDHitCollection *rndCollection = event->rndHitCollection();
+    if (0 == rndCollection) {
```
⬇️ Suggested change

```diff
-    if (0 == rndCollection) {
+    if (!rndCollection) {
```

## Answer
Taken, with new functionality added.
```diff
-    if (0 == rndCollection) {
+	 if ( !rndCollection || "GEANT" == fttFromGEANT ){
```


In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:

```diff
+        StMatrixF covmat = hit->covariantMatrix();
+
+        // copy covariance matrix element by element from StMatrixF
+        hitCov3(0,0) = covmat[0][0]; hitCov3(0,1) = covmat[0][1]; hitCov3(0,2) = covmat[0][2];
+        hitCov3(1,0) = covmat[1][0]; hitCov3(1,1) = covmat[1][1]; hitCov3(1,2) = covmat[1][2];
+        hitCov3(2,0) = covmat[2][0]; hitCov3(2,1) = covmat[2][1]; hitCov3(2,2) = covmat[2][2];
```

Something like this should work
⬇️ Suggested change

```diff
-        StMatrixF covmat = hit->covariantMatrix();
-
-        // copy covariance matrix element by element from StMatrixF
-        hitCov3(0,0) = covmat[0][0]; hitCov3(0,1) = covmat[0][1]; hitCov3(0,2) = covmat[0][2];
-        hitCov3(1,0) = covmat[1][0]; hitCov3(1,1) = covmat[1][1]; hitCov3(1,2) = covmat[1][2];
-        hitCov3(2,0) = covmat[2][0]; hitCov3(2,1) = covmat[2][1]; hitCov3(2,2) = covmat[2][2];
+std::copy(&covmat(0,0), &covmat(0,0) + 9, hitCov3.GetMatrixArray());
```

## Answer
Taken

In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:

```diff
+    vector<double> fttZ;
+    if ( gGeoManager ){
+        FwdGeomUtils fwdGeoUtils( gGeoManager );
+        fttZ = fwdGeoUtils.fttZ( {0.0, 0.0, 0.0, 0.0} );
+    } else {
+        fttZ = {0.0, 0.0, 0.0, 0.0};
+        LOG_WARN << "Could not load Ftt geometry, tracks will be invalid" << endm;
+    }
```

To make it prettier hide this check inside FwdGeoUtils::fttZ()
⬇️ Suggested change

```diff
-    vector<double> fttZ;
-    if ( gGeoManager ){
-        FwdGeomUtils fwdGeoUtils( gGeoManager );
-        fttZ = fwdGeoUtils.fttZ( {0.0, 0.0, 0.0, 0.0} );
-    } else {
-        fttZ = {0.0, 0.0, 0.0, 0.0};
-        LOG_WARN << "Could not load Ftt geometry, tracks will be invalid" << endm;
-    }
+    vector<double> fttZ = FwdGeomUtils( gGeoManager ).fttZ( {0.0, 0.0, 0.0, 0.0} );
```

## Answer 
good suggestion, taken but kept a block to check and warn if FTT geometry is not found, and elevated to `LOG_ERROR`

```diff
+	vector<double> fttZ = FwdGeomUtils( gGeoManager ).fttZ( {0.0, 0.0, 0.0, 0.0} );
+	if ( fttZ.size() < 4 || fttZ[0] < 200.0 ) { // check that valid z-locations were found
+        LOG_ERROR << "Could not load Ftt geometry, tracks will be invalid" << endm;
+   }
```


In StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:

```diff
>
+    McTrack() {
+        mPt = -999;
+        mEta = -999;
+        mPhi = -999;
+        mQ = 0;
+    }
+    McTrack(double pt, double eta = -999, double phi = -999, int q = 0,
+            int start_vertex = -1) {
+        mPt = pt;
+        mEta = eta;
+        mPhi = phi;
+        mQ = q;
+        mStartVertex = start_vertex;
+    }
```

Reduce the boilerplate code by using delegating constructors

## Answer
Taken, added `set` which is reused in both ctors:

```diff
-    McTrack() {
-        mPt = -999;
-        mEta = -999;
-        mPhi = -999;
-        mQ = 0;
-    }
-    McTrack(double pt, double eta = -999, double phi = -999, int q = 0,
-            int start_vertex = -1) {
-        mPt = pt;
-        mEta = eta;
-        mPhi = phi;
-        mQ = q;
-        mStartVertex = start_vertex;
-    }

+	McTrack() {
+        set( -999, -999, -999, 0, -1 );
+    }
+    McTrack(double pt, double eta = -999, double phi = -999, int q = 0,
+            int start_vertex = -1) {
+        set( pt, eta, phi, q, start_vertex );
+    }

+    void set(double pt, double eta = -999, double phi = -999, int q = 0, int start_vertex = -1){
+        mPt = pt;
+        mEta = eta;
+        mPhi = phi;
+        mQ = q;
+        mStartVertex = start_vertex;
+    }
```


In StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:
```diff
>
+        _covmat.ResizeTo( 3, 3 );
+        _covmat = covmat;
```
I believe there is a copy constructor for TMatrixDSym so you can use it in the initializer list _covmat(covmat)

## Answer
Based on the docs, it seems like that should work, but I get the following error. I tried adding `const` also without luck.
Maybe I am missing something simple, if so let me know.

Let as is for now.

```log
In file included from /tmp/star-sw/StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:2:0:
/tmp/star-sw/StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h: In constructor 'FwdHit::FwdHit(unsigned int, float, float, float, int, int, TMatrixDSym, std::shared_ptr<McTrack>)':
/tmp/star-sw/StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:82:23: error: no match for call to '(TMatrixDSym {aka TMatrixTSym<double>}) (TMatrixDSym&)'
         _covmat(covmat);
                       ^
In file included from /usr/local/include/TVectorT.h:27:0,
                 from /usr/local/include/TVectorD.h:21,
                 from /tmp/star-install/include/GenFit/AbsTrackRep.h:32,
                 from /tmp/star-install/include/GenFit/Track.h:27,
                 from /tmp/star-sw/StRoot/StFwdTrackMaker/StFwdTrackMaker.h:7,
                 from /tmp/star-sw/StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:1:
/usr/local/include/TMatrixTSym.h:38:31: note: candidates are:
 template<class Element> class TMatrixTSym : public TMatrixTBase<Element> {
                               ^
In file included from /usr/local/include/TVectorT.h:27:0,
                 from /usr/local/include/TVectorD.h:21,
                 from /tmp/star-install/include/GenFit/AbsTrackRep.h:32,
                 from /tmp/star-install/include/GenFit/Track.h:27,
                 from /tmp/star-sw/StRoot/StFwdTrackMaker/StFwdTrackMaker.h:7,
                 from /tmp/star-sw/StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:1:
/usr/local/include/TMatrixTSym.h:201:41: note: Element TMatrixTSym<Element>::operator()(Int_t, Int_t) const [with Element = double; Int_t = int]
 template <class Element> inline Element TMatrixTSym<Element>::operator()(Int_t rown,Int_t coln) const
                                         ^
/usr/local/include/TMatrixTSym.h:201:41: note:   candidate expects 2 arguments, 1 provided
/usr/local/include/TMatrixTSym.h:217:42: note: Element& TMatrixTSym<Element>::operator()(Int_t, Int_t) [with Element = double; Int_t = int]
 template <class Element> inline Element &TMatrixTSym<Element>::operator()(Int_t rown,Int_t coln)
                                          ^
/usr/local/include/TMatrixTSym.h:217:42: note:   candidate expects 2 arguments, 1 provided
make[3]: *** [CMakeFiles/StFwdTrackMaker.dir/StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx.o] Error 1
make[2]: *** [CMakeFiles/StFwdTrackMaker.dir/all] Error 2
make[1]: *** [CMakeFiles/StFwdTrackMaker.dir/rule] Error 2
make: *** [StFwdTrackMaker] Error 2
```



In StRoot/StFwdTrackMaker/include/Tracker/STARField.h:

```diff
>
+#include "TFile.h"
+#include "TH3.h"
```
⬇️ Suggested change

```diff
-#include "TFile.h"
-#include "TH3.h"
```

Seem to be unused in this file

## Answer 
Taken


In CommentsV1.md:

```diff
+## In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
+```diff
+>
++    std::string loggerFile = SAttr("logfile"); // user can changed before Init
++    loguru::add_file( loggerFile.c_str(), loguru::Truncate, loguru::Verbosity_2);
++    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
++
+```
+Unfortunately, I don't think we should introduce another logging library into the STAR library collection. At least I don't think it can be done now as a dependency for this library. The benefit of using loguru is not clear but any additional resource will require someone to maintain it even if it does not look like it needs maintenance now.
+### Answer
+loguru has been removed see commit: https://github.com/jdbrice/star-fwd-integration/commit/c81c63d7298622f3f25b1abd86f6135fe37cd54a
```

Thanks! Can you then remove the actual header file StFwdTrackMaker/include/Tracker/loguru.h?

## Answer
Removed


In CommentsV1.md:
```diff
+## In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
+```diff
+>
++            FwdHit *hit = new FwdHit(count++, x, y, z, -plane_id, track_id, hitCov3, mcTrackMap[track_id]);
++
++            // Add the hit to the hit map
++            hitMap[hit->getSector()].push_back(hit);
+```
+I don't see where you deallocate the requested memory for the hits. If you don't delete the hits the memory will leak.
+### Answer
+Hits are now deleted at the end of the Make() function, see commit : https://github.com/jdbrice/star-fwd-integration/commit/17d5ae01418676b3d99f4d20ca4eccdce520ca9d
```

I already commented on this one in the code... But it does seem more logical to delete the hits in the HitLoader

## Answer

I went ahead and removed the IHitLoader interface and replaced it with a standalone class "FwdDataSource". Now it is setup to more clearly be just a container class. It does handle the deleting of all pointers in an atomic way.


In CommentsV1.md:
```diff
+## In StRoot/StFwdTrackMaker/include/Tracker/FwdHit.h:
+```diff
+> +    inline bool operator()(Seed_t trackA, Seed_t trackB) {
++        std::map<unsigned int, unsigned int> hit_counts;
++        // we are assuming that the same hit can never be used twice on a single
++        // track!
++
++        for (auto h : trackA) {
++            hit_counts[static_cast<FwdHit *>(h)->_id]++;
++        }
++
++        // now look at the other track and see if it has the same hits in it
++        for (auto h : trackB) {
++            hit_counts[static_cast<FwdHit *>(h)->_id]++;
++
++            // incompatible if they share a single hit
++            if (hit_counts[static_cast<FwdHit *>(h)->_id] >= 2) {
++                return false;
++            }
++        }
++
++        // no hits are shared, they are compatible
++        return true;
++    }
+```
+Use std::equal()
+https://en.cppreference.com/w/cpp/algorithm/equal
+
+### Answer
+I dont think the `std::equal()` implementation of this is more compact. Maybe I miss something, but I dont want to know if the ranges are equal, but share one or more elements. If I use `std::equal()` wouldnt I still need loops? Let as is for now, but open to improvements.
```


Yes, std::equal() perhaps is not the right choice here. But I think you can do this without creating a temporary map. The following should be equivalent to what you have
```cpp
        for (auto ha : trackA) {
            for (auto hb : trackB) {
                if (static_cast<FwdHit *>(hb)->_id == static_cast<FwdHit *>(ha)->_id)
                    return false;
            }
        }
```


In CommentsV1.md:

```diff
+## In StRoot/StFwdTrackMaker/include/Tracker/FwdTracker.h:
+```diff
+>
++        for (auto *h : hits) {
++            auto *hit = dynamic_cast<FwdHit *>(h);
++            if (0 == hit)
++                continue;
++            int idtruth = hit->_tid;
++            count[idtruth]++;
++            total++;
++        }
+
+Why don't use functions from the standard library? std::count_if seems to be appropriate here.
+### Answer
+I tried an implementation, but in order to fill the map in the way needed for the next step you stil need a loop even if std::ccount_if is used. If I am missing something and you see a simpler way to implement this method then feel free to point out my mistake.
```

You are correct count_if would not be very advantageous here. What you have is ok. You may want to remove the check for null pointers as I believe you have the control of what goes into the hit container. In this case you can also just use hits.size() in place of total. Also, my feeling is that you can use unordered_map instead of std::map<int, int> count;

## Answer
Removed the null check.
Removed local variable `total` and instead use `hits.size()`
Also modified to use the unordered_map container.

Several changes all contributed to this function, it now reads:
```cpp
static int dominantContribution(std::vector<KiTrack::IHit *> hits, double &qa) {
        
	// track_id, hits on track
	std::unordered_map<int,int> truth;
	for ( auto hit : hits ) {
		FwdHit* fhit = dynamic_cast<FwdHit*>(hit);
		truth[ fhit->_tid ]++;
	}

	using namespace std;
	using P = decltype(truth)::value_type;
	auto dom = max_element(begin(truth), end(truth), [](P a, P b){ return a.second < b.second; });

	// QA represents the percentage of hits which
	// vote the same way on the track
	if ( hits.size() > 0 )
		qa = double(dom->second) / double(hits.size()) ;
	else 
		qa = 0;

	return dom->first;
};
```



In CommentsV1.md:

```diff
+## In StRoot/StFwdTrackMaker/include/Tracker/FwdTracker.h:
+```diff
+>
++        for (auto &iter : count) {
++            if (iter.second > cmx) {
++                cmx = iter.second;
++                idtruth = iter.first;
++            }
++        }
+```
+std::max_element?
+### Answer
+If this were a vector yes, but to use on a map I think I would need something like https://stackoverflow.com/a/9371137
+and with the added complexity I dont see the advantage.
```


I agree, in this case it is more of a question of style

```cpp
  using P = decltype(truth)::value_type;
  auto d = max_element(begin(truth), end(truth), [](P a, P b){ return a.second < b.second; });
```

## Answer
Taken, I didnt realize that I could do this:
```cpp
using P = decltype(truth)::value_type;
```
that makes it much more readable.


In StRoot/StFwdTrackMaker/include/Tracker/CriteriaKeeper.h:

```diff
>
+            same_track = (static_cast<FwdHit *>(a)->_tid == static_cast<FwdHit *>(b)->_tid && static_cast<FwdHit *>(a)->_tid != 0);
+            if (same_track)
+                track_id = static_cast<FwdHit *>(a)->_tid;
+        }
+
+        // three hit criteria (two two-segments)
+        if ((parent->getHits().size() == 2) && (child->getHits().size() == 2)) {
+            KiTrack::IHit *a = child->getHits()[0];
+            KiTrack::IHit *b = child->getHits()[1];
+            KiTrack::IHit *c = parent->getHits()[1];
+
+            same_track = (static_cast<FwdHit *>(a)->_tid == static_cast<FwdHit *>(b)->_tid && static_cast<FwdHit *>(b)->_tid == static_cast<FwdHit *>(c)->_tid && static_cast<FwdHit *>(a)->_tid != 0);
+            if (same_track)
+                track_id = static_cast<FwdHit *>(a)->_tid;
+        }
```

The fact that you have so many casts in the code usually indicative of a less than optimal design. The problem may not be with your code but with the library you are using. In fact, I just looked up how you use the Seed_t container in KiTracks. It does have an awful interface KiTracks::SegementBuilder::SegmentBuilder( std::map< int , std::vector< IHit* > > map_sector_hits );

One option is just to modify KiTracks, e.g. add a template. Another workaround would be to use in your code std::vector\<FwdHit\> but create a vector of pointers before passing to KiTracks.

As I think it is beyond this review I'll leave it up to you.

## Answer
Yes, you will see in some of my other answers that the interface that KiTrack provides is very limiting / discourages better solutions. I think I'd like to modify KiTrack to use smart pointers (e.g. std::shared_ptr) and that way a lot of the lifetime and memory management issues can be solved with simpler classes/code.  I'd like to go ahead and finish the review though (for this we need to finalize the plan on Genfit and Kitrack anyways) and then work on this as I continue development on the package.

In StRoot/StFwdTrackMaker/FwdTrackerConfig.cxx:
```diff
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
+    if ( !exists( path ) )
+        return dv;
+    FwdTrackerConfig::canonize( path );
+    // directly return string
+    return ( this->mNodes.at( path ) );
```
Most of the time using this-> to access class members is a matter of style. Not sure if it is specified in the guidelines but most of the STAR code don't use this->. My recommendation is to not use it.

## Answer
Taken, `this->` removed

In StRoot/StFwdTrackMaker/FwdTrackerConfig.h:

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
If you don't like the solution with lambda here is one with an explicit cast:
⬇️ Suggested change

```diff
-        path.erase(std::remove_if(path.begin(), path.end(), ::isspace), path.end());
+        path.erase(std::remove_if(path.begin(), path.end(), static_cast<int(*) (int)>(std::isspace)), path.end());
```
It may sound nitpicking but the general recommendation is to use functions from the std:: namespace and implementation from the libstdc++ in c++

## Answer
Taken, i.e. using the static_cast as suggested. Used in a few places wherever ::isspace was used previously


In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
>
+        p.SetPerp(minR + (floor((r - minR) / mRasterR) * mRasterR + mRasterR / 2.0));
+        p.SetPhi(-TMath::Pi() + (floor((phi + TMath::Pi()) / mRasterPhi) * mRasterPhi + mRasterPhi / 2.0));
```

Should qualify floor as std::floor or using namespace std

## Answer 
Taken
```diff
-        p.SetPerp(minR + (floor((r - minR) / mRasterR) * mRasterR + mRasterR / 2.0));
-        p.SetPhi(-TMath::Pi() + (floor((phi + TMath::Pi()) / mRasterPhi) * mRasterPhi + mRasterPhi / 2.0));

+ 	p.SetPerp(minR + (std::floor((r - minR) / mRasterR) * mRasterR + mRasterR / 2.0));
+ 	p.SetPhi(-TMath::Pi() + (std::floor((phi + TMath::Pi()) / mRasterPhi) * mRasterPhi + mRasterPhi / 2.0));
```


In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
>      mForwardTracker->finish();
-
-    gDirectory->mkdir("StFwdTrackMaker");
-    gDirectory->cd("StFwdTrackMaker");
-    for (auto nh : histograms) {
-        nh.second->SetDirectory(gDirectory);
-        nh.second->Write();
+
+    if ( mGenHistograms ) {
+        gDirectory->mkdir("StFwdTrackMaker");
+        gDirectory->cd("StFwdTrackMaker");
```

When dealing with ROOT global objects you probably want to make sure you are writing to your file. It may happen that gDirectory points to a file opened by another Maker

## Answer
Taken, now the StFwdTrackMaker creates an output file if needed (`mGenHistograms == true`) and the other internal classes write to the gDirectory, only if `mGenHistograms == true`. This way I dont need to pass a TFile* through all the child objects, but since they all use the `mGenHistogram` flag, they will only ever write to the private file.


In StRoot/StFwdTrackMaker/StFwdTrackMaker.h:
```diff
> +    TFile *mTreeFile = 0;
+    TTree *mTree = 0;
```
⬇️ Suggested change
```diff
-    TFile *mTreeFile = 0;
-    TTree *mTree = 0;
+    TFile *mTreeFile = nullptr;
+    TTree *mTree = nullptr;
```

## Answer
Taken


In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> @@ -465,103 +473,95 @@ void StFwdTrackMaker::loadStgcHitsFromGEANT( std::map<int, shared_ptr<McTrack>>
 
     int nstg = 0;
     if (g2t_stg_hits == nullptr) {
```
⬇️ Suggested change
```diff
-    if (g2t_stg_hits == nullptr) {
+    if (!g2t_stg_hits) {
```
Don't you want to return from this function if the input is not available?

## Answer
Yes that makes sense, taken. Also in the equivalent place in `loadFstHitsFromGEANT`.



In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> +    // delete the hits from the hitmap
+    for ( auto kv : hitMap ){
+        for ( auto h : kv.second ){
+            delete h;
+        }
+        kv.second.clear();
+     }
 
+    for ( auto kv : fsiHitMap ){
+        for ( auto h : kv.second ){
+            delete h;
+        }
+        kv.second.clear();
+    }
```
It is a strange place to delete hits. This should be done in the HitLoader destructor

## Answer
The deleting of the hits has been moved to the FwdDataSource::clear() (formerly HitLoader class) to make it a more atomic operation.

```diff
-    // delete the hits from the hitmap
-    for ( auto kv : hitMap ){
-        for ( auto h : kv.second ){
-            delete h;
-        }
-        kv.second.clear();
-     }
 
-    for ( auto kv : fsiHitMap ){
-        for ( auto h : kv.second ){
-            delete h;
-        }
-        kv.second.clear();
-    }

+	// delete and clear hits + tracks for this event
+   mForwardData->clear();
```

I also call clear in the destructor to ensure memory is freed before exit. 
The problem here is that the KiTrack code expects raw pointers. For this reason I cannot use shared_ptr. 

Related to the above comments about KiTracks terrible interface. I would prefer to modify KiTrack to use shared_ptr everywhere. Then I would do as you suggest and use the FwdDataSource lifetime to naturally clean up (i.e. clean up in the destructor, and let the FwdDataSource instance be a local scope variable instance so it gets destroyed when the Make function goes out of scope)


In StRoot/StFwdTrackMaker/StFwdTrackMaker.cxx:
```diff
> +    TMatrixDSym tamvoc(3);
+    tamvoc( 0, 0 ) = cm(0, 0); tamvoc( 0, 1 ) = cm(0, 1); tamvoc( 0, 2 ) = 0.0;
+    tamvoc( 1, 0 ) = cm(1, 0); tamvoc( 1, 1 ) = cm(1, 1); tamvoc( 1, 2 ) = 0.0;
+    tamvoc( 2, 0 ) = 0.0;      tamvoc( 2, 1 ) = 0.0; tamvoc( 2, 2 )      = 0.01*0.01;
+
+
+    return tamvoc;
+}
+
+void StFwdTrackMaker::loadStgcHits( std::map<int, shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count ){
+    LOG_SCOPE_FUNCTION( INFO );
+
+    // Get the StEvent handle to see if the rndCollection is available
+    StEvent *event = (StEvent *)this->GetDataSet("StEvent");
+    StRnDHitCollection *rndCollection = nullptr;
+    if (nullptr != event) {
```
⬇️ Suggested change
```diff
-    if (nullptr != event) {
+    if (event) {
```

## Answer
Taken