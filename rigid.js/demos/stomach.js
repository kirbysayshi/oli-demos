function CWorld(vWorldDimensions){
	this.worldD = vWorldDimensions;
	
	// Contains every body in the world
	this.bodies = new CBodyList();
}

CWorld.prototype = {
	//---------------------------------------------------------------------
	// Takes one step using some defaults and redraws the screen.
	//---------------------------------------------------------------------
	BasicRender: function(){
		this.Update( this.deltaT );
		this.ctx.clearRect(0, 0, this.viewport[0], this.viewport[1]);
		this.bodies.Render(this.ctx);
		this.fpsDiv.innerHTML = this.fpsMeter.check();
	}
	, Update: function(dt){
		this.bodies.Update(dt);
	}
	//---------------------------------------------------------------------
	// creates a basic scene, including canvas and FPS meter
	//---------------------------------------------------------------------
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
	, InitBasicScene: function(){
		this.bodies.DeleteAll();
		
		this.bodies.worldForces = []; // blank out existing world forces
		this.bodies.AddWorldForce(V3.$(0, 100, 0));
		
		//------------------------------------------------------------------
		// Set the world outer limit
		//------------------------------------------------------------------
		this.bodies.NewMesh(
			new CMeshBlob(
				V3.$(this.worldD[0] / 2, this.worldD[1] / 2, 0), 
				Math.min(this.worldD[0], this.worldD[1]) * 0.5, 
				24, 0.2, true));

		//------------------------------------------------------------------
		// setup a bunch of soft bodies
		//------------------------------------------------------------------
		for(var i = 0; i < 5; i++)
		{
			var P0 = V3.$(
				Math.random() * Math.min(this.worldD[0], this.worldD[1]),
				Math.random() * Math.min(this.worldD[0], this.worldD[1]), 
				0);
			var fBodyRadius = (Math.random() * Math.min(this.worldD[0], this.worldD[1]) / 30.0) 
				+ Math.min(this.worldD[0], this.worldD[1]) / 20.0;
			var iNumParticles = 10;
			var iRigidity = (Math.floor(Math.random() * 32767)) & 1 + 2;
			var fMass = Math.random()*3.0 + 1.0;
			this.bodies.NewBody(
				new CSoftBody(P0, fBodyRadius, iNumParticles, fMass, iRigidity, 0.5));
		}
        
		//------------------------------------------------------------------
		// setup a bunch of boxes
		//------------------------------------------------------------------
		for(var i = 0; i < 4; i++)
		{
			var Pos  = V3.$(
				Math.random()*Math.min(this.worldD[0], this.worldD[1])
				, Math.random()*Math.min(this.worldD[0], this.worldD[1])
				, 0);
			var fSize = Math.random()*(Math.min(this.worldD[0], this.worldD[1]) / 30.0) 
				+ Math.min(this.worldD[0], this.worldD[1]) / 40.0;
			var fMass = Math.random()*3.0  + 1.5;
			var iRigidity = (Math.floor(Math.random() * 32767)) & 1 + 1;
			this.bodies.NewBody(new CBox(Pos, fSize, fMass, iRigidity, 0.1));
		}
        
		//------------------------------------------------------------------
		// setup a bunch of chains
		//------------------------------------------------------------------
		for(var i = 0; i < 3; i++)
		{
			var P0 = V3.$(
				Math.random()*Math.min(this.worldD[0], this.worldD[1])
				, Math.random()*Math.min(this.worldD[0], this.worldD[1])
				, 0);
			var fLength = Math.random()*(Math.min(this.worldD[0], this.worldD[1]) / 10.0) 
				+ Math.min(this.worldD[0], this.worldD[1]) / 6.0;
			var angle = Math.random()*Math.PI * 2.0;
			var P1 = V3.add(P0, V3.scale(V3.$(Math.cos(angle), Math.sin(angle), 0), fLength));
			var iNumParticles = 8;
			var iRigidity = (Math.floor(Math.random() * 32767)) & 1 + 1;
			var fRadius = 0.75 * fLength / (iNumParticles);
			var fMass = Math.random()*3.0 + 1.0;
			this.bodies.NewBody(new CChain(P0, P1, iNumParticles, iRigidity, fRadius, fMass));
		}

		//------------------------------------------------------------------
		// setup a bunch of static meshes
		//------------------------------------------------------------------
		for(var i = 0; i < 6; i++)
		{
			var P = V3.$(
				Math.random()*(Math.min(this.worldD[0], this.worldD[1]) * 1.2)
				, Math.random()*(Math.min(this.worldD[0], this.worldD[1]) * 1.2)
				, 0);
			var fRadius = Math.random()*(Math.min(this.worldD[0], this.worldD[1]) * 0.1) 
				+ Math.min(this.worldD[0], this.worldD[1]) * 0.05;
        
			this.bodies.NewMesh(new CMeshBlob(P, fRadius, 8, 0.4, false));
		}
		
	}
	, BasicKeyHandler: function(e){
		if (e.keyCode == 27)
			clearInterval(this.INTERVALREFERENCE);

		if (e.keyCode == 32) 
			this.InitBasicScene();
	}
	, BasicMouseClickHandler: function(e){
		
		// find the body(s) closest to the cursor, and apply -10000x gravity
		var mouseX = e.clientX - this.canvas.offsetLeft;
		var mouseY = e.clientY - this.canvas.offsetTop;
		
		for(var i = 0; i < this.bodies.m_iNumBodies; i++){
			var b = this.bodies.m_pxBodies[i];
			var d = V3.length( V3.sub( b.m_xBoundingPos, V3.$(mouseX, mouseY, 0) ) );
			if(d <= b.m_fBoundingRad){
				b.AddForce( V3.$(0, -10000, 0) );
				console.log(b);
			}
		}
		
	}
	, BasicMouseMoveHandler: function(e){
		if (e.shiftKey == true){
			var mouseX = e.clientX - this.canvas.offsetLeft;
			var mouseY = e.clientY - this.canvas.offsetTop;
			
			for(var i = 0; i < this.bodies.m_iNumBodies; i++){
				var b = this.bodies.m_pxBodies[i];
				
				b.AddForce( V3.$( (mouseX - b.m_xBoundingPos[0]) * 3, (mouseY - b.m_xBoundingPos[1]) * 3, 0) );
			}
		}
	}
	//---------------------------------------------------------------------
	// Begin running the basic scene
	//---------------------------------------------------------------------
	, StartBasicRun: function(){
		var interval = 1000 / this.targetFrameRate;
		var self = this;
		this.INTERVALREFERENCE = setInterval( function(){ self.BasicRender() }, interval );
	}
};

// Export Interface for Closure Compiler Advanced Compilation
window['CWorld'] = CWorld;
CWorld.prototype['InitBasicRenderer'] = CWorld.prototype.InitBasicRenderer;
CWorld.prototype['StartBasicRun'] = CWorld.prototype.StartBasicRun;
CWorld.prototype['BasicKeyHandler'] = CWorld.prototype.BasicKeyHandler;
CWorld.prototype['InitBasicScene'] = CWorld.prototype.InitBasicScene;