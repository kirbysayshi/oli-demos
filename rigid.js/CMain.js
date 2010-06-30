//--------------------------------------------------------------------------
// reference for the main interval loop
//--------------------------------------------------------------------------
var INTERVALREFERENCE = 0;

var CTX = null;
var CTXBUFFER = null;

//--------------------------------------------------------------------------
// mouse coords
//--------------------------------------------------------------------------
var mouse_x = 0.0;
var mouse_y = 0.0;
var mouse_b = 0;

//--------------------------------------------------------------------------
// window size
//--------------------------------------------------------------------------
var width  = 640;
var height = 480;

//--------------------------------------------------------------------------
//
// Game interface
//
//--------------------------------------------------------------------------
var GI = new CGameCode();

//--------------------------------------------------------------------------
// FPS meter
//--------------------------------------------------------------------------
var FPSM = new FPSMonitor(100 / 30);
var fps = document.getElementById('fpsMeter');

//-----------------------------------------------------
// the game data
//-----------------------------------------------------
var xAttractor = new V3.$(0, 0, 0);		// attracts the objects

//-----------------------------------------------------
// resets the world with random data
//-----------------------------------------------------
function Init(){
	GI.GameInit();
}

//-----------------------------------------------------
// Update the objects
//-----------------------------------------------------
function Update(dt){
	//-----------------------------------------------------
	// compute attractor position under the mouse cursor
	//-----------------------------------------------------
	xAttractor[0] = (mouse_x / width) * GI.dbg_world_size;
	xAttractor[1] = (mouse_y / height) * GI.dbg_world_size;

	//-----------------------------------------------------
	// set attractor on mouse button, else particles will 
	// fall under gravity
	//-----------------------------------------------------
	if (mouse_b != 0)
		GI.GameSetAttractor(xAttractor);

	GI.GameUpdate(dt);
}

//-----------------------------------------------------
// displays the objects
//-----------------------------------------------------
function Display(ctx, ctxb) {
	//--------------------------------------------------------------------------
	// render stuff
	//--------------------------------------------------------------------------
	ctxb.clearRect(0,0,width,height);
	GI.GameRender(ctxb);
	ctx.clearRect(0,0,width,height);
	ctx.drawImage(ctxb.canvas, 0, 0);
}


function Motion(e) {
	if(e.shiftKey == true){
		mouse_x = e.clientX;
		mouse_y = e.clientY;
		mouse_b = 1;
		//console.log(e.clientX, e.clientY);
	} else {
		mouse_x = e.clientX;
		mouse_y = e.clientY;
		mouse_b = 0;
	}
}

function Timer(t) {
	Update(1.0 / 30);

	Display(CTX, CTXBUFFER);
	fps.innerHTML = FPSM.check();
	INTERVALREFERENCE = setTimeout(Timer, 1000 / 30);
}
	
function Reshape(w, h) {
	width  = w;
	height = h;
	glViewport(	0, 0, w, h);
}

// k == char, x == int, y == int
function Keyboard(k) {
	if (k.keyCode == 27)
		clearInterval(INTERVALREFERENCE);

	if (k.keyCode == 32) 
		Init();
	else
		Update(1.0 / 30.0);
}

// argc == int, argv == char
function main() {
	//--------------------------------------------------------------------------
	// Canvas init
	//--------------------------------------------------------------------------
	var CANVAS = document.createElement("canvas");
	CANVAS.setAttribute("width",  width);
	CANVAS.setAttribute("height", height);
	CTX = CANVAS.getContext('2d'); // global
	document.getElementsByTagName("body")[0].appendChild(CANVAS);
	
	var CANVASBUFFER = document.createElement("canvas");
	CANVASBUFFER.setAttribute("width",  width + 100);
	CANVASBUFFER.setAttribute("height", height + 100);
	CTXBUFFER = CANVASBUFFER.getContext('2d'); // global
	
	document.title = "ping pong";

	// mouse just moving
	//window.addEventListener("mousemove", PassiveMotion, false);
	// mouse moving with button clicked
	CANVAS.addEventListener("mousemove", Motion, false);
	// all keyboard events
	window.addEventListener("keydown", Keyboard, false);
	
	console.log("-----------------------------------");
	console.log("- Soft bodies Verlet Physics Demo -");
	console.log("-----------------------------------");
	console.log("by olivier renault");
	console.log("olivierrenault@hotmail.com");
	console.log("");
	console.log("Demonstrates soft body collison and physics");
	console.log("using particle systems and linear constraints");
	console.log("");
	console.log("features");
	console.log("--------");
	console.log("Verlet integrator, and stiff linear constraints.");
	console.log("particle collisions and mesh collisions.");
	console.log("friction and collision response.");
	console.log("soft bodies, hard bodies, chains.");
	console.log("");
	console.log("controls");
	console.log("--------");
	console.log("- keep mouse button pressed, and move mouse to");
	console.log("  attract bodies");
	console.log("- release button for normal gravity mode");
	console.log("- press space bar to initialise");
	console.log("- press esc to shutdown");
	
	Init();
	INTERVALREFERENCE = setTimeout(Timer, 1000 / 30);
}

main();