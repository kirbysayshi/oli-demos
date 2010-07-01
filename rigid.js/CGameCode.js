function CGameCode(){
	
	this.dbg_UseAttractor = false;
	this.dbg_world_size = 400;
	this.dbg_Attractor = V3.$(0, 0, 0);
	
	//------------------------------------------------------------------
	// the list of bodies
	//------------------------------------------------------------------
	this.gBodies = new CBodyList();
}

// these were originally enums
CGameCode.eNumBoxes = 4;
CGameCode.eNumChains = 3;
CGameCode.eNumSoftBodies = 5;
CGameCode.eNumMeshBlobs = 6;

CGameCode.prototype = {
	GameInit: function() {
		//------------------------------------------------------------------
		// Clean up
		//------------------------------------------------------------------
		this.gBodies.DeleteAll();

		//------------------------------------------------------------------
		// Set the world outer limit
		//------------------------------------------------------------------
		this.gBodies.NewMesh(new CMeshBlob(V3.$(this.dbg_world_size / 2, this.dbg_world_size/2, 0), this.dbg_world_size * 0.8, 24, 0.2, true));

		//var m = new CMesh();
		//var offset = V3.$(50, 50, 0);
        //
		//m.SetVertices([ 
		//	V3.add(V3.$(0,0,0), offset), V3.add(V3.$(400,0,0), offset), 
		//	V3.add(V3.$(400,400,0), offset), V3.add(V3.$(0,400,0), offset) ], 4, true);
        //
		//this.gBodies.NewMesh(m);
		//	
		//this.gBodies.NewBody(new CBox(V3.$(450, 200, 0), 20, 2, 2));

		//------------------------------------------------------------------
		// setup a bunch of soft bodies
		//------------------------------------------------------------------
		for(var i = 0; i < CGameCode.eNumSoftBodies; i++)
		{
			var P0 = V3.$(Math.random() * this.dbg_world_size, Math.random() * this.dbg_world_size, 0);
        
			var fBodyRadius = (Math.random() * this.dbg_world_size / 30.0) + this.dbg_world_size / 20.0;
        
			var iNumParticles = 10;
        
			var iRigidity = (Math.floor(Math.random() * 32767)) & 1 + 2;
        
			var fMass = Math.random()*3.0 + 1.0;
        
			this.gBodies.NewBody(new CSoftBody(P0, fBodyRadius, iNumParticles, fMass, iRigidity));
		}
        
		//------------------------------------------------------------------
		// setup a bunch of boxes
		//------------------------------------------------------------------
		for(var i = 0; i < CGameCode.eNumBoxes; i++)
		{
			var Pos  = V3.$(Math.random()*this.dbg_world_size, Math.random()*this.dbg_world_size, 0);
        
			var fSize = Math.random()*(this.dbg_world_size / 30.0) + this.dbg_world_size / 40.0;
        
			var fMass = Math.random()*3.0  + 1.5;
        
			var iRigidity = (Math.floor(Math.random() * 32767)) & 1 + 1;
        
			this.gBodies.NewBody(new CBox(Pos, fSize, fMass, iRigidity));
		}
        
		//------------------------------------------------------------------
		// setup a bunch of chains
		//------------------------------------------------------------------
		for(var i = 0; i < CGameCode.eNumChains; i++)
		{
			var P0 = V3.$(Math.random()*this.dbg_world_size, Math.random()*this.dbg_world_size, 0);
        
			var fLength = Math.random()*(this.dbg_world_size / 10.0) + this.dbg_world_size / 6.0;
        
			var angle = Math.random()*Math.PI * 2.0;
        
			var P1 = V3.add(P0, V3.scale(V3.$(Math.cos(angle), Math.sin(angle), 0), fLength));
        
			var iNumParticles = 8;
        
			var iRigidity = (Math.floor(Math.random() * 32767)) & 1 + 1;
        
			var fRadius = 0.75 * fLength / (iNumParticles);
        
			var fMass = Math.random()*3.0 + 1.0;
        
			this.gBodies.NewBody(new CChain(P0, P1, iNumParticles, iRigidity, fRadius, fMass));
		}

		//------------------------------------------------------------------
		// setup a bunch of static meshes
		//------------------------------------------------------------------
		for(var i = 0; i < CGameCode.eNumMeshBlobs; i++)
		{
			var P = V3.$(Math.random()*(this.dbg_world_size * 1.2), Math.random()*(this.dbg_world_size * 1.2), 0);
        
			var fRadius = Math.random()*(this.dbg_world_size * 0.1) + this.dbg_world_size * 0.05;
        
			this.gBodies.NewMesh(new CMeshBlob(P, fRadius, 8, 0.4, false));
		}
	}
	, GameUpdate: function(dt) {
		if (this.dbg_UseAttractor)
			this.gBodies.Update(dt, this.dbg_Attractor);
		else
			this.gBodies.Update(dt, false);

		this.dbg_UseAttractor = false;
	}

	, GameRender: function(ctx) {
		this.gBodies.Render(ctx);
	}
	, GameSetAttractor: function(xAttractor)
	{
		this.dbg_Attractor		= xAttractor;
		this.dbg_UseAttractor	= true;
	}
};