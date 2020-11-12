#!/bin/sh

MAINDIR=`pwd`
cd dependencies

echo "Building boost..."
tar -xvzf boost_1_59_0.tar.gz 
cd boost_1_59_0
./bootstrap.sh --prefix=${MAINDIR}
./bjam -j6
./bjam install

cd ..

echo "Building boost numeric bindings..."
tar -xvzf boost-numeric-bindings-20081116.tar.gz
cd boost-numeric-bindings
cp -R boost/numeric/bindings ${MAINDIR}/include/boost/numeric

cd ..

echo "Building fltk..."
tar -xvzf fltk-1.3.3-source.tar.gz 
cd fltk-1.3.3
./configure --prefix=${MAINDIR} --enable-shared 
make -j6 install

cd ..

echo "Building activemq..."
tar -xvzf activemq-cpp-library-3.9.0-src.tar.gz
cd activemq-cpp-library-3.9.0
./configure --prefix=${MAINDIR} --enable-shared 
make -j6 install

cd ..

echo "Building ode..."
tar -xvzf ode-0.12.tar.gz
cd ode-0.12
./configure --prefix=${MAINDIR} --enable-shared 
make -j6 install

cd ..

echo "Building xerces..."
tar -xvzf xerces-c-3.1.3.tar.gz
cd xerces-c-3.1.3
./configure --prefix=${MAINDIR} --enable-shared 
make -j6 install

cd ..

echo "Building google protocol buffers..."
tar -xvzf protobuf-2.5.0.tar.gz
cd protobuf-2.5.0
./configure --prefix=${MAINDIR} --enable-shared 
make -j6 install

cd ..

echo "Building glew..."
tar -xvzf glew-1.6.0.tgz
cd glew-1.6.0
make GLEW_DEST=${MAINDIR} -j6 install

cd ..

echo "Building freealut..."
tar -xvzf freealut-1.1.0.tar.gz
cd freealut-1.1.0
# modify for osx
sed '86 a\
LIBS=\"$LIBS -framework IOKit -framework OpenAL\"' configure.ac > configure.osx
mv configure.osx configure.ac
./autogen.sh
./configure --disable-debug --disable-dependencies-tracking  --prefix=${MAINDIR} --enable-shared 
make -j6 install

cd ..

echo "Building libsndfile..."
tar -xvzf libsndfile-1.0.25.tar.gz
cd libsndfile-1.0.25
./configure --prefix=${MAINDIR}
sed '/Carbon.h/i\ 
\/\/' programs/sndfile-play.c > tmp
mv tmp programs/sndfile-play.c
make -j6 install

cd ..
echo "Finished building dependencies for SmartBody"

