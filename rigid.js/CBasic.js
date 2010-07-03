/**
 * This creates a basic "scene", where something can be easily rendered.
 *
 * @param vWorldDimensions $s Vector Array [0, 0, 0]
 * @return void
 * @author Andrew Petersen
 * @constructor
 */
function CBasic(vWorldDimensions){
	this.worldD = vWorldDimensions;
	
	// Contains every body in the world
	this.bodies = new CBodyList();
}

CBasic.prototype = {
	//---------------------------------------------------------------------
	// Takes one step using some defaults and redraws the screen.
	//---------------------------------------------------------------------
	BasicRender: function(){
		this.bodies.Update( this.deltaT );
		this.ctx.clearRect(0, 0, this.viewport[0], this.viewport[1]);
		this.bodies.Render(this.ctx);
		this.fpsDiv.innerHTML = this.fpsMeter.check();
	}
	
	//---------------------------------------------------------------------
	// creates a basic scene, including canvas and FPS meter
	//---------------------------------------------------------------------
	/**
	 * undocumented function
	 *
	 * @param  string width  desc
	 * @param  string  height  desc
	 * @param  string  targetFrameRate  desc
	 * @param  string  deltaT  desc
	 * @return  void   desc
	 */
	, InitBasicRenderer: function(width, height, targetFrameRate, deltaT){
		this.viewport = V3.$(width, height, 0);
		this.targetFrameRate = targetFrameRate;
		this.deltaT = deltaT;
		this.canvas = document.createElement("canvas");
		this.canvas.setAttribute("width",  width);
		this.canvas.setAttribute("height", height);
		this.ctx = this.canvas.getContext('2d');
		document.getElementsByTagName("body")[0].appendChild(this.canvas);
		
		this.fpsMeter = new FPSMonitor(targetFrameRate);
		
		this.fpsDiv = document.createElement("div");
		this.fpsDiv.id = "fpsMeter";
		document.getElementsByTagName("body")[0].appendChild(this.fpsDiv);
		
		// add listeners
		var self = this;
		window.addEventListener("keydown", function(e){ self.BasicKeyHandler(e) }, false);
		this.canvas.addEventListener("click", function(e){ self.BasicMouseClickHandler(e) }, false);
		this.canvas.addEventListener("mousemove", function(e){ self.BasicMouseMoveHandler(e) }, false);
		
	}
	
	//---------------------------------------------------------------------
	// BEGIN START
	//---------------------------------------------------------------------
	, StartBasicRun: function(){
		var interval = 1000 / this.targetFrameRate;
		var self = this;
		this.INTERVALREFERENCE = setInterval( function(){ self.BasicRender() }, interval );
	}
	
	//---------------------------------------------------------------------
	// EVENT HANDLERS: OVERWRITE THESE
	//---------------------------------------------------------------------
	, BasicKeyHandler: function(e){
		// stop execution permanently (ESC)
		if (e.keyCode == 27)
			clearInterval(this.INTERVALREFERENCE);

		// reset the scene (SPACEBAR)
		if (e.keyCode == 32) 
			this.InitBasicScene();
	}
	, BasicMouseClickHandler: function(e){
		
		// find the body(s) closest to the cursor, and apply -10000x gravity
		var mouseX = e.clientX - this.canvas.offsetLeft;
		var mouseY = e.clientY - this.canvas.offsetTop;
		
	}
	, BasicMouseMoveHandler: function(e){
		if (e.shiftKey == true){
			// do something
		}
	}
}