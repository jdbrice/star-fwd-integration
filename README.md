# star fwd software integration on RCF
This code provides a snapshot of the star-sw development of the STAR forward tracking and detector simulator software.

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
... Details about the StFwdTrackMaker here

### StFstSimMaker
... Details about the StFstSimMaker here

### StFttSimMaker
... Details about the StFttSimMaker here

## Build on RCF
The file `build.sh` invokes cons with additional flags to provide header files for the external dependencies of Genfit and KiTrack.
Build with:
```sh
./build.sh
```
This modified `cons` call just adds include paths via the `EXTRA_CPPPATH` variable. Currently the header files for the dependencies are found here:
```
/star/data03/pwg/jdb/FWD/cmake/star-install-SL20c-64-Release/sl74_x8664_gcc485/include/
```


## NOTES:
As of this writing, the code builds on RCF but I do not have a setup for running the forward tracking yet - since I need to build Genfit and KiTrack in RCF (32bit).
