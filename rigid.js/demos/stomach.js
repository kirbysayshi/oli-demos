function Stomach(){
	
	// world dimensions are a "vector"
	var worldDimensions = V3.$(640, 480);
	// call the parent constructor
	CBasic.call(this, worldDimensions);
	
	// we're not overriding these, so we can use "this"
	this.InitBasicRenderer(640, 480, 60, 1 / 30);
	this.InitBasicScene();
	this.StartBasicRun();
	
}

//---------------------------------------------------------------------
// INHERIT the CBasic scene
//---------------------------------------------------------------------
ChildInheritsParent(Stomach, CBasic);

//---------------------------------------------------------------------
// CLASS METHODS
//---------------------------------------------------------------------
	
//---------------------------------------------------------------------
// setup the stomach world!
//---------------------------------------------------------------------
Stomach.prototype.InitBasicScene = function(){
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

//---------------------------------------------------------------------
// EVENT HANDLERS, these override those found in CBasic
//---------------------------------------------------------------------
Stomach.prototype.BasicKeyHandler = function(e){
	// no need to reproduce existing functionality, call parent!
	CBasic.prototype.BasicKeyHandler.call(this, e);
	// this could also be written as:
	// Stomach.superClass_.BasicKeyHandler.call(this, e);
}
Stomach.prototype.BasicMouseClickHandler = function(e){
	
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
Stomach.prototype.BasicMouseMoveHandler = function(e){
	if (e.shiftKey == true){
		var mouseX = e.clientX - this.canvas.offsetLeft;
		var mouseY = e.clientY - this.canvas.offsetTop;
		
		for(var i = 0; i < this.bodies.m_iNumBodies; i++){
			var b = this.bodies.m_pxBodies[i];
			
			b.AddForce( V3.$( (mouseX - b.m_xBoundingPos[0]) * 3, (mouseY - b.m_xBoundingPos[1]) * 3, 0) );
		}
	}
}

// Export Interface for Closure Compiler Advanced Compilation
window['Stomach'] = Stomach;