// ========================================================================= //
// Authors: Matthias Borner                                                  //
// mailto:matthias.borner@igd.fraunhofer.de                                  //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
// ========================================================================= //

#ifndef HALF_EDGE_DS_ELEMENTS_H
#define HALF_EDGE_DS_ELEMENTS_H

#include "Vec3.h"          // vector (x, y, z)

// forward declarations, so elements know each other when defined further below
struct Solid;
struct Face;
struct Loop;
struct Edge;
struct HalfEdge;
struct Vertex;

// data structure elements. this is an minimal extract of the data structure presented in the slides.
// we skip identifiers and pointers to unsorted lists of elements.
// by default all pointers are initialized with nullptr, so you can use "if (some_solid->toFace)" to check, if the pointer is set.
struct Solid
{
     Face* toFace;               // can be any face of the solid
     Solid();
     
     // TODO: create methods for creating and traversing its elements
};

struct Face
{
     Solid* toSolid;           // back to the solid
     Loop* outerLoop;          // to the boundary of the face
     Loop* innerLoop;          // to a possible hole. Can be null_ptr. Multiple holes are accessible through innerLoop->nextLoop.
     Face();

     int countInnerLoops() const;
};

struct Loop
{
     Face* toFace;             // back to the face
     Loop* nextLoop;           // next inner loop. cyclic: points to itself, if there is no other loop.
     Loop* prevLoop;           // previous inner loop. cyclic: points to itself, if there is no other loop.
     HalfEdge* toHE;           // can be any half edge of the loop
     Loop();
     
     // TODO: create methods for creating and traversing its elements
     HalfEdge* findHalfedgeStartingAt(Vertex* v);
};

struct Edge
{
     HalfEdge* he1;
     HalfEdge* he2;
     Edge();
     
     static Edge* CreateEdge(HalfEdge** he1, HalfEdge** he2);
};

struct HalfEdge
{
     Loop* toLoop;              // back to the loop
     Edge* toEdge;              // back to the parent edge
     HalfEdge* nextHE;          // next half edge
     HalfEdge* prevHE;          // previous half edge
     Vertex* startV;            // vertex at the tail of the half edge
     HalfEdge();
     
     void setNextHE(HalfEdge* next);
     HalfEdge* getEdgeSibling() const;
};

struct Vertex
{
     HalfEdge* outgoingHE;     // can be any outgoing half edge
     Vec3f coordinates;        // stores the 3d coordinates of the vertex
     Vertex();
     
     // TODO: create methods for creating and traversing its elements
};


#endif
