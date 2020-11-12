(function (){
	var isFirefox = typeof InstallTrigger !== 'undefined';   // Firefox 1.0+
	if(isFirefox){
		overlay = document.createElement('div');
		overlay.style.background = 'rgba(0, 0, 0, .7)';
		overlay.style.top = '0';
		overlay.style.color = '#fff';
		overlay.style.display = 'inline-block';
		overlay.style.fontFamily = 'Arial';
		overlay.style.fontSize = '20px';
		overlay.style.lineHeight = '12px';
		overlay.style.padding = '5px 8px';
		overlay.style.position = 'fixed';
		overlay.style.right = '0';
		overlay.style.zIndex = '1000000';
		overlay.innerHTML = 'FPS: -';
		document.body.appendChild(overlay);

		last_count = window.mozPaintCount;
		last_time = performance.now();

		timeoutFun = function () {
			var curCount, curTime;

			curCount = window.mozPaintCount;
			curTime = performance.now();
			overlay.innerHTML = 'FPS: ' + ((curCount - last_count) / (curTime - last_time) * 1000).toFixed(2);
			last_count = curCount;
			last_time = curTime;
			setTimeout(timeoutFun, 1000);
		};

		setTimeout(timeoutFun, 1000);		
	}
}())