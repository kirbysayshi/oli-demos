function CChain(P0, P1, iNumParticles, iRigidity, fParticleRadius, fParticleMass){
	CBody.call(this, iRigidity);
	this.m_iSelfCollisionStep = 0;
	this.SetColor(0.0, 1.0, 0.3, 0.5);
	this.SetChain(P0, P1, iNumParticles, iRigidity, fParticleRadius, fParticleMass);
}

ChildInheritsParent(CChain, CBody);

CChain.prototype.SetChain = function(P0, P1, iNumParticles, iRigidity, fParticleRadius, fParticleMass){

	if (iNumParticles < 2)
		return;

	if (iRigidity <= 0)
		iRigidity = 1;

	this.SetRigidity(iRigidity);

	//-----------------------------------------------------------
	// copy particles
	//-----------------------------------------------------------
	var P = V3.clone(P0);
	var D = V3.sub(P0, P1);
	D[0] /= iNumParticles;
	D[1] /= iNumParticles;
	D[2] /= iNumParticles;

	for(var i = 0; i < iNumParticles; i++)
	{
		this.AddParticle(new CParticle(P, fParticleRadius, (i != 0)? fParticleMass : 0.0));

		P = V3.add(P, D); // must be  different position obj, not the same!
	}

	//------------------------------------------------------------------
	// link particles together
	//------------------------------------------------------------------
	for(var i = 0; i < iNumParticles-1; i++)
	{
		this.AddConstraint(new CLinConstraint(this.GetParticle(i), this.GetParticle(i+1)));

		P = V3.add(P, D);
	}
	
	this.ComputeBoundingSphere();

	this.m_iSelfCollisionStep = 2;
};

//------------------------------------------------------------------
// collide joints with each other. Skip particles too close
//------------------------------------------------------------------
CChain.prototype.SelfCollide = function(){
	var bCollided = false;

	for(var i = 0; i < this.GetNumParticles(); i++)
	{
		for (var j = i + this.m_iSelfCollisionStep; j < this.GetNumParticles(); j++)
		{
			bCollided |= this.GetParticle(i).Collide(this.GetParticle(j));
		}
	}
	return bCollided;
};