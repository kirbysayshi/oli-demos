function Scroller(){
	
	CBasic.call(this, V3.$(2400, 480, 0));
	
	this.scrollFocus = {};
	this.scrollOffset = V3.$(0,0,0);
	
	this.ID = new ID(false);
	
	this.InitBasicRenderer(640, 480, 60, 1 / 30);
	this.InitBodies();
	this.StartBasicRun(this);
}

ChildInheritsParent(Scroller, CBasic);

Scroller.prototype.BasicRender = function(){
	
	this.CheckInput(this.deltaT);
	this.scrollFocus.CheckJumpFall();
	
	CBasic.prototype.BasicRender.call(this);
}

Scroller.prototype.InitBodies = function(){
	// build bodies
	
	// create "ground"
	var m = new CMesh();
	var vs = [
		  V3.$(0, 440, 0)
		, V3.$(600, 440, 0)
		, V3.$(1200, 280, 0)
		, V3.$(1200, 440, 0)
		, V3.$(2400, 440, 0)
		, V3.$(2400, 480, 0)
		, V3.$(0, 480, 0)
	];
	m.SetVertices(vs, 7, true);
	this.bodies.NewMesh( m );
	
	//var p = new CRigidBody(2, 1);
	//p.AddParticle( new CParticle( V3.$(1600, 80, 0),  10, 1000, 1 ) );
	//p.AddParticle( new CParticle( V3.$(1700, 80, 0),  10, 1000, 1 ) );
	//p.AddParticle( new CParticle( V3.$(1700, 240, 0), 10, 1000, 1 ) );
	//p.AddParticle( new CParticle( V3.$(1600, 240, 0), 10, 1000, 1 ) );
	//p.SetColor(1.0, 0.3, 0.3, 0.5);
	//p.SetRigidBodyConstraints();
	//this.bodies.NewBody(p);
	
	
	var p1 = new CRigidBody(2, 0.5);
	p1.AddParticle( new CParticle( V3.$(300, 10, 0),  2, 10, 0.5 ) );
	p1.SetColor(1.0, 0.3, 0.3, 0.5);
	p1.SetRigidBodyConstraints();
	this.bodies.NewBody(p1);
	
	var p2 = new CRigidBody(2, 0.5);
	p2.AddParticle( new CParticle( V3.$(300, 50, 0),  10, 10, 0.5 ) );
	p2.SetColor(1.0, 0.3, 0.3, 0.5);
	p2.SetRigidBodyConstraints();
	this.bodies.NewBody(p2);
	
	var p3 = new CRigidBody(2, 0.5);
	p3.AddParticle( new CParticle( V3.$(300, 90, 0),  15, 10, 0.5 ) );
	p3.SetColor(1.0, 0.3, 0.3, 0.5);
	p3.SetRigidBodyConstraints();
	this.bodies.NewBody(p3);
	
	var p4 = new CRigidBody(2, 0.5);
	p4.AddParticle( new CParticle( V3.$(300, 130, 0),  20, 10, 0.5 ) );
	p4.SetColor(1.0, 0.3, 0.3, 0.5);
	p4.SetRigidBodyConstraints();
	this.bodies.NewBody(p4);
	
	//var b1 = new CBox(V3.$(300, 10, 0), 10, 10, 11, 0.5);
	//var b2 = new CBox(V3.$(300, 50, 0), 10, 10, 11, 0.5);
	//var b3 = new CBox(V3.$(300, 90, 0), 10, 10, 11, 0.5);
	//var b4 = new CBox(V3.$(300, 130, 0), 10, 10, 11, 0.5);
	//
	//this.bodies.NewBody(b1);
	//this.bodies.NewBody(b2);
	//this.bodies.NewBody(b3);
	//this.bodies.NewBody(b4);
	
	// create rigid sphere
	//var r = 10;
	//var s = V3.$(100,20,0);
	//var b = new CRigidBody(2, 2);
	//for(var i = 0; i < 8; i++){
	//	var t = (Math.PI * 2.0) * (i / 8);
	//	var xPos = V3.add(s, V3.scale(V3.$(Math.cos(t), Math.sin(t), 0), r));
	//	b.AddParticle( new CParticle( xPos, r, 3.5, 2 ) );
	//}
	//var b = new CBox(V3.$(800, 20, 0), 15, 1, 2, 1);
	//b.SetRigidBodyConstraints();
	//var b = new CSoftBody(V3.$(100, 20, 0), 20, 8, 3.5, 2, 1);
	//b.SetColor(1.0, 0.3, 0.3, 0.5);
	var b = new Hero( V3.$(100, 20, 0) );
	
	this.bodies.NewBody(b);
	
	this.SetScrollFocus(b);
	
	this.bodies.AddWorldForce(V3.$(0, 100, 0)); // gravity
}

Scroller.prototype.GetScrollAmount = function(){
	var amount = V3.$(0,0,0);
	var limit = V3.$( this.worldD[0]-this.viewport[0], this.worldD[1]-this.viewport[1], 0 );
	var diff = V3.sub(this.scrollFocus.GetBoundingPos(), V3.$(this.viewport[0] * 0.5, this.viewport[1] * 0.5, 0));

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
	
	// correct to prevent from scrolling passed world bounds
	if(amount[0] < 0) amount[0] = 0;
	if(amount[1] < 0) amount[1] = 0;
	if(amount[0] > limit[0]) amount[0] = limit[0];
	if(amount[1] > limit[1]) amount[1] = limit[1];
	
	return amount;
}

Scroller.prototype.SetScrollFocus = function(body){
	this.scrollFocus = body;
}

Scroller.prototype.BasicMouseClickHandler = function(e){
	var mouseX = e.clientX - this.canvas.offsetLeft;
	var mouseY = e.clientY - this.canvas.offsetTop;
	var pos = this.scrollFocus.GetBoundingPos();
	var dir = V3.neg(V3.direction(pos, [mouseX, mouseY, 0]));
	
	var p = new CRigidBody(2, 0.5);
	p.AddParticle( new CParticle( V3.add(pos, V3.scale(dir, this.scrollFocus.GetBoundingRad())), 3, 10, 0.5 ) );
	p.SetColor(1.0, 0.3, 0.3, 0.5);
	p.SetRigidBodyConstraints();
	this.bodies.NewBody(p);
	p.AddForce( V3.scale(dir, 10000) );
}

Scroller.prototype.BasicKeyHandler = function(e){
	if (e.keyCode == ID.ESCAPE)
		clearInterval(this.INTERVALREFERENCE);
}

Scroller.prototype.CheckInput = function(dt){
	this.ID.Update(dt);
	
	this.scrollFocus.AddForce(V3.$( this.ID.IsKeyDown(ID.D) ? 150 : 0, 0, 0));
	this.scrollFocus.AddForce(V3.$( this.ID.IsKeyDown(ID.A) ? -150 : 0, 0, 0));
	//if( this.ID.IsNewKeyPress(ID.SPACE) ) this.scrollFocus.Jump();
	if (this.ID.IsKeyDown(ID.SPACE)) this.scrollFocus.Thrust( this.ID.TimePressed(ID.SPACE) );
}