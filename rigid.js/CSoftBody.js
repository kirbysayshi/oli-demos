function CSoftBody(xPos, fBodyRadius, iNumParticles, fParticleMass, iRigidity){
	CBody.call(this);
	this.SetColor(0.6, 0.6, 1.0, 0.5);
	this.SetSoftBody(xPos, fBodyRadius, iNumParticles, fParticleMass, iRigidity);
}

ChildInheritsParent(CSoftBody, CBody);

CSoftBody.prototype.SetSoftBody = function(xPos, fBodyRadius, iNumParticles, fParticleMass, iRigidity) {
	//-----------------------------------------------------
	// make sure we have sufficient number of particles
	//-----------------------------------------------------
	if (iNumParticles < 4)
		iNumParticles = 4;

	//-----------------------------------------------------
	// make sure we have an even number of particles
	//-----------------------------------------------------
	iNumParticles += iNumParticles % 2 == 0 ? 0 : 1;

	//-----------------------------------------------------
	// example : 8 particles at surface, plus one particle at
	// centre, all particles at surface linked to particles at centre
	// num particles : 8 + 1
	// num constraints : 8 + 8 (8 surface cosntraints + 8 to the centre)
	//-----------------------------------------------------

	this.SetRigidity(iRigidity);

	//-----------------------------------------------------
	// example : 4 particles => r = R * sqrt(2) / 2
	// a bit dodgy. works at the moment
	//-----------------------------------------------------
	var fParticleRadius = (Math.sqrt(2.0) * 0.75) * fBodyRadius * (4 / iNumParticles);

	//-----------------------------------------------------
	// set particles in a circular fashion
	//-----------------------------------------------------
	for(var i = 0; i < iNumParticles; i++)
	{
		var angle = (Math.PI * 2.0) * (i / iNumParticles);

		var fDist = VectorUtil.frand(fBodyRadius * 0.2) + fBodyRadius * 0.8;

		var Pos = V3.add(xPos, V3.scale(V3.$(Math.cos(angle), Math.sin(angle), 0), fDist));

		this.AddParticle( new CParticle(Pos, fParticleRadius, fParticleMass) );
	}

	var iNumConstraints = 0;

	//-----------------------------------------------------
	// set surface links
	//-----------------------------------------------------
	for(var i = 0; i < iNumParticles; i++)
	{
		var j = (i + 1) % iNumParticles;

		this.AddConstraint(new CLinConstraint(this.GetParticle(i), this.GetParticle(j)));
	}

	//-----------------------------------------------------
	// link particles with opposite particle on the body
	//-----------------------------------------------------
	for(var i = 0; i < iNumParticles / 2; i++)
	{
		var j = (i + iNumParticles / 2) % iNumParticles;

		this.AddConstraint( new CLinConstraint(this.GetParticle(i), this.GetParticle(j)) );
	}
};