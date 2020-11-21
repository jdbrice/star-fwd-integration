#!/usr/bin/env bash
set -euo pipefail

echo docker run --rm -ti $@ -w /tmp/work -v `pwd`/work:/tmp/work -v `pwd`/tests:/tmp/work/tests -v `pwd`/StRoot:/tmp/star-sw-master/StRoot starbnl/star-sw:latest-build bash
docker run --rm -ti $@ -w /tmp/work -v `pwd`/work:/tmp/work -v `pwd`/tests:/tmp/work/tests -v `pwd`/star-sw:/tmp/star-sw-master -v `pwd`/StRoot:/tmp/star-sw-master/StRoot starbnl/star-sw:latest-build bash 