function CBodyList(){
	this.m_pxBodies = []; // array of CBody(s)
	this.m_iNumBodies = 0;

	this.m_pxMeshes = []; // array of CMesh(s)
	this.m_iNumMeshes = 0;
}

CBodyList.prototype = {
	DeleteAll: function(){
		while (this.m_iNumBodies > 0)
			this.DeleteBody(this.m_pxBodies[0]);

		while (this.m_iNumMeshes > 0)
			this.DeleteMesh(this.m_pxMeshes[0]);
	}
	, DeleteBody: function(pxBody) {
		if (pxBody == undefined)
			return;

		for(var i = 0; i < this.m_iNumBodies; i++)
		{
			if (this.m_pxBodies[i] == pxBody)
			{
				this.m_pxBodies[i] = undefined; // "delete"
				
				this.m_iNumBodies--;
				this.m_pxBodies[i] = this.m_pxBodies[this.m_iNumBodies];
				this.m_pxBodies[this.m_iNumBodies] = undefined;
				return;
			}
		}
	}
	, NewBody: function(pxBody) {
		if (this.m_iNumBodies >= CBodyList.eMaxBodies)
			return;

		this.m_pxBodies[this.m_iNumBodies] = pxBody;

		this.m_iNumBodies++;
	}
	, DeleteMesh: function(pxMesh){
		if (pxMesh == undefined)
			return;

		for(var i = 0; i < this.m_iNumMeshes; i++)
		{
			if (this.m_pxMeshes[i] == pxMesh)
			{
				this.m_pxMeshes[i] = undefined; // "delete"

				this.m_iNumMeshes--;
				this.m_pxMeshes[i] = this.m_pxMeshes[this.m_iNumMeshes];
				this.m_pxMeshes[this.m_iNumMeshes] = undefined;
				return;
			}
		}
	}
	, NewMesh: function(pxMesh) {
		if (this.m_iNumMeshes >= CBodyList.eMaxMeshes)
			return;

		this.m_pxMeshes[this.m_iNumMeshes] = pxMesh;

		this.m_iNumMeshes++;
	}
	//-----------------------------------------------------------
	// update all bodies. Move their particles, apply 
	// collision constraints, then stiff cosntraints
	//-----------------------------------------------------------
	, Update: function(dt, pxAttractor) {

		//-----------------------------------------------------------
		// either attract the bodies to a position, or use gravity
		//-----------------------------------------------------------
		for(var i = 0; i < this.m_iNumBodies; i++)
		{
			if (this.m_pxBodies[i])
				this.m_pxBodies[i].AttractParticles(pxAttractor);
		}


		for(var i = 0; i < this.m_iNumBodies; i++)
		{
			if (this.m_pxBodies[i])
				this.m_pxBodies[i].UpdateParticles(dt);
		}

		for(var i = 0; i < this.m_iNumBodies; i++)
		{
			if (this.m_pxBodies[i] == undefined)
				continue;

			//---------------------------------------------
			// Do special self collisions
			//---------------------------------------------
			this.m_pxBodies[i].SelfCollide();

			for(var j = i+1; j < this.m_iNumBodies; j ++)
			{
				if (this.m_pxBodies[j] == undefined)
					continue;

				this.m_pxBodies[i].Collide(this.m_pxBodies[j]);
			}
		}

		//-----------------------------------------------------------
		// Collide with static geometry
		//-----------------------------------------------------------
		for(var i = 0; i < this.m_iNumBodies; i ++)
		{
			if (this.m_pxBodies[i] == undefined)
				continue;

			for(var j = 0; j < this.m_iNumMeshes; j ++)
			{
				if (this.m_pxMeshes[j] == undefined)
					continue;

				this.m_pxMeshes[j].CollideBody(this.m_pxBodies[i]);
			}
		}

		//------------------------------------------------------------------
		// Update constraints
		//------------------------------------------------------------------
		for(var i = 0; i < this.m_iNumBodies; i ++)
		{
			if (this.m_pxBodies[i])
			{
				this.m_pxBodies[i].UpdateConstraints();
			}
		}

		//------------------------------------------------------------------
		// Update the bounding volume
		//------------------------------------------------------------------
		for(var i = 0; i < this.m_iNumBodies; i ++)
		{
			if (this.m_pxBodies[i])
			{
				this.m_pxBodies[i].UpdateBoundingVolume();
			}
		}
	}
	, Render: function(ctx) {
		for(var i = 0; i < this.m_iNumBodies; i ++)
		{
			if (this.m_pxBodies[i])
				this.m_pxBodies[i].Render(ctx);
		}
		for(var i = 0; i < this.m_iNumMeshes; i ++)
		{
			if (this.m_pxMeshes[i])
				this.m_pxMeshes[i].Render(ctx);
		}
	}
};

// these are technically supposed to be enums
CBodyList.eMaxMeshes = 256;
CBodyList.eMaxBodies = 256;