function FPSMonitor(target){
	this.startTime = new Date().getTime(),
	this.time,
	this.frameTime,
	this.prevFrameTime = this.getTimer(),
	this.secondTime,
	this.prevSecondTime = this.getTimer(),
	this.frames = 0,
	this.fps = "...";
	this.targetFPS = target || 60;
	this.deviance = 0;
}

FPSMonitor.prototype = {
	check: function(){
		this.time = this.getTimer();

		this.frameTime = this.time - this.prevFrameTime;
		this.secondTime = this.time - this.prevSecondTime;
		
		if(this.secondTime >= 1000) {
			this.fps = this.frames.toString();
			this.frames = 0;
			this.prevSecondTime = this.time;
		}
		else
		{
			this.frames++;
		}
		
		this.prevFrameTime = this.time;
		this.deviance = this.fps / this.targetFPS;
		return ((this.fps + " FPS / ") + this.frameTime) + " MS, Deviance: " + this.deviance + "%";
	},
	getTimer: function(){
		return new Date().getTime() - this.startTime;
	}
};