#!/bin/bash
mkdir -p /tmp/builds/hippo
sudo apt-get install -y --no-install-recommends libtool make wget hhvm-dev libdouble-conversion-dev liblz4-dev
cd /tmp/builds/hippo
wget -O /tmp/builds/hippo/mongo.tgz https://github.com/mongodb/mongo-hhvm-driver/releases/download/1.2.2/hhvm-mongodb-1.2.2.tgz
tar -xzf mongo.tgz
cd hhvm-mongodb-1.2.2
hphpize
cmake .
make configlib
make -j $(nproc --all)
sudo make install
