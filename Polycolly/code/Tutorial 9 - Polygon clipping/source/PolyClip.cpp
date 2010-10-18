
#include "Vector.h"
#include "PolyClip.h"

Vector* PolyClip::BuildBox(int& iNumVertices, float width, float height)
{
	Vector* axVertices = new Vector[4];
	
	axVertices[0] = Vector(-width/2, -height/2);
	axVertices[1] = Vector( width/2, -height/2);
	axVertices[2] = Vector( width/2,  height/2);
	axVertices[3] = Vector(-width/2,  height/2);
	iNumVertices = 4;
	return axVertices;
}

Vector* PolyClip::BuildBlob(int iNumVertices, float radiusx, float radiusy)
{
	if (iNumVertices == 0)
		return NULL;
	
	Vector* axVertices = new Vector[iNumVertices];

	if (iNumVertices == 1)
	{
		axVertices[0] = Vector(0, 0);
		return axVertices;
	}

	float a = Pi() / iNumVertices;
	float da = -2.0f * Pi() / iNumVertices;

	for(int i = 0; i < iNumVertices; i ++)
	{
		a += da;

		axVertices[i] = Vector(cos(a) * radiusx, sin(a) * radiusy);
	}
	return axVertices;
}

void PolyClip::Transform(const Vector* axVertices, int iNumVertices, Vector* axTargetVertices, const Vector& xOffset, float angle)
{
	 for(int i = 0; i < iNumVertices; i ++)
	 {
	  axTargetVertices[i] = axVertices[i];
	  axTargetVertices[i].Rotate(angle);
	  axTargetVertices[i] += xOffset;
	 }
}
// clip a ray to a polygon
bool PolyClip::ClipSegment(const Vector* A, int Anum, const Vector& PA,
						   const Vector& xStart, const Vector& xEnd, 
						   float& tnear, float& tfar, Vector& Nnear, Vector& Nfar)
{
	if (!A) return false;

	Vector xPos = xStart - PA;
	Vector xDir = xEnd - xStart;
	
	// test separation axes of A
	for(int j = Anum-1, i = 0; i < Anum; j = i, i ++)
	{
		Vector E0 = A[j];
		Vector E1 = A[i];
		Vector E  = E1 - E0;
		Vector En(E.y, -E.x);
		Vector D = E0 - xPos;

		float denom = D * En;
		float numer = xDir * En;

		// ray parallel to plane
		if (fabs(numer) < 1.0E-8f)
		{
			// origin outside the plane, no intersection
			if (denom < 0.0f)
				return false;
		}
		else
		{
			float tclip = denom / numer;

			// near intersection
			if (numer < 0.0f)
			{
				if (tclip > tfar)
					return false;

				if (tclip > tnear)
				{
					tnear = tclip;
					Nnear = En;
					Nnear.Normalise();
				}
			}
			// far intersection
			else
			{
				if (tclip < tnear)
					return false;

				if (tclip < tfar)
				{
					tfar = tclip;
					Nfar = En;
					Nfar.Normalise();
				}
			}
		}
	}

	return true;
}




#include "Perlin.h"

GLuint BindTexture()
{
	enum { width = 128, height = 128 };
	static unsigned char rgba[height][width][4];
	static GLuint texture = -1;
	
	if(!glIsTexture(texture))
	{
		//----------------------------------------------------------------------------------------------- 
		// OR [11/02/2004 ] : compute point positions
		//----------------------------------------------------------------------------------------------- 
		for(int j = 0; j < height; j ++)
		{
			for(int i = 0; i < width; i ++)
			{
				int r = (int)(CPerlin::PerlinNoise_2D(j, i) * 128.0f + 128.0f);
				int g = (int)(CPerlin::PerlinNoise_2D(j + width, i) * 128.0f + 128.0f);
				int b = (int)(CPerlin::PerlinNoise_2D(j + width, i + height) * 128.0f + 128.0f);
				int a = (int)(CPerlin::PerlinNoise_2D(j + width * 2, i) * 128.0f + 128.0f);

				rgba[j][i][0] = clampi(r, 0, 255);
				rgba[j][i][1] = clampi(g, 0, 255);
				rgba[j][i][2] = clampi(b, 0, 255);
				rgba[j][i][3] = clampi(a, 0, 255);
			}
		}
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		float col[] = { 0.3f, 0.3f, 0.3f, 0.3f };
		glTexEnvi	   (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_LUMINANCE);
		glTexEnvfv	   (GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, col);
	}

	return texture;
}

void PolyClip::Render(const Vector& xOffset, float angle, u_int ARGBfill, u_int ARGBline, const Vector* axVertices, int iNumVertices)
{
	if (!axVertices) return;

	if (iNumVertices == 1)
	{
		glColor4ub(ARGB_R(ARGBfill), ARGB_G(ARGBfill), ARGB_B(ARGBfill), ARGB_A(ARGBfill));
		glEnable(GL_POINT_SIZE);
		glPointSize(4.0f);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glTranslatef(xOffset.x, xOffset.y, 0.0f);
			glBegin(GL_POINTS);
				glVertex2f(axVertices[0].x, axVertices[0].y);
			glEnd();
		glPopMatrix();
		glDisable(GL_POINT_SIZE);
		return;
	}

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	float scale = 0.06f;
	glScalef(scale, scale, scale);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, BindTexture());

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(xOffset.x, xOffset.y, 0.0f);

	glRotatef(RadiansToDegrees(angle), 0, 0, -1);

	glColor4ub(ARGB_R(ARGBfill), ARGB_G(ARGBfill), ARGB_B(ARGBfill), ARGB_A(ARGBfill));

	glBegin(GL_TRIANGLE_FAN);

	for(int i = 0; i < iNumVertices; i ++)
	{
		glTexCoord2f(axVertices[i].x, axVertices[i].y);
		glVertex2f(axVertices[i].x, axVertices[i].y);
	}
	glTexCoord2f(axVertices[0].x, axVertices[0].y);
	glVertex2f(axVertices[0].x, axVertices[0].y);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	if (ARGBline != 0)
	{
		glColor4ub(ARGB_R(ARGBline), ARGB_G(ARGBline), ARGB_B(ARGBline), ARGB_A(ARGBline));
		
		glBegin(GL_LINE_LOOP);

		for(int i = 0; i < iNumVertices; i ++)
		{
			glVertex2f(axVertices[i].x, axVertices[i].y);
		}
		glVertex2f(axVertices[0].x, axVertices[0].y);
		glEnd();
	}
	glPopMatrix();
}	


bool PolyClip::PlaneClip(	const Vector* A, int Anum,
	 						Vector* B, int& Bnum,
	 						const Vector& xPlaneNormal, const Vector& xPlaneOrigin)
{
	bool bBack[32];
	bool bBackVerts = false;
	bool bFrontVerts = false;

	for(int i = 0; i < Anum; i ++)
	{
		float side = (A[i] - xPlaneOrigin) * xPlaneNormal;
		
		bBack[i] = (side < 0.0f)? true : false;
		
		bBackVerts |= bBack[i];
		bFrontVerts |= !bBack[i];
	}
	
	if (!bBackVerts)
	{
		Bnum = 0;
	    return false;
    }
    
    if (!bFrontVerts)
    {
		Bnum = Anum;
		for(int i = 0; i < Anum; i ++)	B[i]= A[i];
		return true;
	}

	Bnum = 0;
	int i = Anum-1;
	for(int ip1 = 0; ip1 < Anum; i=ip1, ip1++)
	{
		if(bBack[i])
		{
		    B[Bnum++] = A[i];
		}

	    if (bBack[ip1] ^ bBack[i])
		{
			Vector O = (xPlaneOrigin - A[i]);
			Vector D = (A[ip1] - A[i]);
           	float t = (O * xPlaneNormal) / (D * xPlaneNormal);
			B[Bnum++] = A[i] +  D * t;
		}
	}
	return true;
}

bool PolyClip::PolygonClip(const Vector* axClipperVertices, int iClipperNumVertices,
                           const Vector* axPolygonVertices, int iPolygonNumVertices,
				 	   	   Vector* axClippedPolygon, int& iClippedPolygonNumVertices)
{
 
 Vector Temp[32];
 int TempNum = iPolygonNumVertices;
 memcpy(Temp, axPolygonVertices, sizeof(*Temp) * TempNum);

 int i = iClipperNumVertices-1;
 for(int ip1 = 0; ip1 < iClipperNumVertices; i=ip1, ip1++)
 {
  Vector A0(axClipperVertices[i]);
  Vector A1(axClipperVertices[ip1]);
  Vector D(A1 - A0);
  Vector N(-D.y, D.x);
  Vector O(A0 + D * 0.5f);

  RenderPlane(N, O, 0xFFFFFFFF);
  
  if (!PlaneClip(Temp, TempNum, axClippedPolygon, iClippedPolygonNumVertices, N, O))
  {
   return false;
  }
   TempNum = iClippedPolygonNumVertices;
   memcpy(Temp, axClippedPolygon, sizeof(*Temp) * TempNum);
 }
 return true;
}

