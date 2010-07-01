function CBody(iRigidity, fFriction){
	//--------------------------------------------
	// particles and constraints on a body
	//--------------------------------------------
	this.m_xParticles = []; // array of particles
	this.m_xConstraints = []; // array of constraints
	this.m_iNumParticles = 0;
	this.m_iNumConstraints = 0;
	this.m_iMaxParticles = 100;
	this.m_iMaxConstraints = 100;
	this.m_iRigidity = typeof(iRigidity) === "undefined" ? this.m_iRigidity = 1 : iRigidity;
	this.m_fFriction = typeof(fFriction) === "undefined" ? this.m_fFriction = 0.5: fFriction;

	//--------------------------------------------
	// General bounding sphere around the body 
	// (for culling)
	//--------------------------------------------
	this.m_fBoundingRad = 0;
	this.m_xBoundingPos = V3.$(0,0,0);
	this.m_afColor = [];
	this.m_afColor[0] = 0.0; // R
	this.m_afColor[1] = 1.0; // G
	this.m_afColor[2] = 0.0; // B
	this.m_afColor[3] = 0.5; // A
	
	this.forces = []; // each step the forces here are applied and removed from the queue
}

CBody.prototype = {
	// this should be used in place of the = operator
	//-----------------------------------------------------------
	// Copy a rigid body, NOTE: THIS IS THE ONLY JQUERY DEPENDENCY
	//-----------------------------------------------------------
	Copy: function(xBody){
		this.m_iRigidity = (xBody.m_iRigidity <= 0) ? 1 : xBody.m_iRigidity;
		this.m_fFriction = xBody.m_iRigidity;

		//-----------------------------------------------------------
		// Allocate memory space
		//-----------------------------------------------------------
		// HA HA HA HA HA HA HA ALLOCATION.

		//-----------------------------------------------------------
		// copy particles
		//-----------------------------------------------------------
		for(var i = 0; i < xBody.m_iNumParticles; i++) {
			this.AddParticle( $.extend({}, xBody.m_xParticles[i]) );
		}

		//-----------------------------------------------------------
		// Copy constraints
		//-----------------------------------------------------------
		for(var i = 0; i < xBody.m_iNumConstraints; i ++) {
			var pxConstraint = $.extend({}, xBody.m_xConstraints[i]);

			//-----------------------------------------------------------
			// the indices of the particles in the constraint
			//-----------------------------------------------------------
			var i0 = xBody.FindParticle(pxConstraint.m_pxPA);
			var i1 = xBody.FindParticle(pxConstraint.m_pxPB);

			if (i0 > xBody.m_iNumParticles || i0 < 0)
				continue;

			if (i1 > xBody.m_iNumParticles || i1 < 0)
				continue;

			this.AddConstraint( new CLinConstraint( this.m_xParticles[i0], this.m_xParticles[i1] ) );
		}

		return this;
	}
	, RenderBoundingSphere: function(ctx) {		
		// 0.8 * 255 = 204

		ctx.strokeStyle = "rgba(204,204,204,0.3)";
		ctx.beginPath();
		ctx.arc( this.m_xBoundingPos[0], this.m_xBoundingPos[1], this.m_fBoundingRad, 0, Math.PI*2, false );
		ctx.stroke();
	}
	, Render: function(ctx){
		this.RenderBoundingSphere(ctx);
		
		//---------------------------------------------------------------
		// Render particles
		//---------------------------------------------------------------
		for(var i = 0; i < this.m_iNumParticles; i++){ 
			this.m_xParticles[i].Render(ctx, this.m_afColor);
		}
		
		//---------------------------------------------------------------
		// Render constraints
		//---------------------------------------------------------------
		for(var i = 0; i < this.m_iNumConstraints; i++){ 
			this.m_xConstraints[i].Render(ctx);
		}
	}
	, AddForce: function(force){
		this.forces.push(force);
	}
	, ApplyForces: function(worldForces){
		//-----------------------------------------------------------
		// apply all forces acting on the body
		//-----------------------------------------------------------
		for(var i = 0; i < this.m_iNumParticles; i++)
		{
			var p = this.m_xParticles[i];
			var j = 0;
			// apply world forces
			for(j = 0; j < worldForces.length; j++){
				p.AddForce( V3.scale( worldForces[j] , p.m_fMass) );
			}
			
			// apply body forces
			for(j = 0; j < this.forces.length; j++){
				p.AddForce( V3.scale( this.forces[j] , p.m_fMass) );
			}
		}
		
		this.forces = []; // blank it out
	}
	//-----------------------------------------------------------
	// Update particle's estimated position		
	//-----------------------------------------------------------
	, UpdateParticles: function(dt){
		for(var i=0; i< this.m_iNumParticles; i++) 
		{
			this.m_xParticles[i].Update(dt);
		}
	}
	//-----------------------------------------------------------
	// Checks if two particle's bounding volume intersect
	// simple radius check test
	//-----------------------------------------------------------
	, BoundingVolumesIntersect: function(xBody) {
		var D = V3.sub(this.m_xBoundingPos, xBody.m_xBoundingPos);

		var r = this.m_fBoundingRad + xBody.m_fBoundingRad;

		var d2 = V3.dot(D, D);
		var r2 = r*r;

		return (d2 < r2);
	}
	
	//-----------------------------------------------------------
	// collide with self
	//-----------------------------------------------------------
	, SelfCollide: function(){
		return false;
	}
	
	//-----------------------------------------------------------
	// collide two bodies. Shift particles around so they stop intersecting
	//-----------------------------------------------------------
	, Collide: function(xBody){
		if (!this.BoundingVolumesIntersect(xBody))
			return false;

		var bCollided = false;

		for(var i=0; i< this.m_iNumParticles; i++) 
		{
			for(var j = 0; j < xBody.m_iNumParticles; j ++)
			{
				if (this.m_xParticles[i].Collide(xBody.m_xParticles[j]))
					bCollided = true;
			}
		}
		return bCollided;
	}

	//-----------------------------------------------------------
	// update the constraints. run the iterations several times
	//-----------------------------------------------------------
	, UpdateConstraints: function(){
		for(var iter = 0; iter < this.m_iRigidity; iter++)
		{
			//-----------------------------------------------------------
			// Apply stiff constraint
			//-----------------------------------------------------------
			for(var i = 0; i < this.m_iNumConstraints; i++)
			{
				this.m_xConstraints[i].Update();
			}
		}
	}
	//--------------------------------------------
	// update the bounding volume for culling
	//--------------------------------------------
	, UpdateBoundingVolume: function() {
		this.ComputeBoundingSphere();
	}
	, AddParticle: function(xParticle)	{
		if (this.m_iNumParticles >= this.m_iMaxParticles)
			return;

		this.m_xParticles[this.m_iNumParticles++] = xParticle;
	}
	, AddConstraint: function(xConstraint) {
		if (this.m_iNumConstraints >= this.m_iMaxConstraints)
			return;

		if (this.FindConstraint(xConstraint))
			return;

		this.m_xConstraints[this.m_iNumConstraints++] = xConstraint;
	}
	, FindConstraint: function(xConstraint) {
		for(var i = 0; i < this.m_iNumConstraints; i++)
		{
			if (this.m_xConstraints[i] == xConstraint)
				return true;
		}
		return false;
	}
	//----------------------------------------------------------
	// returns the index of the particle if found, -1 otherwise
	//----------------------------------------------------------
	, FindParticle: function(xParticle){
		var index;
		for(var i = 0; i < this.m_iNumParticles; i++){
			if(this.m_xParticles[i] == xParticle){
				index = i;
			}
		}
		return index == undefined ? -1 : index;
	}
	, GetParticle: function(i) { return this.m_xParticles[i]; }
	, GetConstraint: function(i) { return this.m_xConstraints[i]; }
	, GetNumParticles: function() { return this.m_iNumParticles; }
	, GetNumConstraints: function() { return this.m_iNumConstraints; }
	, GetRigidity: function() { return this.m_iRigidity; }
	, SetRigidity: function(iRigidity) { this.m_iRigidity = (iRigidity < 1) ? 1 : iRigidity; }
	, GetBoundingPos: function() { return this.m_xBoundingPos; }
	, GetBoundingRad: function() { return this.m_fBoundingRad; }
	, ComputeBoundingSphere: function()	{
		if (this.m_iNumParticles <= 0)
			return;

		var Min = {}, Max = {};
		this.m_xParticles[0].GetBoundingBox(Min, Max);

		for(var i = 1; i < this.m_iNumParticles; i ++)
		{
			var Pmin = {}, Pmax = {};
			this.m_xParticles[i].GetBoundingBox(Pmin, Pmax);

			if (Pmin[0] < Min[0]) Min[0] = Pmin[0];
			if (Pmin[1] < Min[1]) Min[1] = Pmin[1];
			if (Pmin[2] < Min[2]) Min[2] = Pmin[2];
                                               
			if (Pmax[0] > Max[0]) Max[0] = Pmax[0];
			if (Pmax[1] > Max[1]) Max[1] = Pmax[1];
			if (Pmax[2] > Max[2]) Max[2] = Pmax[2];
		}

		V3.scale(V3.add(Max, Min), 0.5, this.m_xBoundingPos);
		this.m_fBoundingRad = V3.length(V3.scale(V3.sub(Max, Min), 0.5));
	}
	, SetColor: function(r, g, b, a){
		this.m_afColor[0] = r;
		this.m_afColor[1] = g; 
		this.m_afColor[2] = b; 
		this.m_afColor[3] = a;
	}
};