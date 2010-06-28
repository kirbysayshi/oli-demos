function CLinConstraint(pA, pB){
	this.m_pxPA = pA;			// the two particles
	this.m_pxPB = pB;			// attached
	this.m_fRestLength = V3.length(V3.sub(pA.m_xP1, pB.m_xP1));	// distance the particles should be
}

CLinConstraint.prototype = {
	//------------------------------------------------------------------
	// See if the link is duplicated
	//------------------------------------------------------------------
	Compare: function(xConstraint){
		if (this.m_pxPA == xConstraint.m_pxPA && this.m_pxPB == xConstraint.m_pxPB)
			return true;

		if (this.m_pxPB == xConstraint.m_pxPA && this.m_pxPA == xConstraint.m_pxPB)
			return true;

		return false;
	}
	, Update: function(){
		if (!this.m_pxPA || !this.m_pxPB)
			return;

		//-----------------------------------------------------------
		// cache stuff
		//-----------------------------------------------------------
		var restlength  = this.m_fRestLength;
		var restlength2 = restlength*restlength;
		var invmass1    = this.m_pxPA.m_fInvMass;
		var invmass2    = this.m_pxPB.m_fInvMass;
		var invmass     = invmass1+invmass2;

		if (invmass < 0.00001)
			return;

		//-----------------------------------------------------------
		// relative position of particles
		//-----------------------------------------------------------
		var x1         = this.m_pxPA.m_xP1;
		var x2         = this.m_pxPB.m_xP1;
		var delta      = V3.sub(x2, x1);

		var diff;
		var delta2 = V3.dot(delta, delta);

		//-----------------------------------------------------------
		// Using Square root approximation
		// calcualte the difference from the current distance to the ideal distance
		//-----------------------------------------------------------
		diff  = restlength2/(delta2 + restlength2)-0.5;
		diff *= -2.0;

		//-----------------------------------------------------------
		// move particles so their distance = ideal distance
		//-----------------------------------------------------------
		V3.scale(delta, diff / invmass, delta);
		V3.add(x1, V3.scale(delta, invmass1), x1);
		V3.sub(x2, V3.scale(delta, invmass2), x2);
	}
	, Render: function(ctx){
		if (!this.m_pxPA || !this.m_pxPB)
			return;
		ctx.strokeStyle = "#999999";	
		ctx.beginPath();
		ctx.moveTo(this.m_pxPA.m_xP1[0], this.m_pxPA.m_xP1[1]);
		ctx.lineTo(this.m_pxPB.m_xP1[0], this.m_pxPB.m_xP1[1]);
		ctx.stroke();
	}
};