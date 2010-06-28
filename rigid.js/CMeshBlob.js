function CMeshBlob(xPos, fMeshRadius, iNumVertices, fRandomness, bInvert){
	CMesh.call(this);
	if(bInvert == undefined) bInvert = false;
	this.SetBlob(xPos, fMeshRadius, iNumVertices, fRandomness, bInvert);
}

ChildInheritsParent(CMeshBlob, CMesh);

CMeshBlob.prototype.SetBlob = function(xPos, fMeshRadius, iNumVertices, fRandomness, bInvert){
	if(bInvert == undefined) bInvert = false;
	if (iNumVertices < 4)
		iNumVertices = 4;

	var V = [];

	fMeshRadius *= 1;

	for(var i = 0; i < iNumVertices; i++)
	{
		var angle = -Math.PI * 2.0 * (i / iNumVertices);

		var fRad = (Math.random()*fMeshRadius * fRandomness) + fMeshRadius * (1.0 - fRandomness);

		V[i] = V3.add(xPos, V3.scale(V3.$(Math.cos(angle), Math.sin(angle), 0), fRad));
	}

	this.SetVertices(V, iNumVertices, bInvert);
}