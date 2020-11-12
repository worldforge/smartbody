cd "$(dirname "$0")"
DYLD_LIBRARY_PATH=../lib:$DYLD_LIBRARY_PATH PYTHONHOME=/System/Library/Frameworks/Python.framework/Versions/2.7 ./simplesmartbody
