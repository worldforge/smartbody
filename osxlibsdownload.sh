#!/bin/sh

mkdir -p dependencies



echo "Downloading boost..."
curl http://smartbody.ict.usc.edu/dependencies/boost_1_59_0.tar.gz > dependencies/boost_1_59_0.tar.gz

echo "Downloading boost numeric bindings..."
curl http://smartbody.ict.usc.edu/dependencies/boost-numeric-bindings-20081116.tar.gz > dependencies/boost-numeric-bindings-20081116.tar.gz

echo "Downloading fltk..."
curl http://smartbody.ict.usc.edu/dependencies/fltk-1.3.3-source.tar.gz > dependencies/fltk-1.3.3-source.tar.gz

echo "Downloading activemq..."
curl http://smartbody.ict.usc.edu/dependencies/activemq-cpp-library-3.9.0-src.tar.gz > dependencies/activemq-cpp-library-3.9.0-src.tar.gz

echo "Downloading ode..."
curl http://smartbody.ict.usc.edu/dependencies/ode-0.12.tar.gz > dependencies/ode-0.12.tar.gz

echo "Downloading xerces..."
curl http://smartbody.ict.usc.edu/dependencies/xerces-c-3.1.3.tar.gz > dependencies/xerces-c-3.1.3.tar.gz

echo "Downloading google protocol buffers..."
curl http://smartbody.ict.usc.edu/dependencies/protobuf-2.5.0.tar.gz > dependencies/protobuf-2.5.0.tar.gz 

echo "Downloading glew..."
curl http://smartbody.ict.usc.edu/dependencies/glew-1.6.0.tgz > dependencies/glew-1.6.0.tgz

echo "Downloading freealut..."
curl http://smartbody.ict.usc.edu/dependencies/freealut-1.1.0.tar.gz > dependencies/freealut-1.1.0.tar.gz

echo "Downloading libsndfile..."
curl http://smartbody.ict.usc.edu/dependencies/libsndfile-1.0.25.tar.gz > dependencies/libsndfile-1.0.25.tar.gz

echo "Finished downloading dependencies for SmartBody on OSX"
