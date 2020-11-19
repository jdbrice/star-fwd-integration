# star fwd software integration on RCF
This code provides a snapshot of the star-sw development of the STAR forward tracking and detector simulator software.

## TL;DR.
1) checkout the github repo : `git clone https://github.com/jdbrice/star-fwd-integration.git`
2) run `starver dev`, then run `./rcf-build.sh` to build the code
3) make a simulation dataset : `starsim -w 0 -b tests/testg.kumac nevents=1000 ntrack=1 etamn=2.5 etamx=4.0 ptmn=0.2 ptmx=1.0`
4) run the forward tracking test with : `source rcf-env.sh` then `root4star -b -q -l tests/fast_track.C`
5) Optional: check test.root for the debug output of the forward tracking, e.g. "PtRes" histogram shows average pT resolution.

## What is included?
```
StRoot
|-StFwdTrackMaker (Maker for running forward tracking)
|-|-StFwdTrackMaker.h
|-|-StFwdTrackMaker.cxx
|-|-XmlConfig/ (See: https://github.com/jdbrice/XmlConfig )
|-|-include/Tracker (forward tracking package)
|
|-StFstSimMaker (Simulator for the forward silicon tracker)
|-|-StFstFastSimMaker.h
|-|-StFstFastSimMaker.cxx
|
|-StFttSimMaker (Simulator for the forward sTGC tracker)
|-|-StFttFastSimMaker.h
|-|-StFttFastSimMaker.cxx
```

### StFwdTrackMaker
This is the main package for running forward tracking through StRoot. The package consists of a "maker" that interfaces with the STAR environment and can be run as part of the `StChain`.  The `StFwdTrackMaker` and internal tracking framework maintain a clear separation of concerns. From the perspective of the `StFwdTrackMaker` the tracking package is meant to be a "black box" - space points from detector hits are fed into it, and track seeds / fit tracks are output and the internal implementation is irrelavent.  
#### What the `StFwdTrackMaker` does:
- `StFwdTrackMaker` loads detector hit data
  - directly from GEANT for MC level tracking performance
  - from the StEvent hit collections
- Provides primary vertex information (currently only MC PV information is provided)
- Writes track info into `StEvent`

#### What the tracking package does:
- Once, during initialization
  - loads magnetic field map from `StarMagField`
  - sets up geometry for tracking in material 
- Each event
  - finds track seeds
  - fits tracks using GenFit
  - passes back a list of fit tracks 
  

### StFstSimMaker
This package provides simulators for the forward silicon tracker. Currently only the "fast" simulator is included. The fast simulator processes GEANT hits stored in the `g2t_fsi_hit` table. The primary function of this package is to digitize the GEANT hits onto the R-phi strips of the silicon sensor layout. The hits are stored into `StRndHit` objects and the covariance matrix is computed according to the local geometry of the hit.

### StFttSimMaker
This package provides simulators for the forward silicon tracker. Currently only the "fast" simulator is included. The fast simulator processes GEANT hits stored in the `g2t_stg_hit` table. The primary function of this package is to digitize the GEANT hits onto the strip layout of the sTGC module geometry. The hits are stored into `StRndHit` objects and the covariance matrix is computed according to the nominal XY resolution of 100 microns. Since the sTGC is essentially a sandwich of two 1D detectors, ghost hits are present at the intersection of lit strips. These ghost hits are computed according to XY strips and added to the hit collection. 


### Note about Fast Simulators
Since the fast simulator is meant to be the simplest response simulator, they are essentially complete.
However a few things will change in the future:
- `StRndHit` will be replaced a dedicated hit object for `fst` type hits. 
- The `StRndHitCollection` will be replaced with a dedicated hit collection for `fst` type hits

These updates to `StEvent` are being worked on in parallel (the addition of dedicated hit types and collections). The important things to note are that 1) this code already works with the `StEvent` in `dev`, 2) `StEvent` can be updated separately with no conflicts, 3) The update will be atomic/transparent since no other code currently depends the `StRndHit` / `StRndHitCollection`.


## Building the packages on RCF
The file `build.sh` invokes cons with additional flags to provide header files for the external dependencies of Genfit and KiTrack.
Build with:
```sh
starver dev
./rcf-build.sh
```
This modified `cons` call just adds include paths via the `EXTRA_CPPPATH` variable. Currently the header files for the dependencies are found here:
```
/star/data03/pwg/jdb/FWD/cmake/star-install-SL20c-64-Release/sl74_x8664_gcc485/include/
```

## Running tests
### Generate simulation file as input 
A simple kumac is included for generating single particle events for testing.
generate an `fzd` file with:
```sh
starsim -w 0 -b tests/testg.kumac nevents=1000 ntrack=1 etamn=2.5 etamx=4.0 ptmn=0.2 ptmx=1.0
```

### Running the forward tracking

The fast_track.C script is a basic example of how to run the two fast simulators and the forward tracking package.
It can be run with:
```
source rcf-env.sh
root4star -b -q -l tests/fast_track.C
```
This will produce a number of output files for evaluating the fast simulators and the tracking.
Specifically one may look at "test.root" which contains the forward tracking output. 
The histogram "FitStatus" shows a summary of the fitting steps.
The histogram "PtRes" shows the pT resolution.


## Prebuilt dependencies
The `GenFit2` and `KiTrack` libraries are built with CMAKE. The prebuilt shared libraries are here:
```
/star/data03/pwg/jdb/FWD/cmake-test/star-install-SL20c-32-Release/sl74_gcc485/lib/
```
built in 32-bit release mode.



## NOTES:
As of this writing, the code builds on RCF but I do not have a setup for running the forward tracking yet - since I need to build Genfit and KiTrack in RCF (32bit).


### TTree output format

```cpp
mTree->Branch("n", &mTreeN, "n/I"); // number of hits
mTree->Branch("x", mTreeX, "x[n]/F"); // hit position x
mTree->Branch("y", mTreeY, "y[n]/F"); // y
mTree->Branch("z", mTreeZ, "z[n]/F"); // z
mTree->Branch("tid", &mTreeTID, "tid[n]/I"); // track id producing this hit
mTree->Branch("vid", &mTreeVID, "vid[n]/I"); // volume id for this hit
mTree->Branch("hpt", &mTreeHPt, "hpt[n]/F"); // Mc track pt
mTree->Branch("hsv", &mTreeHSV, "hsv[n]/I"); // Mc track start vertex

// mc tracks
mTree->Branch("nt", &mTreeNTracks, "nt/I"); // # of mc tracks
mTree->Branch("pt", &mTreePt, "pt[nt]/F"); // track pt
mTree->Branch("eta", &mTreeEta, "eta[nt]/F"); // eta
mTree->Branch("phi", &mTreePhi, "phi[nt]/F"); // phi
mTree->Branch("tid", &mTreeTID, "tid/I"); // track id
```