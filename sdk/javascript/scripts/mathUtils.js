Math.clip = function(number, min, max) {
  return Math.max(min, Math.min(number, max));
}
Math.SrVecAdd = function(v1, v2){
	return  new Module.SrVec(v1.getData(0) + v2.getData(0), v1.getData(1) + v2.getData(1), v1.getData(2) + v2.getData(2));
}
Math.SrVecSub = function(v1, v2){
	return  new Module.SrVec(v1.getData(0) - v2.getData(0), v1.getData(1) - v2.getData(1), v1.getData(2) - v2.getData(2));
}
Math.SrVecScale = function(v1, s){
	return  new Module.SrVec(v1.getData(0) * s, v1.getData(1) * s, v1.getData(2) * s);
}
