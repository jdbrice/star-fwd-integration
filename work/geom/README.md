# Geometry Testing

## In Docker
Assuming that you work inside the docker container from `/tmp/work/`:
1. build geom: `./geom/geom-build.sh` - this uses `geom/FstmGeo.xml` so update it accordingly
2. export geometry for GENFIT: `root4star -b -q -l 'tests/build_geom.C( "dev2021", "tests/fGeom.Flem1.root" )'` e.g. for an updated geom from FLEMMING
3. build starsim: `./build-starsim.sh`
4. run starsim to create sim file from new geometry : `starsim -w 0 -b tests/testg.kumac nevents=1000 ntrack=1 etamn=2.5 etamx=4.0 ptmn=0.1 ptmx=5.0` - this creates `tests/sim.fzd`
5. Build all tracking code: `./build 1`
6. Update geometry and sim file in `fast_track.C` and `fast_track.xml`
7. Run tracking: `./tests/fast_track.C >& FLEM1.FST.LOG`


## on RCF
Follow a similar procedure. 
1. Update geometry in StarVmc/Geometry/...
2. Build new geometry
3. export geometry for GENFIT
4. run starsim
5. build tracking code (cons)
6. update geometry and sim file in fast_track.C, fast_track.xml
7. run the tracking code, check for time out tracks " Forward tracking on this event took 17957.2 ms", should be less than 100 ms.