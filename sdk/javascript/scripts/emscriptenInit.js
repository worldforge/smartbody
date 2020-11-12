var statusElement = document.getElementById('status');
var progressElement = document.getElementById('progress');
var spinnerElement = document.getElementById('spinner');
var instruction = document.getElementById('instruction');
var Module = {
	preRun: [],
	postRun: [],
	print: (function() {
	  return function(text) {
		if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
		// These replacements are necessary if you render to raw HTML
		//text = text.replace(/&/g, "&amp;");
		//text = text.replace(/</g, "&lt;");
		//text = text.replace(/>/g, "&gt;");
		//text = text.replace('\n', '<br>', 'g');
		console.log(text);
	  };
	})(),
	printErr: function(text) {
	  if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
	  if (0) { // XXX disabled for safety typeof dump == 'function') {
		dump(text + '\n'); // fast, straight to the real console
	  } else {
		console.error(text);
	  }
	},
	canvas: (function() {
	  var canvas = document.getElementById('canvas');
	  // As a default initial behavior, pop up an alert when webgl context is lost. To make your
	  // application robust, you may want to override this behavior before shipping!
	  // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
	  canvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);
	  return canvas;
	})(),
	setStatus: function(text) {
	  if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
	  //console.log(text + "" + Module.setStatus.last);
	  if (text === Module.setStatus.text) return;
	  var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
	  var now = Date.now();
	  if (m && now - Date.now() < 30) return; // if this is a progress update, skip it if too soon
	  if (m) {
		text = m[1];
		progressElement.value = parseInt(m[2])*100;
		progressElement.max = parseInt(m[4])*100;
		progressElement.hidden = false;
		spinnerElement.hidden = false;
	  } else {
		progressElement.value = null;
		progressElement.max = null;
		progressElement.hidden = true;
		
		if (!text){
			instruction.style.display = 'block';
			spinnerElement.style.visibility = 'hidden';
		}
	  }
	  statusElement.innerHTML = text;
	},
	totalDependencies: 0,
	monitorRunDependencies: function(left) {
	  this.totalDependencies = Math.max(this.totalDependencies, left);
	  Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
	}
};

Module.setStatus('Downloading...');
window.onerror = function(event) {
	// TODO: do not warn on ok events like simulating an infinite loop or exitStatus
	Module.setStatus('Exception thrown, see JavaScript console');
	spinnerElement.style.visibility = 'hidden';
	Module.setStatus = function(text) {
	  if (text) Module.printErr('[post-exception status] ' + text);
	};
};
//file relocation
Module.locateFile = function (name) { 
	var extension = name.substr(name.lastIndexOf('.')+1);
	if(extension == 'data') 
		return '../data/' + name;
	else if(extension == 'mem')
		return '../' + name;
	else return name; 
};
function runCommand(){
	if(typeof scene === 'undefined'){
		alert('Scene is loading...');
	}else{
		var command = document.getElementById("command").value;
		//console.log('echo command:' + command);
		scene.run(command);
	}
}

