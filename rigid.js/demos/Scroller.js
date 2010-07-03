function Scroller(){
	
	CBasic.call(this, $(1200, 480, 0));
	
	this.scrollFocus = {};
	this.scrollOffset = V3.$(0,0,0);
	
	this.InitBasicRenderer(320, 240, 60, 1 / 30);
	this.InitBodies();
	this.StartBasicRun(this);
}

ChildInheritsParent(Scroller, CBasic);

Scroller.prototype.BasicRender = function(){

	this.Scroll( this.GetScrollAmount() );
	
	// call parent
	CBasic.prototype.BasicRender.call(this);
}

Scroller.prototype.InitBodies = function(){
	// build bodies
	
	// create "ground"
	var m = new CMesh();
	var vs = [
		  new CVertex(V3.$(0, 440, 0))
		, new CVertex(V3.$(1200, 440, 0))
		, new CVertex(V3.$(1200, 480, 0))
		, new CVertex(V3.$(0, 480, 0))
	];
	m.SetVertices(vs, 4, false);
	this.bodies.NewMesh( m );
	
	// create rigid sphere
	var r = 15;
	var s = V3.$(20,20,0);
	var b = new CRigidBody(2, 0.5);
	for(var i = 0; i < 8; i++){
		var t = (Math.PI * 2.0) * (i / 8);
		var xPos = V3.add(s, V3.scale(V3.$(Math.cos(t), Math.sin(t), 0), r));
		b.AddParticle( new CParticle( xPos, r, 3.5, 0.5 ) );
	}
	b.SetColor(1.0, 0.3, 0.3, 0.5);
	b.SetRigidBodyConstraints();
	this.bodies.NewBody(b);
	
	this.SetScrollFocus(b);
	
	this.bodies.AddWorldForce(V3.$(0, 100, 0)); // gravity
}

Scroller.prototype.Scroll = function(vScroll){
	V3.add(this.scrollOffset, vScroll, this.scrollOffset);
}

Scroller.prototype.GetScrollAmount = function(){
	var xBodyPos = this.scrollFocus.GetBoundingPos();
	var amount = V3.$(0,0,0);
	var limit = V3.$( this.worldD[0]-this.viewport[0], this.worldD[1]-this.viewport[1], 0 );
	var diff = V3.sub(xBodyPos, V3.$(this.viewport[0] * 0.5, this.viewport[1] * 0.5, 0));

	// need to scroll right?
	if( diff[0] > 0.1 ){
		amount[0] += diff[0];
	}
	// need to scroll left?
	if( diff[0] < -0.1 ){
		amount[0] += diff[0];
	}
	// need to scroll up?
	if( diff[1] > 0.1 ){
		amount[1] += diff[1];
	}
	// need to scroll down?
	if( diff[1] < -0.1 ){
		amount[1] += diff[1];
	}
	// correct to prevent from leaving world bounds
	
	return amount;
}

Scroller.prototype.SetScrollFocus = function(body){
	this.scrollFocus = body;
}


Scroller.prototype.BasicKeyHandler = function(e){
	// no need to reproduce existing functionality, call parent!
	
	// this could also be written as:
	// Stomach.superClass_.BasicKeyHandler.call(this, e);
	
	if(e.keyCode == 32){
		
	}
	CBasic.prototype.BasicKeyHandler.call(this, e);
}