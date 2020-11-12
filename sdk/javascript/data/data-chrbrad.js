
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
    var PACKAGE_NAME = './data-chrbrad.data';
    var REMOTE_PACKAGE_BASE = 'data-chrbrad.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 13474182;
    var PACKAGE_UUID = '0796b7a9-5c48-43b8-a9ca-afdbad7ed155';
  
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
Module['FS_createPath']('/', 'ChrBrad', true, true);
Module['FS_createPath']('/ChrBrad', 'face', true, true);
Module['FS_createPath']('/ChrBrad', 'mesh', true, true);

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
      new DataRequest(0, 30060, 0, 0).open('GET', '/ChrBrad/ChrBrad.sk');
    new DataRequest(30060, 36724, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@001_inner_brow_raiser_lf.skm');
    new DataRequest(36724, 43382, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@001_inner_brow_raiser_rt.skm');
    new DataRequest(43382, 50042, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@002_outer_brow_raiser_lf.skm');
    new DataRequest(50042, 56718, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@002_outer_brow_raiser_rt.skm');
    new DataRequest(56718, 63401, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@004_brow_lowerer_lf.skm');
    new DataRequest(63401, 70074, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@004_brow_lowerer_rt.skm');
    new DataRequest(70074, 76738, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@005_upper_lid_raiser.skm');
    new DataRequest(76738, 83452, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@006_cheek_raiser.skm');
    new DataRequest(83452, 90115, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@007_lid_tightener.skm');
    new DataRequest(90115, 96777, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@010_upper_lip_raiser.skm');
    new DataRequest(96777, 103443, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@012_lip_corner_puller_lf.skm');
    new DataRequest(103443, 110117, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@012_lip_corner_puller_rt.skm');
    new DataRequest(110117, 116802, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@025_lips_part.skm');
    new DataRequest(116802, 123512, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@026_jaw_drop.skm');
    new DataRequest(123512, 130162, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@045_blink_lf.skm');
    new DataRequest(130162, 136820, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@045_blink_rt.skm');
    new DataRequest(136820, 143513, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@101_upset.skm');
    new DataRequest(143513, 154834, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@112_happy.skm');
    new DataRequest(154834, 166099, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@124_disgust.skm');
    new DataRequest(166099, 177379, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@126_fear.skm');
    new DataRequest(177379, 188699, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@127_surprise.skm');
    new DataRequest(188699, 200002, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@129_angry.skm');
    new DataRequest(200002, 211301, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@130_sad.skm');
    new DataRequest(211301, 222567, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@131_contempt.skm');
    new DataRequest(222567, 233813, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@132_browraise1.skm');
    new DataRequest(233813, 245061, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@133_browraise2.skm');
    new DataRequest(245061, 256313, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@134_hurt_brows.skm');
    new DataRequest(256313, 267591, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@136_furrow.skm');
    new DataRequest(267591, 274229, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@face_neutral.skm');
    new DataRequest(274229, 280905, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@FV.skm');
    new DataRequest(280905, 287605, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@open.skm');
    new DataRequest(287605, 294292, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@PBM.skm');
    new DataRequest(294292, 300960, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@ShCh.skm');
    new DataRequest(300960, 307599, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@tBack.skm');
    new DataRequest(307599, 314236, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@tRoof.skm');
    new DataRequest(314236, 320882, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@tTeeth.skm');
    new DataRequest(320882, 327587, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@W.skm');
    new DataRequest(327587, 334259, 0, 0).open('GET', '/ChrBrad/face/ChrBrad@wide.skm');
    new DataRequest(334259, 12023950, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad.dae');
    new DataRequest(12023950, 12145499, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_EyeBrown01_Dif.png');
    new DataRequest(12145499, 12203130, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Eyelash_Dif.png');
    new DataRequest(12203130, 12204005, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_EyeShell_Dif.png');
    new DataRequest(12204005, 12331572, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Eye_Dif.png');
    new DataRequest(12331572, 12435185, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Hair_Dif.png');
    new DataRequest(12435185, 12526991, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Hands_Dif.png');
    new DataRequest(12526991, 12573604, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Hands_Nrm.png');
    new DataRequest(12573604, 12741522, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Head_Dif.png');
    new DataRequest(12741522, 12799352, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Head_Nrm.png');
    new DataRequest(12799352, 12921845, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_LowerBody_Dif.png');
    new DataRequest(12921845, 13004471, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_LowerBody_Nrm.png');
    new DataRequest(13004471, 13092815, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Shoes_Dif.png');
    new DataRequest(13092815, 13162847, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Shoes_Nrm.png');
    new DataRequest(13162847, 13251845, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Teeth_Dif.png');
    new DataRequest(13251845, 13283647, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Teeth_Nrm.png');
    new DataRequest(13283647, 13302910, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_Teeth_Spc.png');
    new DataRequest(13302910, 13372030, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_UpperBody_Dif.png');
    new DataRequest(13372030, 13474182, 0, 0).open('GET', '/ChrBrad/mesh/ChrBrad_UpperBody_Nrm.png');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/ChrBrad/ChrBrad.sk"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@001_inner_brow_raiser_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@001_inner_brow_raiser_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@002_outer_brow_raiser_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@002_outer_brow_raiser_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@004_brow_lowerer_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@004_brow_lowerer_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@005_upper_lid_raiser.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@006_cheek_raiser.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@007_lid_tightener.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@010_upper_lip_raiser.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@012_lip_corner_puller_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@012_lip_corner_puller_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@025_lips_part.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@026_jaw_drop.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@045_blink_lf.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@045_blink_rt.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@101_upset.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@112_happy.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@124_disgust.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@126_fear.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@127_surprise.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@129_angry.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@130_sad.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@131_contempt.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@132_browraise1.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@133_browraise2.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@134_hurt_brows.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@136_furrow.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@face_neutral.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@FV.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@open.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@PBM.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@ShCh.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@tBack.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@tRoof.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@tTeeth.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@W.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/face/ChrBrad@wide.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad.dae"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_EyeBrown01_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Eyelash_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_EyeShell_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Eye_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Hair_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Hands_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Hands_Nrm.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Head_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Head_Nrm.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_LowerBody_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_LowerBody_Nrm.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Shoes_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Shoes_Nrm.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Teeth_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Teeth_Nrm.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_Teeth_Spc.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_UpperBody_Dif.png"].onload();
          DataRequest.prototype.requests["/ChrBrad/mesh/ChrBrad_UpperBody_Nrm.png"].onload();
          Module['removeRunDependency']('datafile_./data-chrbrad.data');

    };
    Module['addRunDependency']('datafile_./data-chrbrad.data');
  
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
