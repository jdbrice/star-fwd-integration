#ifndef ST_FST_FAST_SIM_MAKER_H
#define ST_FST_FAST_SIM_MAKER_H

class StFtsHit;
class StEvent;
class StRnDHitCollection;
class StRnDHit;

#include "StChain/StMaker.h"
#include <vector>

#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"


class StFstFastSimMaker : public StMaker {
	public:
		explicit StFstFastSimMaker(const Char_t *name = "fstFastSim");
		virtual ~StFstFastSimMaker() {}
		Int_t Make();
		int Init();
		int Finish();
		virtual const char *GetCVS() const;
		
		/// Set offset for each disk ( x=R*cos(idisk*60 degrees), y=R*sin(...) )
		void setRaster(float R = 1.0) { mRaster = R; }

		/// Set min/max active radii for each disk
		void setDisk(const int i, const float rmn, const float rmx);
		void setInEfficiency(float ineff = 0.1) { mInEff = ineff; }
		void setQAFileName(TString filename = 0.1) { mQAFileName = filename; }
		void setFillHist(const bool hist = false) { mHist = hist; }
		

	private:
		void fillSilicon(StEvent *event);
		StRnDHitCollection *hitCollection = nullptr;

		int mNumR;
		int mNumPHI;
		int mNumSEC;
		float mRaster;
		float mInEff;
		bool mHist;
		TString mQAFileName;

		TH3F *hTrutHitYXDisk;
		TH2F *hTrutHitRDisk;
		TH2F *hTrutHitRShower[3];
		TH2F *hTrutHitPhiDisk;
		TH2F *hTrutHitPhiZ;
		TH3F *hRecoHitYXDisk;
		TH2F *hRecoHitRDisk;
		TH2F *hRecoHitPhiDisk;
		TH2F *hRecoHitPhiZ;
		TH2F *hGlobalDRDisk;
		TH1F *hGlobalZ;

		TH2F *h2GlobalXY;
		TH2F *h2GlobalSmearedXY;
		TH2F *h2GlobalDeltaXY;
		TH3F *h3GlobalDeltaXYDisk;
		TH3F *h3GlobalDeltaXYR;
		TFile *fOut;

		ClassDef(StFstFastSimMaker, 0)
};

inline const char *StFstFastSimMaker::GetCVS() const {
	static const char cvs[] = "Tag $Name:  $ $Id: StFstFastSimMaker.h,v 1.1 2018/11/06 18:56:05 jdb Exp $ built " __DATE__ " " __TIME__;
	return cvs;
}

#endif
