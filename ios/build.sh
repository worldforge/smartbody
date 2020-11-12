#! /bin/bash

set -e

pushd $(dirname "${0}")
SB_root_dir=$(pwd -L)
popd
SB_work_dir="${SB_root_dir}/build"
SB_downloads_dir="${SB_work_dir}"
SB_prefix_root_dir="${SB_work_dir}/output"
SB_patch_dir="${SB_root_dir}"

# Current iPhone SDK
: ${SB_IPHONE_SDK_VERSION:=`xcodebuild -showsdks | grep iphoneos | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
# Specific iPhone SDK
: ${SB_MIN_IOS_VERSION:=9.0}

# Current OSX SDK
: ${SB_OSX_SDK_VERSION:=`xcodebuild -showsdks | grep macosx | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
: ${SB_MIN_OSX_VERSION:=10.10}

: ${SB_XCODE_ROOT:=`xcode-select -print-path`}

SB_environments="iphoneos iphonesimulator"
#SB_environments="iphoneos"
#SB_environments="iphonesimulator"

function set_environment {
	sb_platform="$1"
	local extra_c_flags=""
	case "${sb_platform}" in
	"macosx")
	  sb_cap_platform="MacOSX"
		sb_host_flags="--host=x86-apple-darwin --build=x86_64-apple-darwin"
		sb_arch="x86_64"
		sb_sdk="${sb_cap_platform}${SB_OSX_SDK_VERSION}.sdk"
		sb_sys_root="$SB_XCODE_ROOT/Platforms/${sb_cap_platform}.platform/Developer/SDKs/${sb_sdk}"
		sb_version_flags=" -mmacosx-version-min=${SB_MIN_OSX_VERSION}"
		;;
	"iphoneos")
	  sb_cap_platform="iPhoneOS"
		sb_arch="arm64"
		sb_host_flags="--host=arm-apple-darwin --target=arm-apple-darwin --build=x86_64-apple-darwin"
		sb_sdk="${sb_cap_platform}${SB_IPHONE_SDK_VERSION}.sdk"
		sb_sys_root="$SB_XCODE_ROOT/Platforms/${sb_cap_platform}.platform/Developer/SDKs/${sb_sdk}"
		sb_version_flags=" -mios-version-min=${SB_MIN_IOS_VERSION}"
		extra_c_flags="$extra_c_flags -fembed-bitcode"
		;;
	"iphonesimulator")
	  sb_cap_platform="iPhoneSimulator"
		sb_arch="x86_64"
		sb_host_flags="--host=x86-apple-darwin --build=x86_64-apple-darwin"
		sb_sdk="${sb_cap_platform}${SB_IPHONE_SDK_VERSION}.sdk"
		sb_sys_root="$SB_XCODE_ROOT/Platforms/${sb_cap_platform}.platform/Developer/SDKs/${sb_sdk}"
		sb_version_flags=" -mios-version-min=${SB_MIN_IOS_VERSION}"
		;;
	esac

	sb_archs_list_flags="-arch ${sb_arch}"

	sb_prefix="${SB_prefix_root_dir}/${sb_platform}"
	local toolchain="$SB_XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain"
	CMAKE_XCODE_EFFECTIVE_PLATFORMS="-${sb_platform}"

	export CFLAGS=" -fvisibility-inlines-hidden ${sb_version_flags} ${extra_c_flags} -g -DNDEBUG -Os ${sb_archs_list_flags} -isysroot ${sb_sys_root} -I\"${sb_prefix}/include\" " # -I${SYSRT}/usr/include "
	export CC="$toolchain/usr/bin/clang"
	export CXX="$toolchain/usr/bin/clang++"
	export CPP="$CC -E"
	export CPPFLAGS="${CFLAGS}"
	export CXXFLAGS="${CFLAGS} -std=c++11 -stdlib=libc++"
	export LDFLAGS="${sb_archs_list_flags} -isysroot ${sb_sys_root} " #"-L\"${SYSRT}/usr/lib\"" # -L\"${SYSRT}/usr/lib/system\""
}

function _pushd {
  command pushd "$@" > /dev/null
}

function _popd {
  command popd "$@" > /dev/null
}

function _make_lib_wo_install {
	_pushd "$1"

	set +e
	make clean
	set -e

	if [ -f configure ]
		then chmod u+x configure
	fi

	if [ -f install-sh ]
		then chmod u+x install-sh
	fi

	echo "${sb_host_flags}"
	echo "${CFLAGS}"
	echo "${CPPFLAGS}"
	echo "$2"
	
	./configure --prefix="${sb_prefix}" ${sb_host_flags} $2

	make -j 16
	_popd
}

function make_lib {
	_make_lib_wo_install "$@"
	_pushd "$1"
	make install
	_popd
}

function _cmake_lib_wo_install {
	if [ -d build ]
	then
		rm -r build
	fi 
	mkdir build
	cd build
	echo ">>>> $@"
	echo "$LDFLAGS"
	echo "$CFLAGS"
	cmake -DCMAKE_MACOSX_RPATH="ON" -DCMAKE_OSX_DEPLOYMENT_TARGET="" \
		-DCMAKE_OSX_SYSROOT="$sb_sys_root" \
		-DCMAKE_INSTALL_PREFIX="$sb_prefix" \
		-DCMAKE_C_FLAGS="$CFLAGS" \
		-DCMAKE_CXX_FLAGS="$CXXFLAGS" \
		-DBUILD_SHARED_LIBS=OFF \
		-DCMAKE_LIBRARY_PATH_FLAG="$LDFLAGS" \
		-DCMAKE_XCODE_EFFECTIVE_PLATFORMS="$CMAKE_XCODE_EFFECTIVE_PLATFORMS" \
		$@ ..
	make
}

function _cmake_lib_w_install {
	_cmake_lib_wo_install "$@"
	make install
}

function cmake_lib {
	_pushd "$1"
	shift 
	_cmake_lib_w_install "$@"
	_popd
}

function _cmake_lib_no_install {
	_pushd "$1"
	shift 
	_cmake_lib_wo_install "$@"
	_popd
}

old_c_flags="$CFLAGS"
old_cxx_flags="$CXXFLAGS"

function push_cflags {
	old_c_flags="$CFLAGS"
	old_cxx_flags="$CXXFLAGS"
	export CFLAGS="$CFLAGS $@"
	export CXXFLAGS="$CXXFLAGS $@"
}

function pop_cflags {
	export CFLAGS="$old_c_flags"
	export CXXFLAGS="$old_cxx_flags"
}

old_ld_flags="$LDFLAGS"

function push_ldflags {
	old_ld_flags="$LDFLAGS"
	export LDFLAGS="$LDFLAGS $@"
}

function pop_ldflags {
	export LDFLAGS="$old_ld_flags"
}

old_path="$PATH"

function push_path {
	old_path="$PATH"
	export PATH="$@:$PATH"
}

function pop_path {
	export PATH="$old_path"
}

old_platform="$sb_platform"
function push_environment {
	old_platform="$sb_platform"
	set_environment "$1"
}

function pop_environment {
	set_environment "$old_platform"
}

function download {
  local url="$1"
  local filename=`basename "${url}"`
  if [ ! -e "${SB_downloads_dir}/${filename}" ]
  then
    mkdir -p "${SB_downloads_dir}"
    _pushd "${SB_downloads_dir}"
    curl -O -L "${url}"
    _popd
  fi
  echo "${SB_downloads_dir}/${filename}"
}

function uncompress {
  local filename="$1"
  local dirname="$2"
  local patch="$3"
  local target_dir="${SB_work_dir}/${dirname}"

  if [ ! -d "${target_dir}" ]
  then
    if [ ! -e "${filename}" ]
    then
      echo "${filename} does not exist"
      exit -1
    fi

    mkdir -p "${target_dir}"
    tar zxf "${filename}" --strip-components 1 -C "${target_dir}"

    if [ "${patch}" != "" ]
    then
      _pushd "${target_dir}"
      patch -p1 < "${patch}" 1>&2
      _popd
    fi
  fi
  echo "${target_dir}"
}

function backup_file {
  local path="$1"

	if [ ! -f "${path}" ]
	then
	  echo ""
	  return
	fi

	local index=1
	while [ -f "${path}.${index}.bak" ]
	do
	  index=`expr ${index} + 1`
	done
	local bak_path="${path}.${index}.bak"
	cp "${path}" "${bak_path}"
  echo "${bak_path}"
}

function restore_file {
  local path="$1"
  local bak_path="$2"
	if [ "${bak_path}" != "" -a -f "${bak_path}" ]
	then
	  mv "${bak_path}" "${path}"
	else
	  rm "${path}"
	fi
}

function make_xcode_project {
  local path="$1"
  #_pushd "${sb_root_dir}/steersuite-1.3/steerlib"
  xcodebuild -project "${path}" -configuration "Release" ${sb_archs_list_flags} -sdk ${sb_platform} install
  #_popd
}

#-------------------------------------------------------------------------------
# dependencies

# this is a bit complicated because apr generates a tool and then produces code 
# with it. If we do cross compile, the tool will be in arm and will not run. 
# so we build it first for macosx, save the tool, and build for arm. Once we 
# fail, we copy the tool and build again.
function make_apr {
  local APR_NAME="$(uncompress $(download "https://archive.apache.org/dist/apr/apr-1.6.2.tar.gz") "apr-1.6.2")"
	local APR_CONFIG_FLAGS=" --disable-shared --disable-dso --enable-threads ac_cv_file__dev_zero=yes ac_cv_func_setpgrp_void=yes apr_cv_process_shared_works=yes apr_cv_mutex_robust_shared=no apr_cv_tcp_nodelay_with_cork=yes ac_cv_sizeof_struct_iovec=8 apr_cv_mutex_recursive=yes"

  local native_gen_test_char="${SB_prefix_root_dir}/gen_test_char"

  if [ ! -e "${native_gen_test_char}" ]
  then
    push_environment "macosx"
    _make_lib_wo_install "${APR_NAME}" "${APR_CONFIG_FLAGS}"
    mkdir -p "${sb_prefix}"
    cp "${APR_NAME}/tools/gen_test_char" "${native_gen_test_char}"
    pop_environment
   fi

	_pushd "${APR_NAME}"

  set +e
  make clean
  set -e

  ./configure --prefix="${sb_prefix}" ${sb_host_flags} ${APR_CONFIG_FLAGS}

  set +e
  make -j 16
  set -e

  cp "${native_gen_test_char}" "tools/"
  rm "include/private/apr_escape_test_char.h"
  make -j 16
  make install

	_popd
}

function make_vhcl {
  rsync -raC "${SB_root_dir}/../lib/vhcl" "${SB_work_dir}"
  cp "${SB_root_dir}/vhcl-CMakeLists.txt" "${SB_work_dir}/vhcl/CMakeLists.txt"
  cmake_lib "${SB_work_dir}/vhcl"
}

function make_vhmsg {
  rsync -raC "${SB_root_dir}/../lib/vhmsg" "${SB_work_dir}"
  push_cflags " -I${sb_prefix}/include/activemq-cpp-3.9.4"
  cp "${SB_root_dir}/vhmsg-CMakeLists.txt" "${SB_work_dir}/vhmsg/vhmsg-c/CMakeLists.txt"
  cmake_lib "${SB_work_dir}/vhmsg/vhmsg-c" " -DBUILD_SHARED=OFF"
  pop_cflags
}

function make_protobuf {
	local PROTOBUF_NAME="$(uncompress $(download "https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.tar.gz") "protobuf-2.6.1" "${SB_patch_dir}/protobuf.patch")"
	local native_protoc="${SB_prefix_root_dir}/macosx/bin/protoc"

	if [ ! -e "${native_protoc}" ]
	then
    push_environment "macosx"
    make_lib "${PROTOBUF_NAME}" "--disable-shared"
    pop_environment
  fi

	make_lib "${PROTOBUF_NAME}" "--disable-shared --with-protoc=${native_protoc} "
}

function make_protocols {
	local native_protoc="${SB_prefix_root_dir}/macosx/bin/protoc"

	if [ ! -e "${native_protoc}" ]
	then
    make_protobuf
  fi

  local proto_dir="${SB_root_dir}/smartbody-lib/protocols"
  mkdir -p "${proto_dir}"
  _pushd "${SB_root_dir}/../core/smartbody/SmartBody/src/protocols"
  "${native_protoc}" --cpp_out="${proto_dir}" *.proto
  _popd
}

function make_boost {
  local boost_path="$(uncompress $(download "https://dl.bintray.com/boostorg/release/1.65.1/source/boost_1_65_1.tar.gz") "boost_1_65_1" )"
  local boost_nb_path="$(uncompress $(download "https://mathema.tician.de/dl/software/boost-numeric-bindings/boost-numeric-bindings-20081116.tar.gz") "boost_numeric_bindings" )"

  _pushd "${boost_path}"

	./bootstrap.sh --with-libraries="filesystem,python,system"
	
  local JAM_FILE="tools/build/src/user-config.jam"
  local JAM_BACKUP="$(backup_file "${JAM_FILE}")"

  push_cflags " -DBOOST_AC_USE_PTHREADS -DBOOST_SP_USE_PTHREADS "

  cat >> "${JAM_FILE}" <<EOF
using python : 2.7  : "${sb_prefix}" ;

using darwin : ${SB_IPHONE_SDK_VERSION}~iphone
: ${CXX} ${sb_archs_list_flags} -mios-version-min=${SB_MIN_IOS_VERSION} ${CXXFLAGS}
: <striper> <root>${SB_XCODE_ROOT}/Platforms/iPhoneOS.platform/Developer
: <architecture>arm <target-os>iphone
;
using darwin : ${SB_IPHONE_SDK_VERSION}~iphonesim
: ${CXX} ${sb_archs_list_flags} -mios-version-min=${SB_MIN_IOS_VERSION} ${CXXFLAGS}
: <striper> <root>${SB_XCODE_ROOT}/Platforms/iPhoneSimulator.platform/Developer
: <architecture>x86 <target-os>iphone
;
using darwin : ${SB_OSX_SDK_VERSION}
: ${CXX} ${sb_archs_list_flags} -mmacosx-version-min=${SB_MIN_OSX_VERSION} ${CXXFLAGS}
: <striper> <root>${SB_XCODE_ROOT}/Platforms/MacOSX.platform/Developer
: <architecture>x86 <target-os>darwin
;
EOF

  case "${sb_platform}" in
  "macosx")
    ;;
  "iphoneos")
    ./b2 -a -j16 --build-dir="build" --stagedir="build/stage" --prefix=${sb_prefix} toolset=darwin cxxflags="${CXXFLAGS}" architecture=arm address-model=64 target-os=iphone macosx-version=iphone-${SB_IPHONE_SDK_VERSION} define=_LITTLE_ENDIAN link=static install
    ;;
  "iphonesimulator")
    ./b2 -a -j16 --build-dir="build" --stagedir="build/stage" --prefix=${sb_prefix} toolset=darwin-${SB_IPHONE_SDK_VERSION}~iphonesim cxxflags="${CXXFLAGS}" architecture=x86 address-model=64 target-os=iphone macosx-version=iphonesim-${SB_IPHONE_SDK_VERSION} link=static install
    ;;
  esac

  rsync -r "${boost_nb_path}/boost" "${sb_prefix}/include"

  restore_file "${JAM_FILE}" "${JAM_BACKUP}"
  pop_cflags

  _popd
}

function make_python {
  local PYTHON_VERSION="2.7.13"
	local PYTHON_NAME="$(uncompress $(download "https://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tgz") "Python-${PYTHON_VERSION}" "${SB_patch_dir}/python.patch")"
	cp -f "${SB_patch_dir}/Setup.embedded" "${PYTHON_NAME}/Modules/Setup.embedded"

	local native_python="${SB_prefix_root_dir}/macosx/bin/python"

	if [ ! -e "${native_python}" ]
	then
    push_environment "macosx"
    sb_host_flags=""
    make_lib "${PYTHON_NAME}" " --disable-shared --disable-toolbox-glue --disable-universalsdk --disable-framework --without-ensurepip --without-pymalloc --without-doc-strings --disable-ipv6"
    pop_environment
  fi

  case "${sb_platform}" in
  "iphoneos")
    sb_host_flags="--host=aarch64-apple-ios --build=x86_64-apple-darwin"
    ;;
  "iphonesimulator")
    sb_host_flags="--host=x86_64-apple-ios --build=x86_64-apple-darwin"
    ;;
  esac

  push_path "${SB_prefix_root_dir}/macosx/bin"
  push_cflags " -DTARGET_OS_IPHONE=1"
  push_ldflags " -L${sb_prefix}/lib -l expat "
  rm -f "${PYTHON_NAME}/Modules/Setup.local"
  make_lib "${PYTHON_NAME}" "  --disable-shared --disable-toolbox-glue --disable-universalsdk --disable-framework --without-ensurepip ac_cv_file__dev_ptmx=no ac_cv_file__dev_ptc=no --without-pymalloc --without-doc-strings --disable-ipv6  "
  pop_ldflags
  pop_cflags
  pop_path
}

function make_freealut {
  cmake_lib "$(uncompress $(download "https://github.com/vancegroup/freealut/archive/freealut_1_1_0.tar.gz") "freealut_1_1_0" "${SB_patch_dir}/freealut.patch")" "-DBUILD_STATIC=ON -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF"
  rm "${sb_prefix}/lib/libalut.0.dylib"
  rm "${sb_prefix}/lib/libalut.dylib"
}

for environment in ${SB_environments}
do
  set_environment ${environment}

  make_apr
  cmake_lib "$(uncompress $(download "https://github.com/libexpat/libexpat/archive/R_2_2_4.tar.gz") "libexpat-R_2_2_4")/expat" "-DBUILD_tests=OFF -DBUILD_shared=OFF"
  make_lib "$(uncompress $(download "https://archive.apache.org/dist/apr/apr-util-1.6.0.tar.gz") "apr-util-1.6.0")" "--with-apr=${sb_prefix} --with-expat=${sb_prefix}"
  make_lib "$(uncompress $(download "http://www-us.apache.org/dist/activemq/activemq-cpp/3.9.4/activemq-cpp-library-3.9.4-src.tar.gz") "activemq-cpp-library-3.9.4" "${SB_patch_dir}/activemq.patch" )"  "--disable-ssl --with-apr=${sb_prefix} --enable-shared=no"
  make_lib "$(uncompress $(download "http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.25.tar.gz") "libsndfile-1.0.25" "${SB_patch_dir}/sndfile.patch")" "--disable-shared --disable-sqlite --disable-alsa --disable-external-libs "
  make_freealut
  make_vhcl
  make_vhmsg
  make_lib "$(uncompress $(download "http://archive.apache.org/dist/xerces/c/3/sources/xerces-c-3.1.4.tar.gz") "xerces-c-3.1.4" "${SB_patch_dir}/xerces.patch")"  "--disable-shared --enable-msgloader-inmemory"
  make_protobuf
  make_protocols
  make_python
  make_boost
  make_xcode_project "${SB_root_dir}/steersuite-1.3/steerlib/SteerLib.xcodeproj"
  make_xcode_project "${SB_root_dir}/steersuite-1.3/pprAI/pprAI.xcodeproj"

done
