//-----------------------------------------------------------
// simple square box body	
//-----------------------------------------------------------
function CBox(xPos, fSize, fParticleMass, iRigidity){
	CRigidBody.call(this);
	this.SetColor(1.0, 0.3, 0.3, 0.5);
	this.SetBox(xPos, fSize, fParticleMass, iRigidity);
}

ChildInheritsParent(CBox, CRigidBody);

CBox.prototype.SetBox = function(xPos, fSize, fParticleMass, iRigidity) {

	this.SetRigidity(iRigidity);

	var fRad = fSize * Math.sqrt(2.0) / 2.0;
	
	var s = fSize / 2.0;
	
	var Corners = [
		  V3.add(V3.$(-s, -s, 0), xPos)
		, V3.add(V3.$( s, -s, 0), xPos)
		, V3.add(V3.$( s,  s, 0), xPos)
		, V3.add(V3.$(-s,  s, 0), xPos)
	];
	
	//-----------------------------------------------------------
	// simple square box body Some masses will be set to < 0.0f,
	// marking the particle as unmovable.
	//-----------------------------------------------------------
	this.AddParticle(new CParticle(Corners[0], fRad, fParticleMass));
	this.AddParticle(new CParticle(Corners[1], fRad, fParticleMass));
	this.AddParticle(new CParticle(Corners[2], fRad, fParticleMass));
	this.AddParticle(new CParticle(Corners[3], fRad, fParticleMass));

	this.SetRigidBodyConstraints();
}