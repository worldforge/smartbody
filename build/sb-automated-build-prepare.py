
import ctypes
import email.mime.text
import errno
import fnmatch
import os
import platform
import shutil
import smtplib
import stat
import subprocess
import sys
import time


def prepareBuildFolder(svnUrl):
    scriptsFolder = "scripts"
    if os.path.exists(scriptsFolder):
        subprocess.call("svn update {0}".format(scriptsFolder).split(" "))
    else:
        subprocess.call("svn co {0} {1}".format(svnUrl, scriptsFolder).split(" "))


def printUsage():
    print ""
    print "Usage:  python create-build-prepare.py [svnUrl]"


##############################################################


if len(sys.argv) <= 1:
    printUsage()
else:
    svnUrl = sys.argv[1]
    prepareBuildFolder(svnUrl)
