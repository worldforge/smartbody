
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
    var PACKAGE_NAME = './data-auto-rigging.data';
    var REMOTE_PACKAGE_BASE = 'data-auto-rigging.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 5956318;
    var PACKAGE_UUID = 'c5fb419e-8f2d-434b-8bb7-368cff8c1188';
  
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
Module['FS_createPath']('/', 'AutoRigging', true, true);
Module['FS_createPath']('/AutoRigging', 'mesh', true, true);
Module['FS_createPath']('/AutoRigging', 'motions', true, true);

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
      new DataRequest(0, 3992368, 0, 0).open('GET', '/AutoRigging/mesh/SmartBodyRig.dae');
    new DataRequest(3992368, 4022227, 0, 0).open('GET', '/AutoRigging/motions/ChrBackovic.sk');
    new DataRequest(4022227, 4052296, 0, 0).open('GET', '/AutoRigging/motions/ChrBrad.sk');
    new DataRequest(4052296, 4891487, 0, 0).open('GET', '/AutoRigging/motions/ChrBrad@Guitar01.skm');
    new DataRequest(4891487, 4921502, 0, 0).open('GET', '/AutoRigging/motions/ChrGarza.sk');
    new DataRequest(4921502, 5381311, 0, 0).open('GET', '/AutoRigging/motions/ChrGarza@IdleFight01_KickForwardHigh01.skm');
    new DataRequest(5381311, 5793589, 0, 0).open('GET', '/AutoRigging/motions/ChrGarza@IdleFight01_PunchForwardMediumFar01.skm');
    new DataRequest(5793589, 5956126, 0, 0).open('GET', '/AutoRigging/motions/ChrMarine@Jog01.skm');
    new DataRequest(5956126, 5956318, 0, 0).open('GET', '/AutoRigging/motions/motion.txt');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/AutoRigging/mesh/SmartBodyRig.dae"].onload();
          DataRequest.prototype.requests["/AutoRigging/motions/ChrBackovic.sk"].onload();
          DataRequest.prototype.requests["/AutoRigging/motions/ChrBrad.sk"].onload();
          DataRequest.prototype.requests["/AutoRigging/motions/ChrBrad@Guitar01.skm"].onload();
          DataRequest.prototype.requests["/AutoRigging/motions/ChrGarza.sk"].onload();
          DataRequest.prototype.requests["/AutoRigging/motions/ChrGarza@IdleFight01_KickForwardHigh01.skm"].onload();
          DataRequest.prototype.requests["/AutoRigging/motions/ChrGarza@IdleFight01_PunchForwardMediumFar01.skm"].onload();
          DataRequest.prototype.requests["/AutoRigging/motions/ChrMarine@Jog01.skm"].onload();
          DataRequest.prototype.requests["/AutoRigging/motions/motion.txt"].onload();
          Module['removeRunDependency']('datafile_./data-auto-rigging.data');

    };
    Module['addRunDependency']('datafile_./data-auto-rigging.data');
  
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
