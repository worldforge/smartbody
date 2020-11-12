
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
    var PACKAGE_NAME = './data-brad-mesh.data';
    var REMOTE_PACKAGE_BASE = 'data-brad-mesh.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 6941994;
    var PACKAGE_UUID = '270aa946-71fd-4239-9d34-b45a272508d9';
  
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
Module['FS_createPath']('/mesh', 'Brad', true, true);

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
      new DataRequest(0, 130, 0, 0).open('GET', '/mesh/Brad/BeltShape.mtl');
    new DataRequest(130, 59781, 0, 0).open('GET', '/mesh/Brad/BeltShape.obj');
    new DataRequest(59781, 2354679, 0, 0).open('GET', '/mesh/Brad/Brad.dae');
    new DataRequest(2354679, 3053871, 0, 0).open('GET', '/mesh/Brad/Brad_Face_2048.dds');
    new DataRequest(3053871, 3930354, 0, 0).open('GET', '/mesh/Brad/Brad_Face_2048.png');
    new DataRequest(3930354, 4105258, 0, 0).open('GET', '/mesh/Brad/Brad_Hand.dds');
    new DataRequest(4105258, 4254955, 0, 0).open('GET', '/mesh/Brad/Brad_Hand.png');
    new DataRequest(4254955, 4429859, 0, 0).open('GET', '/mesh/Brad/Brad_Pants.dds');
    new DataRequest(4429859, 4596141, 0, 0).open('GET', '/mesh/Brad/Brad_Pants.png');
    new DataRequest(4596141, 5295333, 0, 0).open('GET', '/mesh/Brad/Brad_Shirt.dds');
    new DataRequest(5295333, 5810697, 0, 0).open('GET', '/mesh/Brad/Brad_Shirt.png');
    new DataRequest(5810697, 5810829, 0, 0).open('GET', '/mesh/Brad/EyesShape.mtl');
    new DataRequest(5810829, 5839749, 0, 0).open('GET', '/mesh/Brad/EyesShape.obj');
    new DataRequest(5839749, 5839881, 0, 0).open('GET', '/mesh/Brad/Face1Shape.mtl');
    new DataRequest(5839881, 5980435, 0, 0).open('GET', '/mesh/Brad/Face1Shape.obj');
    new DataRequest(5980435, 5980564, 0, 0).open('GET', '/mesh/Brad/HandsShape.mtl');
    new DataRequest(5980564, 6209461, 0, 0).open('GET', '/mesh/Brad/HandsShape.obj');
    new DataRequest(6209461, 6209593, 0, 0).open('GET', '/mesh/Brad/InnerMouthShape.mtl');
    new DataRequest(6209593, 6227595, 0, 0).open('GET', '/mesh/Brad/InnerMouthShape.obj');
    new DataRequest(6227595, 6227727, 0, 0).open('GET', '/mesh/Brad/LowerTeethShape.mtl');
    new DataRequest(6227727, 6252208, 0, 0).open('GET', '/mesh/Brad/LowerTeethShape.obj');
    new DataRequest(6252208, 6252632, 0, 0).open('GET', '/mesh/Brad/PantsShape.mtl');
    new DataRequest(6252632, 6495687, 0, 0).open('GET', '/mesh/Brad/PantsShape.obj');
    new DataRequest(6495687, 6495787, 0, 0).open('GET', '/mesh/Brad/polySurfaceShape1.obj');
    new DataRequest(6495787, 6495917, 0, 0).open('GET', '/mesh/Brad/ShirtShape.mtl');
    new DataRequest(6495917, 6819872, 0, 0).open('GET', '/mesh/Brad/ShirtShape.obj');
    new DataRequest(6819872, 6820296, 0, 0).open('GET', '/mesh/Brad/ShoesShape.mtl');
    new DataRequest(6820296, 6905043, 0, 0).open('GET', '/mesh/Brad/ShoesShape.obj');
    new DataRequest(6905043, 6905175, 0, 0).open('GET', '/mesh/Brad/TongueShape.mtl');
    new DataRequest(6905175, 6916503, 0, 0).open('GET', '/mesh/Brad/TongueShape.obj');
    new DataRequest(6916503, 6916633, 0, 0).open('GET', '/mesh/Brad/UpperTeethShape.mtl');
    new DataRequest(6916633, 6941994, 0, 0).open('GET', '/mesh/Brad/UpperTeethShape.obj');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/mesh/Brad/BeltShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Brad/BeltShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Brad/Brad.dae"].onload();
          DataRequest.prototype.requests["/mesh/Brad/Brad_Face_2048.dds"].onload();
          DataRequest.prototype.requests["/mesh/Brad/Brad_Face_2048.png"].onload();
          DataRequest.prototype.requests["/mesh/Brad/Brad_Hand.dds"].onload();
          DataRequest.prototype.requests["/mesh/Brad/Brad_Hand.png"].onload();
          DataRequest.prototype.requests["/mesh/Brad/Brad_Pants.dds"].onload();
          DataRequest.prototype.requests["/mesh/Brad/Brad_Pants.png"].onload();
          DataRequest.prototype.requests["/mesh/Brad/Brad_Shirt.dds"].onload();
          DataRequest.prototype.requests["/mesh/Brad/Brad_Shirt.png"].onload();
          DataRequest.prototype.requests["/mesh/Brad/EyesShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Brad/EyesShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Brad/Face1Shape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Brad/Face1Shape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Brad/HandsShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Brad/HandsShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Brad/InnerMouthShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Brad/InnerMouthShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Brad/LowerTeethShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Brad/LowerTeethShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Brad/PantsShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Brad/PantsShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Brad/polySurfaceShape1.obj"].onload();
          DataRequest.prototype.requests["/mesh/Brad/ShirtShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Brad/ShirtShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Brad/ShoesShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Brad/ShoesShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Brad/TongueShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Brad/TongueShape.obj"].onload();
          DataRequest.prototype.requests["/mesh/Brad/UpperTeethShape.mtl"].onload();
          DataRequest.prototype.requests["/mesh/Brad/UpperTeethShape.obj"].onload();
          Module['removeRunDependency']('datafile_./data-brad-mesh.data');

    };
    Module['addRunDependency']('datafile_./data-brad-mesh.data');
  
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
