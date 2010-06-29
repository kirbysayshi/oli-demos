function CWorld(vWorldDimensions){
	this.worldD = vWorldDimensions;
	
	// Contains every body in the world
	this.bodies = new CBodyList();
}

CWorld.prototype = {
	
	BasicRender: function(){
		this.Update( 1 / 30 );
		this.ctx.clearRect(0, 0, this.viewport[0], this.viewport[1]);
		this.bodies.Render(this.ctx);
		this.fpsDiv.innerHTML = this.fpsMeter.check();
	}
	, Update: function(dt){
		this.bodies.Update(dt, false);
	}
	, InitBasicRenderer: function(width, height, targetFrameRate){
		this.viewport = V3.$(width, height, 0);
		this.targetFrameRate = targetFrameRate;
		this.canvas = document.createElement("canvas");
		this.canvas.setAttribute("width",  width);
		this.canvas.setAttribute("height", height);
		this.ctx = this.canvas.getContext('2d');
		document.getElementsByTagName("body")[0].appendChild(this.canvas);
		
		this.fpsMeter = new FPSMonitor(targetFrameRate);
		
		this.fpsDiv = document.createElement("div");
		this.fpsDiv.id = "fpsMeter";
		document.getElementsByTagName("body")[0].appendChild(this.fpsDiv);
		
		this.InitBasicScene();
	}	
	, InitBasicScene: function(){
		this.bodies.DeleteAll();
		
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
				new CSoftBody(P0, fBodyRadius, iNumParticles, fMass, iRigidity));
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
			this.bodies.NewBody(new CBox(Pos, fSize, fMass, iRigidity));
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
				, Math.random()*(this.dbg_world_size * 1.2)
				, 0);
			var fRadius = Math.random()*(Math.min(this.worldD[0], this.worldD[1]) * 0.1) 
				+ Math.min(this.worldD[0], this.worldD[1]) * 0.05;
        
			this.bodies.NewMesh(new CMeshBlob(P, fRadius, 8, 0.4, false));
		}
		var self = this;
		window.addEventListener("keydown", function(e){ self.BasicKeyHandler(e) }, false);
	}
	, BasicKeyHandler: function(e){
		if (e.keyCode == 27)
			clearInterval(this.INTERVALREFERENCE);

		if (e.keyCode == 32) 
			this.InitBasicScene();
	}
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