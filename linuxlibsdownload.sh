#!/bin/sh

AGENT=Mozilla/4.0
mkdir -p dependencies


echo "Downloading boost numeric bindings..."
curl -A "$AGENT" http://smartbody.ict.usc.edu/dependencies/boost-numeric-bindings-20081116.tar.gz > dependencies/boost-numeric-bindings-20081116.tar.gz

echo "Downloading activemq..."
curl -A "$AGENT" http://smartbody.ict.usc.edu/dependencies/activemq-cpp-library-3.9.0-src.tar.gz > dependencies/activemq-cpp-library-3.9.0-src.tar.gz

echo "Downloading ode..."
curl -A "$AGENT" http://smartbody.ict.usc.edu/dependencies/ode-0.12.tar.gz > dependencies/ode-0.12.tar.gz

echo "Downloading google protocol buffers..."
curl -A "$AGENT" http://smartbody.ict.usc.edu/dependencies/protobuf-2.5.0.tar.gz > dependencies/protobuf-2.5.0.tar.gz 

#echo "Downloading Irrlicht engine..."
#wget --directory-prefix=dependencies http://downloads.sourceforge.net/project/irrlicht/Irrlicht%20SDK/1.8/1.8.3/irrlicht-1.8.3.zip 

echo "Finished downloading dependencies for SmartBody on linux"
