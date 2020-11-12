var defaultInterface = {
	defaultInterface : function (){
		var isMouseDown = false;
		var prePos = {}; 
		var curPos = {};
		var srpi = 3.1415926;
		var camera = scene.getActiveCamera();
		var curMouse;
		//jQuery Mousedown event
		$("#canvas").mousedown(function(event){
			isMouseDown = true;
			curMouse = event.which;
		});
		//jQuery Mousemove event
		$("canvas").mousemove(function(event){
			curPos = defaultInterface.convertToCanvasCoordinates(event.pageX, event.pageY);
			if(typeof(prePos.cx) != 'undefined' && isMouseDown) {
				var dx = (curPos.cx - prePos.cx) * camera.getAspectRatio();
				var dy = (curPos.cy - prePos.cy) / camera.getAspectRatio();
				//event.which: 1 = LB, 2 = MB, 3 = RB
				if(curMouse == 1 && event.altKey){
					var deltaX = -(curPos.cx - prePos.cx) / Module.canvas.width;
					var deltaY = -(curPos.cy - prePos.cy) / Module.canvas.height;
					if (deltaX == 0.0 && deltaY == 0.0)
						return 1;

					var origUp = new Module.SrVec(camera.getUpVector());
					var origCenter = new Module.SrVec(camera.getCenter());
					var origCamera = new Module.SrVec(camera.getEye());

					var dirX = origUp;
					var dirY = new Module.SrVec();
					dirY.cross(origUp, Math.SrVecSub(origCenter, origCamera));
					dirY.normalize();

					var cameraPoint = Module.rotatePoint(origCamera, origCenter, dirX, -deltaX * srpi);
					cameraPoint = Module.rotatePoint(cameraPoint, origCenter, dirY, deltaY * srpi);
					camera.setEye(cameraPoint.getData(0), cameraPoint.getData(1), cameraPoint.getData(2));
				}
				//event = Right button + alt, do dollying
				else if(curMouse == 3 && event.altKey){
					//normalize dx and dy to [-1, 1]
					var amount = dx / Module.canvas.width - dy / Module.canvas.height;
					var cameraPos = new Module.SrVec(camera.getEye());
					var targetPos = new Module.SrVec(camera.getCenter());
					var diff = Math.SrVecSub(targetPos, cameraPos);
					var distance = diff.len();
					diff.normalize();

					if (amount >= distance)
						amount = distance - .000001;
					var adjustment = Math.SrVecScale(diff, distance * amount);
					cameraPos.addVec(adjustment);
					camera.setEye(cameraPos.getData(0), cameraPos.getData(1), cameraPos.getData(2));
				}
				//event = Middle button + alt, do translating
				else if(curMouse == 2 && event.altKey){
					camera.apply_translation_from_mouse_motion ( prePos.cx / Module.canvas.width, -prePos.cy / Module.canvas.height, 
																 curPos.cx / Module.canvas.width, -curPos.cy / Module.canvas.height);
				}
			}
			//store the last position
			prePos = {
				cx : curPos.cx,
				cy : curPos.cy
			}
			
		});
		//jQuery Mouseup event
		$("canvas").mouseup(function(event){
			isMouseDown = false;
		});
	},
	convertToCanvasCoordinates : function(pageX, pageY){
		//pageX and pageY is relative to the top-left of the entire document fileCreatedDate
		var rect   = Module.canvas.getBoundingClientRect();
		return { 
			cx : pageX - rect.left, 
			cy : pageY - rect.top
		};
	}
}
