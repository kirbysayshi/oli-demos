
CBasic.prototype.BasicRender = function(){

	// get new scroll amount
	//V3.add(this.scrollOffset, this.GetScrollAmount(), this.scrollOffset);
	this.scrollOffset = this.GetScrollAmount();
	
	this.bodies.Update( this.deltaT );
	this.ctx.clearRect(0, 0, this.viewport[0], this.viewport[1]);
	this.bodies.Render(this.ctx, this.scrollOffset);
	this.fpsDiv.innerHTML = this.fpsMeter.check();
}

CBodyList.prototype.Render = function(ctx, vOffset) {
	for(var i = 0; i < this.m_iNumBodies; i ++)
	{
		if (this.m_pxBodies[i])
			this.m_pxBodies[i].Render(ctx, vOffset);
	}
	for(var i = 0; i < this.m_iNumMeshes; i ++)
	{
		if (this.m_pxMeshes[i])
			this.m_pxMeshes[i].Render(ctx, vOffset);
	}
}

CBody.prototype.Render = function(ctx, vOffset){
	this.RenderBoundingSphere(ctx, vOffset);
	
	//---------------------------------------------------------------
	// Render particles
	//---------------------------------------------------------------
	for(var i = 0; i < this.m_iNumParticles; i++){ 
		this.m_xParticles[i].Render(ctx, this.m_afColor, vOffset);
	}
	
	//---------------------------------------------------------------
	// Render constraints
	//---------------------------------------------------------------
	for(var i = 0; i < this.m_iNumConstraints; i++){ 
		this.m_xConstraints[i].Render(ctx, vOffset);
	}
}

CBody.prototype.RenderBoundingSphere = function(ctx, vOffset) {		
	// 0.8 * 255 = 204

	ctx.strokeStyle = "rgba(204,204,204,0.3)";
	ctx.beginPath();
	ctx.arc( this.m_xBoundingPos[0] - vOffset[0]
		, this.m_xBoundingPos[1] - vOffset[1]
		, this.m_fBoundingRad, 0, Math.PI*2, false );
	ctx.stroke();
}

//-----------------------------------------------------------
// Render particle, and bounding sphere
//-----------------------------------------------------------
CParticle.prototype.Render = function(ctx, color, vOffset) {

	// bounding sphere
	ctx.fillStyle = "rgba(" 
		+ Math.round(color[0]*255) + "," 
		+ Math.round(color[1]*255) + "," 
		+ Math.round(color[2]*255) + "," 
		+ color[3] + ")";
	ctx.beginPath();
	ctx.arc( this.m_xP0[0] - vOffset[0]
		, this.m_xP0[1] - vOffset[1]
		, this.m_fRadius, 0, Math.PI*2, false );
	ctx.fill();
	
	// position point
	ctx.fillStyle = "rgba(" 
		+ Math.round(color[0]*255) + "," 
		+ Math.round(color[1]*255) + "," 
		+ Math.round(color[2]*255) + "," 
		+ color[3] + ")";
	ctx.beginPath();
	ctx.arc( this.m_xP0[0] - vOffset[0]
		, this.m_xP0[1] - vOffset[1]
		, 2, 0, Math.PI*2, false );
	ctx.fill();

}

CLinConstraint.prototype.Render = function(ctx, vOffset){
	if (!this.m_pxPA || !this.m_pxPB)
		return;
	ctx.strokeStyle = "#999999";	
	ctx.beginPath();
	ctx.moveTo(this.m_pxPA.m_xP1[0] - vOffset[0]
		, this.m_pxPA.m_xP1[1] - vOffset[1]);
	ctx.lineTo(this.m_pxPB.m_xP1[0] - vOffset[0]
		, this.m_pxPB.m_xP1[1] - vOffset[1]);
	ctx.stroke();
}

CMesh.prototype.Render = function(ctx, vOffset){
	ctx.strokeStyle = "rgba(179, 179, 179, 1.0)";
	ctx.lineWidth = 2;
	ctx.beginPath();
	
	// move to the first vertex
	ctx.moveTo(this.m_xVertices[0].m_xPosition[0] - vOffset[0]
		, this.m_xVertices[0].m_xPosition[1] - vOffset[1]);

	var i = 0;
	var j = this.m_iNumVertices-1;
	for(i = 0; i < this.m_iNumVertices; i++){			
		ctx.lineTo(this.m_xVertices[i].m_xPosition[0] - vOffset[0]
			, this.m_xVertices[i].m_xPosition[1] - vOffset[1]);
		if(i == j)
			ctx.lineTo(this.m_xVertices[0].m_xPosition[0] - vOffset[0]
				, this.m_xVertices[0].m_xPosition[1] - vOffset[1]);
	}
	ctx.stroke();
	ctx.lineWidth = 1;
	
	this.RenderBoundingSphere(ctx, vOffset);
}
CMesh.prototype.RenderBoundingSphere = function(ctx, vOffset){
	ctx.strokeStyle = "rgba(0,255,0,0.3)";
	ctx.beginPath();
	ctx.arc( this.m_xBoundingPos[0] - vOffset[0]
		, this.m_xBoundingPos[1] - vOffset[1]
		, this.m_fBoundingRad, 0, Math.PI*2, false );
	ctx.stroke();
}