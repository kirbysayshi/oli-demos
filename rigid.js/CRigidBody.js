function CRigidBody(iRigidity, fFriction){
	CBody.call(this, iRigidity, fFriction);
}
//CRigidBody.prototype = new CBody();
//CRigidBody.prototype.constructor = CRigidBody;
ChildInheritsParent(CRigidBody, CBody);

CRigidBody.prototype.SetRigidBodyConstraints = function() {
	//-----------------------------------------------------------
	// link particles to every others
	//-----------------------------------------------------------
	for(var i = 0; i < this.GetNumParticles(); i ++)
	{
		for(var j = i+1; j < this.GetNumParticles(); j ++)
		{
			this.AddConstraint(new CLinConstraint(this.GetParticle(i), this.GetParticle(j)));
		}
	}

	//-----------------------------------------------------------
	// Calculate the bounding volume for the body
	//-----------------------------------------------------------
	this.ComputeBoundingSphere();
};

//--------------------------------------------
// Number of constraints connections for a given 
// number of particles, so that every particle is linked 
// to every other particles.
//--------------------------------------------
CRigidBody.prototype.CalculateNumRigidBodyConstraints = function(iNumParticles)
{
	if (iNumParticles < 2)
		return 0;

	return (iNumParticles-1) + this.CalculateNumRigidBodyConstraints(iNumParticles-1);
}