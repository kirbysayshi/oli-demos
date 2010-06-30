function FPSMonitor(target){	
	this.startTime = new Date().getTime(),
	this.time,
	this.frameTime,
	this.prevFrameTime = new Date().getTime() - this.startTime,
	this.secondTime,
	this.prevSecondTime = new Date().getTime() - this.startTime,
	this.frames = 0,
	this.fps = "...";
	this.targetFPS = target || 60;
	this.deviance = 0;
	
	this.limit = 20,
	this.counter = 0,
	this.last = +new Date(),
	this.lastString = "";
}

FPSMonitor.prototype = {
	check: function(){
		this.time = new Date().getTime() - this.startTime;

		this.frameTime = this.time - this.prevFrameTime;
		this.secondTime = this.time - this.prevSecondTime;
		
		if(this.secondTime >= 1000) {
			this.fps = this.frames.toString();
			this.frames = 0;
			this.prevSecondTime = this.time;
		}
		else {
			this.frames++;
		}
		
		this.prevFrameTime = this.time;
		this.deviance = this.fps / this.targetFPS;
		return ((this.fps + " FPS / ") + this.frameTime) + " MS, Deviance: " + (this.deviance*100) + "%";
	}
	, tick: function(){
		
		if (this.counter++ > this.limit){
			var now = +new Date();
			var _f = 1000 / ((now - this.last) / this.counter);
			this.last = now;
			this.counter = 0;
			this.lastString = 'FPS ' + _f.toFixed(2) + ', ' + ((_f.toFixed(2) / this.targetFPS)*100) + "% of target";
		} 
		return this.lastString;
	}
};