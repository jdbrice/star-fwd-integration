#include "StFstFastSimMaker.h"

#include "St_base/StMessMgr.h"

#include "StEvent/StEvent.h"
#include "StEvent/StRnDHit.h"
#include "StEvent/StRnDHitCollection.h"

#include "tables/St_g2t_fts_hit_Table.h"
#include "tables/St_g2t_track_Table.h"

#include "StThreeVectorF.hh"

#include "TCanvas.h"
#include "TCernLib.h"
#include "TH2F.h"
#include "TLine.h"
#include "TRandom3.h"
#include "TString.h"
#include "TVector2.h"
#include "TVector3.h"

#include <array>


// lets not polute the global scope
namespace FstGlobal{
    // TCanvas *canvas = 0;
    StMatrixF Hack1to6(const StHit *stHit);

    constexpr float PI = atan2(0.0, -1.0);
    constexpr float SQRT12 = sqrt(12.0);

    //
    // Disk segmentation
    //
    float RMIN[] = {0.95 * 4.3, 0.95 * 4.3, 0.95 * 4.3, 0.95 * 5.0, 0.95 * 5.0, 0.95 * 5.0};
    float RMAX[] = {1.05 * 15.0, 1.05 * 25.0, 1.05 * 25.0, 1.05 * 28.0, 1.05 * 28.0, 1.05 * 28.0};

    //NEXT IS only for disk ARRAY 456 with the radius from 5 to 28.
    float RSegment[] = {5., 7.875, 10.75, 13.625, 16.5, 19.375, 22.25, 25.125, 28.};



    const bool verbose = true;
}

StFstFastSimMaker::StFstFastSimMaker(const Char_t *name)
	: StMaker{name},
    mEnable{true, true, true, true, true, true, true, true, true, true, true, true},
    mNumR{64},
    mNumPHI{128},
    mNumSEC{12},
    mRaster{0},
    mInEff{0},
    mHist{false},
    mQAFileName(0),
    hTrutHitYXDisk(0),
    hTrutHitRDisk(0),
    hTrutHitRShower{0},
    hTrutHitPhiDisk(0),
    hTrutHitPhiZ(0),
    hRecoHitYXDisk(0),
    hRecoHitRDisk(0),
    hRecoHitPhiDisk(0),
    hRecoHitPhiZ(0),
    hGlobalDRDisk(0),
    hGlobalZ(0),
    h2GlobalXY(0),
    h2GlobalSmearedXY(0),
    h2GlobalDeltaXY(0),
    h3GlobalDeltaXYDisk(0),
    h3GlobalDeltaXYR(0) { }

int StFstFastSimMaker::Init() {

	if(mHist){
		fOut = new TFile(mQAFileName.Data(), "RECREATE");
		AddHist(hTrutHitYXDisk = new TH3F("hTrutHitYXDisk", "Global hits before segmentation", 151, -75.5, 75.5, 151, -75.5, 75.5, 10, 0, 10));
		AddHist(hTrutHitRDisk = new TH2F("hTrutHitRDisk", "Global hits before segmentation", 400, 0, 40, 10, 0, 10));
		AddHist(hTrutHitRShower[0] = new TH2F("hTrutHitRShower_4", "Global hits before segmentation", 400, 0, 40, 20, -10, 10));
		AddHist(hTrutHitRShower[1] = new TH2F("hTrutHitRShower_5", "Global hits before segmentation", 400, 0, 40, 20, -10, 10));
		AddHist(hTrutHitRShower[2] = new TH2F("hTrutHitRShower_6", "Global hits before segmentation", 400, 0, 40, 20, -10, 10));
		AddHist(hTrutHitPhiDisk = new TH2F("hTrutHitPhiDisk", "Global hits before segmentation", 360, 0, 360, 10, 0, 10));
		AddHist(hTrutHitPhiZ = new TH2F("hTrutHitPhiZ", "Global hits before segmentation", 360, 0, 360, 6000, 0, 600));
		AddHist(hRecoHitYXDisk = new TH3F("hRecoHitYXDisk", "Global hits after segmentation", 151, -75.5, 75.5, 151, -75.5, 75.5, 10, 0, 10));
		AddHist(hRecoHitRDisk = new TH2F("hRecoHitRDisk", "Global hits after segmentation", 400, 0, 40, 10, 0, 10));
		AddHist(hRecoHitPhiDisk = new TH2F("hRecoHitPhiDisk", "Global hits after segmentation", 360, 0, 360, 10, 0, 10));
		AddHist(hRecoHitPhiZ = new TH2F("hRecoHitPhiZ", "Global hits after segmentation", 360, 0, 360, 6000, 0, 600));
		AddHist(hGlobalDRDisk = new TH2F("hGlobalDRDisk", "; Reco. r - MC r [cm]; Events;", 1000, -50, 50, 10, 0, 10));
		AddHist(hGlobalZ = new TH1F("hGlobalZ", "; Z [cm]; Events;", 6000, 0, 600));
		AddHist(h3GlobalDeltaXYR = new TH3F("h3GlobalDeltaXYR", ";globalDeltaX; globalDeltaY; R", 300, -0.3, 0.3, 300, -3, 3, 100, 0, 30));
		AddHist(h2GlobalXY = new TH2F("h2GlobalXY", ";globalX; globalY", 1510, -75.5, 75.5, 1510, -75.5, 75.5));
		AddHist(h2GlobalSmearedXY = new TH2F("h2GlobalSmearedXY", ";globalSmearedX; globalSmearedY", 1510, -75.5, 75.5, 1510, -75.5, 75.5));
		AddHist(h2GlobalDeltaXY = new TH2F("h2GlobalDeltaXY", ";globalDeltaX; globalDeltaY", 151, -75.5, 75.5, 151, -75.5, 75.5));
		AddHist(h3GlobalDeltaXYDisk = new TH3F("h3GlobalDeltaXYDisk", ";globalDeltaX; globalDeltaY; Disk", 151, -75.5, 75.5, 151, -75.5, 75.5, 10, 0, 10));
	}
	return StMaker::Init();
}

void StFstFastSimMaker::setDisk(const int i, const float rmn, const float rmx) {
    FstGlobal::RMIN[i] = rmn;
    FstGlobal::RMAX[i] = rmx;
}

Int_t StFstFastSimMaker::Make() {
	LOG_DEBUG << "StFstFastSimMaker::Make" << endm;
	// Get the existing StEvent, or add one if it doesn't exist.
	StEvent *event = static_cast<StEvent *>(GetDataSet("StEvent"));
	if (!event) {
		event = new StEvent;
		AddData(event);
		LOG_DEBUG << "Creating StEvent" << endm;
	}

	if (0 == event->rndHitCollection()) {
		event->setRnDHitCollection(new StRnDHitCollection());
		LOG_DEBUG << "Creating StRnDHitCollection for FTS" << endm;
	}

	// Digitize GEANT FTS hits
	fillSilicon(event);
	//    event->rndHitCollection()->Print();

	return kStOk;
}

/* Fill an event with StFtsHits. */
/* This should fill StFtsStrip for realistic simulator and let clustering fill StFtsHit */
/* For now skipping StFtsStrip and clustering, and fill StFtsHits directly here*/

void StFstFastSimMaker::fillSilicon(StEvent *event) {

	StRnDHitCollection *fsicollection = event->rndHitCollection();

	/*static const*/ const int NDISC = 6;
	//static const int MAXR  =128; // JCW: let's give Stv best shot at this possible...
	//static const int MAXPHI=128*12;
	/*static const */ const int MAXR = mNumR;
	/*static const */ const int MAXPHI = mNumPHI * mNumSEC;

	//I guess this should be RSEG[NDISC][MAXR+1] array to give better R segements
	//For now this is just unform R segments regardless of disc
	//static const float RMIN[NDISC]={ 2.5, 2.5, 2.5, 2.5}; //hack this need to get right numbers
	//static const float RMAX[NDISC]={23.2,23.2,23.2,23.2}; //hack this need to get right numbers
	//    static const float PI=atan2(0.0,-1.0);
	//    static const float SQRT12=sqrt(12.0);
	/*
	   Rmin =     2.56505
	   Rmax =    11.56986
	   Rmin =     3.41994
	   Rmax =    15.42592
	   Rmin =     4.27484
	   Rmax =    19.28199
	   Rmin =     5.13010
	   Rmax =    23.13971
	   */
	// static const float RMIN[] = {   2.56505,   3.41994,   4.27484,  5.13010, 5.985, 6.83988 };
	// static const float RMAX[] = {  11.56986,  15.42592,  19.28199, 23.13971, 26.99577, 30.84183 };
	// Raster each disk by 1mm, 60 degree offset for every disk
	static float X0[] = {0, 0, 0, 0, 0, 0};
	static float Y0[] = {0, 0, 0, 0, 0, 0};
	if (mRaster > 0)
		for (int i = 0; i < 6; i++) {
			X0[i] = mRaster * TMath::Cos(i * 60 * TMath::DegToRad());
			Y0[i] = mRaster * TMath::Sin(i * 60 * TMath::DegToRad());
		}

	//table to keep pointer to hit for each disc, r & phi strips
	StRnDHit *_map[NDISC][MAXR][MAXPHI];
	double ***enrsum = (double ***)malloc(NDISC * sizeof(double **));
	double ***enrmax = (double ***)malloc(NDISC * sizeof(double **));
	//memset( _map, 0, NDISC*MAXR*MAXPHI*sizeof(StRnDHit*) );
	//StRnDHit* ***_map = (StRnDHit* ***)malloc(NDISC*sizeof(StRnDHit* **));[NDISC][MAXR][MAXPHI];
	for (int id = 0; id < NDISC; id++) {
		enrsum[id] = (double **)malloc(MAXR * sizeof(double *));
		enrmax[id] = (double **)malloc(MAXR * sizeof(double *));
		for (int ir = 0; ir < MAXR; ir++) {
			enrsum[id][ir] = (double *)malloc(MAXPHI * sizeof(double));
			enrmax[id][ir] = (double *)malloc(MAXPHI * sizeof(double));
		}
	}

	for (int id = 0; id < NDISC; id++) {
		for (int ir = 0; ir < MAXR; ir++) {
			for (int ip = 0; ip < MAXPHI; ip++) {
				_map[id][ir][ip] = 0;
				enrsum[id][ir][ip] = 0;
				enrmax[id][ir][ip] = 0;
			}
		}
	}

	// Read the g2t table
	St_g2t_fts_hit *hitTable = static_cast<St_g2t_fts_hit *>(GetDataSet("g2t_fsi_hit"));
	if (!hitTable) {
		LOG_INFO << "g2t_fsi_hit table is empty" << endm;
		return; // Nothing to do
	}           // if

	const Int_t nHits = hitTable->GetNRows();
	LOG_DEBUG << "g2t_fsi_hit table has " << nHits << " hits" << endm;
	const g2t_fts_hit_st *hit = hitTable->GetTable();
	//    StPtrVecFtsHit hits; //temp storage for hits
	StPtrVecRnDHit hits;

	// track table
	St_g2t_track *trkTable = static_cast<St_g2t_track *>(GetDataSet("g2t_track"));
	if (!trkTable) {
		LOG_INFO << "g2t_track table is empty" << endm;
		return; // Nothing to do
	}           // if

	const Int_t nTrks = trkTable->GetNRows();
	LOG_DEBUG << "g2t_track table has " << nTrks << " tracks" << endm;
	const g2t_track_st *trk = trkTable->GetTable();

	gRandom->SetSeed(0);
	int count = 0;
	for (Int_t i = 0; i < nHits; ++i) {
		hit = (g2t_fts_hit_st *)hitTable->At(i);
		if (hit) {
			int volume_id = hit->volume_id;
			LOG_INFO << "volume_id = " << volume_id << endm;
			int d = volume_id / 1000;        // disk id
			int w = (volume_id % 1000) / 10; // wedge id
			int s = volume_id % 10;          // sensor id
			LOG_INFO << "d = " << d << ", w = " << w << ", s = " << s << endm;

			//     LOG_INFO << " volume id = " << d << endm;
			//if (d > 6) continue;   // skip large disks
			//if (false == mEnable[d - 1]) continue; // disk switched off

			float e = hit->de;
			int t = hit->track_p;

			trk = (g2t_track_st *)trkTable->At(t);
			int isShower = false;
			if (trk)
				isShower = trk->is_shower;

			float xc = X0[d - 1];
			float yc = Y0[d - 1];

			float x = hit->x[0];
			float y = hit->x[1];
			float z = hit->x[2];

			if (z > 200)
				continue; // skip large disks

			// rastered
			float xx = x - xc;
			float yy = y - yc;

			float r = sqrt(x * x + y * y);
			float p = atan2(y, x);

			// rastered
			float rr = sqrt(xx * xx + yy * yy);
			float pp = atan2(yy, xx);

			while (p < 0.0)
				p += 2.0 * FstGlobal::PI;
			while (p >= 2.0 * FstGlobal::PI)
				p -= 2.0 * FstGlobal::PI;
			while (pp < 0.0)
				pp += 2.0 * FstGlobal::PI;
			while (pp >= 2.0 * FstGlobal::PI)
				pp -= 2.0 * FstGlobal::PI;

			LOG_INFO << "rr = " << rr << " pp=" << pp << endm;
			LOG_INFO << "RMIN = " << FstGlobal::RMIN[d - 1] << " RMAX= " << FstGlobal::RMAX[d - 1] << endm;

			// Cuts made on rastered value
			if (rr < FstGlobal::RMIN[d - 1] || rr > FstGlobal::RMAX[d - 1])
				continue;
			LOG_INFO << "rr = " << rr << endm;

			// Strip numbers on rastered value
			int ir = int(MAXR * (rr - FstGlobal::RMIN[d - 1]) / (FstGlobal::RMAX[d - 1] - FstGlobal::RMIN[d - 1]));
			// LOG_INFO << "ir1 = " << ir << endm;
			for (int ii = 0; ii < MAXR; ii++)
				if (rr > FstGlobal::RSegment[ii] && rr <= FstGlobal::RSegment[ii + 1])
					ir = ii;
			// LOG_INFO << "ir2 = " << ir << endm;
			// Phi number
			int ip = int(MAXPHI * pp / 2.0 / FstGlobal::PI);

			if (ir >= 8)
				continue;

			if (MAXR)
				assert(ir < MAXR);
			if (MAXPHI)
				assert(ip < MAXPHI);

			StRnDHit *fsihit = 0;
			if (_map[d - 1][ir][ip] == 0) // New hit
			{

				if (FstGlobal::verbose)
					LOG_INFO << Form("NEW d=%1d xyz=%8.4f %8.4f %8.4f r=%8.4f phi=%8.4f iR=%2d iPhi=%4d dE=%8.4f[MeV] truth=%d",
							d, x, y, z, r, p, ir, ip, e * 1000.0, t)
						<< endm;

					count++;
				fsihit = new StRnDHit();
				fsihit->setDetectorId(kFtsId);
				fsihit->setLayer(d);

				//
				// Set position and position error based on radius-constant bins
				//
				float p0 = (ip + 0.5) * 2.0 * FstGlobal::PI / float(MAXPHI);
				float dp = 2.0 * FstGlobal::PI / float(MAXPHI) / FstGlobal::SQRT12;
				// float r0 = RMIN[d - 1] + (ir + 0.5) * (RMAX[d - 1] - RMIN[d - 1]) / float(MAXR);
				// float dr = (RMAX[d - 1] - RMIN[d - 1]) / float(MAXR);
				// ONLY valide for the disk array 456, no difference for each disk
				float r0 = (FstGlobal::RSegment[ir] + FstGlobal::RSegment[ir + 1]) * 0.5;
				float dr = FstGlobal::RSegment[ir + 1] - FstGlobal::RSegment[ir];
				// LOG_INFO << "r0 = " << r00 << endm;
				float x0 = r0 * cos(p0) + xc;
				float y0 = r0 * sin(p0) + yc;
				assert(TMath::Abs(x0) + TMath::Abs(y0) > 0);
				float dz = 0.03 / FstGlobal::SQRT12;
				float er = dr / FstGlobal::SQRT12;
				fsihit->setPosition(StThreeVectorF(x0, y0, z));
				// fsihit->setPositionError(StThreeVectorF(er, dp, dz));
				fsihit->setPositionError(StThreeVectorF(er, dp, dz));

				// StThreeVectorF posErr = fsihit->positionError();
				// cout << " input : " << er*2 <<" , "<<  dp<< " , " << dz <<endl;
				// cout << " output : " << posErr.x() <<" , "<< posErr.y() << " , " << posErr.z()  <<endl;
				fsihit->setErrorMatrix(&FstGlobal::Hack1to6(fsihit)[0][0]);

				fsihit->setCharge(e);
				fsihit->setIdTruth(t, 100);
				hits.push_back(fsihit);
				_map[d - 1][ir][ip] = fsihit;
				enrsum[d - 1][ir][ip] += e; // Add energy to running sum
				enrmax[d - 1][ir][ip] = e;  // Set maximum energy

				LOG_INFO << Form("NEW d=%1d xyz=%8.4f %8.4f %8.4f ", d, x, y, z) << endm;
				LOG_INFO << Form("smeared xyz=%8.4f %8.4f %8.4f ", fsihit->position().x(), fsihit->position().y(), fsihit->position().z()) << endm;

				if(mHist){
					TVector2 hitpos_mc(x, y);
					TVector2 hitpos_rc(fsihit->position().x(), fsihit->position().y());

					hTrutHitYXDisk->Fill(x, y, d);
					hTrutHitRDisk->Fill(hitpos_mc.Mod(), d);
					if (d == 4)
						hTrutHitRShower[0]->Fill(hitpos_mc.Mod(), isShower);
					if (d == 5)
						hTrutHitRShower[1]->Fill(hitpos_mc.Mod(), isShower);
					if (d == 6)
						hTrutHitRShower[2]->Fill(hitpos_mc.Mod(), isShower);
					hTrutHitPhiDisk->Fill(hitpos_mc.Phi() * 180.0 / TMath::Pi(), d);
					hTrutHitPhiZ->Fill(hitpos_mc.Phi() * 180.0 / TMath::Pi(), z);
					hRecoHitYXDisk->Fill(fsihit->position().x(), fsihit->position().y(), d);
					hRecoHitRDisk->Fill(hitpos_rc.Mod(), d);
					hRecoHitPhiDisk->Fill(hitpos_rc.Phi() * 180.0 / TMath::Pi(), d);
					hRecoHitPhiZ->Fill(hitpos_rc.Phi() * 180.0 / TMath::Pi(), z);
					hGlobalDRDisk->Fill(hitpos_rc.Mod() - hitpos_mc.Mod(), d);
					hGlobalZ->Fill(fsihit->position().z());

					// cout << "CHECK : " << fsihit->position().x()-x << " |||  "<<  fsihit->position().y()-y << endl;
					h2GlobalXY->Fill(x, y);
					h2GlobalSmearedXY->Fill(fsihit->position().x(), fsihit->position().y());
					h2GlobalDeltaXY->Fill(fsihit->position().x() - x, fsihit->position().y() - y);
					h3GlobalDeltaXYDisk->Fill(fsihit->position().x() - x, fsihit->position().y() - y, d);

					h3GlobalDeltaXYR->Fill(fsihit->position().x() - x, fsihit->position().y() - y, sqrt(pow(fsihit->position().x(), 2) + pow(fsihit->position().y(), 2)));
				}
			}
			else // Adding energy to old hit
			{
				//     LOG_INFO << Form("ADD d=%1d xyz=%8.4f %8.4f %8.4f r=%8.4f phi=%8.4f iR=%2d iPhi=%4d dE=%8.4f[MeV] truth=%d",
				//            d,x,y,z,r,p,ir,ip,e*1000.0,t) <<endm;

				fsihit = _map[d - 1][ir][ip];
				fsihit->setCharge(fsihit->charge() + e);

				// Add energy to running sum
				enrsum[d - 1][ir][ip] += e;
				double &E = enrmax[d - 1][ir][ip];
				if (e > E)
					E = e;

				// keep idtruth but dilute it...
				t = fsihit->idTruth();

				fsihit->setIdTruth(t, 100 * E / enrsum[d - 1][ir][ip]);
			}
		}
	}
	int nfsihit = hits.size();

	// TODO: put back to StarRandom global
	// Loop over hits and digitize
	for (int i = 0; i < nfsihit; i++) {
		//hack! do digitization here, or more realistic smearing
		// TODO : PUT BACK TO SiRand with above
		double rnd_save = gRandom->Rndm();

		 cout <<"to be saved : " << rnd_save << " , discard prob : "<< mInEff << endl;
		if (rnd_save > mInEff)
			fsicollection->addHit(hits[i]);
	}
	if (FstGlobal::verbose)
		LOG_INFO << Form("Found %d/%d g2t hits in %d cells, created %d hits with ADC>0", count, nHits, nfsihit, fsicollection->numberOfHits()) << endm;
		//    fsicollection->print(1);
		for (int id = 0; id < NDISC; id++) {
			for (int ir = 0; ir < MAXR; ir++) {
				free(enrsum[id][ir]);
				free(enrmax[id][ir]);
			}
		}
	for (int id = 0; id < NDISC; id++) {
		free(enrmax[id]);
		free(enrsum[id]);
	}
	free(enrsum);
	free(enrmax);
	//
	// delete *_map;
	// delete enrmax;
	// delete enrsum;
}
//

int StFstFastSimMaker::Finish() {
	if(mHist){
		fOut->cd();
		hTrutHitYXDisk->Write();
		hTrutHitRDisk->Write();
		hTrutHitRShower[0]->Write();
		hTrutHitRShower[1]->Write();
		hTrutHitRShower[2]->Write();
		hTrutHitPhiDisk->Write();
		hTrutHitPhiZ->Write();
		hRecoHitYXDisk->Write();
		hRecoHitRDisk->Write();
		hRecoHitPhiDisk->Write();
		hRecoHitPhiZ->Write();
		hGlobalDRDisk->Write();
		hGlobalZ->Write();
		h3GlobalDeltaXYR->Write();
		h2GlobalXY->Write();
		h2GlobalSmearedXY->Write();
		h2GlobalDeltaXY->Write();
		h3GlobalDeltaXYDisk->Write();
		fOut->Close();
	}
	return kStOK;
}

//_____________________________________________________________________________
StMatrixF FstGlobal::Hack1to6(const StHit *stHit) {
	//   X = R*cos(Fi), Y=R*sin(Fi), Z = z
	//   dX/dR  = (    cos(Fi)  ,sin(Fi),0)
	//   dX/dFi = (-R*sin(Fi), R*cos(Fi),0)
	//   dX/dZ  = (         0,         0,1)

	auto hiPos = stHit->position();
	auto hiErr = stHit->positionError();
	double Rxy = sqrt(hiPos[0] * hiPos[0] + hiPos[1] * hiPos[1]);
	double cosFi = hiPos[0] / Rxy;
	double sinFi = hiPos[1] / Rxy;
	double T[3][3] = {{cosFi, -Rxy * sinFi, 0}, {sinFi, Rxy * cosFi, 0}, {0, 0, 1}};
	double Ginp[6] = {hiErr[0] * hiErr[0], 0, hiErr[1] * hiErr[1], 0, 0, hiErr[2] * hiErr[2]};
	double Gout[6];

	TCL::trasat(T[0], Ginp, Gout, 3, 3);
	StMatrixF mtxF(3, 3);

	for (int i = 0, li = 0; i < 3; li += ++i) {
		for (int j = 0; j <= i; j++) {
			mtxF[i][j] = Gout[li + j];
			mtxF[j][i] = mtxF[i][j];
		}
	}

	return mtxF;
}
