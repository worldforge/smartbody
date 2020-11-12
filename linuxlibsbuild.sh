#!/bin/sh

set -e

MAINDIR=$(pwd)
mkdir -p dependencies
cd dependencies || exit

# determine which machine we are using
unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac
echo "${machine}"


echo "Building boost numeric bindings..."
tar -xvf boost-numeric-bindings-20081116.tar.gz
cd boost-numeric-bindings || exit
mkdir -p "${MAINDIR}"/include/boost
mkdir -p "${MAINDIR}"/include/boost/numeric
cp -R boost/numeric/bindings "${MAINDIR}"/include/boost/numeric

cd ..

echo "Building activemq..."
tar -xvzf activemq-cpp-library-3.9.0-src.tar.gz
cd activemq-cpp-library-3.9.0 || exit
if [ x"${machine}" = "xMac" ] ; then
./configure --prefix="${MAINDIR}" --enable-shared
else
./configure --prefix="${MAINDIR}" --enable-shared
fi
make -j6 install

cd ..

echo "Building ode..."
tar -xvzf ode-0.12.tar.gz
cd ode-0.12 || exit
./configure --prefix="${MAINDIR}" --enable-shared  --enable-double-precision --disable-demos
make -j6 install

cd ..

echo "Building google protocol buffers..."
tar -xvzf protobuf-2.5.0.tar.gz
cd protobuf-2.5.0 || exit
./configure --prefix="${MAINDIR}" --enable-shared
make -j6 install

cd ..

#echo "Building Irrlicht engine..."
#unzip irrlicht-1.8.3.zip
#cd irrlicht-1.8.3
#cd source/Irrlicht
#make -j6
#cd ../../..
#mv irrlicht-1.8.3 ..

#cd ..



echo "Finished building linux dependencies for SmartBody"

