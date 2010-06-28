//-------------------------------------------------
// Vertex class. 
// Has a normal as well as a position
// We can use the normal to smooth out collsion response
// when the vertex is 'flat'. 
// also, it avoids objects getting stuck inside meshes,
// as it pushes vertices outward. 
//-------------------------------------------------
function CVertex(xPos, xNorm){
	this.m_xPosition = xPos;
	this.m_xNormal = xNorm == undefined ? V3.$(0,0,0) : xNorm;
}

CVertex.prototype = {
	//-------------------------------------------------
	// collide with a particle
	// simple radius check.
	// then, we push the sphere away. 
	// we can use the normal to smooth out the collsion
	// response, by pushing the sphere along the vertex
	// normal until no penetration. That will avoid 'sticky'
	// collsions of spheres against vertices, where objects
	// 'bump. on flat vertices. 
	//-------------------------------------------------
	Collide: function(xParticle) {
		//-------------------------------------------------
		// radius check
		//-------------------------------------------------
		var D = V3.sub(xParticle.m_xP1, this.m_xPosition);

		var d2 = V3.dot(D, D);

		var r  = xParticle.m_fRadius;
		var r2 = r * r;

		if (d2 > r2)
			return false;

		//if(CVertex.SMOOTH_CORNERS){
		// t either returns false or a float
		var t = VectorUtil.RaySphereIntersection(xParticle.m_xP1, xParticle.m_fRadius, this.m_xPosition, V3.neg(this.m_xNormal), 0.0, 1000.0);
		if (t) {
			return xParticle.StaticCollisionResponse(this.m_xNormal, t);
			
		} else {
			//-------------------------------------------------
			// Calculate the relative direction of article to vertex
			//-------------------------------------------------
			var d = Math.sqrt(d2);

			D[0] /= d;
			D[1] /= d;
			D[2] /= d;

			//-------------------------------------------------
			// Calculate the depth of intersection along that vector
			//-------------------------------------------------
			var depth = r - d;

			//-------------------------------------------------
			// Push the sphere away.
			// as said, you can try to find the depth of intersection 
			// along the vertex normal, and push the sphere that way, 
			// to smooth out response. 
			// but it's simpler to not bother. 
			//-------------------------------------------------
			return xParticle.StaticCollisionResponse(D, depth);
		}
		return true;
	}
};

//-------------------------------------------------
// we can use the normal at vertex to smooth out the collsion
// response, by pushing the sphere along the vertex
// normal until no penetration. That will avoid 'sticky'
// collsions of spheres against vertices, where objects
// bump on flat vertices. 
//-------------------------------------------------
CVertex.SMOOTH_CORNERS = true;