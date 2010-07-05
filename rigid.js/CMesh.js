function CMesh(){
	this.m_xVertices = [];
	this.m_iNumVertices = 0;
	this.m_fBoundingRad = 0;
	this.m_xBoundingPos = V3.$(0,0,0);
}

CMesh.prototype = {

	//--------------------------------------------------------------------------
	// Set closed loop of vertices. if necessary, invert the winding order
	//--------------------------------------------------------------------------
	SetVertices: function(xVertices, iNumVertices, bInvert) {

		this.m_iNumVertices = iNumVertices;

		//-------------------------------------------------
		// copy the vertices
		//-------------------------------------------------
		for(var i = 0; i < iNumVertices; i ++)
		{
			var j = i;

			if (bInvert)
				j = (iNumVertices - 1 - i);

			this.m_xVertices[j] = new CVertex(xVertices[i]);
		}

		//----------------------------------------
		// compute vertex normals using the 
		// vector from previous vertex to next
		// vertex in list, and taking the perpendicular
		// vector.
		//----------------------------------------
		for(var i = 0; i < iNumVertices; i ++)
		{
			var j = (i - 1);
			var k = (i + 1);
			if (j < 0) j += iNumVertices;
			if (k >= iNumVertices) k -= iNumVertices;

			var P = this.m_xVertices[j].m_xPosition;
			var Q = this.m_xVertices[k].m_xPosition;

			var E = V3.sub(Q, P);
			var N = V3.$(-E[1], E[0], 0);

			V3.normalize(N, N);

			V3.add(this.m_xVertices[i].m_xNormal, N, this.m_xVertices[i].m_xNormal);
		}
		//----------------------------------------
		// Normalize the vertices normals
		//----------------------------------------
		for(var i = 0; i < this.m_iNumVertices; i++)
		{
			V3.normalize(this.m_xVertices[i].m_xNormal, this.m_xVertices[i].m_xNormal);
		}

		this.ComputeBoundingSphere();
	}
	//------------------------------------------------------------------
	// test collsion with a body
	//------------------------------------------------------------------
	, CollideBody: function(xBody) {
		// FIXME: Possible pass by reference error!
		//------------------------------------------------------------------
		// bounding not intersecting
		//------------------------------------------------------------------
		if (!this.BoundingVolumesIntersect(xBody))
			return false;

		var bCollided = false;

		//------------------------------------------------------------------
		// Collide with each particles
		//------------------------------------------------------------------
		for(var i = 0; i < xBody.GetNumParticles(); i++)
		{
			bCollided |= this.CollideParticle(xBody.GetParticle(i));
		}
		return bCollided;
	}
	//------------------------------------------------------------------
	// Collide with particle in a body
	//------------------------------------------------------------------
	, CollideParticle: function(xParticle) {
		// FIXME: Possible pass by reference error (xParticle)
		var bCollided = false;

		var j = this.m_iNumVertices-1;
		for(var i = 0; i < this.m_iNumVertices; j = i, i++)
		{
			// FIXME: Possible pass by reference error
			var V0 = this.m_xVertices[j];
			var V1 = this.m_xVertices[i];

			bCollided |= this.CollideEdge(xParticle, V0, V1);
		}

		return bCollided;
	}
	//------------------------------------------------------------------
	// Collide a particle with an edge
	//------------------------------------------------------------------
	, CollideEdge: function(xParticle, V0, V1)	{
		//------------------------------------------------------------------
		// edge direction
		//------------------------------------------------------------------
		var E = V3.sub(V1.m_xPosition, V0.m_xPosition);

		//------------------------------------------------------------------
		// relative position of particle to edge
		//------------------------------------------------------------------
		var D = V3.sub(xParticle.m_xP1, V0.m_xPosition);

		//------------------------------------------------------------------
		// See where particle's prjection on edge is (before, after, or on edge)
		//------------------------------------------------------------------
		var de = V3.dot(D, E);

		//------------------------------------------------------------------
		// Before : test with the start vertex
		//------------------------------------------------------------------
		if (de < 0.0)
		{
			return V0.Collide(xParticle);
		}

		//------------------------------------------------------------------
		// After : test with the end vertex
		//------------------------------------------------------------------
		var e2 = V3.dot(E, E);

		if (de > e2)
		{
			return V1.Collide(xParticle);
		}

		//-------------------------------------------------
		// On edge : Interpolate to find point closest to 
		// particle on the segment. Interpolate normal as well
		//-------------------------------------------------
		var t = de / e2;		// interpolate
		var u = (1.0 - t);

		//var Pseg = V3.scale(V3.add(V0.m_xPosition, E), t);				// point on segment
		var Pseg = V3.add(V0.m_xPosition, V3.scale(E, t));
		var Nseg = V3.add(V3.scale(V0.m_xNormal, u), V3.scale(V1.m_xNormal, t));	// normal at point on segment

		V3.normalize(Nseg, Nseg);									// normalise

		var Vseg = new CVertex(Pseg, Nseg);							// setup vertex
		
		return Vseg.Collide(xParticle);						// collide
	}
	//--------------------------------------------
	// General bounding sphere around the body 
	// (for culling)
	//--------------------------------------------
	, ComputeBoundingSphere: function()	{
		if (this.m_iNumVertices == 0)
			return;

		var Min = V3.clone(this.m_xVertices[0].m_xPosition);
		var Max = V3.clone(Min);

		for(var i = 1; i < this.m_iNumVertices; i ++)
		{
			var P = V3.clone(this.m_xVertices[i].m_xPosition);

			if (P[0] < Min[0]) Min[0] = P[0];
			if (P[1] < Min[1]) Min[1] = P[1];
			if (P[2] < Min[2]) Min[2] = P[2];

			if (P[0] > Max[0]) Max[0] = P[0];
			if (P[1] > Max[1]) Max[1] = P[1];
			if (P[2] > Max[2]) Max[2] = P[2];
		}
		V3.scale(V3.add(Max, Min), 0.5, this.m_xBoundingPos);
		this.m_fBoundingRad = V3.length(V3.scale(V3.sub(Max, Min), 0.5));
	}
	//-----------------------------------------------------------
	// Checks if two particle's bounding volume intersect
	// simple radius check test
	//-----------------------------------------------------------
	, BoundingVolumesIntersect: function(xBody) {
		var D = V3.sub(this.m_xBoundingPos, xBody.GetBoundingPos());

		var r = this.m_fBoundingRad + xBody.GetBoundingRad();

		var d2 = V3.dot(D, D);
		var r2 = r*r;

		return (d2 < r2);
	}
};