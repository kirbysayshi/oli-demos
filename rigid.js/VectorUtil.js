function VectorUtil() {};
// all floats; t0, t1 pass by reference
VectorUtil.FindRoots = function(a, b, c) {
	// FIXME: POSSIBLE PASS BY REFERENCE BREAKAGE
	var d = b*b - (4.0 * a * c);

	if (d < 0.0)
		return false;

	d = Math.sqrt(d);

	if(a == 0)
		return false; // divide by zero check

	var one_over_two_a = 1.0 / (2.0 * a);

	t0 = (-b - d) * one_over_two_a;
	t1 = (-b + d) * one_over_two_a;

	if (t1 < t0)
	{
		var t = t0;
		t0 = t1;
		t1 = t;
	}
	return { t0: t0, t1: t1 };
};

// vector, float, vector, vector, float, float, float
// C, O, D, t should be passed by reference
VectorUtil.RaySphereIntersection = function( C, r, O, D, tmin, tmax ) {
	// FIXME: POSSIBLE PASS BY REFERENCE BREAKAGE
	

	var H = V3.sub(O, C);
	var a = V3.dot(D, D);
	var b = V3.dot(H, D) * 2.0;
	var c = V3.dot(H, H) - (r*r);

	var roots = VectorUtil.FindRoots(a, b, c);
	var t0 = roots.t0, t1 = roots.t1;
	if (!roots)
		return false;

	if (t0 > tmax || t1 < tmin)
		return false;

	var t = t0;

	if (t0 < tmin)
		t = t1;

	return t;
}

VectorUtil.frand = function(x){
	if(x == undefined) x = 1;
	return Math.random() * x;
}
