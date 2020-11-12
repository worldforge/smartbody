
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
    var PACKAGE_NAME = './data-gestures2.data';
    var REMOTE_PACKAGE_BASE = 'data-gestures2.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 27374379;
    var PACKAGE_UUID = '4ebfff21-8769-4664-9a5b-64e42c6fadaa';
  
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
Module['FS_createPath']('/behaviorsets', 'Gestures2', true, true);
Module['FS_createPath']('/behaviorsets/Gestures2', 'motions', true, true);
Module['FS_createPath']('/behaviorsets/Gestures2', 'skeletons', true, true);

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
      new DataRequest(0, 4602365, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01.skm');
    new DataRequest(4602365, 5177286, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ArmStretch01.skm');
    new DataRequest(5177286, 5424131, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatFistMidLf01.skm');
    new DataRequest(5424131, 5560626, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatFistMidLf02.skm');
    new DataRequest(5560626, 5885256, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatHighBt01.skm');
    new DataRequest(5885256, 6224766, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatHighBt02.skm');
    new DataRequest(6224766, 6312311, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatHighBt03.skm');
    new DataRequest(6312311, 6669692, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatHighLf01.skm');
    new DataRequest(6669692, 6902308, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatLowBt01.skm');
    new DataRequest(6902308, 6982553, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatLowBt02.skm');
    new DataRequest(6982553, 7196343, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatLowLf01.skm');
    new DataRequest(7196343, 7465457, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatLowLf02.skm');
    new DataRequest(7465457, 7683316, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatLowRt01.skm');
    new DataRequest(7683316, 7956118, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatMidBt01.skm');
    new DataRequest(7956118, 8203227, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatMidLf01.skm');
    new DataRequest(8203227, 8398625, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatPointMidLf01.skm');
    new DataRequest(8398625, 8557190, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BesidesLf01.skm');
    new DataRequest(8557190, 8980771, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ChopBoth01.skm');
    new DataRequest(8980771, 9319713, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ChopLf01.skm');
    new DataRequest(9319713, 9507803, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ChopLf02.skm');
    new DataRequest(9507803, 9688519, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ChopRt02.skm');
    new DataRequest(9688519, 10303274, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_Contemplate01.skm');
    new DataRequest(10303274, 10605314, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ExampleLf01.skm');
    new DataRequest(10605314, 11444512, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_Guitar01.skm');
    new DataRequest(11444512, 11520403, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_HereBt01.skm');
    new DataRequest(11520403, 11582901, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_HereBt02.skm');
    new DataRequest(11582901, 11907581, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_HoweverLf01.skm');
    new DataRequest(11907581, 12008489, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_InclusivityNegBt01.skm');
    new DataRequest(12008489, 12167320, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_InclusivityNegBt02.skm');
    new DataRequest(12167320, 12269871, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_InclusivityPosBt01.skm');
    new DataRequest(12269871, 12349019, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_InclusivityPosBt02.skm');
    new DataRequest(12349019, 12651464, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_IndicateLeftBt01.skm');
    new DataRequest(12651464, 12972030, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_IndicateLeftLf01.skm');
    new DataRequest(12972030, 13197291, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_IndicateRightBt01.skm');
    new DataRequest(13197291, 13452009, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_IndicateRightRt01.skm');
    new DataRequest(13452009, 13757815, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_MeLf01.skm');
    new DataRequest(13757815, 14041443, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_MeLf02.skm');
    new DataRequest(14041443, 14343985, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_NegativeBt01.skm');
    new DataRequest(14343985, 14613211, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_NegativeBt02.skm');
    new DataRequest(14613211, 14718977, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_NegativeBt03.skm');
    new DataRequest(14718977, 15113513, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_NegativeRt01.skm');
    new DataRequest(15113513, 15360678, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_OfferBoth01.skm');
    new DataRequest(15360678, 15699696, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_OfferLf01.skm');
    new DataRequest(15699696, 16024128, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_PleaBt02.skm');
    new DataRequest(16024128, 16363130, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_PointLf01.skm');
    new DataRequest(16363130, 16532691, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_PointLf02.skm');
    new DataRequest(16532691, 16750412, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_PointRt01.skm');
    new DataRequest(16750412, 17052732, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ReceiveRt01.skm');
    new DataRequest(17052732, 17373443, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_SafeLf01.skm');
    new DataRequest(17373443, 17837425, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ScratchChest01.skm');
    new DataRequest(17837425, 18305209, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ScratchHeadLf01.skm');
    new DataRequest(18305209, 18717859, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ScratchTempleLf01.skm');
    new DataRequest(18717859, 18979452, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ShoulderStretch01.skm');
    new DataRequest(18979452, 19263308, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_Shrug01.skm');
    new DataRequest(19263308, 19324760, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_Shrug02.skm');
    new DataRequest(19324760, 19884533, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_SqueezeHand01.skm');
    new DataRequest(19884533, 20072570, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_StopBt01.skm');
    new DataRequest(20072570, 20205467, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_StopBt02.skm');
    new DataRequest(20205467, 20292910, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_SurroundBt01.skm');
    new DataRequest(20292910, 20370309, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_SurroundBt02.skm');
    new DataRequest(20370309, 20962854, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_Think01.skm');
    new DataRequest(20962854, 21577432, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToIdle02.skm');
    new DataRequest(21577432, 21846236, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToIdle03.skm');
    new DataRequest(21846236, 21923835, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToLocIdle01.skm');
    new DataRequest(21923835, 22539454, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_TouchHands01.skm');
    new DataRequest(22539454, 23168684, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToWalk01_Turn180Rt01.skm');
    new DataRequest(23168684, 23911697, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToWalk01_Turn360Rt01.skm');
    new DataRequest(23911697, 24691417, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToWalk01_Turn90Rt01.skm');
    new DataRequest(24691417, 25435054, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WeightShift01.skm');
    new DataRequest(25435054, 26016996, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WeightShift02.skm');
    new DataRequest(26016996, 26094481, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WeRt01.skm');
    new DataRequest(26094481, 26165335, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WeRt02.skm');
    new DataRequest(26165335, 26244544, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WhenBt01.skm');
    new DataRequest(26244544, 26315447, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WhenBt02.skm');
    new DataRequest(26315447, 26599270, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_YouLf01.skm');
    new DataRequest(26599270, 26809345, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_YouLf02.skm');
    new DataRequest(26809345, 26993752, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_YouLf03.skm');
    new DataRequest(26993752, 27159847, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_YouPointLf01.skm');
    new DataRequest(27159847, 27344319, 0, 0).open('GET', '/behaviorsets/Gestures2/motions/ChrBrad@Idle01_YouRt01.skm');
    new DataRequest(27344319, 27374379, 0, 0).open('GET', '/behaviorsets/Gestures2/skeletons/ChrBrad2.sk');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ArmStretch01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatFistMidLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatFistMidLf02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatHighBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatHighBt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatHighBt03.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatHighLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatLowBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatLowBt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatLowLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatLowLf02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatLowRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatMidBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatMidLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BeatPointMidLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_BesidesLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ChopBoth01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ChopLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ChopLf02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ChopRt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_Contemplate01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ExampleLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_Guitar01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_HereBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_HereBt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_HoweverLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_InclusivityNegBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_InclusivityNegBt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_InclusivityPosBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_InclusivityPosBt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_IndicateLeftBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_IndicateLeftLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_IndicateRightBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_IndicateRightRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_MeLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_MeLf02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_NegativeBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_NegativeBt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_NegativeBt03.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_NegativeRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_OfferBoth01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_OfferLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_PleaBt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_PointLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_PointLf02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_PointRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ReceiveRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_SafeLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ScratchChest01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ScratchHeadLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ScratchTempleLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ShoulderStretch01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_Shrug01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_Shrug02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_SqueezeHand01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_StopBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_StopBt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_SurroundBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_SurroundBt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_Think01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToIdle02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToIdle03.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToLocIdle01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_TouchHands01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToWalk01_Turn180Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToWalk01_Turn360Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_ToWalk01_Turn90Rt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WeightShift01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WeightShift02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WeRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WeRt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WhenBt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_WhenBt02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_YouLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_YouLf02.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_YouLf03.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_YouPointLf01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/motions/ChrBrad@Idle01_YouRt01.skm"].onload();
          DataRequest.prototype.requests["/behaviorsets/Gestures2/skeletons/ChrBrad2.sk"].onload();
          Module['removeRunDependency']('datafile_./data-gestures2.data');

    };
    Module['addRunDependency']('datafile_./data-gestures2.data');
  
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
