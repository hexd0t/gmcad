#define RADPERDEG 0.0174533

#include "HalfEdgeDSRendering.h"

#include <GL/glut.h>
const float VDIAM = 0.10f;
const float EDIAM = 0.05f;

void renderDS(const HalfEdgeDS& heDS)
{
	for (auto const *v : heDS.getVertices()) renderV(v); // render all vertices as points
	for (auto const *e : heDS.getEdges()) renderE(e); // render all edges as lines
}

void renderE(const Edge* e, const Vec3f& color /*= Vec3f(0.0f, 1.0f, 0.0f)*/)
{
	glColor3f(color.x, color.y, color.z);
	Vec3f p1 = e->he1->startV->coordinates;
	Vec3f p2 = e->he2->startV->coordinates;
	Vec3f dir = p2 - p1;
	float length = dir.length();
	if (length < 0.00001f) return;

	GLUquadricObj *quadObj;

	glPushMatrix();

	glTranslated(p1.x, p1.y, p1.z);

	if ((dir.x != 0.) || (dir.y != 0.)) {
		glRotated(atan2(dir.y, dir.x) / RADPERDEG, 0., 0., 1.);
		glRotated(atan2(sqrt(dir.x*dir.x + dir.y*dir.y), dir.z) / RADPERDEG, 0., 1., 0.);
	}
	else if (dir.z < 0) {
		glRotated(180, 1., 0., 0.);
	}

	quadObj = gluNewQuadric();
	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);
	gluCylinder(quadObj, EDIAM*0.5f, EDIAM*0.5f, length, 16, 1);
	gluDeleteQuadric(quadObj);
	glPopMatrix();
}

void renderHE(const HalfEdge* he, const Vec3f& color /*= Vec3f(0.0f, 1.0f, 0.0f)*/)
{
	// TODO: render the half-edge with the given color
}

void renderV(const Vertex* v, const Vec3f& color /*= Vec3f(1.0f, 0.0f, 1.0f)*/)
{
	glColor3f(color.x, color.y, color.z);
	Vec3f p = v->coordinates;
	GLUquadricObj *quadObj;

	glPushMatrix();
	glTranslated(p.x, p.y, p.z);

	quadObj = gluNewQuadric();
	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);
	gluSphere(quadObj, VDIAM * 0.5f, 16, 16);
	gluDeleteQuadric(quadObj);
	glPopMatrix();
}


void renderHEActive(const HalfEdge* he)
{
	// TODO: render the currently selected half-edge.
	// use renderArrow method to visualize the direction of the half-edge
}

void renderArrow(const Vec3f& p1, const Vec3f& p2, float diameter)
{
	Vec3f dir = p2 - p1;
	float length = dir.length();
	if (length < 0.00001f) return;

	GLUquadricObj *quadObj;

	glPushMatrix ();

	glTranslated(p1.x, p1.y, p1.z);

	if((dir.x!=0.)||(dir.y!=0.)) {
		glRotated(atan2(dir.y,dir.x)/RADPERDEG,0.,0.,1.);
		glRotated(atan2(sqrt(dir.x*dir.x+dir.y*dir.y),dir.z)/RADPERDEG,0.,1.,0.);
	} else if (dir.z<0){
		glRotated(180,1.,0.,0.);
	}

	glTranslatef(0,0,length-4*diameter);

	quadObj = gluNewQuadric ();
	gluQuadricDrawStyle (quadObj, GLU_FILL);
	gluQuadricNormals (quadObj, GLU_SMOOTH);
	gluCylinder(quadObj, 2*diameter, 0.0, 4*diameter, 32, 1);
	gluDeleteQuadric(quadObj);

	quadObj = gluNewQuadric ();
	gluQuadricDrawStyle (quadObj, GLU_FILL);
	gluQuadricNormals (quadObj, GLU_SMOOTH);
	gluDisk(quadObj, 0.0, 2*diameter, 32, 1);
	gluDeleteQuadric(quadObj);

	glTranslatef(0,0,-length+4*diameter);

	quadObj = gluNewQuadric ();
	gluQuadricDrawStyle (quadObj, GLU_FILL);
	gluQuadricNormals (quadObj, GLU_SMOOTH);
	gluCylinder(quadObj, diameter, diameter, length-4*diameter, 32, 1);
	gluDeleteQuadric(quadObj);

	quadObj = gluNewQuadric ();
	gluQuadricDrawStyle (quadObj, GLU_FILL);
	gluQuadricNormals (quadObj, GLU_SMOOTH);
	gluDisk(quadObj, 0.0, diameter, 32, 1);
	gluDeleteQuadric(quadObj);

	glPopMatrix ();
}
