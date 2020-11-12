
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
    var PACKAGE_NAME = './data-chrbrad-default-motions.data';
    var REMOTE_PACKAGE_BASE = 'data-chrbrad-default-motions.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
    var REMOTE_PACKAGE_SIZE = 27526748;
    var PACKAGE_UUID = '4bff2788-7d63-443f-9f84-584164376705';
  
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
Module['FS_createPath']('/ChrBrad', 'motions', true, true);

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
      new DataRequest(0, 4602365, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01.skm');
    new DataRequest(4602365, 5177286, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ArmStretch01.skm');
    new DataRequest(5177286, 5424131, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatFistMidLf01.skm');
    new DataRequest(5424131, 5560626, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatFistMidLf02.skm');
    new DataRequest(5560626, 5885256, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatHighBt01.skm');
    new DataRequest(5885256, 6224766, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatHighBt02.skm');
    new DataRequest(6224766, 6312311, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatHighBt03.skm');
    new DataRequest(6312311, 6669692, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatHighLf01.skm');
    new DataRequest(6669692, 6902308, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatLowBt01.skm');
    new DataRequest(6902308, 6982553, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatLowBt02.skm');
    new DataRequest(6982553, 7196343, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatLowLf01.skm');
    new DataRequest(7196343, 7465457, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatLowLf02.skm');
    new DataRequest(7465457, 7683316, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatLowRt01.skm');
    new DataRequest(7683316, 7956118, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatMidBt01.skm');
    new DataRequest(7956118, 8203227, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatMidLf01.skm');
    new DataRequest(8203227, 8398625, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BeatPointMidLf01.skm');
    new DataRequest(8398625, 8557190, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_BesidesLf01.skm');
    new DataRequest(8557190, 8980771, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ChopBoth01.skm');
    new DataRequest(8980771, 9163200, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ChopBt01.skm');
    new DataRequest(9163200, 9502142, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ChopLf01.skm');
    new DataRequest(9502142, 9690232, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ChopLf02.skm');
    new DataRequest(9690232, 9870948, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ChopRt02.skm');
    new DataRequest(9870948, 10485703, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_Contemplate01.skm');
    new DataRequest(10485703, 10787743, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ExampleLf01.skm');
    new DataRequest(10787743, 11626941, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_Guitar01.skm');
    new DataRequest(11626941, 11702832, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_HereBt01.skm');
    new DataRequest(11702832, 11765330, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_HereBt02.skm');
    new DataRequest(11765330, 12090010, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_HoweverLf01.skm');
    new DataRequest(12090010, 12190918, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_InclusivityNegBt01.skm');
    new DataRequest(12190918, 12349749, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_InclusivityNegBt02.skm');
    new DataRequest(12349749, 12452300, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_InclusivityPosBt01.skm');
    new DataRequest(12452300, 12531448, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_InclusivityPosBt02.skm');
    new DataRequest(12531448, 12833893, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_IndicateLeftBt01.skm');
    new DataRequest(12833893, 13154459, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_IndicateLeftLf01.skm');
    new DataRequest(13154459, 13379720, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_IndicateRightBt01.skm');
    new DataRequest(13379720, 13634438, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_IndicateRightRt01.skm');
    new DataRequest(13634438, 13940244, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_MeLf01.skm');
    new DataRequest(13940244, 14223872, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_MeLf02.skm');
    new DataRequest(14223872, 14526414, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_NegativeBt01.skm');
    new DataRequest(14526414, 14795640, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_NegativeBt02.skm');
    new DataRequest(14795640, 14901406, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_NegativeBt03.skm');
    new DataRequest(14901406, 15295942, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_NegativeRt01.skm');
    new DataRequest(15295942, 15543107, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_OfferBoth01.skm');
    new DataRequest(15543107, 15882125, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_OfferLf01.skm');
    new DataRequest(15882125, 16206557, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_PleaBt02.skm');
    new DataRequest(16206557, 16545559, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_PointLf01.skm');
    new DataRequest(16545559, 16715120, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_PointLf02.skm');
    new DataRequest(16715120, 16932841, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_PointRt01.skm');
    new DataRequest(16932841, 17235161, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ReceiveRt01.skm');
    new DataRequest(17235161, 17555872, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_SafeLf01.skm');
    new DataRequest(17555872, 18019854, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ScratchChest01.skm');
    new DataRequest(18019854, 18487638, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ScratchHeadLf01.skm');
    new DataRequest(18487638, 18900288, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ScratchTempleLf01.skm');
    new DataRequest(18900288, 19161881, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ShoulderStretch01.skm');
    new DataRequest(19161881, 19445737, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_Shrug01.skm');
    new DataRequest(19445737, 19507189, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_Shrug02.skm');
    new DataRequest(19507189, 20066962, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_SqueezeHand01.skm');
    new DataRequest(20066962, 20254999, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_StopBt01.skm');
    new DataRequest(20254999, 20387896, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_StopBt02.skm');
    new DataRequest(20387896, 20475339, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_SurroundBt01.skm');
    new DataRequest(20475339, 20552738, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_SurroundBt02.skm');
    new DataRequest(20552738, 21145283, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_Think01.skm');
    new DataRequest(21145283, 21759861, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ToIdle02.skm');
    new DataRequest(21759861, 22028665, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ToIdle03.skm');
    new DataRequest(22028665, 22106264, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ToLocIdle01.skm');
    new DataRequest(22106264, 22721883, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_TouchHands01.skm');
    new DataRequest(22721883, 23351113, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ToWalk01_Turn180Rt01.skm');
    new DataRequest(23351113, 24094126, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ToWalk01_Turn360Rt01.skm');
    new DataRequest(24094126, 24873846, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_ToWalk01_Turn90Rt01.skm');
    new DataRequest(24873846, 25617483, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_WeightShift01.skm');
    new DataRequest(25617483, 26199425, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_WeightShift02.skm');
    new DataRequest(26199425, 26276910, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_WeRt01.skm');
    new DataRequest(26276910, 26347764, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_WeRt02.skm');
    new DataRequest(26347764, 26426973, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_WhenBt01.skm');
    new DataRequest(26426973, 26497876, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_WhenBt02.skm');
    new DataRequest(26497876, 26781699, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_YouLf01.skm');
    new DataRequest(26781699, 26991774, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_YouLf02.skm');
    new DataRequest(26991774, 27176181, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_YouLf03.skm');
    new DataRequest(27176181, 27342276, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_YouPointLf01.skm');
    new DataRequest(27342276, 27526748, 0, 0).open('GET', '/ChrBrad/motions/ChrBrad@Idle01_YouRt01.skm');

    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
      // copy the entire loaded file into a spot in the heap. Files will refer to slices in that. They cannot be freed though.
      var ptr = Module['_malloc'](byteArray.length);
      Module['HEAPU8'].set(byteArray, ptr);
      DataRequest.prototype.byteArray = Module['HEAPU8'].subarray(ptr, ptr+byteArray.length);
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ArmStretch01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatFistMidLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatFistMidLf02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatHighBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatHighBt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatHighBt03.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatHighLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatLowBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatLowBt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatLowLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatLowLf02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatLowRt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatMidBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatMidLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BeatPointMidLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_BesidesLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ChopBoth01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ChopBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ChopLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ChopLf02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ChopRt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_Contemplate01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ExampleLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_Guitar01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_HereBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_HereBt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_HoweverLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_InclusivityNegBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_InclusivityNegBt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_InclusivityPosBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_InclusivityPosBt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_IndicateLeftBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_IndicateLeftLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_IndicateRightBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_IndicateRightRt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_MeLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_MeLf02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_NegativeBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_NegativeBt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_NegativeBt03.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_NegativeRt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_OfferBoth01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_OfferLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_PleaBt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_PointLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_PointLf02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_PointRt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ReceiveRt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_SafeLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ScratchChest01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ScratchHeadLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ScratchTempleLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ShoulderStretch01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_Shrug01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_Shrug02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_SqueezeHand01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_StopBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_StopBt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_SurroundBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_SurroundBt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_Think01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ToIdle02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ToIdle03.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ToLocIdle01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_TouchHands01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ToWalk01_Turn180Rt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ToWalk01_Turn360Rt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_ToWalk01_Turn90Rt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_WeightShift01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_WeightShift02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_WeRt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_WeRt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_WhenBt01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_WhenBt02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_YouLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_YouLf02.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_YouLf03.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_YouPointLf01.skm"].onload();
          DataRequest.prototype.requests["/ChrBrad/motions/ChrBrad@Idle01_YouRt01.skm"].onload();
          Module['removeRunDependency']('datafile_./data-chrbrad-default-motions.data');

    };
    Module['addRunDependency']('datafile_./data-chrbrad-default-motions.data');
  
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
