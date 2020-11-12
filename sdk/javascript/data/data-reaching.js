
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
    var PACKAGE_NAME = './data-reaching.data';
    var REMOTE_PACKAGE_BASE = 'data-reaching.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 2387313;
    var PACKAGE_UUID = '52c2192c-933b-48f2-946f-8d496c610636';
  
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
Module['FS_createPath']('/behaviorsets', 'reaching', true, true);
Module['FS_createPath']('/behaviorsets/reaching', 'motions', true, true);
Module['FS_createPath']('/behaviorsets/reaching', 'skeletons', true, true);

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
      new DataRequest(0, 94915, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_High1.skm');
    new DataRequest(94915, 181124, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_High2.skm');
    new DataRequest(181124, 255508, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_Low1.skm');
    new DataRequest(255508, 342964, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_Low2.skm');
    new DataRequest(342964, 421764, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_MidHigh1.skm');
    new DataRequest(421764, 509128, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_MidHigh2.skm');
    new DataRequest(509128, 592149, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_MidLow1.skm');
    new DataRequest(592149, 678352, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_MidLow2.skm');
    new DataRequest(678352, 766987, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_Lf.skm');
    new DataRequest(766987, 854581, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_MiddleHigh.skm');
    new DataRequest(854581, 952118, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_MiddleLow.skm');
    new DataRequest(952118, 1039492, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_MiddleMidHigh.skm');
    new DataRequest(1039492, 1112398, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_MiddleMidLow.skm');
    new DataRequest(1112398, 1188376, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_Rt.skm');
    new DataRequest(1188376, 1275733, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachLfHigh.skm');
    new DataRequest(1275733, 1392079, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachLfLow.skm');
    new DataRequest(1392079, 1462277, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachLfMidHigh.skm');
    new DataRequest(1462277, 1549580, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachLfMidLow.skm');
    new DataRequest(1549580, 1628037, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachMiddleHigh.skm');
    new DataRequest(1628037, 1732710, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachMiddleLow.skm');
    new DataRequest(1732710, 1795634, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachMiddleMidHigh.skm');
    new DataRequest(1795634, 1873906, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachMiddleMidLow.skm');
    new DataRequest(1873906, 1952563, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachRtHigh.skm');
    new DataRequest(1952563, 2057031, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachRtLow.skm');
    new DataRequest(2057031, 2119955, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachRtMidHigh.skm');
    new DataRequest(2119955, 2198200, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachRtMidLow.skm');
    new DataRequest(2198200, 2212187, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandDefault.skm');
    new DataRequest(2212187, 2229092, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGrasp.skm');
    new DataRequest(2229092, 2231549, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGrasp2.skm');
    new DataRequest(2231549, 2234024, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspPinchTinySphere_Pinch.skm');
    new DataRequest(2234024, 2236499, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspPinchTinySphere_Reach.skm');
    new DataRequest(2236499, 2238976, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspPinchTinySphere_Release.skm');
    new DataRequest(2238976, 2241446, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspSmSphere_Grasp.skm');
    new DataRequest(2241446, 2243909, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspSmSphere_Reach.skm');
    new DataRequest(2243909, 2246385, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspSmSphere_Release.skm');
    new DataRequest(2246385, 2261795, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandReach.skm');
    new DataRequest(2261795, 2264249, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandReach2.skm');
    new DataRequest(2264249, 2266706, 0, 0).open('GET', '/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandRelease2.skm');
    new DataRequest(2266706, 2368537, 0, 0).open('GET', '/behaviorsets/reaching/motions/HandsAtSide_RArm_GestureYou.skm');
    new DataRequest(2368537, 2387313, 0, 0).open('GET', '/behaviorsets/reaching/skeletons/common.sk');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_High1.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_High2.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_Low1.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_Low2.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_MidHigh1.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_MidHigh2.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_MidLow1.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachBehind_MidLow2.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_Lf.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_MiddleHigh.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_MiddleLow.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_MiddleMidHigh.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_MiddleMidLow.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachClose_Rt.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachLfHigh.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachLfLow.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachLfMidHigh.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachLfMidLow.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachMiddleHigh.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachMiddleLow.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachMiddleMidHigh.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachMiddleMidLow.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachRtHigh.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachRtLow.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachRtMidHigh.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_ArmReachRtMidLow.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandDefault.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGrasp.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGrasp2.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspPinchTinySphere_Pinch.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspPinchTinySphere_Reach.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspPinchTinySphere_Release.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspSmSphere_Grasp.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspSmSphere_Reach.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandGraspSmSphere_Release.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandReach.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandReach2.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/ChrHarmony_Relax001_HandRelease2.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/motions/HandsAtSide_RArm_GestureYou.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/reaching/skeletons/common.sk"].onload();
          Module['removeRunDependency']('datafile_./data-reaching.data');

    };
    Module['addRunDependency']('datafile_./data-reaching.data');
  
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
