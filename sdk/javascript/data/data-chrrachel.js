
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
    var PACKAGE_NAME = './data-chrrachel.data';
    var REMOTE_PACKAGE_BASE = 'data-chrrachel.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 8410101;
    var PACKAGE_UUID = 'd93e09a0-cb5f-446e-b45c-fba56971da3d';
  
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
Module['FS_createPath']('/', 'ChrRachel', true, true);
Module['FS_createPath']('/ChrRachel', 'face', true, true);
Module['FS_createPath']('/ChrRachel', 'mesh', true, true);

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
      new DataRequest(0, 30055, 0, 0).open('GET', '/ChrRachel/ChrRachel.sk');
    new DataRequest(30055, 36727, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@001_inner_brow_raiser_lf.skm');
    new DataRequest(36727, 43399, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@001_inner_brow_raiser_rt.skm');
    new DataRequest(43399, 50069, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@002_outer_brow_raiser_lf.skm');
    new DataRequest(50069, 56745, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@002_outer_brow_raiser_rt.skm');
    new DataRequest(56745, 63426, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@004_brow_lowerer_lf.skm');
    new DataRequest(63426, 70097, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@004_brow_lowerer_rt.skm');
    new DataRequest(70097, 76765, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@005_upper_lid_raiser.skm');
    new DataRequest(76765, 83437, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@006_cheek_raiser.skm');
    new DataRequest(83437, 90116, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@007_lid_tightener.skm');
    new DataRequest(90116, 96796, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@010_upper_lip_raiser.skm');
    new DataRequest(96796, 103478, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@012_lip_corner_puller_lf.skm');
    new DataRequest(103478, 110160, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@012_lip_corner_puller_rt.skm');
    new DataRequest(110160, 116853, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@014_dimpler.skm');
    new DataRequest(116853, 123555, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@015_lip_corner_depressor.skm');
    new DataRequest(123555, 130238, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@017_chin_raiser.skm');
    new DataRequest(130238, 136930, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@018_lip_pucker.skm');
    new DataRequest(136930, 143623, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@020_lip_stretcher.skm');
    new DataRequest(143623, 150322, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@023_lip_tightener.skm');
    new DataRequest(150322, 157011, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@024_lip_pressor.skm');
    new DataRequest(157011, 163692, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@025_lips_part.skm');
    new DataRequest(163692, 170374, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@026_jaw_drop.skm');
    new DataRequest(170374, 177034, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@045_blink_lf.skm');
    new DataRequest(177034, 183694, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@045_blink_rt.skm');
    new DataRequest(183694, 190385, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@099_big_smile.skm');
    new DataRequest(190385, 197070, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@100_small_smile.skm');
    new DataRequest(197070, 203767, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@101_upset.skm');
    new DataRequest(203767, 210448, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@102_skeptical.skm');
    new DataRequest(210448, 217145, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@103_concern.skm');
    new DataRequest(217145, 223816, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@104_disgust.skm');
    new DataRequest(223816, 230454, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@face_neutral.skm');
    new DataRequest(230454, 237128, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@FV.skm');
    new DataRequest(237128, 243812, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@open.skm');
    new DataRequest(243812, 250501, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@PBM.skm');
    new DataRequest(250501, 257175, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@ShCh.skm');
    new DataRequest(257175, 263828, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@tBack.skm');
    new DataRequest(263828, 270477, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@tRoof.skm');
    new DataRequest(270477, 277127, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@tTeeth.skm');
    new DataRequest(277127, 283814, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@W.skm');
    new DataRequest(283814, 290472, 0, 0).open('GET', '/ChrRachel/face/ChrRachel@wide.skm');
    new DataRequest(290472, 7796906, 0, 0).open('GET', '/ChrRachel/mesh/ChrRachel.dae');
    new DataRequest(7796906, 7932129, 0, 0).open('GET', '/ChrRachel/mesh/ChrRachel_Hair_Dif.png');
    new DataRequest(7932129, 7966618, 0, 0).open('GET', '/ChrRachel/mesh/ChrRachel_Hair_Nrm.png');
    new DataRequest(7966618, 8126654, 0, 0).open('GET', '/ChrRachel/mesh/ChrRachel_Head_Dif.png');
    new DataRequest(8126654, 8161143, 0, 0).open('GET', '/ChrRachel/mesh/ChrRachel_Head_Nrm.png');
    new DataRequest(8161143, 8217807, 0, 0).open('GET', '/ChrRachel/mesh/ChrRachel_LowerBody_Dif.png');
    new DataRequest(8217807, 8262572, 0, 0).open('GET', '/ChrRachel/mesh/ChrRachel_LowerBody_Nrm.png');
    new DataRequest(8262572, 8354256, 0, 0).open('GET', '/ChrRachel/mesh/ChrRachel_UpperBody_Dif.png');
    new DataRequest(8354256, 8410101, 0, 0).open('GET', '/ChrRachel/mesh/ChrRachel_UpperBody_Nrm.png');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/ChrRachel/ChrRachel.sk"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@001_inner_brow_raiser_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@001_inner_brow_raiser_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@002_outer_brow_raiser_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@002_outer_brow_raiser_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@004_brow_lowerer_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@004_brow_lowerer_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@005_upper_lid_raiser.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@006_cheek_raiser.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@007_lid_tightener.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@010_upper_lip_raiser.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@012_lip_corner_puller_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@012_lip_corner_puller_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@014_dimpler.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@015_lip_corner_depressor.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@017_chin_raiser.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@018_lip_pucker.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@020_lip_stretcher.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@023_lip_tightener.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@024_lip_pressor.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@025_lips_part.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@026_jaw_drop.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@045_blink_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@045_blink_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@099_big_smile.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@100_small_smile.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@101_upset.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@102_skeptical.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@103_concern.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@104_disgust.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@face_neutral.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@FV.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@open.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@PBM.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@ShCh.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@tBack.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@tRoof.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@tTeeth.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@W.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/face/ChrRachel@wide.skm"].onload();
          DataRequest.prototype.requests["/ChrRachel/mesh/ChrRachel.dae"].onload();
          DataRequest.prototype.requests["/ChrRachel/mesh/ChrRachel_Hair_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrRachel/mesh/ChrRachel_Hair_Nrm.png"].onload();
          DataRequest.prototype.requests["/ChrRachel/mesh/ChrRachel_Head_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrRachel/mesh/ChrRachel_Head_Nrm.png"].onload();
          DataRequest.prototype.requests["/ChrRachel/mesh/ChrRachel_LowerBody_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrRachel/mesh/ChrRachel_LowerBody_Nrm.png"].onload();
          DataRequest.prototype.requests["/ChrRachel/mesh/ChrRachel_UpperBody_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrRachel/mesh/ChrRachel_UpperBody_Nrm.png"].onload();
          Module['removeRunDependency']('datafile_./data-chrrachel.data');

    };
    Module['addRunDependency']('datafile_./data-chrrachel.data');
  
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
