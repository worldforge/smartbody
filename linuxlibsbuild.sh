#!/bin/sh

MAINDIR=`pwd`
mkdir -p dependencies
cd dependencies

# determine which machine we are using
unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac
echo ${machine}


echo "Building boost numeric bindings..."
gunzip boost-numeric-bindings-20081116.tar.gz
tar -xvf boost-numeric-bindings-20081116.tar
cd boost-numeric-bindings
mkdir -p ${MAINDIR}/include/boost
mkdir -p ${MAINDIR}/include/boost/numeric
cp -R boost/numeric/bindings ${MAINDIR}/include/boost/numeric

cd ..

echo "Building activemq..."
tar -xvzf activemq-cpp-library-3.9.0-src.tar.gz
cd activemq-cpp-library-3.9.0
if ${machine} == "Mac" then
./configure --prefix=${MAINDIR} --enable-shared 
else
./configure --prefix=${MAINDIR} --enable-shared --with-apr=/usr/local/opt/apr 
fi
make -j6 install

cd ..

echo "Building ode..."
tar -xvzf ode-0.12.tar.gz
cd ode-0.12
./configure --prefix=${MAINDIR} --enable-shared  --enable-double-precision
make -j6 install

cd ..

echo "Building google protocol buffers..."
tar -xvzf protobuf-2.5.0.tar.gz
cd protobuf-2.5.0
./configure --prefix=${MAINDIR} --enable-shared 
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

