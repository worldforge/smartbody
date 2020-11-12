
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
    var PACKAGE_NAME = './data-male-mocap-locomotion.data';
    var REMOTE_PACKAGE_BASE = 'data-male-mocap-locomotion.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 11187187;
    var PACKAGE_UUID = '723eb0b9-cd6f-4b3c-a669-c5f1e5677d11';
  
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
Module['FS_createPath']('/behaviorsets', 'MaleMocapLocomotion', true, true);
Module['FS_createPath']('/behaviorsets/MaleMocapLocomotion', 'motions', true, true);
Module['FS_createPath']('/behaviorsets/MaleMocapLocomotion', 'skeletons', true, true);

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
      new DataRequest(0, 246945, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01.skm');
    new DataRequest(246945, 403290, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_StepBackwardsRt01.skm');
    new DataRequest(403290, 633814, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_StepForwardRt01.skm');
    new DataRequest(633814, 819833, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_StepSidewaysRt01.skm');
    new DataRequest(819833, 1000744, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_ToWalk01.skm');
    new DataRequest(1000744, 1351496, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_ToWalk01_Turn180Rt01.skm');
    new DataRequest(1351496, 1628455, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_ToWalk01_Turn90Rt01.skm');
    new DataRequest(1628455, 1790994, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Jog01.skm');
    new DataRequest(1790994, 2056776, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Meander01.skm');
    new DataRequest(2056776, 2204638, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Run01.skm');
    new DataRequest(2204638, 2385654, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@RunCircleRt01.skm');
    new DataRequest(2385654, 2548248, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@RunTightCircleRt01.skm');
    new DataRequest(2548248, 2735251, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@StrafeFastRt01.skm');
    new DataRequest(2735251, 2998252, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@StrafeSlowRt01.skm');
    new DataRequest(2998252, 3197688, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Turn180Rt.skm');
    new DataRequest(3197688, 3444086, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Turn360Rt01.skm');
    new DataRequest(3444086, 3687673, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Turn90Rt.skm');
    new DataRequest(3687673, 3923898, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Walk01.skm');
    new DataRequest(3923898, 4178383, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Walk01_ToIdle01.skm');
    new DataRequest(4178383, 4473841, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@WalkCircleRt01.skm');
    new DataRequest(4473841, 4806398, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@WalkTightCircleRt01.skm');
    new DataRequest(4806398, 5053341, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01.skm');
    new DataRequest(5053341, 5209691, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_StepBackwardsRt01.skm');
    new DataRequest(5209691, 5440214, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_StepForwardRt01.skm');
    new DataRequest(5440214, 5626226, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_StepSidewaysRt01.skm');
    new DataRequest(5626226, 5807135, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_ToWalk01.skm');
    new DataRequest(5807135, 6157885, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_ToWalk01_Turn180Rt01.skm');
    new DataRequest(6157885, 6434844, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_ToWalk01_Turn90Rt01.skm');
    new DataRequest(6434844, 6597381, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Jog01.skm');
    new DataRequest(6597381, 6863161, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Meander01.skm');
    new DataRequest(6863161, 6988887, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Run01.skm');
    new DataRequest(6988887, 7146116, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Run01_JumpHigh01.skm');
    new DataRequest(7146116, 7358040, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Run01_JumpLow01.skm');
    new DataRequest(7358040, 7548059, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Run01_JumpMid01.skm');
    new DataRequest(7548059, 7729073, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@RunCircleRt01.skm');
    new DataRequest(7729073, 7891665, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@RunTightCircleRt01.skm');
    new DataRequest(7891665, 8078666, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@StrafeFastRt01.skm');
    new DataRequest(8078666, 8341507, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@StrafeSlowRt01.skm');
    new DataRequest(8341507, 8540921, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Turn180Rt.skm');
    new DataRequest(8540921, 8740337, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Turn180Rt01.skm');
    new DataRequest(8740337, 8986744, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Turn360Rt01.skm');
    new DataRequest(8986744, 9230287, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Turn90Rt.skm');
    new DataRequest(9230287, 9473832, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Turn90Rt01.skm');
    new DataRequest(9473832, 9710055, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Walk01.skm');
    new DataRequest(9710055, 9964538, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Walk01_ToIdle01.skm');
    new DataRequest(9964538, 10259994, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@WalkCircleRt01.skm');
    new DataRequest(10259994, 10592550, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@WalkTightCircleRt01.skm');
    new DataRequest(10592550, 10828676, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@WalkUp01.skm');
    new DataRequest(10828676, 11157328, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@WalkUp02.skm');
    new DataRequest(11157328, 11187187, 0, 0).open('GET', '/behaviorsets/MaleMocapLocomotion/skeletons/ChrBackovic.sk');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_StepBackwardsRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_StepForwardRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_StepSidewaysRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_ToWalk01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_ToWalk01_Turn180Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Idle01_ToWalk01_Turn90Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Jog01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Meander01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Run01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@RunCircleRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@RunTightCircleRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@StrafeFastRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@StrafeSlowRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Turn180Rt.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Turn360Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Turn90Rt.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Walk01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@Walk01_ToIdle01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@WalkCircleRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine02@WalkTightCircleRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_StepBackwardsRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_StepForwardRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_StepSidewaysRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_ToWalk01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_ToWalk01_Turn180Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Idle01_ToWalk01_Turn90Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Jog01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Meander01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Run01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Run01_JumpHigh01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Run01_JumpLow01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Run01_JumpMid01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@RunCircleRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@RunTightCircleRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@StrafeFastRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@StrafeSlowRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Turn180Rt.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Turn180Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Turn360Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Turn90Rt.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Turn90Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Walk01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@Walk01_ToIdle01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@WalkCircleRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@WalkTightCircleRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@WalkUp01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/motions/ChrMarine@WalkUp02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/MaleMocapLocomotion/skeletons/ChrBackovic.sk"].onload();
          Module['removeRunDependency']('datafile_./data-male-mocap-locomotion.data');

    };
    Module['addRunDependency']('datafile_./data-male-mocap-locomotion.data');
  
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
