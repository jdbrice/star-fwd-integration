
## In StRoot/StFstSimMaker/StFstFastSimMaker.h:
```diff
>
+class StRnDHitCollection;
+class StRnDHit;
+
+#include "StChain/StMaker.h"
+#include <vector>
+
+#include "TH1F.h"
+#include "TH2F.h"
+#include "TH3F.h"
+
+
+class StFstFastSimMaker : public StMaker {
+	public:
+		explicit StFstFastSimMaker(const Char_t *name = "fstFastSim");
+		virtual ~StFstFastSimMaker() {}
+		Int_t Make();
```
Use fundamental standard and ROOT types such as Int_t consistently. The general rule is to use the ROOT types for persistent data.

## Answer
```diff
-		Int_t Make();
+		int Make();
```

## In StRoot/StFstSimMaker/StFstFastSimMaker.h:
```diff
>
+	public:
+		explicit StFstFastSimMaker(const Char_t *name = "fstFastSim");
+		virtual ~StFstFastSimMaker() {}
+		Int_t Make();
+		int Init();
+		int Finish();
+		virtual const char *GetCVS() const;
+
+		/// Set offset for each disk ( x=R*cos(idisk*60 degrees), y=R*sin(...) )
+		void setRaster(float R = 1.0) { mRaster = R; }
+
+		/// Set min/max active radii for each disk
+		void setDisk(const int i, const float rmn, const float rmx);
+		void setInEfficiency(float ineff = 0.1) { mInEff = ineff; }
+		void setQAFileName(TString filename = 0.1) { mQAFileName = filename; }
+		void setFillHist(const bool hist = false) { mHist = hist; }
```
If I remember correctly, class methods should start with a capital letter
## Answer
```diff

-		void setRaster(float R = 1.0) { mRaster = R; }

-		void setDisk(const int i, const float rmn, const float rmx);
-		void setInEfficiency(float ineff = 0.1) { mInEff = ineff; }
-		void setQAFileName(TString filename = 0.1) { mQAFileName = filename; }
-		void setFillHist(const bool hist = false) { mHist = hist; }

-		void fillSilicon(StEvent *event);

+ 		void SetRaster(float R = 1.0) { mRaster = R; }

+		void SetDisk(const int i, const float rmn, const float rmx);
+		void SetInEfficiency(float ineff = 0.1) { mInEff = ineff; }
+		void SetQAFileName(TString filename = 0.1) { mQAFileName = filename; }
+		void SetFillHist(const bool hist = false) { mHist = hist; }

+		void FillSilicon(StEvent *event);

```




## In StRoot/StFstSimMaker/StFstFastSimMaker.cxx:
```diff
> @@ -0,0 +1,442 @@
+#include "StFstFastSimMaker.h"
+
+#include "St_base/StMessMgr.h"
+
+#include "StEvent/StEvent.h"
+#include "StEvent/StRnDHit.h"
+#include "StEvent/StRnDHitCollection.h"
+
+#include "tables/St_g2t_fts_hit_Table.h"
+#include "tables/St_g2t_track_Table.h"
+
+#include "StThreeVectorF.hh"
```
It is a good idea to prefix the header files with its package name so it is easier to identify where it is coming from.

##Answer
```diff
-#include "StThreeVectorF.hh"
+#include "StarClassLibrary/StThreeVectorF.hh"
```


## In StRoot/StFstSimMaker/StFstFastSimMaker.cxx:
```diff
>
+	StRnDHit *_map[NDISC][MAXR][MAXPHI];
+	double ***enrsum = (double ***)malloc(NDISC * sizeof(double **));
+	double ***enrmax = (double ***)malloc(NDISC * sizeof(double **));
+
+	for (int id = 0; id < NDISC; id++) {
+		enrsum[id] = (double **)malloc(MAXR * sizeof(double *));
+		enrmax[id] = (double **)malloc(MAXR * sizeof(double *));
+		for (int ir = 0; ir < MAXR; ir++) {
+			enrsum[id][ir] = (double *)malloc(MAXPHI * sizeof(double));
+			enrmax[id][ir] = (double *)malloc(MAXPHI * sizeof(double));
+		}
+	}
+
+	for (int id = 0; id < NDISC; id++) {
+		for (int ir = 0; ir < MAXR; ir++) {
+			for (int ip = 0; ip < MAXPHI; ip++) {
+				_map[id][ir][ip] = 0;
+				enrsum[id][ir][ip] = 0;
+				enrmax[id][ir][ip] = 0;
+			}
+		}
+	}
```
What is the point of allocating all of these arrays dynamically? You do know all the dimensions beforehand. This hard-to-read code can be rewritten with a one dimensional std::array indexed by idx = i_max*(j_max*k + j) + i

## Answer
I threw out the dynamic arrays and instead use `std::map`

```diff
-	StRnDHit *_map[NDISC][MAXR][MAXPHI];
-	double ***enrsum = (double ***)malloc(NDISC * sizeof(double **));
-	double ***enrmax = (double ***)malloc(NDISC * sizeof(double **));
-
-	for (int id = 0; id < NDISC; id++) {
-		enrsum[id] = (double **)malloc(MAXR * sizeof(double *));
-		enrmax[id] = (double **)malloc(MAXR * sizeof(double *));
-		for (int ir = 0; ir < MAXR; ir++) {
-			enrsum[id][ir] = (double *)malloc(MAXPHI * sizeof(double));
-			enrmax[id][ir] = (double *)malloc(MAXPHI * sizeof(double));
-		}
-	}
-
-	for (int id = 0; id < NDISC; id++) {
-		for (int ir = 0; ir < MAXR; ir++) {
-			for (int ip = 0; ip < MAXPHI; ip++) {
-				_map[id][ir][ip] = 0;
-				enrsum[id][ir][ip] = 0;
-				enrmax[id][ir][ip] = 0;
-			}
-		}
-	}


+ 	typedef std::tuple<int, int, int> FstKeyTriple;

+	std::map< FstGlobal::FstKeyTriple, StRnDHit* > hitMap;
+	std::map< FstGlobal::FstKeyTriple, double > energySum;
+	std::map< FstGlobal::FstKeyTriple, double > energyMax;


```
And updates in the corresponding implementations.



## In StRoot/StFstSimMaker/StFstFastSimMaker.cxx:
```diff
>
+	const g2t_fts_hit_st *hit = hitTable->GetTable();
+
+	StPtrVecRnDHit hits;
+
+	// track table
+	St_g2t_track *trkTable = static_cast<St_g2t_track *>(GetDataSet("g2t_track"));
+	if (!trkTable) {
+		LOG_INFO << "g2t_track table is empty" << endm;
+		return; // Nothing to do
+	}           // if
+
+	const Int_t nTrks = trkTable->GetNRows();
+	LOG_DEBUG << "g2t_track table has " << nTrks << " tracks" << endm;
+	const g2t_track_st *trk = trkTable->GetTable();
+
+	gRandom->SetSeed(0);
```
gRandom is a global variable which means you reset the seed for the entire framework. This should not be done by your library

## Answer
```diff
-	gRandom->SetSeed(0);

```

Also use of `gRandom` is replaced with `StarRandom`



## In StRoot/StFstSimMaker/StFstFastSimMaker.cxx:
```diff
>
+			float y = hit->x[1];
+			float z = hit->x[2];
+
+			if (z > 200)
+				continue; // skip large disks
+
+			// rastered
+			float xx = x - xc;
+			float yy = y - yc;
+
+			float r = sqrt(x * x + y * y);
+			float p = atan2(y, x);
+
+			// rastered
+			float rr = sqrt(xx * xx + yy * yy);
+			float pp = atan2(yy, xx);
```
What's the point of doing the majority of calculations in this routine in single precision?

## Answer
`double` precision is used consistently throughout now


## In StRoot/StFstSimMaker/StFstFastSimMaker.cxx:
```diff
>
+			if (MAXR)
+				assert(ir < MAXR);
+			if (MAXPHI)
+				assert(ip < MAXPHI);
+
+			StRnDHit *fsihit = 0;
+			if (_map[d - 1][ir][ip] == 0) // New hit
+			{
+
+				if (FstGlobal::verbose)
+					LOG_INFO << Form("NEW d=%1d xyz=%8.4f %8.4f %8.4f r=%8.4f phi=%8.4f iR=%2d iPhi=%4d dE=%8.4f[MeV] truth=%d",
+							d, x, y, z, r, p, ir, ip, e * 1000.0, t)
+						<< endm;
+
+					count++;
+				fsihit = new StRnDHit();
```
A possible memory leak

## Answer
I dont think so, since these are the hit pointers that are saved into the StEvent collection. At least, if I delete them at the end of `FillSilicon()`  then I get a seg fault later since the hits in StEvent are null. But I am not sure how StEvent handles saving (and freeing) pointers? So maybe they need to be released somewhere else?



## In StRoot/StFttSimMaker/StFttFastSimMaker.cxx:
```diff
>
+    if (!hitTable) {
+        LOG_INFO << "g2t_stg_hit table is empty" << endm;
+        return;
+    } // if !hitTable
+
+    StRnDHitCollection *ftscollection = event->rndHitCollection();
+
+    std::vector<StRnDHit *> hits;
+
+    // Prepare to loop over all hits
+    const int nhits = hitTable->GetNRows();
+    const g2t_fts_hit_st *hit = hitTable->GetTable();
+
+    // TODO resolve StarRandom, not built in DOCKER env
+    // StarRandom &rand = StarRandom::Instance();
+    TRandom3 *rand = new TRandom3();
```
Your function has a memory leak
## Answer:
Using the StarRandom instance now
```diff
-    // TODO resolve StarRandom, not built in DOCKER env
-    // StarRandom &rand = StarRandom::Instance();
-    TRandom3 *rand = new TRandom3();

+	StarRandom &rand = StarRandom::Instance();

```
And updated also where `rand` is used


## In StRoot/StFttSimMaker/StFttFastSimMaker.cxx:
```diff
>
+        ahit->setDouble2(x_rot);
+        ahit->setDouble3(y_rot);
+
+        ahit->setLayer(disk); // disk mapped to layer
+        ahit->setLadder(2);   // indicates a point
+        ahit->setWafer(quad); // quadrant number
+
+        ahit->setIdTruth(hit->track_p, 0);
+        ahit->setDetectorId(kFtsId); // TODO: use dedicated ID for Ftt when StEvent is updated
+
+        float Ematrix[] = {
+            dx * dx, 0.f, 0.f,
+            0.f, dy * dy, 0.f,
+            0.f, 0, 0.f, dz * dz};
+        ahit->setErrorMatrix(Ematrix);
+        hits.push_back(ahit);
```
Another memory leak

## Answer

Per answer above, I am assuming that I dont need to free pointers saved into StEvent collections. Besides that, this case is actually a little more difficult, since the hit pointer is used in one case but not another (depending on the `STGC_MAKE_GHOST_HITS` flag).

For this reason I delete the hits only at the bottom of the `STGC_MAKE_GHOST_HITS == true` case.