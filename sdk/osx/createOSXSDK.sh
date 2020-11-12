# generate a SmartBodySmartBodySDK folder
mkdir -p SmartBodySDK
# documentation
cp ../../SmartBodyManual.pdf ./SmartBodySDK/
cp ../../SmartBodyPythonAPI.html ./SmartBodySDK/
cp ../../"3rd party licenses.txt" ./SmartBodySDK/

# copy include
mkdir -p SmartBodySDK/include
rsync -ap --exclude=".svn" ../../include ./SmartBodySDK
rsync -ap --exclude=".svn" ../../lib/vhmsg/vhmsg-c/include/*.h ./SmartBodySDK/include/vhmsg/
rsync -ap --exclude=".svn" ../../lib/bonebus/include/*.h ./SmartBodySDK/include/bonebus/
rsync -ap --exclude=".svn" ../../lib/vhcl/include/*.h ./SmartBodySDK/include/vhcl/
rsync -ap --exclude=".svn" ../../core/smartbody/SmartBody/src/external/protobuf/include/* ./SmartBodySDK/include/protobuf/
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/pprAI/include/*.h ./SmartBodySDK/include/steersuite/
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/steerlib/include/* ./SmartBodySDK/include/steersuite/
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/external/* ./SmartBodySDK/include/steersuite/

# copy lib
rsync -ap --exclude=".svn" ../../lib/*.dylib ./SmartBodySDK/lib
rsync -ap --exclude=".svn" ../../lib/*.a ./SmartBodySDK/lib
rsync -ap --exclude=".svn" ../../lib/*.la ./SmartBodySDK/lib
# copy libraries already built
cp ../../foo/lib/vhcl/libvhcl.a ./SmartBodySDK/lib
cp ../../foo/lib/vhmsg/vhmsg-c/libvhmsg.a ./SmartBodySDK/lib
cp ../../foo/lib/bonebus/libbonebus.a ./SmartBodySDK/lib
cp ../../core/smartbody/sbgui/bin/libsteerlib.dylib ./SmartBodySDK/lib
cp ../../core/smartbody/sbgui/bin/libpprAI.dylib ./SmartBodySDK/lib
cp ../../core/smartbody/sbgui/bin/libpinocchio.dylib ./SmartBodySDK/lib

# copy bin & copy all the dynamic libraries to bin folder
mkdir SmartBodySDK/bin
cp ../../bin/protoc ./SmartBodySDK/bin
chmod +x ./SmartBodySDK/bin/protoc

# copy startup commands
cp ../../bin/startSBGUI.command ./SmartBodySDK/bin
cp ../../bin/startSimpleSmartBody.command ./SmartBodySDK/bin

# copy readme.txt
rsync -ap --exclude=".svn" ./readme.txt ./SmartBodySDK/readme.txt

# copy build*.sh
rsync -ap --exclude=".svn" ./build*.sh ./SmartBodySDK/

# copy src
mkdir -p ./SmartBodySDK/src
mkdir -p ./SmartBodySDK/src/sbgui/external
mkdir -p ./SmartBodySDK/src/sbgui/external/polyvox
mkdir -p ./SmartBodySDK/src/sbgui/external/polyvox/library
rsync -ap --exclude=".svn" ../../core/smartbody/sbgui/src/* ./SmartBodySDK/src/sbgui/
rsync -ap --exclude=".svn" ../../core/smartbody/sbgui/external/polyvox/library/PolyVoxCore/* ./SmartBodySDK/src/sbgui/external/polyvox/library/PolyVoxCore/
rsync -ap --exclude=".svn" ../../core/smartbody/sbgui/external/polyvox/CMakeLists.txt ./SmartBodySDK/src/sbgui/external/polyvox/
rsync -ap --exclude=".svn" ../../core/smartbody/sbgui/external/Pinocchio/*.cpp ./SmartBodySDK/src/sbgui/external/Pinocchio/
rsync -ap --exclude=".svn" ../../core/smartbody/sbgui/external/Pinocchio/*.h ./SmartBodySDK/src/sbgui/external/Pinocchio/
rsync -ap --exclude=".svn" ../../core/smartbody/sbgui/external/Pinocchio/CMakeLists.txt ./SmartBodySDK/src/sbgui/external/Pinocchio/
rsync -ap --exclude=".svn" ../../core/smartbody/simplesmartbody/simplesmartbody.cpp ./SmartBodySDK/src/simplesmartbody/
rsync -ap --exclude=".svn" ../../core/smartbody/Smartbody/src/* ./SmartBodySDK/src/Smartbody/

# copy Python27
rsync -ap --exclude=".svn" ./Python27 ./SmartBodySDK/
mkdir -p ./SmartBodySDK/include/python2.7
mv -f ./SmartBodySDK/Python27/include/python2.7/* ./SmartBodySDK/include/python2.7/
mv -f ./SmartBodySDK/Python27/lib/libpython2.7.a ./SmartBodySDK/lib
rm -rf ./SmartBodySDK/Python27/include

# copy CMakeLists.txt
rsync -ap --exclude=".svn" ./build/CMakeLists.txt ./SmartBodySDK/CMakeLists.txt
rsync -ap --exclude=".svn" ./build/CMakeLists-src.txt ./SmartBodySDK/src/CMakeLists.txt
rsync -ap --exclude=".svn" ./build/CMakeLists-simplesmartbody.txt ./SmartBodySDK/src/simplesmartbody/CMakeLists.txt
rsync -ap --exclude=".svn" ./build/CMakeLists-SmartBody.txt ./SmartBodySDK/src/SmartBody/CMakeLists.txt
rsync -ap --exclude=".svn" ./build/CMakeLists-sbgui.txt ./SmartBodySDK/src/sbgui/CMakeLists.txt


# first need to create data & data/mesh folder
# copy data folder
mkdir -p ./SmartBodySDK/data
mkdir -p ./SmartBodySDK/data/mesh
rsync -ap --exclude=".svn" ../../data/behaviorsets/* ./SmartBodySDK/data/behaviorsets/
rsync -ap --exclude=".svn" ../../data/ChrBrad/* ./SmartBodySDK/data/ChrBrad/
rsync -ap --exclude=".svn" ../../data/ChrRachel/* ./SmartBodySDK/data/ChrRachel/
rsync -ap --exclude=".svn" ../../data/examples/* ./SmartBodySDK/data/examples/
rsync -ap --exclude=".svn" ../../data/fonts/* ./SmartBodySDK/data/fonts/
rsync -ap --exclude=".svn" ../../data/Sinbad/* ./SmartBodySDK/data/Sinbad/
rsync -ap --exclude=".svn" ../../data/scripts/* ./SmartBodySDK/data/scripts/
rsync -ap --exclude=".svn" ../../data/mesh/ChrBrad/* ./SmartBodySDK/data/mesh/ChrBrad/
rsync -ap --exclude=".svn" ../../data/mesh/ChrRachel/* ./SmartBodySDK/data/mesh/ChrRachel/
rsync -ap --exclude=".svn" ../../data/mesh/Sinbad/* ./SmartBodySDK/data/mesh/Sinbad/
rsync -ap --exclude=".svn" ../../data/mesh/Ogre/* ./SmartBodySDK/data/mesh/Ogre/

# after copying, rebuild it
cd SmartBodySDK
./buildCMake.sh
#rm -rf buildCMake
