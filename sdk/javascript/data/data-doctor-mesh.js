
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
    var PACKAGE_NAME = './data-doctor-mesh.data';
    var REMOTE_PACKAGE_BASE = 'data-doctor-mesh.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 4972857;
    var PACKAGE_UUID = '43beec57-a5fc-42f7-8412-a71d047ac6fe';
  
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
Module['FS_createPath']('/', 'mesh', true, true);
Module['FS_createPath']('/mesh', 'Doctor', true, true);

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
      new DataRequest(0, 174904, 0, 0).open('GET', '/mesh/Doctor/AH_doctor_upper_body.dds');
    new DataRequest(174904, 874096, 0, 0).open('GET', '/mesh/Doctor/AH_dr_face_texture.dds');
    new DataRequest(874096, 1049000, 0, 0).open('GET', '/mesh/Doctor/AH_teeth_tongue_eyes.dds');
    new DataRequest(1049000, 4138570, 0, 0).open('GET', '/mesh/Doctor/Doctor.dae');
    new DataRequest(4138570, 4313474, 0, 0).open('GET', '/mesh/Doctor/doctor_hand.dds');
    new DataRequest(4313474, 4488378, 0, 0).open('GET', '/mesh/Doctor/doctor_lower_body.dds');
    new DataRequest(4488378, 4488512, 0, 0).open('GET', '/mesh/Doctor/faceShape.mtl');
    new DataRequest(4488512, 4617096, 0, 0).open('GET', '/mesh/Doctor/faceShape.obj');
    new DataRequest(4617096, 4617223, 0, 0).open('GET', '/mesh/Doctor/left_handShape.mtl');
    new DataRequest(4617223, 4695048, 0, 0).open('GET', '/mesh/Doctor/left_handShape.obj');
    new DataRequest(4695048, 4695181, 0, 0).open('GET', '/mesh/Doctor/pantsShape.mtl');
    new DataRequest(4695181, 4736591, 0, 0).open('GET', '/mesh/Doctor/pantsShape.obj');
    new DataRequest(4736591, 4788282, 0, 0).open('GET', '/mesh/Doctor/polySurfaceShape10.obj');
    new DataRequest(4788282, 4788409, 0, 0).open('GET', '/mesh/Doctor/right_handShape.mtl');
    new DataRequest(4788409, 4866792, 0, 0).open('GET', '/mesh/Doctor/right_handShape.obj');
    new DataRequest(4866792, 4866928, 0, 0).open('GET', '/mesh/Doctor/shirtShape.mtl');
    new DataRequest(4866928, 4972857, 0, 0).open('GET', '/mesh/Doctor/shirtShape.obj');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/mesh/Doctor/AH_doctor_upper_body.dds"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/AH_dr_face_texture.dds"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/AH_teeth_tongue_eyes.dds"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/Doctor.dae"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/doctor_hand.dds"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/doctor_lower_body.dds"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/faceShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/faceShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/left_handShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/left_handShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/pantsShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/pantsShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/polySurfaceShape10.obj"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/right_handShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/right_handShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/shirtShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Doctor/shirtShape.obj"].onload();
          Module['removeRunDependency']('datafile_./data-doctor-mesh.data');

    };
    Module['addRunDependency']('datafile_./data-doctor-mesh.data');
  
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
