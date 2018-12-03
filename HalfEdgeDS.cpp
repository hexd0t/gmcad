#include "HalfEdgeDS.h"

#include <stdio.h>		// cout
#include <iostream>		// cout

HalfEdgeDS::HalfEdgeDS()
{
	clearDS();
	createDefaultObject();
}	

HalfEdgeDS::~HalfEdgeDS()
{
	clearDS();
}

void HalfEdgeDS::createDefaultObject()
{
	Solid* s;
	Loop* l;
	Vertex* v1;
	Vertex* v2;
	MEVVLS(&s, 0, 0, &l, &v1, &v2, Vec3f(1.f, 0.f, 1.f), Vec3f(1.f, 1.f, 1.f));
	MEV(s, l, v1, nullptr, nullptr, Vec3f(2.f, 0.f, 1.f));
}

void HalfEdgeDS::clearDS()
{
	// for every "new" in this code, we need to call delete. take care, that you delete ALL elements
	for (auto *p : vertices) delete p;
	vertices.clear();
	for (auto *p : halfEdges) delete p;
	halfEdges.clear();
	for (auto *p : edges) delete p;
	edges.clear();
	for (auto *p : loops) delete p;
	loops.clear();
	for (auto *p : faces) delete p;
	faces.clear();
	for (auto *p : solids) delete p;
	solids.clear();
}

void HalfEdgeDS::MEVVLS(Solid ** solid, Edge ** edge, Face ** face, Loop ** loop, Vertex ** vertex1, Vertex ** vertex2, const Vec3f & coords1, const Vec3f & coords2)
{
	//Neue Datenstrukturen allokieren
	Solid* s = new Solid();
	if (solid) *solid = s;

	Vertex* v1 = new Vertex();
	if (vertex1) *vertex1 = v1;

	Vertex* v2 = new Vertex();
	if (vertex2) *vertex2 = v2;

	Loop* l = new Loop();
	if (loop) *loop = l;

	HalfEdge* he1 = nullptr;
	HalfEdge* he2 = nullptr;
	Edge* e = Edge::CreateEdge(&he1, &he2);
	if (edge) *edge = e;

	Face* f = new Face();
	if (face) *face = f;

	//Korrekte verweise anlegen
	v1->coordinates = coords1;
	v2->coordinates = coords2;
	v1->outgoingHE = he1;
	v2->outgoingHE = he2;

	he1->setNextHE(he2);
	he2->setNextHE(he1);
	he1->toLoop = l;
	he2->toLoop = l;
	he1->startV = v1;
	he2->startV = v2;

	l->toHE = he1;
	l->toFace = f;

	f->outerLoop = l;
	f->toSolid = s;

	s->toFace = f;

	//Globale listen pflegen
	this->faces.push_back(f);
	this->solids.push_back(s);
	this->edges.push_back(e);
	this->halfEdges.push_back(he1);
	this->halfEdges.push_back(he2);
	this->vertices.push_back(v1);
	this->vertices.push_back(v2);
}

void HalfEdgeDS::MEV(Solid * solid, Loop * loop, Vertex * vertex1, Edge ** edge, Vertex ** vertex2, const Vec3f & coords2)
{
	//Neue Datenstrukturen allokieren
	Vertex* v2 = new Vertex();
	if (vertex2) *vertex2 = v2;

	HalfEdge* he1 = nullptr;
	HalfEdge* he2 = nullptr;
	Edge* e = Edge::CreateEdge(&he1, &he2);
	if (edge) *edge = e;

	//"Schnitt"-stelle im Loop finden
	HalfEdge* next = loop->findHalfedgeStartingAt(vertex1);
	HalfEdge* prev = next->prevHE;

	//Korrekte verweise anlegen
	v2->coordinates = coords2;
	v2->outgoingHE = he2;

	prev->setNextHE(he1);
	he1->setNextHE(he2);
	he2->setNextHE(next);

	he1->toLoop = loop;
	he2->toLoop = loop;

	he1->startV = vertex1;
	he2->startV = v2;

	this->edges.push_back(e);
	this->halfEdges.push_back(he1);
	this->halfEdges.push_back(he2);
	this->vertices.push_back(v2);
}

std::ostream& operator<< (std::ostream& os, HalfEdgeDS& ds)
{
	os << "Elements in the lists of the data structure:" << std::endl;
	os << "  " << ds.solids.size() << " solids, "
		<< ds.faces.size() << " faces, "
		<< ds.loops.size() << " loops, "
		<< ds.edges.size() << " edges, "
		<< ds.halfEdges.size() << " halfEdges, "
		<< ds.vertices.size() << " vertices.";
	return os;
}
