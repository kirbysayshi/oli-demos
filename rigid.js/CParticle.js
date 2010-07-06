function CParticle(xPos, fRadius, Mass, fFriction){
	this.m_xP0 = xPos;						// Vector: Current positions
	this.m_xP1 = xPos;						// Vector: Previous positions
	this.m_xAcceleration = V3.$(0, 0, 0);	// Vector: Force accumulators
	this.m_fMass;							// float: mass
	this.m_fInvMass;							// float: inverse mass
	this.m_fRadius = fRadius;				// float: radius of the particle
	
	this.m_fFriction = fFriction == undefined ? 0.5 : fFriction; // friction to use for collisions
	
	this.SetMass(Mass);
}

CParticle.prototype = {
	//-----------------------------------------------------------
	// Particle has infinite mass, unmovable
	//-----------------------------------------------------------
	IsUnmovable: function()	{ 
		return (this.m_fMass == 0.0);
	}
	//-----------------------------------------------------------
	// Collide two particles together. 
	// simple radius check. If they overlap, push them apart
	//-----------------------------------------------------------
	, Collide: function(xParticle) {
		//-----------------------------------------------------------
		// can't move them
		//-----------------------------------------------------------
		if (this.IsUnmovable() && xParticle.IsUnmovable())
			return false;

		//-----------------------------------------------------------
		// relative distance
		//-----------------------------------------------------------
		var D = V3.sub(this.m_xP1, xParticle.m_xP1);

		var r = this.m_fRadius + xParticle.m_fRadius;

		var d2 = V3.dot(D, D); // float
		var r2 = r*r; // float

		// TODO: if the particles are directly on top of each other, ie their D is 0, a divide by zero will occur

		//-----------------------------------------------------------
		// particles too far apart
		//-----------------------------------------------------------
		if (d2 > r2)
			return false;

		var d = Math.sqrt(d2);

		//D /= d;
		D[0] /= d; // x
		D[1] /= d; // y
		D[2] /= d; // z

		var depth   = (r - d);

		//-----------------------------------------------------------
		// apply response (impact and friction).
		//-----------------------------------------------------------
		return this.CollisionResponse(xParticle, D, depth);

		return true;
	}
	//-----------------------------------------------------------
	// Add force to the particle
	//-----------------------------------------------------------
	, AddForce: function(xForce) { 
		if (this.IsUnmovable()) 
			return; 

		V3.add(this.m_xAcceleration, V3.scale(xForce, this.m_fInvMass), this.m_xAcceleration); 
	}
	//-----------------------------------------------------------
	// Set mass. Check if it is infinite (well, = 0.0f)
	//-----------------------------------------------------------
	, SetMass: function(fMass) { 
		if (fMass < 0.00001)
		{
			this.m_fMass = 0.0;
			this.m_fInvMass = 0.0;
			return;
		}

		this.m_fMass  = fMass;
		this.m_fInvMass = 1.0 / this.m_fMass; 
	}
	, Update: function(dt) {
		//----------------------------------------------------------
		// Stop particle if inmovable
		//----------------------------------------------------------
		if (this.IsUnmovable())
		{
			this.m_xAcceleration = V3.$(0.0, 0.0, 0.0);
			this.m_xP0			 = V3.clone(this.m_xP1);
			return;
		}

		//----------------------------------------------------------
		// Verlet integration
		//----------------------------------------------------------
		var Temp		 		= V3.clone(this.m_xP1);
		V3.add(
			V3.add(
				V3.sub(this.m_xP1, this.m_xP0), 
				V3.scale(this.m_xAcceleration, (dt * dt)) ), 
			this.m_xP1, this.m_xP1);
		this.m_xP0			 	= Temp;
		this.m_xAcceleration  	= V3.$(0,0,0);
	}

	//-----------------------------------------------------------
	// Calculate bounding box around particle (for rigid body bounding boxes)
	//-----------------------------------------------------------
	, GetBoundingBox: function(Min, Max) {
		// FIXME: POSSIBLE PASS BY REFERENCE BREAKAGE
		//Min.x = this.m_xP1[0] - this.m_fRadius;
		//Min.y = this.m_xP1[1] - this.m_fRadius;
		//Min.z = this.m_xP1[2] - this.m_fRadius;
        //
		//Max.x = this.m_xP1[0] + this.m_fRadius;
		//Max.y = this.m_xP1[1] + this.m_fRadius;
		//Max.z = this.m_xP1[2] + this.m_fRadius;
		
		var radius = V3.$(this.m_fRadius, this.m_fRadius, this.m_fRadius);
		V3.sub(this.m_xP1, radius, Min);
		V3.add(this.m_xP1, radius, Max);

	}
	//-----------------------------------------------------------
	// collsion response of collision vs static geometry
	//-----------------------------------------------------------
	, StaticCollisionResponse: function(Ncoll, dcoll) {
		//-----------------------------------------------------------
		// the amount of friction
		//-----------------------------------------------------------
		var friction = this.m_fFriction; // TODO: make this class-level, or even a global const

		//-----------------------------------------------------------
		// Move particle away from plane
		//-----------------------------------------------------------
		V3.add(this.m_xP1, V3.scale(Ncoll, dcoll), this.m_xP1);

		var V = V3.sub(this.m_xP1, this.m_xP0);

		//-----------------------------------------------------------
		// calculate velcity along normal, and collision plane
		//-----------------------------------------------------------
		var Vn = V3.scale(Ncoll, V3.dot(V, Ncoll));
		var Vt = V3.sub(V, Vn);

		//-----------------------------------------------------------
		// apply friction.
		//-----------------------------------------------------------
		V3.sub(this.m_xP1, V3.scale(Vt, friction), this.m_xP1);

		return true;
	}

	//-----------------------------------------------------------
	// collsion response of collision vs particle
	//-----------------------------------------------------------
	, CollisionResponse: function(xParticle, Ncoll, dcoll) {
		//-----------------------------------------------------------
		// the amount of friction
		//-----------------------------------------------------------
		var friction = this.m_fFriction; // TODO: break this out into a class var or global

		//-----------------------------------------------------------
		// Move particle away from plane
		//-----------------------------------------------------------
		var invmass = (this.m_fInvMass + xParticle.m_fInvMass);

		V3.sub(xParticle.m_xP1, 
			V3.scale(Ncoll, (dcoll * xParticle.m_fInvMass)), 
			xParticle.m_xP1);
		V3.add(this.m_xP1, 
			V3.scale(Ncoll, (dcoll * this.m_fInvMass)), 
			this.m_xP1);

		var V0 = V3.sub(this.m_xP1, this.m_xP0);
		var V1 = V3.sub(xParticle.m_xP1, xParticle.m_xP0);
		var V  = V3.sub(V0, V1);

		//-----------------------------------------------------------
		// calculate velcity along normal, and collision plane
		//-----------------------------------------------------------
		var Vn = V3.scale(Ncoll, V3.dot(V, Ncoll));
		var Vt = V3.sub(V, Vn);

		//-----------------------------------------------------------
		// apply friction.
		//-----------------------------------------------------------
		Vt[0] /= invmass; // x
		Vt[1] /= invmass; // y
		Vt[2] /= invmass; // z 

		V3.sub(this.m_xP1, V3.scale(Vt, (friction *	this.m_fInvMass)), this.m_xP1);
		V3.add(xParticle.m_xP1, V3.scale(Vt, friction * xParticle.m_fInvMass), xParticle.m_xP1);

		return true;
	}
	, GetVelocity: function(){
		return V3.sub(this.m_xP1, this.m_xP0);
	}
};