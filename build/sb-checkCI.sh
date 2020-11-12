#!/bin/sh

python sb-automated-build-prepare.py svn://svn.code.sf.net/p/smartbody/code/trunk/build

python scripts/sb-automated-build.py checktime svn://svn.code.sf.net/p/smartbody/code/trunk user password 200 d: d:/VHToolkit-Builds -mac-ci 72 nospam@ict.usc.edu
