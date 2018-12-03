#include "HalfEdgeDSElements.h"

#include <iostream>
#include <iomanip>

Solid::Solid()
	: toFace(nullptr)
{
}
	
// TODO: create methods for creating and traversing its elements


Face::Face()
	: toSolid(nullptr)
	, outerLoop(nullptr)
	, innerLoop(nullptr)
{
}
	
// TODO: create methods for creating and traversing its elements


Loop::Loop()
	: toFace(nullptr)
	, nextLoop(this)
	, prevLoop(this)
	, toHE(nullptr)
{
}

HalfEdge * Loop::findHalfedgeStartingAt(Vertex * v)
{
	HalfEdge* currentHE = this->toHE;
	if (currentHE->startV == v)
		return currentHE;
	do {
		currentHE = currentHE->nextHE;
	} while (currentHE->startV != v && currentHE != this->toHE);
	return currentHE == this->toHE ? nullptr : currentHE;
}
	
// TODO: create methods for creating and traversing its elements


Edge::Edge()
	: he1(nullptr)
	, he2(nullptr)
{
}

Edge* Edge::CreateEdge(HalfEdge ** he1, HalfEdge ** he2)
{
	Edge* e = new Edge();

	e->he1 = new HalfEdge();
	e->he2 = new HalfEdge();
	if (he1)	*he1 = e->he1;
	if (he2)	*he2 = e->he2;

	e->he1->toEdge = e;
	e->he2->toEdge = e;

	return e;
}
	
// TODO: create methods for creating and traversing its elements


HalfEdge::HalfEdge()
	: toLoop(nullptr)
	, toEdge(nullptr)
	, nextHE(nullptr)
	, prevHE(nullptr)
	, startV(nullptr)
{
}

void HalfEdge::setNextHE(HalfEdge * next)
{
	this->nextHE = next;
	next->prevHE = this;
}
	
// TODO: create methods for creating and traversing its elements


Vertex::Vertex()
	: outgoingHE(nullptr)
	, coordinates(Vec3f())
{
}
	
// TODO: create methods for creating and traversing its elements
