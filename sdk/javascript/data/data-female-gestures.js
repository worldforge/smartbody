
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
    var PACKAGE_NAME = './data-female-gestures.data';
    var REMOTE_PACKAGE_BASE = 'data-female-gestures.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 14618746;
    var PACKAGE_UUID = '52fa6624-0620-451c-bce3-8ce952062830';
  
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
Module['FS_createPath']('/behaviorsets', 'FemaleGestures', true, true);
Module['FS_createPath']('/behaviorsets/FemaleGestures', 'motions', true, true);
Module['FS_createPath']('/behaviorsets/FemaleGestures', 'skeletons', true, true);

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
      new DataRequest(0, 354071, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleCross01_ThrowAwayRt01.skm');
    new DataRequest(354071, 949687, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01.skm');
    new DataRequest(949687, 1253009, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatFlipBt01.skm');
    new DataRequest(1253009, 1478851, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatFlipRt01.skm');
    new DataRequest(1478851, 1800602, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatForwardBt01.skm');
    new DataRequest(1800602, 2137050, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatForwardRt01.skm');
    new DataRequest(2137050, 2399707, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatMidBt01.skm');
    new DataRequest(2399707, 2662422, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatMidRt01.skm');
    new DataRequest(2662422, 2961902, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_ChopBt01.skm');
    new DataRequest(2961902, 3261329, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_ChopRt01.skm');
    new DataRequest(3261329, 3560812, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_MeBt01.skm');
    new DataRequest(3560812, 3823571, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_MeRt01.skm');
    new DataRequest(3823571, 4141509, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateForwardLowBt01.skm');
    new DataRequest(4141509, 4367104, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateForwardLowRt01.skm');
    new DataRequest(4367104, 4629485, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateForwardMidBt01.skm');
    new DataRequest(4629485, 4910220, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateForwardMidRt01.skm');
    new DataRequest(4910220, 5209290, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateLeftLowBt01.skm');
    new DataRequest(5209290, 5530443, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateLeftLowRt01.skm');
    new DataRequest(5530443, 5839678, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateLeftMidBt01.skm');
    new DataRequest(5839678, 6160715, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateLeftMidRt01.skm');
    new DataRequest(6160715, 6463613, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateRightLowBt01.skm');
    new DataRequest(6463613, 6784619, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateRightLowRt01.skm');
    new DataRequest(6784619, 7068382, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateRightMidBt01.skm');
    new DataRequest(7068382, 7352705, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateRightMidRt01.skm');
    new DataRequest(7352705, 7593062, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoForwardBt01.skm');
    new DataRequest(7593062, 7858842, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoForwardRt01.skm');
    new DataRequest(7858842, 8087097, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoLeftBt01.skm');
    new DataRequest(8087097, 8445311, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoLeftRt01.skm');
    new DataRequest(8445311, 8674497, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoRightBt01.skm');
    new DataRequest(8674497, 8937139, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoRightRt01.skm');
    new DataRequest(8937139, 9154451, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_ScrewYouBt01.skm');
    new DataRequest(9154451, 9530467, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_ScrewYouRt01.skm');
    new DataRequest(9530467, 9887508, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopForwardBt01.skm');
    new DataRequest(9887508, 10246171, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopForwardRt01.skm');
    new DataRequest(10246171, 10566729, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopLeftBt01.skm');
    new DataRequest(10566729, 10887284, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopLeftRt01.skm');
    new DataRequest(10887284, 11282401, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopRightBt01.skm');
    new DataRequest(11282401, 11600424, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopRightRt01.skm');
    new DataRequest(11600424, 11903607, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouForwardBt01.skm');
    new DataRequest(11903607, 12317134, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouForwardRt01.skm');
    new DataRequest(12317134, 12673823, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouLeftBt01.skm');
    new DataRequest(12673823, 13032214, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouLeftRt01.skm');
    new DataRequest(13032214, 13350286, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouRightBt01.skm');
    new DataRequest(13350286, 13687102, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouRightRt01.skm');
    new DataRequest(13687102, 14588702, 0, 0).open('GET', '/behaviorsets/FemaleGestures/motions/ChrConnor@IdleUpright01_KickChair01.skm');
    new DataRequest(14588702, 14618746, 0, 0).open('GET', '/behaviorsets/FemaleGestures/skeletons/ChrConnor.sk');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleCross01_ThrowAwayRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatFlipBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatFlipRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatForwardBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatForwardRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatMidBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_BeatMidRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_ChopBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_ChopRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_MeBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_MeRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateForwardLowBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateForwardLowRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateForwardMidBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateForwardMidRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateLeftLowBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateLeftLowRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateLeftMidBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateLeftMidRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateRightLowBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateRightLowRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateRightMidBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NegateRightMidRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoForwardBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoForwardRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoLeftBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoLeftRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoRightBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_NoRightRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_ScrewYouBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_ScrewYouRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopForwardBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopForwardRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopLeftBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopLeftRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopRightBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_StopRightRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouForwardBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouForwardRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouLeftBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouLeftRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouRightBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleStand01_YouRightRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/motions/ChrConnor@IdleUpright01_KickChair01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/FemaleGestures/skeletons/ChrConnor.sk"].onload();
          Module['removeRunDependency']('datafile_./data-female-gestures.data');

    };
    Module['addRunDependency']('datafile_./data-female-gestures.data');
  
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
