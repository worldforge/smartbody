# generate a smartbody folder
mkdir -p smartbody
mkdir -p smartbody/bin

# documentation
cp ../../SmartBodyManual.pdf ./smartbody/
cp ../../SmartBodyPythonAPI.html ./smartbody/
cp ../../"3rd party licenses.txt" ./smartbody/

# copy include (do not include third party ones)
mkdir -p smartbody/include
rsync -ap --exclude=".svn" ../../lib/vhmsg/vhmsg-c/include/*.h ./smartbody/include/vhmsg/
rsync -ap --exclude=".svn" ../../lib/bonebus/include/*.h ./smartbody/include/bonebus/
rsync -ap --exclude=".svn" ../../lib/vhcl/include/*.h ./smartbody/include/vhcl/
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/pprAI/include/*.h ./smartbody/include/steersuite/
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/steerlib/include/* ./smartbody/include/steersuite/
mkdir -p smartbody/include/steersuite/external
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/external/tinyxml/*.h ./smartbody/include/steersuite/external/tinyxml/
mkdir -p smartbody/include/steersuite/mersenne
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/external/mersenne/*.h ./smartbody/include/steersuite/external/mersenne/
mkdir -p smartbody/include/steersuite/external/glfw
mkdir -p smartbody/include/steersuite/external/glfw/include
mkdir -p smartbody/include/steersuite/external/glfw/include/GL
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/external/glfw/include/GL/glfw.h ./smartbody/include/steersuite/external/glfw/include/GL

# copy lib
#rsync -ap --exclude=".svn" ./lib ./smartbody

# copy bin
#rsync -ap --exclude=".svn" ./bin ./smartbody

# copy .so for SmartBody (assuming it's prebuilt)
#cp ../../core/smartbody/sbgui/bin/sbgui ./smartbody/bin
#cp ../../core/smartbody/sbgui/bin/simplesmartbody ./smartbody/bin
#cp ../../core/smartbody/sbgui/bin/libSmartBody.so ./smartbody/bin

# copy dylib for pprAI and steerlib
#cp ../../core/smartbody/sbgui/bin/libpprAI.so ./smartbody/bin
#cp ../../core/smartbody/sbgui/bin/libsteerlib.so ./smartbody/bin

# copy readme.txt
rsync -ap --exclude=".svn" ./README.txt ./smartbody/README.txt

# copy .smartbodysettings file
rsync -ap --exclude=".svn" ./bin/.smartbodysettings ./smartbody/bin

# copy build*.sh
#rsync -ap --exclude=".svn" ./build*.sh ./smartbody/

# copy src
rsync -ap --exclude=".svn" ./src ./smartbody
mkdir -p ./smartbody/src/sbgui
rsync -ap --exclude=".svn" ../../core/smartbody/sbgui/src/* ./smartbody/src/sbgui/
mkdir -p ./smartbody/src/simplesmartbody
rsync -ap --exclude=".svn" ../../core/smartbody/simplesmartbody/simplesmartbody.cpp ./smartbody/src/simplesmartbody/
mkdir -p ./smartbody/src/SmartBody
rsync -ap --exclude=".svn" ../../core/smartbody/SmartBody/src/* ./smartbody/src/SmartBody/
mkdir -p ./smartbody/src/sbgui
rsync -ap --exclude=".svn" --exclude="fltk-1.3.2" --exclude="cegui" --exclude="cegui-0.8.2" --exclude="polyvox" ../../core/smartbody/sbgui/src/* ./smartbody/src/sbgui/
mkdir -p ./smartbody/src/sbgui/external
mkdir -p ./smartbody/src/sbgui/external/polyvox
mkdir -p ./smartbody/src/sbgui/external/polyvox/library
mkdir -p ./smartbody/src/sbgui/external/polyvox/library/PolyVoxCore
rsync -arp --exclude=".svn" ../../core/smartbody/sbgui/external/polyvox/library/PolyVoxCore/* ./smartbody/src/sbgui/external/polyvox/library/PolyVoxCore
mkdir -p ./smartbody/src/sbgui/external/Pinocchio
rsync -ap --exclude=".svn" ../../core/smartbody/sbgui/external/Pinocchio/*.cpp ./smartbody/src/sbgui/external/Pinocchio/
rsync -ap --exclude=".svn" ../../core/smartbody/sbgui/external/Pinocchio/*.h ./smartbody/src/sbgui/external/Pinocchio/

mkdir -p ./smartbody/src/vhcl
rsync -ap --exclude=".svn" ../../lib/vhcl/src/* ./smartbody/src/vhcl/
mkdir -p ./smartbody/src/vhmsg
rsync -ap --exclude=".svn" ../../lib/vhmsg/vhmsg-c/src/* ./smartbody/src/vhmsg/
mkdir -p ./smartbody/src/bonebus
rsync -ap --exclude=".svn" ../../lib/bonebus/src/* ./smartbody/src/bonebus/
mkdir -p ./smartbody/src/steerlib
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/steerlib/src/* ./smartbody/src/steerlib/
mkdir -p ./smartbody/src/pprAI
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/pprAI/src/* ./smartbody/src/pprAI/
mkdir -p ./smartbody/src/external/tinyxml
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/external/tinyxml/*.cpp ./smartbody/src/external/tinyxml/
mkdir -p ./smartbody/src/external/glfw
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/external/glfw/lib/*.c ./smartbody/src/external/glfw/
mkdir -p ./smartbody/src/external/glfw/x11
rsync -ap --exclude=".svn" ../../core/smartbody/steersuite-1.3/external/glfw/lib/x11/*.c ./smartbody/src/external/glfw/x11
mkdir -p smartbody/src/assimp-3.1.1
rsync -ap --exclude=".svn" ../../lib/assimp-3.1.1/* ./smartbody/src/assimp-3.1.1/

# first need to create data & data/mesh folder
# copy data folder
mkdir -p ./smartbody/data
mkdir -p ./smartbody/data/mesh
rsync -ap --exclude=".svn" ../../data/behaviorsets/* ./smartbody/data/behaviorsets/
rsync -ap --exclude=".svn" ../../data/ChrBrad/* ./smartbody/data/ChrBrad/
rsync -ap --exclude=".svn" ../../data/ChrMaarten/* ./smartbody/data/ChrMaarten/
rsync -ap --exclude=".svn" ../../data/ChrRachel/* ./smartbody/data/ChrRachel/
rsync -ap --exclude=".svn" ../../data/examples/* ./smartbody/data/examples/
rsync -ap --exclude=".svn" ../../data/fonts/* ./smartbody/data/fonts/
rsync -ap --exclude=".svn" ../../data/Sinbad/* ./smartbody/data/Sinbad/
rsync -ap --exclude=".svn" ../../data/scripts/* ./smartbody/data/scripts/
rsync -ap --exclude=".svn" ../../data/mesh/ChrBrad/* ./smartbody/data/mesh/ChrBrad/
rsync -ap --exclude=".svn" ../../data/mesh/ChrMaarten/* ./smartbody/data/mesh/ChrMaarten/
rsync -ap --exclude=".svn" ../../data/mesh/ChrRachel/* ./smartbody/data/mesh/ChrRachel/
rsync -ap --exclude=".svn" ../../data/mesh/Sinbad/* ./smartbody/data/mesh/Sinbad/
rsync -ap --exclude=".svn" ../../data/mesh/Ogre/* ./smartbody/data/mesh/Ogre/

# integration examples
mkdir smartbody/src/ogresmartbody
cp ../win32/src/ogresmartbody/*.cpp smartbody/src/ogresmartbody
cp ../win32/src/ogresmartbody/*.h smartbody/src/ogresmartbody
cp ../win32/src/ogresmartbody/plugins.cfg-linux smartbody/bin/plugins.cfg
cp ../win32/src/ogresmartbody/resources.cfg-linux smartbody/bin/resources.cfg
cp ../win32/src/ogresmartbody/*.py smartbody/data/
mkdir smartbody/src/irrlichtsmartbody
cp ../win32/src/irrlichtsmartbody/*.cpp  smartbody/src/irrlichtsmartbody
cp ../win32/src/irrlichtsmartbody/*.h smartbody/src/irrlichtsmartbody
mkdir -p smartbody/irrlicht-1.8.3
cp -R ../../core/irrlichtsmartbody/media/* smartbody/irrlicht-1.8.3/media
cp ../win32/src/irrlichtsmartbody/*.py smartbody/data/

# build files
cp build/CMakeLists.txt ./smartbody/
cp build/CMakeLists.txt-SmartBody ./smartbody/src/SmartBody/CMakeLists.txt
cp build/CMakeLists.txt-src ./smartbody/src/CMakeLists.txt
cp build/CMakeLists.txt-sbgui ./smartbody/src/sbgui/CMakeLists.txt
cp build/CMakeLists.txt-polyvox ./smartbody/src/sbgui/external/polyvox/library/PolyVoxCore/CMakeLists.txt
cp build/CMakeLists.txt-simplesmartbody ./smartbody/src/simplesmartbody/CMakeLists.txt
cp build/CMakeLists.txt-vhcl ./smartbody/src/vhcl/CMakeLists.txt
cp build/CMakeLists.txt-vhmsg ./smartbody/src/vhmsg/CMakeLists.txt
cp build/CMakeLists.txt-bonebus ./smartbody/src/bonebus/CMakeLists.txt
cp build/CMakeLists.txt-steerlib ./smartbody/src/steerlib/CMakeLists.txt
cp build/CMakeLists.txt-pprAI ./smartbody/src/pprAI/CMakeLists.txt
cp build/CMakeLists.txt-polyvox ./smartbody/src/sbgui/external/polyvox/CMakeLists.txt
cp build/CMakeLists.txt-Pinocchio ./smartbody/src/sbgui/external/Pinocchio/CMakeLists.txt
cp build/CMakeLists.txt-ogresmartbody ./smartbody/src/ogresmartbody/CMakeLists.txt
cp build/CMakeLists.txt-irrlichtsmartbody ./smartbody/src/irrlichtsmartbody/CMakeLists.txt

mkdir ./smartbody/dependencies 
cp ../../dependencies/boost-numeric-bindings-20081116.tar.gz ./smartbody/dependencies
cp ../../dependencies/activemq-cpp-library-3.9.0-src.tar.gz ./smartbody/dependencies
cp ../../dependencies/ode-0.12.tar.gz ./smartbody/dependencies
cp ../../dependencies/protobuf-2.5.0.tar.gz ./smartbody/dependencies

# build script
cp ../../linuxlibsbuild.sh ./smartbody


# tar and gzip it up!
tar -cvzf SmartBodySDK-linux.tar.gz smartbody

