
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
    var PACKAGE_NAME = './data-chrmaarten.data';
    var REMOTE_PACKAGE_BASE = 'data-chrmaarten.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 9568936;
    var PACKAGE_UUID = '77da4ae4-e73b-445d-bc0d-f6b02dc59d2f';
  
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
Module['FS_createPath']('/ChrMaarten', 'face', true, true);
Module['FS_createPath']('/ChrMaarten', 'mesh', true, true);

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
      new DataRequest(0, 30069, 0, 0).open('GET', '/ChrMaarten/ChrBrad.sk');
    new DataRequest(30069, 36738, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@001_inner_brow_raiser_lf.skm');
    new DataRequest(36738, 43411, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@001_inner_brow_raiser_rt.skm');
    new DataRequest(43411, 50076, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@002_outer_brow_raiser_lf.skm');
    new DataRequest(50076, 56745, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@002_outer_brow_raiser_rt.skm');
    new DataRequest(56745, 63435, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@004_brow_lowerer_lf.skm');
    new DataRequest(63435, 70100, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@005_upper_lid_raiser.skm');
    new DataRequest(70100, 76781, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@006_cheek_raiser.skm');
    new DataRequest(76781, 83451, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@007_lid_tightener.skm');
    new DataRequest(83451, 90116, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@010_upper_lip_raiser.skm');
    new DataRequest(90116, 96785, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@012_lip_corner_puller_lf.skm');
    new DataRequest(96785, 103458, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@012_lip_corner_puller_rt.skm');
    new DataRequest(103458, 110138, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@025_lips_part.skm');
    new DataRequest(110138, 116843, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@026_jaw_drop.skm');
    new DataRequest(116843, 123498, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@045_blink_lf.skm');
    new DataRequest(123498, 130157, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@045_blink_rt.skm');
    new DataRequest(130157, 136865, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@101_upset.skm');
    new DataRequest(136865, 143506, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@face_neutral.skm');
    new DataRequest(143506, 150177, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@FV.skm');
    new DataRequest(150177, 156848, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@open.skm');
    new DataRequest(156848, 163536, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@PBM.skm');
    new DataRequest(163536, 170199, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@ShCh.skm');
    new DataRequest(170199, 176841, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@tBack.skm');
    new DataRequest(176841, 183481, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@tRoof.skm');
    new DataRequest(183481, 190126, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@tTeeth.skm');
    new DataRequest(190126, 196808, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@W.skm');
    new DataRequest(196808, 203465, 0, 0).open('GET', '/ChrMaarten/face/ChrMaarten@wide.skm');
    new DataRequest(203465, 9058010, 0, 0).open('GET', '/ChrMaarten/mesh/ChrMaarten.dae');
    new DataRequest(9058010, 9067326, 0, 0).open('GET', '/ChrMaarten/mesh/ChrMaarten_Eyelash_Dif.png');
    new DataRequest(9067326, 9102157, 0, 0).open('GET', '/ChrMaarten/mesh/ChrMaarten_Eye_Dif.png');
    new DataRequest(9102157, 9233799, 0, 0).open('GET', '/ChrMaarten/mesh/ChrMaarten_Head_Dif.png');
    new DataRequest(9233799, 9326063, 0, 0).open('GET', '/ChrMaarten/mesh/ChrMaarten_LowerBody_Dif.png');
    new DataRequest(9326063, 9425875, 0, 0).open('GET', '/ChrMaarten/mesh/ChrMaarten_LowerBody_Nrm.png');
    new DataRequest(9425875, 9468003, 0, 0).open('GET', '/ChrMaarten/mesh/ChrMaarten_UpperBody_Dif.png');
    new DataRequest(9468003, 9568936, 0, 0).open('GET', '/ChrMaarten/mesh/ChrMaarten_UpperBody_Nrm.png');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/ChrMaarten/ChrBrad.sk"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@001_inner_brow_raiser_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@001_inner_brow_raiser_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@002_outer_brow_raiser_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@002_outer_brow_raiser_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@004_brow_lowerer_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@005_upper_lid_raiser.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@006_cheek_raiser.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@007_lid_tightener.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@010_upper_lip_raiser.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@012_lip_corner_puller_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@012_lip_corner_puller_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@025_lips_part.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@026_jaw_drop.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@045_blink_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@045_blink_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@101_upset.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@face_neutral.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@FV.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@open.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@PBM.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@ShCh.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@tBack.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@tRoof.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@tTeeth.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@W.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/face/ChrMaarten@wide.skm"].onload();
          DataRequest.prototype.requests["/ChrMaarten/mesh/ChrMaarten.dae"].onload();
          DataRequest.prototype.requests["/ChrMaarten/mesh/ChrMaarten_Eyelash_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrMaarten/mesh/ChrMaarten_Eye_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrMaarten/mesh/ChrMaarten_Head_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrMaarten/mesh/ChrMaarten_LowerBody_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrMaarten/mesh/ChrMaarten_LowerBody_Nrm.png"].onload();
          DataRequest.prototype.requests["/ChrMaarten/mesh/ChrMaarten_UpperBody_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrMaarten/mesh/ChrMaarten_UpperBody_Nrm.png"].onload();
          Module['removeRunDependency']('datafile_./data-chrmaarten.data');

    };
    Module['addRunDependency']('datafile_./data-chrmaarten.data');
  
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
