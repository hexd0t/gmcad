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
	// WARNING: this example does NOT create a valid topology. It just creates the minimum elements to draw one line.
	// CARE: for every "new" we need a "delete". if an element is added to the according list, it is deleted automatically within clearDS().

	// create example elements. 	
	Vertex* v1 = new Vertex;
	Vertex* v2 = new Vertex;
	HalfEdge* he1 = new HalfEdge;
	HalfEdge* he2 = new HalfEdge;
	Edge* e = new Edge;
	// set up connections
	v1->coordinates = Vec3f(1.0f, 2.0f, 3.0f);
	v2->coordinates = Vec3f(3.0f, 2.0f, 1.0f);
	he1->startV = v1;
	he1->nextHE = he2;
	he2->startV = v2;
	he2->nextHE = he1;
	e->he1 = he1;
	e->he2 = he2;
	// add elements to the lists
	vertices.push_back(v1);
	vertices.push_back(v2);
	halfEdges.push_back(he1);
	halfEdges.push_back(he2);
	edges.push_back(e);

	// TODO: Create a new VALID test object including all topological elements and linkage. The object should be volumetric and consist of at least one hole (H > 0).
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
