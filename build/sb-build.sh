#!/bin/sh

python sb-automated-build-prepare.py svn://svn.code.sf.net/p/smartbody/code/trunk/build

python scripts/sb-automated-build.py build password -mac-ci nospam@ict.usc.edu
