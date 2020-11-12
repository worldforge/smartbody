
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


timeToForceBuildIfRunning = 24 * 60 * 60  # in hours * 60 * 60


def getFreeSpace(folder):

    # Return folder/drive free space (in bytes)

    if platform.system() == 'Windows':
        freeBytes = ctypes.c_ulonglong(0)
        ctypes.windll.kernel32.GetDiskFreeSpaceExW(ctypes.c_wchar_p(folder), None, None, ctypes.pointer(freeBytes))
        return freeBytes.value
    else:
        s = os.statvfs(folder)
        return s.f_bavail * s.f_bsize


def getLowestBuildDir(baseFolder, postfix):

    # find the folder with the lowest build number which also has the matching postfix

    lowestbuild = 999999
    lowestbuilddir = ""

    for dir in os.listdir(baseFolder):
        if os.path.isdir(os.path.join(baseFolder, dir)):
            m = re.search('-{0}\Z'.format(postfix), dir)
            if m is None:
               continue;

            m = re.search('\S*Build(\d+)-\S+', dir)
            if m is not None:
                buildnum = int(m.group(1))
                if buildnum < lowestbuild:
                    lowestbuild = buildnum
                    lowestbuilddir = dir

    return lowestbuilddir


def findSVNInfo(svnHost, svnUser, svnPassword, svnInfo):

    # run svn info, looking for the specified line of info and return it

    p = subprocess.Popen("svn info --non-interactive --username {0} --password {1} {2}".format(svnUser, svnPassword, svnHost).split(" "), stdout=subprocess.PIPE)
    svnOutput = []
    for line in p.stdout:
        svnOutput.append(line.strip())
    p.wait()

    for line in svnOutput:
        if (line.startswith(svnInfo)):
            return line[len(svnInfo):]

    return ""


def handleRemoveReadonly(func, path, exc):

    # needed for rmtree()

    excvalue = exc[1]
    if func in (os.rmdir, os.remove) and excvalue.errno == errno.EACCES:
        os.chmod(path, stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO) # 0777
        func(path)
    else:
        raise


def intWithCommas(x):

    # http://stackoverflow.com/questions/1823058/how-to-print-number-with-commas-as-thousands-separators-in-python-2-x

    if type(x) not in [type(0), type(0L)]:
        raise TypeError("Parameter must be an integer.")
    if x < 0:
        return '-' + intWithCommas(-x)
    result = ''
    while x >= 1000:
        x, r = divmod(x, 1000)
        result = ",%03d%s" % (r, result)
    return "%d%s" % (x, result)


def handleRemoveReadonly(func, path, exc):

    # needed for rmtree()

    excvalue = exc[1]
    if func in (os.rmdir, os.remove) and excvalue.errno == errno.EACCES:
        os.chmod(path, stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO) # 0777
        func(path)
    else:
        raise



def deleteDir(folder, excludes=None):
    # if no exclusion set, shortcut the slow method
    if excludes is None:
        if os.path.exists(folder):
            shutil.rmtree(folder, onerror=handleRemoveReadonly)
        return

    # delete the file unless it matches the exclusion
    for base, dirs, files in os.walk(folder):
        for file in files:
            if not fnmatch.fnmatch(file, excludes):
                os.remove(os.path.join(base, file))


    # delete empty folders  (horribly inefficient)
    folderFound = True
    while folderFound:
        folderFound = False
        for base, dirs, files in os.walk(folder):
            if len(dirs) == 0 and \
               len(files) == 0:
                os.rmdir(base)

                folderFound = True


def deleteFiles(folder, wildcard):
    # recursively delete all files that match the wildcard
    for base, dirs, files in os.walk(folder):
        for file in files:
            if fnmatch.fnmatch(file, wildcard):
                os.remove(os.path.join(base, file))


def checkBuildRunning():
    # Check to see if a build is already running
    if os.path.exists("BUILD_RUNNING"):
        print "Build is already running.\n";
        if time.time() - os.path.getctime("BUILD_RUNNING") > timeToForceBuildIfRunning:
            print "Build has been running for too long, forcing build."
            os.remove("BUILD_RUNNING")
        else:
            print "  Exiting..."
            sys.exit()


def cleanBuild():
    # remove the exported build folder on fresh builds
    if os.path.exists("build"):
        print "--- Removing 'build' folder"
        shutil.rmtree("build", onerror=handleRemoveReadonly)

    # revert modified files, delete unversioned and ignored files when doing incremental builds
    if os.path.exists("build.sandbox"):
        print "--- Cleaning build.sandbox folder"
        if platform.system() == "Windows":
            p = subprocess.Popen("tortoiseproc /path:build.sandbox /command:cleanup /noui /noprogressui /nodlg /revert /delunversioned /delignored /externals".split(" "), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            cleanSandboxOutput = []
            for line in p.stdout:
                cleanSandboxOutput.append(line.strip())
            p.wait()
        else:
            # remove unversioned/ignored files
            # search for '?' (unversioned), trim the beginning of the line, then call rm on the file/folder
            cleanSandboxOutput = []
            p = subprocess.Popen("svn status --no-ignore build.sandbox | grep '^\?' | sed 's/^\?       //'  | xargs -Ixx rm -rf xx", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            for line in p.stdout:
                cleanSandboxOutput.append(line.strip())
            p.wait()

            # search for 'I' (ignored), trim the beginning of the line, then call rm on the file/folder
            p = subprocess.Popen("svn status --no-ignore build.sandbox | grep '^\I' | sed 's/^\I       //'  | xargs -Ixx rm -rf xx", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            for line in p.stdout:
                cleanSandboxOutput.append(line.strip())
            p.wait()

            # revert everything in sandbox, doesn't do externals, see http://subversion.tigris.org/issues/show_bug.cgi?id=3824 for future fix
            p = subprocess.Popen("svn revert -R build.sandbox", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            for line in p.stdout:
                cleanSandboxOutput.append(line.strip())
            p.wait()

            # revert everything in externals
            # search for: Performing status on external item at '...'
            # trim the beginning up to the first quote, trim the end quote, then call svn revert on the folder
            p = subprocess.Popen("for i in $(svn status build.sandbox | grep ^Perf | cut -c40-); do echo $i | sed '$s/.$//' | xargs -Ixx svn revert -R xx ; done", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            for line in p.stdout:
                cleanSandboxOutput.append(line.strip())
            p.wait()


def cleanSandbox():
    # separate command that removes build.sandbox to do a clean checkout.
    # not called by the main build process. 

    checkBuildRunning()

    cleanBuild()

    print "--- Removing build.sandbox folder to make way for a clean checkout"

    if os.path.exists("build.sandbox"):
        shutil.rmtree("build.sandbox", onerror=handleRemoveReadonly)


def getBuildFolderName(buildNumber, buildDate, doFreshBuild, buildSuffix, buildSuccess, destinationFolder):

    # ex. Build42-04-12-2010-ci-failed

    buildFolderName = "Build{0}-{1}".format(buildNumber, buildDate)
    if not doFreshBuild:
        buildFolderName = buildFolderName + "-inc"

    buildFolderName = buildFolderName + buildSuffix

    if not buildSuccess:
        buildFolderName = buildFolderName + "-failed"

    if doFreshBuild:
        buildFolder = os.path.join(destinationFolder, buildFolderName)
    else:
        buildFolder = os.path.join(destinationFolder, os.path.join("incremental", buildFolderName))

    return buildFolderName, buildFolder


def isError(line):
    #  ": warning "
    #  ": error "
    #  ": fatal error "
    #  ":  warning :"   # mono
    #  ": warning: "   # gcc
    #  ": error: "     # gcc
    #  ": fatal error: "  # gcc
    #  "make[X]: *** " ... "Error "  # gcc
    #  "make: *** " ... "Error "  # gcc
    #  "CMake Error"  # cmake
    #  "(beginning of line) "error: " or "warning: "  # xcode
    #  "[BEROR]"   # xcode
    #  "Android.mk:427: *** commands commence before first target.  Stop."   # android gcc
    #  "Error building player "   # unity
    #  " ... foobar.java:218: warning: ... is internal proprietary API and may be removed in a future release  # java
    #  " ... : warning CS0419: Ambiguous reference in cref attribute ...  # mono - fixed in newer version.  ubuntu isn't updated yet
    #  "ld: warning: CPU_SUBTYPE_ARM_ALL subtype is deprecated:"  # ios warning - need to recompile 3rd party dependencies
    #  "means the weak symbol cannot be overridden at runtime. This was likely caused by different translation units being compiled with different visibility settings."  # ios - 3rd party
    #  "ld: warning: 32-bit absolute address out of range ("  # ios - unknown
    #  "doesn't contain architecture information for armv7."  # ios - need to recompile 3rd party dependencies

    if ": error " in line or \
       ": fatal error " in line or \
       ": error: " in line or \
       ": fatal error: " in line or \
       (line.startswith("make[") and "Error " in line) or \
       (line.startswith("make: *** ") and "Error " in line) or \
       "CMake Error" in line or \
       line.startswith("error: ") or \
       "[BEROR]" in line or \
       ("***" in line and "Stop." in line) or \
       "Error building player " in line:
        return True

    return False


def isWarning(line):
    if ": warning " in line or \
       ": warning: " in line or \
       ":  warning :" in line or \
       line.startswith("warning: "):

        # exceptions
        if ": (not reported) : warning:" in line or \
           "is internal proprietary API and may be removed in a future release" in line or \
           "warning CS0419: Ambiguous reference in cref attribute" in line or \
           "ld: warning: CPU_SUBTYPE_ARM_ALL subtype is deprecated:" in line or \
           "means the weak symbol cannot be overridden at runtime. This was likely caused by different translation units being compiled with different visibility settings." in line or \
           "ld: warning: 32-bit absolute address out of range (" in line or \
           "doesn't contain architecture information for armv7." in line:
            return False

        return True

    return False


def makeDist(distLocation):
    # This builds a distribution, not in the traditional sense, but rather takes a build and removes unneeded files to produce a 'clean' build.

    print "--- Removing source and extra data from: {0}".format(distLocation)

    deleteDir(os.path.join(distLocation, "android/vh_wrapper/obj/local/armeabi/objs"))
    deleteDir(os.path.join(distLocation, "core/FestivalRelay/Release"))
    deleteDir(os.path.join(distLocation, "core/ogre-viewer/build/vs2008/Debug"))
    deleteDir(os.path.join(distLocation, "core/ogre-viewer/build/vs2008/Release"))
    deleteDir(os.path.join(distLocation, "core/sbmonitor/gui/Debug"))
    deleteDir(os.path.join(distLocation, "core/sbmonitor/gui/Release"))
    deleteDir(os.path.join(distLocation, "core/smartbody/sbgui/visualc9/obj"))
    deleteDir(os.path.join(distLocation, "core/smartbody/SmartBody/Debug"))
    deleteDir(os.path.join(distLocation, "core/smartbody/SmartBody/Release"))
    deleteDir(os.path.join(distLocation, "core/smartbody/steersuite-1.3/steerlib/build/win32/Debug"))
    deleteDir(os.path.join(distLocation, "core/smartbody/steersuite-1.3/steerlib/build/win32/Release"))
    deleteDir(os.path.join(distLocation, "core/smartbody/steersuite-1.3/pprAI/build/win32/Debug"))
    deleteDir(os.path.join(distLocation, "core/smartbody/steersuite-1.3/pprAI/build/win32/Release"))
    deleteDir(os.path.join(distLocation, "core/smartbody/steersuite-1.3/build/win32/Debug"))
    deleteDir(os.path.join(distLocation, "core/smartbody/steersuite-1.3/build/win32/Release"))
    deleteDir(os.path.join(distLocation, "core/TtsRelay/FestivalDll/obj"))
    deleteDir(os.path.join(distLocation, "lib/qt/qt"))
    deleteDir(os.path.join(distLocation, "tools/elsender/bin"))
    deleteDir(os.path.join(distLocation, "tools/elsender/obj"))


def fullBuild(svnPassword, buildSuffix, doFreshBuild, emailTo):

    """
    This script performs the following tasks:
        - check to see if a previous build is running, and quits if so
        - create a timestamp file to indicate when this build has run
        - svn checkout (or update) from repository to sandbox directory
        - export sandbox to build directory
        - compile project
        - move the build to final destination
        - gather directory statistics
        - filter revision numbers from svn output
        - filter errors and warnings from compiler output
        - generate email report
        - tag repository with email report
        - generate build output
    """


    svnUsername = "iabuild"
    destinationFolder = ""
    if platform.system() == "Windows":
        destinationFolder = "D:/SBM-Builds"
    elif platform.system() == "Darwin":
        destinationFolder = "/Users/fast/build/sbm-builds"
    else:
        destinationFolder = "/data/build/sbm-builds"
    tagSvn = False
    emailReport = True


    totalBuildTime = time.time()


    checkBuildRunning()

    # Create the build running file
    open("BUILD_RUNNING", "w").close()


    cleanTime = time.time()

    cleanBuild()

    cleanTime = time.time() - cleanTime


    # update the timestamp file
    f = open("BUILD_TIME", "w")
    f.write(time.strftime("%Y-%m-%d %H:%M:%S"))
    f.close()

    if doFreshBuild:
        f = open("BUILD_FRESH", "w")
        f.write(time.strftime("%Y-%m-%d %H:%M:%S"))
        f.close()

    # checkout/update build from svn into sandbox
    print "--- Starting build.svn.checkout..."

    buildSvnTime = time.time()

    buildSvnCleanCheckout = False

    if os.path.exists("build.sandbox"):
        p = subprocess.Popen("svn update --non-interactive --username {0} --password {1} build.sandbox".format(svnUsername, svnPassword).split(" "), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        buildSvnOutput = []
        for line in p.stdout:
            buildSvnOutput.append(line.strip())
        p.wait()
    else:
        p = subprocess.Popen("svn checkout --non-interactive --username {0} --password {1} svn://svn.code.sf.net/p/smartbody/code/trunk build.sandbox".format(svnUsername, svnPassword).split(" "), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        buildSvnOutput = []
        for line in p.stdout:
            buildSvnOutput.append(line.strip())
        p.wait()

        buildSvnCleanCheckout = True

    buildSvnTime = time.time() - buildSvnTime



    print "--- Starting build.export..."

    buildExportTime = time.time()

    if doFreshBuild:
        p = subprocess.Popen("svn export --non-interactive --username {0} --password {1} build.sandbox build".format(svnUsername, svnPassword).split(" "), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        buildSvnExportOutput = []
        for line in p.stdout:
            buildSvnExportOutput.append(line.strip())
        p.wait()
    else:
        buildSvnExportOutput = []

    buildExportTime = time.time() - buildExportTime



    print "--- Starting build.compile..."

    buildCompileTime = time.time()

    currentWorkingDir = os.getcwd()

    if doFreshBuild:
        buildDir = "build"
    else:
        buildDir = "build.sandbox"

    p = None
    if platform.system() == "Windows":
        os.chdir(buildDir)
        p = subprocess.Popen("compile-sbm.bat", stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    elif platform.system() == "Darwin":
        os.chdir(buildDir)
        p = subprocess.Popen("./build/sb-compile-osx.sh", stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    else:
        os.chdir(buildDir)
        p = subprocess.Popen("./build/sb-compile-nix.sh", stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    buildCompileOutput = []
    for line in p.stdout:
        buildCompileOutput.append(line.strip())
    p.wait()
    os.chdir(currentWorkingDir)

    buildCompileTime = time.time() - buildCompileTime



    # filter out revision numbers from svn output

    print "--- Analyzing output..."

    buildSvnRevisions = []
    for line in buildSvnOutput:
        # update vs checkout.  the output is different

        if line.startswith("Checked out revision ") or        \
           line.startswith("Checked out external at "):
           buildSvnRevisions.append("   " + line)

        if line.startswith("Updated external to revision ") or \
           line.startswith("Fetching external item into ") or  \
           line.startswith("Updated to revision ") or          \
           line.startswith("External at revision ") or         \
           line.startswith("At revision "):
           buildSvnRevisions.append("   " + line)


    # filter the final revision number

    buildSvnRevision = buildSvnRevisions[len(buildSvnRevisions) - 1]
    buildSvnRevision = ''.join(i for i in buildSvnRevision if i.isdigit())  # only grab the digits on this line
    buildSvnRevision = int(buildSvnRevision)

    print "Build revision: {0}".format(buildSvnRevision)


    # filter out compiler errors and warnings


    buildCompileErrors = []
    buildCompileWarnings = []
    for line in buildCompileOutput:
        if isError(line):
            buildCompileErrors.append("   " + line)

        if isWarning(line):
            buildCompileWarnings.append("   " + line)


    # determine if build succeeded by looking for compiler errors

    if len(buildCompileErrors) == 0:
        buildSuccess = True
        print "Build Success."
    else:
        buildSuccess = False
        print "Build Failed."


    buildDate = time.strftime("%m-%d-%Y")

    print "date: {0}".format(buildDate)


    buildNumber = buildSvnRevision

    buildFolderName, buildFolder = getBuildFolderName(buildNumber, buildDate, doFreshBuild, buildSuffix, buildSuccess, destinationFolder)

    # check for duplicate destination folder since we're using revision number as build number
    while os.path.exists(buildFolder):
        c = str(buildNumber)[-1:]
        if c.isalpha():
            buildNumber = str(buildNumber[:-1]) + chr(ord(c) + 1)
        else:
            buildNumber = str(buildNumber) + "a"

        buildFolderName, buildFolder = getBuildFolderName(buildNumber, buildDate, doFreshBuild, buildSuffix, buildSuccess, destinationFolder)


    print "build: {0}".format(buildNumber)
    print "buildFolderName: {0}".format(buildFolderName)
    print "buildFolder: {0}".format(buildFolder)


    makeDistTime = time.time()

    # prune out files that aren't needed
    if doFreshBuild:
        distLocation = "build"
        makeDist(distLocation)

    makeDistTime = time.time() - makeDistTime


    moveTime = time.time()

    # move build to its destination folder
    os.makedirs(buildFolder)
    if doFreshBuild:
        zipFolderName = 'smartbody'

        if platform.system() == "Windows":
            zipExe = "./{0}/build/zip".format(zipFolderName)
        else:
            zipExe = "zip"

        try:
            print "--- Renaming build to prepare for zipping - {0} {1}".format(buildDir, zipFolderName)
            os.rename(buildDir, zipFolderName)

            print "--- Zipping build"
            p = subprocess.Popen("{0} -r {1} {2}".format(zipExe, zipFolderName, zipFolderName).split(" "), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            for line in p.stdout:
                buildCompileOutput.append(line.strip())
            p.wait()

            print "--- Moving build to folder: {0}".format(buildFolder)
            shutil.move("{0}.zip".format(zipFolderName), buildFolder)

            print "--- Renaming build back to original"
            os.rename(zipFolderName, buildDir)

        except:
            print "--- FAILED to zip and move destination"
            buildSuccess = False    # should also change the folder name, but that's done earlier in the process.

    moveTime = time.time() - moveTime


    print "--- Starting directory statistics..."
    dirSizeTotal = 0
    numFilesTotal = 0
    numDirsTotal = 0
    for dirpath, dirnames, filenames in os.walk(buildFolder):
        for filename in filenames:
            dirSizeTotal += os.path.getsize(os.path.join(dirpath, filename))
            numFilesTotal += 1

        for dirname in dirnames:
            numDirsTotal += 1

    print "size:  {0}".format(dirSizeTotal)
    print "files: {0}".format(numFilesTotal)
    print "dirs:  {0}".format(numDirsTotal)


    totalBuildTime = time.time() - totalBuildTime


    totalBuildTime_t   = time.gmtime(totalBuildTime)
    cleanTime_t        = time.gmtime(cleanTime)
    buildSvnTime_t     = time.gmtime(buildSvnTime)
    buildExportTime_t  = time.gmtime(buildExportTime)
    buildCompileTime_t = time.gmtime(buildCompileTime)
    makeDistTime_t     = time.gmtime(makeDistTime)
    moveTime_t         = time.gmtime(moveTime)


    # generate output to be used in email report

    finalMailFile = os.path.join(buildFolder, "FinalMail.txt")

    buildServerShareName = ""
    if platform.system() == "Windows":
        buildServerShareName = "\\\\vhbuild\\SBM-builds\\"
        emailSubjectPrefix = "[SB]"
    elif platform.system() == "Darwin":
        buildServerShareName = "\\\\vhbuildmac\\sbm-builds\\"
        emailSubjectPrefix = "[SB-MAC]"
    else:
        buildServerShareName = "\\\\vhbuild3\\sbm-builds\\"
        emailSubjectPrefix = "[SB-NIX]"

    if not doFreshBuild:
        buildServerShareName = buildServerShareName + "incremental\\"

    f = open(finalMailFile,"w")
    f.write("Build #{0} - {1}\n".format(buildNumber, "Fresh Build" if doFreshBuild else "Incremental Build"))
    f.write("\n")
    f.write("Build Summary:\n")
    if buildSuccess:
        f.write("   Build Success.\n")
    else:
        f.write("   Build FAILED!!\n")
        f.write("   Log: {0}{1}{2}\n".format(buildServerShareName, buildFolderName, "\\BuildLog.txt"))
    f.write("\n")
    f.write("Broken projects:\n")
    if len(buildCompileErrors) > 0:
        f.write("   {0} error(s)\n".format(len(buildCompileErrors)))
    for line in buildCompileErrors:
        f.write(line + "\n")
    f.write("\n")
    f.write("Dirty projects:\n")
    if len(buildCompileWarnings) > 0:
        f.write("   {0} warning(s)\n".format(len(buildCompileWarnings)))
    for line in buildCompileWarnings:
        f.write(line + "\n")
    f.write("\n")
    f.write("\n")
    f.write("Build {0} took {1}\n".format(buildNumber, time.strftime("%X", totalBuildTime_t)))
    f.write("   cleanTime {0}\n".format(time.strftime("%X", cleanTime_t)))
    f.write("   buildSvnTime {0}\n".format(time.strftime("%X", buildSvnTime_t)))
    f.write("   buildExportTime {0}\n".format(time.strftime("%X", buildExportTime_t)))
    f.write("   buildCompileTime {0}\n".format(time.strftime("%X", buildCompileTime_t)))
    f.write("   makeDistTime {0}\n".format(time.strftime("%X", makeDistTime_t)))
    f.write("   moveTime {0}\n".format(time.strftime("%X", moveTime_t)))
    f.write("\n")
    f.write("Build size: {0} ({1} files, {2} dirs)\n".format(intWithCommas(dirSizeTotal), intWithCommas(numFilesTotal), intWithCommas(numDirsTotal)))
    f.write("\n")
    if doFreshBuild:
        f.write("Build is available at {0}{1}\n".format(buildServerShareName, buildFolderName))
        f.write("\n")
    f.write("Log: {0}{1}{2}\n".format(buildServerShareName, buildFolderName, "\\BuildLog.txt"))
    f.write("\n")
    f.write("Revisions used:\n")
    f.write("sb revisions (based off of r{0}):\n".format(buildSvnRevision))
    for line in buildSvnRevisions:
        f.write(line + "\n")
    f.close()


    # tag the repository with the email report
    if tagSvn:
        p = subprocess.Popen("svn copy --non-interactive --username {0} --password {1} -r {2} {3} {4}/{5} -F ""{6}""".format(svnUsername, svnPassword, buildSvnRevision, "svn://svn.code.sf.net/p/smartbody/code/trunk", "svn://svn.code.sf.net/p/smartbody/code/tags/builds", buildFolderName, finalMailFile).split(" "))
        p.wait()

    if emailReport:
        fp = open(finalMailFile, 'r')
        msg = email.mime.text.MIMEText(fp.read())
        fp.close()

        # emailTo is passed in as a parameter to this main function up above
        msg["Subject"] = "{0} Build Results #{1}".format(emailSubjectPrefix, buildNumber)
        msg["From"] = "svn@svn.ict.usc.edu"
        msg["To"] = emailTo

        s = smtplib.SMTP("smtp.ict.usc.edu")
        s.sendmail("svn@ict.usc.edu", emailTo.split(";"), msg.as_string())
        s.quit()


    ## os.remove(finalMailFile)


    # generate output for log file
    buildLogFile = os.path.join(buildFolder, "BuildLog.txt")

    f = open(buildLogFile,"w")
    f.write("------------------\n")
    f.write("\n")
    for line in buildSvnOutput:
        f.write(line + "\n")
    f.write("\n")
    f.write("\n")
    f.write("------------------\n")
    f.write("\n")
    for line in buildSvnExportOutput:
        f.write(line + "\n")
    f.write("\n")
    f.write("\n")
    f.write("------------------\n")
    f.write("\n")
    for line in buildCompileOutput:
        f.write(line + "\n")
    f.write("\n")
    f.write("\n")
    f.write("------------------\n")


    if os.path.exists("BUILD_RUNNING"):
        os.remove("BUILD_RUNNING")




def checkIfTimeForBuild(svnURL, svnUser, svnPassword, freeUpSpace, minFreeSpaceRequiredForBuildGig, buildDrive, buildOutputFolder, buildSuffix, hoursBetweenFreshBuild, emailTo):

    """
    This function:

    - checks to see if BUILD_RUNNING file exists, exits if so
    - gets the date/time of the last build from BUILD_TIME file
    - runs 'svn info' to get the date/time of the last commit
    - figures out if it's time to run a new build based on the date/times
    - if there isn't enough disk space, deletes folders ending with -ci, starting with the oldest build
    - runs the given build script

    """

    checkBuildRunning()


    # Get time of last build
    if os.path.exists("BUILD_TIME"):
        f = open("BUILD_TIME","r")
        lastBuildDate = f.read()
        f.close()
    else:
        lastBuildDate = "1980-01-01 00:00:01"

    if os.path.exists("BUILD_FRESH"):
        f = open("BUILD_FRESH","r")
        lastFreshBuildDate = f.read()
        f.close()
    else:
        lastFreshBuildDate = "1980-01-01 00:00:01"


    # Get time of last revision from the repository
    lastChangedDate = findSVNInfo( svnURL, svnUser, svnPassword, "Last Changed Date: " )[0:19]

    lastBuildDate_t = time.strptime(lastBuildDate, "%Y-%m-%d %H:%M:%S")
    lastFreshBuildDate_t = time.strptime(lastFreshBuildDate, "%Y-%m-%d %H:%M:%S")
    lastChangedDate_t = time.strptime(lastChangedDate, "%Y-%m-%d %H:%M:%S")


    print "Last Build Date:       {0}".format(time.strftime("%c", lastBuildDate_t))
    print "Last Fresh Build Date: {0}".format(time.strftime("%c", lastFreshBuildDate_t))
    print "Last Changed Date:     {0}".format(time.strftime("%c", lastChangedDate_t))
    print "Current Time:          {0}".format(time.strftime("%c", time.localtime()))


    # figure out if it's time to run a build

    # if recent commit happened, skip build
    # if commit is newer than last build, run build
    # if last fresh build happened longer than wait time, run build and run fresh build

    timeToWaitSinceLastCommit    = 5 * 60   # in minutes * 60
    timeSinceLastBuild           = 10       # in seconds

    doBuild = False
    doFreshBuild = False

    if time.mktime(time.localtime()) - time.mktime(lastChangedDate_t) < timeToWaitSinceLastCommit:
        print "Too soon since last commit"
    else:
        if time.mktime(lastChangedDate_t) - time.mktime(lastBuildDate_t) > timeSinceLastBuild:
            doBuild = True
        else:
            print "Not running Incremental build because there isn't a newer commit"


    hoursBetweenFreshBuildSecs = hoursBetweenFreshBuild * 60 * 60;

    if time.mktime(lastChangedDate_t) - time.mktime(lastFreshBuildDate_t) > timeSinceLastBuild:
        if time.mktime(time.localtime()) - time.mktime(lastFreshBuildDate_t) > hoursBetweenFreshBuildSecs:
            doBuild = True
            doFreshBuild = True
        else:
            print "Not running Fresh build because not enough time since last Fresh build"
    else:
        print "Not running Fresh build because there isn't a newer commit"


    if not doBuild:
        sys.exit()


    if doFreshBuild:
        print "Time to run a new Fresh build"
    else:
        print "Time to run a new Incremental build"


    if doFreshBuild:
        # see how much disk space is left
        # find all dirs that end with -ci
        # find lowest build num
        # delete build
        # loop until enough space is left

        if freeUpSpace:
            if platform.system() == "Windows":
                minFreeSpaceRequiredForBuild = minFreeSpaceRequiredForBuildGig * 1000 * 1000 * 1000;  # ~gigs

                while getFreeSpace(buildDrive) < minFreeSpaceRequiredForBuild:
                    lowestBuildDir = getLowestBuildDir(buildOutputFolder, "ci")

                    if lowestBuildDir == "":
                         print "No CI build dirs found.  Can't delete enough hard disk space.\n"
                         sys.exit()

                    lowestBuildFullPath = os.path.join(buildOutputFolder, lowestBuildDir)

                    print "Removing folder {0} to free up disk space".format(lowestBuildFullPath)
                    shutil.rmtree(lowestBuildFullPath, onerror=handleRemoveReadonly)


        print "Disk has enough space for build"


    # start the build
    fullBuild(svnPassword, buildSuffix, doFreshBuild, emailTo)


def printUsage():
    print ""
    print "Usage:  python create-build.py [task] [args]"
    print "   Where task is:"
    print "     python create-build.py build [svnPassword] [buildSuffix] [emailTo]"
    print "            # performs the full build, both arguments optional, default to \"\""
    print "     python create-build.py cleansandbox"
    print "            # wipes out the svn sandbox to do a fresh checkout"
    print "     python create-build.py checktime [svnURL] [svnUser] [svnPassword]"
    print "            [minSpace] [buildDrive] [buildOutputFolder] [buildSuffix] [hoursBetweenFreshBuild] [emailTo]"
    print "            # determines if time to run a new build, then triggers the build"
    print "            eg:"
    print "            python create-build.py checktime \\"
    print "            https://svn.ict.usc.edu/svn_vhtoolkit/trunk IABuild passwordblah \\"
    print "            200 d: d:/VHToolkit-Builds -ci"


##############################################################


if len(sys.argv) <= 1:
    printUsage()
else:
    if sys.argv[1] == "build":
        svnPassword = ""
        buildSuffix = ""

        if len(sys.argv) > 2:
            svnPassword = sys.argv[2]

        if len(sys.argv) > 3:
            buildSuffix = sys.argv[3]

        if len(sys.argv) > 4:
            emailTo = sys.argv[4]

        fullBuild(svnPassword, buildSuffix, True, emailTo)

    elif sys.argv[1] == "cleansandbox":
        cleanSandbox()
    elif sys.argv[1] == "checktime":
        svnURL = sys.argv[2]
        svnUser = sys.argv[3]
        svnPassword = sys.argv[4]
        minFreeSpaceRequiredForBuildGig = int(sys.argv[5])
        buildDrive = sys.argv[6]
        buildOutputFolder = sys.argv[7]
        buildSuffix = sys.argv[8]
        hoursBetweenFreshBuild = int(sys.argv[9])
        emailTo = sys.argv[10]
        checkIfTimeForBuild(svnURL, svnUser, svnPassword, False, minFreeSpaceRequiredForBuildGig, buildDrive, buildOutputFolder, buildSuffix, hoursBetweenFreshBuild, emailTo)
    else:
        printUsage()
