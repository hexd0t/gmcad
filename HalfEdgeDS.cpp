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
	Vertex* v3;
	Edge* e;
	MEVVLS(&s, nullptr, nullptr, &l, &v1, &v2, Vec3f(1.f, 0.f, 1.f), Vec3f(1.f, 1.f, 1.f));
	MEV(s, l, v1, nullptr, &v3, Vec3f(2.f, 0.f, 1.f));
	MEL(s, l, v2, v3, &e, nullptr, nullptr);
	MVE(s, e, nullptr, nullptr, Vec3f(1.5f, 0.5f, 1.f));
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

void HalfEdgeDS::MEVVLS(Solid ** solid, Face** face, Edge ** edge, Loop ** loop, Vertex ** vertex1, Vertex ** vertex2, const Vec3f & coords1, const Vec3f & coords2)
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

	//Korrekte Verweise anlegen
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

	//Globale Listen pflegen
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
	/******************************************************\
	 *  Vorher:              *  Nacher:                   *
	 *     ╖ prevHE          *     ╖ prevHE               *
	 *     ╙                 *     ╙          he1         *
	 *  V1 ○                 *  V1 ○╒═══════════════╛● V2 *
	 *     ╖                 *     ╖          he2         *
	 *     ╙ nextHE          *     ╙ nextHE               *
	\******************************************************/

	//"Schnitt"-stelle im Loop finden
	HalfEdge* nextHE = loop->findHalfedgeStartingAt(vertex1);
	if (!nextHE)
		return;
	HalfEdge* prevHE = nextHE->prevHE;

	//Neue Datenstrukturen allokieren
	Vertex* v2 = new Vertex();
	if (vertex2) *vertex2 = v2;

	HalfEdge* he1 = nullptr;
	HalfEdge* he2 = nullptr;
	Edge* e = Edge::CreateEdge(&he1, &he2);
	if (edge) *edge = e;

	//Korrekte Verweise anlegen
	v2->coordinates = coords2;
	v2->outgoingHE = he2;

	prevHE->setNextHE(he1);
	he1->setNextHE(he2);
	he2->setNextHE(nextHE);

	he1->toLoop = loop;
	he2->toLoop = loop;

	he1->startV = vertex1;
	he2->startV = v2;

	//Globale Listen pflegen
	this->edges.push_back(e);
	this->halfEdges.push_back(he1);
	this->halfEdges.push_back(he2);
	this->vertices.push_back(v2);
}

void HalfEdgeDS::MEL(Solid * solid, Loop * loop1, Vertex * vertex1, Vertex * vertex2, Edge ** edge, Loop ** loop2, Face** face)
{
	/**********************************************************************\
	 *  Vorher:                        *  Nacher:                         *
	 *     ╖ v1prev       v2next ╖     *     ╖ v1prev         v2next ╖    *
	 *     ╙                     ╙     *     ╙          he1          ╙    *
	 *  V1 ○                     ○ V2  *  V1 ○╒═════════════════════╛○ V2 *
	 *     ╖                     ╖     *     ╖          he2          ╖    *
	 *     ╙ v1next       v2prev ╙     *     ╙ v1next         v2prev ╙    *
	\**********************************************************************/

	//"Schnitt"-stellen im Loop finden
	HalfEdge* v1nextHE = loop1->findHalfedgeStartingAt(vertex1);
	HalfEdge* v2nextHE = loop1->findHalfedgeStartingAt(vertex2);
	if (!v1nextHE || !v2nextHE)
		return;
	HalfEdge* v1prevHE = v1nextHE->prevHE;
	HalfEdge* v2prevHE = v2nextHE->prevHE;

	//Neue Datenstrukturen allokieren
	HalfEdge* he1 = nullptr;
	HalfEdge* he2 = nullptr;
	Edge* e = Edge::CreateEdge(&he1, &he2);
	if (edge) *edge = e;

	Loop* l2 = new Loop();
	if (loop2) *loop2 = l2;

	Face* f = new Face();

	//Korrekte Verweise anlegen
	l2->toFace = f;
	l2->nextLoop = loop1->nextLoop;
	l2->nextLoop->prevLoop = l2;
	loop1->nextLoop = l2;
	l2->prevLoop = loop1;

	f->toSolid = solid;
	f->outerLoop = l2;

	v1prevHE->setNextHE(he1);
	he1->setNextHE(v2nextHE);
	v2prevHE->setNextHE(he2);
	he2->setNextHE(v1nextHE);

	he1->startV = vertex1;
	he2->startV = vertex2;
	he1->toLoop = l2;

	l2->toHE = he2;
	for (HalfEdge* iter = he2; iter != he2; iter = iter->nextHE) {
		iter->toLoop = l2;
	}

	//Globale Listen pflegen
	this->edges.push_back(e);
	this->halfEdges.push_back(he1);
	this->halfEdges.push_back(he2);
	this->loops.push_back(l2);
	this->faces.push_back(f);
}

void HalfEdgeDS::MVE(Solid * solid, Edge * edge1, Vertex ** vertex, Edge ** edge2, const Vec3f & coords)
{
	/*************************************************************\
	 *  Vorher:                    *  Nacher:                    *
	 *    ╖                   ╖    *    ╖                   ╖    *
	 *    ╙       e1he1       ╙    *    ╙  e1he1  V  e2he1  ╙    *
	 *    ○╒═════════════════╛○    *    ○╒═══════╛●╒═══════╛○    *
	 *    ╖       e1he2       ╖    *    ╖  e1he2     e2he2  ╖    *
	 *    ╙                   ╙    *    ╙                   ╙    *
	\*************************************************************/
	//Neue Datenstrukturen allokieren
	Vertex* v = new Vertex();
	if (vertex) *vertex = v;

	HalfEdge* e2he1 = nullptr;
	HalfEdge* e2he2 = nullptr;
	Edge* e2 = Edge::CreateEdge(&e2he1, &e2he2);
	if (edge2) *edge2 = e2;

	HalfEdge* e1he1 = edge1->he1;
	HalfEdge* e1he2 = edge1->he2;

	//Korrekte Verweise anlegen
	e2he1->setNextHE(e1he1->nextHE);
	e1he1->setNextHE(e2he1);
	e1he2->prevHE->setNextHE(e2he2);
	e2he2->setNextHE(e1he2);

	e2he2->startV = e1he2->startV;
	e2he1->startV = v;
	e1he2->startV = v;

	v->coordinates = coords;
	v->outgoingHE = e1he2;
	if (e2he2->startV->outgoingHE == e1he2) {
		e2he2->startV->outgoingHE = e2he2;
	}

	//Globale Listen pflegen
	this->edges.push_back(e2);
	this->halfEdges.push_back(e2he1);
	this->halfEdges.push_back(e2he2);
	this->vertices.push_back(v);
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
