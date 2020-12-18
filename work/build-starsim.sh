
cd /tmp/star-build

STAR_PATCH=""
STAR_BUILD_TYPE=Debug
cmake /tmp/star-sw -DSTAR_SRC=/tmp/star-cvs \
    -DSTAR_PATCH=${STAR_PATCH} -DCMAKE_INSTALL_PREFIX=/tmp/star-install \
    -DCERNLIB_ROOT=/cern/2006 -DCMAKE_BUILD_TYPE=${STAR_BUILD_TYPE}


make starsim -j4
make install -j4

cd /tmp/work