
var Module;
if (typeof Module === 'undefined') Module = eval('(function() { try { return Module || {} } catch(e) { return {} } })()');
if (!Module.expectedDataFileDownloads) {
  Module.expectedDataFileDownloads = 0;
  Module.finishedDataFileDownloads = 0;
}
Module.expectedDataFileDownloads++;
(function() {

    var PACKAGE_PATH;
    if (typeof window === 'object') {
      PACKAGE_PATH = window['encodeURIComponent'](window.location.pathname.toString().substring(0, window.location.pathname.toString().lastIndexOf('/')) + '/');
    } else if (typeof location !== 'undefined') {
      // worker
      PACKAGE_PATH = encodeURIComponent(location.pathname.toString().substring(0, location.pathname.toString().lastIndexOf('/')) + '/');
    } else {
      throw 'using preloaded data can only be done on a web page or in a web worker';
    }
    var PACKAGE_NAME = './data-male-locomotion.data';
    var REMOTE_PACKAGE_BASE = 'data-male-locomotion.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 8656916;
    var PACKAGE_UUID = '4808138a-8997-495a-af50-1b9b07b19426';
  
    function fetchRemotePackage(packageName, packageSize, callback, errback) {
      var xhr = new XMLHttpRequest();
      xhr.open('GET', packageName, true);
      xhr.responseType = 'arraybuffer';
      xhr.onprogress = function(event) {
        var url = packageName;
        var size = packageSize;
        if (event.total) size = event.total;
        if (event.loaded) {
          if (!xhr.addedTotal) {
            xhr.addedTotal = true;
            if (!Module.dataFileDownloads) Module.dataFileDownloads = {};
            Module.dataFileDownloads[url] = {
              loaded: event.loaded,
              total: size
            };
          } else {
            Module.dataFileDownloads[url].loaded = event.loaded;
          }
          var total = 0;
          var loaded = 0;
          var num = 0;
          for (var download in Module.dataFileDownloads) {
          var data = Module.dataFileDownloads[download];
            total += data.total;
            loaded += data.loaded;
            num++;
          }
          total = Math.ceil(total * Module.expectedDataFileDownloads/num);
          if (Module['setStatus']) Module['setStatus']('Downloading data... (' + loaded + '/' + total + ')');
        } else if (!Module.dataFileDownloads) {
          if (Module['setStatus']) Module['setStatus']('Downloading data...');
        }
      };
      xhr.onload = function(event) {
        var packageData = xhr.response;
        callback(packageData);
      };
      xhr.send(null);
    };

    function handleError(error) {
      console.error('package error:', error);
    };
  
      var fetched = null, fetchedCallback = null;
      fetchRemotePackage(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE, function(data) {
        if (fetchedCallback) {
          fetchedCallback(data);
          fetchedCallback = null;
        } else {
          fetched = data;
        }
      }, handleError);
    
  function runWithFS() {

function assert(check, msg) {
  if (!check) throw msg + new Error().stack;
}
Module['FS_createPath']('/', 'behaviorsets', true, true);
Module['FS_createPath']('/behaviorsets', 'MaleLocomotion', true, true);
Module['FS_createPath']('/behaviorsets/MaleLocomotion', 'motions', true, true);
Module['FS_createPath']('/behaviorsets/MaleLocomotion', 'skeletons', true, true);

    function DataRequest(start, end, crunched, audio) {
      this.start = start;
      this.end = end;
      this.crunched = crunched;
      this.audio = audio;
    }
    DataRequest.prototype = {
      requests: {},
      open: function(mode, name) {
        this.name = name;
        this.requests[name] = this;
        Module['addRunDependency']('fp ' + this.name);
      },
      send: function() {},
      onload: function() {
        var byteArray = this.byteArray.subarray(this.start, this.end);

          this.finish(byteArray);

      },
      finish: function(byteArray) {
        var that = this;
        Module['FS_createPreloadedFile'](this.name, null, byteArray, true, true, function() {
          Module['removeRunDependency']('fp ' + that.name);
        }, function() {
          if (that.audio) {
            Module['removeRunDependency']('fp ' + that.name); // workaround for chromium bug 124926 (still no audio with this, but at least we don't hang)
          } else {
            Module.printErr('Preloading file ' + that.name + ' failed');
          }
        }, false, true); // canOwn this data in the filesystem, it is a slide into the heap that will never change
        this.requests[this.name] = null;
      },
    };
      new DataRequest(0, 423253, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle001.skm');
    new DataRequest(423253, 603462, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle001_ToWalk001.skm');
    new DataRequest(603462, 751818, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepBackwardLf01.skm');
    new DataRequest(751818, 914113, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepBackwardRt01.skm');
    new DataRequest(914113, 1062473, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepForwardLf01.skm');
    new DataRequest(1062473, 1224723, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepForwardRt01.skm');
    new DataRequest(1224723, 1457372, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepInPlace01.skm');
    new DataRequest(1457372, 1605849, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepSidewaysLf01.skm');
    new DataRequest(1605849, 1768182, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepSidewaysRt01.skm');
    new DataRequest(1768182, 2071037, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_ToWalk01_Turn180Lf01.skm');
    new DataRequest(2071037, 2303621, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_ToWalk01_Turn360Lf01.skm');
    new DataRequest(2303621, 2536509, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_ToWalk01_Turn360Rt01.skm');
    new DataRequest(2536509, 2839144, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_ToWalk01_Turn90Lf01.skm');
    new DataRequest(2839144, 3016065, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Jog001.skm');
    new DataRequest(3016065, 3090103, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_LiftFootLf01.skm');
    new DataRequest(3090103, 3164128, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_LiftFootRt01.skm');
    new DataRequest(3164128, 3495967, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Meander01.skm');
    new DataRequest(3495967, 3644915, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Run001.skm');
    new DataRequest(3644915, 3793844, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_RunInCircleLeft001.skm');
    new DataRequest(3793844, 3942803, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_RunInCircleRight001.skm');
    new DataRequest(3942803, 4091625, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_RunInTightCircleLeft01.skm');
    new DataRequest(4091625, 4240486, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_RunInTightCircleRight01.skm');
    new DataRequest(4240486, 4467126, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_RunJumpRun01.skm');
    new DataRequest(4467126, 4699299, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Shuffle01.skm');
    new DataRequest(4699299, 5001829, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Shuffle01_Idle01.skm');
    new DataRequest(5001829, 5153799, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_StopToWalkRt01.skm');
    new DataRequest(5153799, 5331156, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_StrafeFastLf01.skm');
    new DataRequest(5331156, 5508003, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_StrafeFastRt01.skm');
    new DataRequest(5508003, 5741063, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_StrafeSlowLf01.skm');
    new DataRequest(5741063, 5973526, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_StrafeSlowRt01.skm');
    new DataRequest(5973526, 6241043, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Turn180Lf01.skm');
    new DataRequest(6241043, 6508896, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Turn180Rt01.skm');
    new DataRequest(6508896, 6723813, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Turn90Lf01.skm');
    new DataRequest(6723813, 6938758, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Turn90Rt01.skm');
    new DataRequest(6938758, 7171467, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Walk001.skm');
    new DataRequest(7171467, 7351302, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_Walk001_ToIdle001.skm');
    new DataRequest(7351302, 7584086, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_WalkInCircleLeft001.skm');
    new DataRequest(7584086, 7817310, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_WalkInCircleRight001.skm');
    new DataRequest(7817310, 8049727, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_WalkInTightCircleLeft001.skm');
    new DataRequest(8049727, 8282537, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_WalkInTightCircleRight001.skm');
    new DataRequest(8282537, 8635914, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/motions/ChrUtah_WalkJumpWalk01.skm');
    new DataRequest(8635914, 8656916, 0, 0).open('GET', '/behaviorsets/MaleLocomotion/skeletons/test_utah.sk');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle001_ToWalk001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepBackwardLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepBackwardRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepForwardLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepForwardRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepInPlace01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepSidewaysLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_StepSidewaysRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_ToWalk01_Turn180Lf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_ToWalk01_Turn360Lf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_ToWalk01_Turn360Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Idle01_ToWalk01_Turn90Lf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Jog001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_LiftFootLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_LiftFootRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Meander01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Run001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_RunInCircleLeft001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_RunInCircleRight001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_RunInTightCircleLeft01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_RunInTightCircleRight01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_RunJumpRun01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Shuffle01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Shuffle01_Idle01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_StopToWalkRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_StrafeFastLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_StrafeFastRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_StrafeSlowLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_StrafeSlowRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Turn180Lf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Turn180Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Turn90Lf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Turn90Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Walk001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_Walk001_ToIdle001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_WalkInCircleLeft001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_WalkInCircleRight001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_WalkInTightCircleLeft001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_WalkInTightCircleRight001.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/motions/ChrUtah_WalkJumpWalk01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleLocomotion/skeletons/test_utah.sk"].onload();
          Module['removeRunDependency']('datafile_./data-male-locomotion.data');

    };
    Module['addRunDependency']('datafile_./data-male-locomotion.data');
  
    if (!Module.preloadResults) Module.preloadResults = {};
  
      Module.preloadResults[PACKAGE_NAME] = {fromCache: false};
      if (fetched) {
        processPackageData(fetched);
        fetched = null;
      } else {
        fetchedCallback = processPackageData;
      }
    
  }
  if (Module['calledRun']) {
    runWithFS();
  } else {
    if (!Module['preRun']) Module['preRun'] = [];
    Module["preRun"].push(runWithFS); // FS is not initialized yet, wait for it
  }

})();
