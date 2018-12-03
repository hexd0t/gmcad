// ========================================================================= //
// Authors: Matthias Borner                                                  //
// mailto:matthias.borner@igd.fraunhofer.de                                  //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
// ========================================================================= //

#ifndef HALF_EDGE_DS_RENDERING_H
#define HALF_EDGE_DS_RENDERING_H

#include "HalfEdgeDS.h"

// render the complete data structure with default colors
void renderDS(const HalfEdgeDS& heDS);

// render one half edge
void renderHE(const HalfEdge* he, const Vec3f& color = Vec3f(0.0f, 1.0f, 0.0f));

// render one edge
void renderE(const Edge* e, const Vec3f& color = Vec3f(0.0f, 1.0f, 0.0f));

// render one vertex
void renderV(const Vertex* v, const Vec3f& color = Vec3f(1.0f, 0.0f, 1.0f));

// special rendering for an selected half edge
void renderHEActive(const HalfEdge* he);

// generic 3D arrow from p1 to p2
void renderArrow(const Vec3f& p1, const Vec3f& p2, float diameter);

#endif