
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
    var PACKAGE_NAME = './data-chrmaarten-default-motions.data';
    var REMOTE_PACKAGE_BASE = 'data-chrmaarten-default-motions.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 17928711;
    var PACKAGE_UUID = '89057b93-6a50-41fc-8000-8432cf80015e';
  
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
Module['FS_createPath']('/', 'ChrMaarten', true, true);
Module['FS_createPath']('/ChrMaarten', 'motions', true, true);

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
      new DataRequest(0, 635104, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Entrance.skm');
    new DataRequest(635104, 1474295, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Guitar01.skm');
    new DataRequest(1474295, 6076660, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Idle01.skm');
    new DataRequest(6076660, 6154257, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@LocIdle01_ToIdle01.skm');
    new DataRequest(6154257, 6715515, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@OpenDoor.skm');
    new DataRequest(6715515, 7824678, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@PushUps01.skm');
    new DataRequest(7824678, 8424483, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Reach_KneeBackLeft01.skm');
    new DataRequest(8424483, 8968656, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Reach_KneeBackRight01.skm');
    new DataRequest(8968656, 9442630, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Reach_KneeCenter01.skm');
    new DataRequest(9442630, 10101977, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Reach_KneeLeft01.skm');
    new DataRequest(10101977, 10627569, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Reach_KneeRight01.skm');
    new DataRequest(10627569, 11227957, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Reach_ShoulderBackLeft01.skm');
    new DataRequest(11227957, 11698845, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Reach_ShoulderBackRight01.skm');
    new DataRequest(11698845, 12091425, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Reach_ShoulderCenter01.skm');
    new DataRequest(12091425, 12486354, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Reach_ShoulderLeft01.skm');
    new DataRequest(12486354, 12860788, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Reach_ShoulderRight01.skm');
    new DataRequest(12860788, 14328742, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Trip01.skm');
    new DataRequest(14328742, 14928668, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Walk01.skm');
    new DataRequest(14928668, 15583164, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@Walk01_ToIdle01.skm');
    new DataRequest(15583164, 16959397, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@WalkCircleRt01.skm');
    new DataRequest(16959397, 17928711, 0, 0).open('GET', '/ChrMaarten/motions/ChrBrad@WalkTightCircleRt01.skm');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Entrance.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Guitar01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Idle01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@LocIdle01_ToIdle01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@OpenDoor.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@PushUps01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Reach_KneeBackLeft01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Reach_KneeBackRight01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Reach_KneeCenter01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Reach_KneeLeft01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Reach_KneeRight01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Reach_ShoulderBackLeft01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Reach_ShoulderBackRight01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Reach_ShoulderCenter01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Reach_ShoulderLeft01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Reach_ShoulderRight01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Trip01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Walk01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@Walk01_ToIdle01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@WalkCircleRt01.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/motions/ChrBrad@WalkTightCircleRt01.skm"].onload();
          Module['removeRunDependency']('datafile_./data-chrmaarten-default-motions.data');

    };
    Module['addRunDependency']('datafile_./data-chrmaarten-default-motions.data');
  
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
