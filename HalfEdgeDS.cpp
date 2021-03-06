﻿#include "HalfEdgeDS.h"

#include <stdio.h>          // cout
#include <iostream>         // cout
#include <numeric>
#include <stdexcept>

void HalfEdgeDS::addSolid(Solid * s)
{
    if (!s->toFace)
        throw std::runtime_error("Tried to create a solid without a face reference!");
    this->solids.push_back(s);
}

void HalfEdgeDS::addFace(Face * f)
{
    if (!f->toSolid)
        throw std::runtime_error("Tried to create a face without a solid reference!");
    if (!f->outerLoop)
        throw std::runtime_error("Tried to create a face without an outer loop reference!");
    if (f->outerLoop->toFace != f)
        throw std::runtime_error("Tried to create a face with an outer loop referencing another face!");
    this->faces.push_back(f);
}

void checkLoop(Loop* l) {
    if (!l->toFace)
        throw std::runtime_error("Tried to create a loop without a face reference!");
    if (!l->nextLoop || !l->prevLoop)
        throw std::runtime_error("Tried to create a loop without a linked list!");
    int count = 0;
    for (auto iter = l->nextLoop; iter != l; iter = iter->nextLoop) {
        if (++count > 5000)
            throw std::runtime_error("Tried to create a loop with a noncylic linked list (never returns to new loop)!");
    }
    if (!l->toHE)
        throw std::runtime_error("Tried to create a loop without a HE reference!");
    if (l->toHE->toLoop != l)
        throw std::runtime_error("Tried to create a loop with a HE referencing another loop!");
    if (l->toFace->outerLoop == l) {
        if (l->nextLoop != l || l->prevLoop != l)
            throw std::runtime_error("Tried to create an outer loop with next/prev pointers!");
    }
}

void HalfEdgeDS::addLoop(Loop * l)
{
    checkLoop(l);
    this->loops.push_back(l);
}

void HalfEdgeDS::addEdge(Edge * e)
{
    if (!e->he1 || !e->he2)
        throw std::runtime_error("Tried to create an edge missing a HE!");
    if (e->he1->toEdge != e || e->he2->toEdge != e)
        throw std::runtime_error("Tried to create an edge with foreign HEs!");
    this->edges.push_back(e);
}

void HalfEdgeDS::addHE(HalfEdge * e)
{
    if(!e->toLoop)
        throw std::runtime_error("Tried to create an HE missing a loop reference!");
    if(!e->toEdge)
        throw std::runtime_error("Tried to create a HE missing an edge!");
    if (!e->prevHE || !e->nextHE)
        throw std::runtime_error("Tried to create a HE missing prev/next references!");
    if(!e->startV)
        throw std::runtime_error("Tried to create a HE missing a start vertex!");
    if(e->nextHE->prevHE != e || e->prevHE->nextHE != e)
        throw std::runtime_error("Tried to create a HE with invalid neighbor references!");
    this->halfEdges.push_back(e);
}

void HalfEdgeDS::addVertex(Vertex * v)
{
    if(!v->outgoingHE)
        throw std::runtime_error("Tried to create a Vertex missing an outgoing HE!");
    if (v->outgoingHE->startV != v)
        throw std::runtime_error("Tried to create a Vertex with a foreign outgoing HE!");
    this->vertices.push_back(v);
}

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
     Loop* l, *l2, *l3;
     Vertex *v1, *v2, *v3, *v4;
     Edge* e;
     MEVVLS(&s, nullptr, nullptr, &l, &v1, &v2, Vec3f(1.f, 0.f, 1.f), Vec3f(3.f, 0.f, 1.f));
     MEV(s, l, v1, nullptr, &v3, Vec3f(1.f, 2.f, 1.f));
     MEV(s, l, v1, nullptr, &v4, Vec3f(1.f, 0.f, 3.f));

     MEL(s, l, v2, v3, &e, &l2, nullptr);
     MEL(s, l, v2, v4, nullptr, &l3, nullptr);
     MEL(s, l3, v3, v4, nullptr, nullptr, nullptr);

     MVE(s, e, nullptr, nullptr, Vec3f(2.f, 1.f, 1.f));

     Edge* constructionEdge;
     Vertex *iV1, *iV2, *iV3;
     MEV(s, l, v1, &constructionEdge, &iV1, Vec3f(1.5f, 0.f, 1.5f));
     MEV(s, l, iV1, nullptr, &iV2, Vec3f(2.f, 0.f, 1.5f));
     MEV(s, l, iV1, nullptr, &iV3, Vec3f(1.5f, 0.f, 2.f));

     MEL(s, l, iV2, iV3, nullptr, nullptr, nullptr);

     KEMH(s, constructionEdge, v1, nullptr);

     Vertex* iV4;
     MEV(s, l, iV1, nullptr, &iV4, Vec3f(1.65f, 0.5f, 1.65f));
     MEL(s, l, iV3, iV4, nullptr, nullptr, nullptr);
     MEL(s, l, iV2, iV4, nullptr, nullptr, nullptr);
}

void HalfEdgeDS::createTorus(float x, float y, float z, float r, float w, int segC, int segA)
{
    const float PI = 3.141592653f;
    float stepC = 2 * PI / segC;
    float stepA = 2 * PI / segA;

    Solid* s;
    Loop* l;
    Vertex* v1;
    Vertex* v2;
    Edge* e;
    MEVVLS(&s, 0, &e, &l, &v1, &v2, Vec3f(x + r, y, z), Vec3f(x + cosf(stepA)*r, y + sinf(stepA)*r, z));

    //generate first cross section
    Vertex* cross_prev_v = v1;
    Vec3f toOutside(1, 0, 0);
    Vec3f alongCross(0, 0, -1);
    std::vector<Vertex*> firstLoopVerts;

    for (size_t i = 1; i < segC; ++i) {
        MEV(s, l, cross_prev_v, 0, &cross_prev_v,
            v1->coordinates - toOutside * w
            + toOutside * w * cosf(stepC*i)
            + alongCross * w * sinf(stepC*i));
        firstLoopVerts.push_back(cross_prev_v);
    }
    MEL(s, l, v1, cross_prev_v, 0, 0, 0);


    //search loop going to v2
    l = getCommonLoop(v1, v2);

    //generate 2nd cross section
    cross_prev_v = v2;
    toOutside = Vec3f(cosf(stepA), sinf(stepA), 0);
    std::vector<Vertex*> prevLoopVerts;
    std::vector<Vertex*> curLoopVerts;
    for (size_t i = 1; i < segC; ++i) {
        MEV(s, l, cross_prev_v, 0, &cross_prev_v,
            v2->coordinates - toOutside * w
            + toOutside * w * cosf(stepC*i)
            + alongCross * w * sinf(stepC*i));
        MEL(s, l, cross_prev_v, firstLoopVerts[i - 1], 0, 0, 0);
        curLoopVerts.push_back(cross_prev_v);
    }
    MEL(s, l, v2, cross_prev_v, 0, &l, 0);
    //continue to use loop of current cross section for next step
    l = getCommonLoop(curLoopVerts[0], curLoopVerts[2]);
    //also search for loop of first cross section (now that it's unique) and keep for later
    Loop* firstCrossSectionLoop = getCommonLoop(firstLoopVerts[0], firstLoopVerts[2]);

    //generate open torus
    Vertex* currentV = v2;
    for (size_t j = 2; j < segA; ++j) {
        prevLoopVerts = curLoopVerts;
        curLoopVerts.clear();
        MEV(s, l, currentV, 0, &currentV, Vec3f(x + cosf(stepA*j)*r, y + sinf(stepA*j)*r, z));

        cross_prev_v = currentV;
        toOutside = Vec3f(cosf(stepA*j), sinf(stepA*j), 0);
        for (size_t i = 1; i < segC; ++i) {
            MEV(s, l, cross_prev_v, 0, &cross_prev_v,
                currentV->coordinates - toOutside * w
                + toOutside * w * cosf(stepC*i)
                + alongCross * w * sinf(stepC*i));
            MEL(s, l, cross_prev_v, prevLoopVerts[i - 1], 0, 0, 0);
            curLoopVerts.push_back(cross_prev_v);
        }
        MEL(s, l, currentV, cross_prev_v, 0, 0, 0);
        //continue to use loop of current cross section for next step
        l = getCommonLoop(curLoopVerts[0], curLoopVerts[2]);
    }
    KPMH(s, firstCrossSectionLoop, l);

    MEKH(s, v1, currentV, firstCrossSectionLoop, l, 0);
    for (size_t i = 1; i < segC; ++i) {
        MEL(s, firstCrossSectionLoop, firstLoopVerts[i - 1], curLoopVerts[i - 1], 0, 0, 0);
    }
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
     this->addFace(f);
     this->addSolid(s);
     this->addEdge(e);
     this->addLoop(l);
     this->addHE(he1);
     this->addHE(he2);
     this->addVertex(v1);
     this->addVertex(v2);
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
          throw std::runtime_error("MEV: Vertex1 needs to be visited by loop!");
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
     this->addEdge(e);
     this->addHE(he1);
     this->addHE(he2);
     this->addVertex(v2);

     checkLoop(loop);
}

void HalfEdgeDS::MEL(Solid * solid, Loop * loop1, Vertex * vertex1, Vertex * vertex2, Edge ** edge, Loop ** loop2, Face** face)
{
     /**********************************************************************\
      *  Vorher:                        *  Nacher:      <ogF>              *
      *     ╖ v1prev       v2next ╖     *     ╖ v1prev         v2next ╖    *
      *     ╙                     ╙     *     ╙          he1          ╙    *
      *  V1 ○        <ogF>        ○ V2  *  V1 ○╒═════════════════════╛○ V2 *
      *     ╖                     ╖     *     ╖          he2          ╖    *
      *     ╙ v1next       v2prev ╙     *     ╙ v1next         v2prev ╙    *
      *                                                  <f>               *
     \**********************************************************************/

     //"Schnitt"-stellen im Loop finden
     HalfEdge* v1nextHE = loop1->findHalfedgeStartingAt(vertex1);
     HalfEdge* v2nextHE = loop1->findHalfedgeStartingAt(vertex2);
     if (!v1nextHE || !v2nextHE)
          throw std::runtime_error("MEL: Vertex 1 and 2 need to be visited by the selected loop!");
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

     f->toSolid = solid;
     f->outerLoop = l2;

     v1prevHE->setNextHE(he1);
     he1->setNextHE(v2nextHE);
     v2prevHE->setNextHE(he2);
     he2->setNextHE(v1nextHE);

     he1->startV = vertex1;
     he2->startV = vertex2;
     he1->toLoop = loop1;
     he2->toLoop = l2;

     l2->toHE = he2;
     loop1->toHE = he1;
     loop1->fixHEReferences();
     l2->fixHEReferences();

     //ToDo:
     //  Welche inner Loops des Original-Face liegen jetzt im neuen Face?

     //Globale Listen pflegen
     this->addEdge(e);
     this->addHE(he1);
     this->addHE(he2);
     this->addLoop(l2);
     this->addFace(f);
     checkLoop(loop1);
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
     e2he1->toLoop = e1he1->toLoop;
     e2he2->toLoop = e1he2->toLoop;

     v->coordinates = coords;
     v->outgoingHE = e1he2;
     if (e2he2->startV->outgoingHE == e1he2) {
          e2he2->startV->outgoingHE = e2he2;
     }

     //Globale Listen pflegen
     this->addEdge(e2);
     this->addHE(e2he1);
     this->addHE(e2he2);
     this->addVertex(v);
}

void HalfEdgeDS::MEKH(Solid * solid, Vertex * v1, Vertex * v2, Loop * l1, Loop * l2, Edge ** edge)
{
    /************************************************************************\
      *  Nachher:                         *  Vorher:                         *
      *     ╖ v1prev         v2next ╖     *     ╖ v1prev         v2next ╖    *
      *     ╙          he1          ╙     *     ╙                       ╙    *
      *  V1 ○╒═════════════════════╛○ V2  *  V1 ○                       ○ V2 *
      *     ╖          he2          ╖     *     ╖                       ╖    *
      *     ╙ v1next         v2prev ╙     *     ╙ v1next         v2prev ╙    *
     \************************************************************************/

    if(l1->toFace != l2->toFace)
        throw std::runtime_error("MEKH: loops need to be part of same face");
    if (l1->toFace->outerLoop != l1)
        throw std::runtime_error("MEKH: outer loop needs to be outer loop of face");
    HalfEdge* v1next = l1->findHalfedgeStartingAt(v1);
    if(!v1next)
        throw std::runtime_error("MEKH: outer vertex needs to be visited by outer loop");
    HalfEdge* v1prev = v1next->prevHE;
    HalfEdge* v2next = l2->findHalfedgeStartingAt(v2);
    if (!v2next)
        throw std::runtime_error("MEKH: inner vertex needs to be visited by inner loop");
    HalfEdge* v2prev = v2next->prevHE;

    HalfEdge* he1, *he2;
    Edge* e = Edge::CreateEdge(&he1, &he2);
    if (edge) *edge = e;

    v1prev->setNextHE(he1);
    he1->setNextHE(v2next);
    v2prev->setNextHE(he2);
    he2->setNextHE(v1next);
    l1->fixHEReferences();

    he1->startV = v1;
    he2->startV = v2;

    Face* f = l1->toFace;
    if (f->innerLoop->nextLoop = f->innerLoop) {
        if (f->innerLoop != l2)
            throw std::runtime_error("MEKH: face has only 1 inner loop, but it isn't the one being deleted!");
        f->innerLoop = nullptr;
    }
    else {
        if (f->innerLoop == l2) {
            f->innerLoop = l2->prevLoop;
        }
        l2->prevLoop->setNextLoop(l2->nextLoop);
    }

    delete l2;

    this->addEdge(e);
    this->addHE(he1);
    this->addHE(he2);
    this->loops.remove(l2);
    checkLoop(l1);
}

void HalfEdgeDS::KEMH(Solid * solid, Edge * edge, Vertex * outerVertex, Loop ** innerLoop)
{
    /************************************************************************\
      *  Vorher:                          *  Nachher:                        *
      *     ╖ v1prev         v2next ╖     *     ╖ v1prev         v2next ╖    *
      *     ╙          he1          ╙     *     ╙                       ╙    *
      *  V1 ○╒═════════════════════╛○ V2  *  V1 ○                       ○ V2 *
      *     ╖          he2          ╖     *     ╖                       ╖    *
      *     ╙ v1next         v2prev ╙     *     ╙ v1next         v2prev ╙    *
     \************************************************************************/
    if (edge->he1->toLoop != edge->he2->toLoop)
        throw std::runtime_error("KEMH: HE1 and HE2 of deleted edge need to be on the same loop! (Hint: looking for KEL?)");
    if(!edge->he1->toLoop->findHalfedgeStartingAt(outerVertex))
        throw std::runtime_error("KEMH: OuterVertex needs to be visited by the loop about to be split!");

    //Neue Datenstrukturen allokieren
    Loop* l2 = new Loop();
    if (innerLoop) *innerLoop = l2;

    //Referenzen sammeln
    Loop* l1 = edge->he1->toLoop;
    HalfEdge* v1prevHE = edge->he1->prevHE;
    HalfEdge* v2nextHE = edge->he1->nextHE;
    HalfEdge* v2prevHE = edge->he2->prevHE;
    HalfEdge* v1nextHE = edge->he2->nextHE;
    Face* f = l1->toFace;

    //Loops trennen
    v1prevHE->setNextHE(v1nextHE);
    v2prevHE->setNextHE(v2nextHE);

    //Überprüfen, ob "v1" oder "v2" auf dem äußeren Loop liegt
    l1->toHE = v1nextHE;
    if (l1->findHalfedgeStartingAt(outerVertex)) {
        l2->toHE = v2nextHE;
    }
    else
    {
        l1->toHE = v2nextHE;
        l2->toHE = v1nextHE;
    }
    //Half-Edges auf innerem Loop auf diesen verweisen lassen
    l2->fixHEReferences();
    
    //Auf neuen inneren Loop von Face aus verweisen
    if (f->innerLoop) { //Das Face hat bereits innere Loops, in Liste einfügen
        l2->setNextLoop(f->innerLoop->nextLoop);
        f->innerLoop->setNextLoop(l2);
    }
    else
    { //Der erste innere Loop in diesem Face
        f->innerLoop = l2;
    }

    l2->toFace = f;

    //Globale Listen pflegen
    this->halfEdges.remove(edge->he1);
    this->halfEdges.remove(edge->he2);
    this->edges.remove(edge);
    this->addLoop(l2);

    delete edge->he1;
    delete edge->he2;
    delete edge;
    checkLoop(l1);
}

void HalfEdgeDS::KPMH(Solid * solid, Loop * keepPeripheral, Loop * makeHole)
{
    if (makeHole->toFace->outerLoop != makeHole)
        throw std::runtime_error("KPMH: makeHole-Loop must be an outer loop!");
    // Das Face der neuen Hole Loop gibt es nicht mehr
    delete makeHole->toFace;
    this->faces.remove(makeHole->toFace);

    Face* f = keepPeripheral->toFace;
    makeHole->toFace = f;
    //Auf inneren Loop von Face aus verweisen
    if (f->innerLoop) { //Das Face hat bereits innere Loops, in Liste einfügen
        makeHole->setNextLoop(f->innerLoop->nextLoop);
        f->innerLoop->setNextLoop(makeHole);
    }
    else
    { //Der erste innere Loop in diesem Face
        f->innerLoop = makeHole;
    }
}

float HalfEdgeDS::EulerPoincareRings()
{
    // V - E + F = 2(S-R) + H  <=> R = S-(V-E+F-H)/2
    // Bei uns gilt Shell = Solid, da keine inneren Shells unterstützt werden
    float S = solids.size();
    float V = vertices.size();
    float E = edges.size();
    float F = faces.size();
    auto calcHoles = [](int currentCount, Face* f) {
        return currentCount + f->countInnerLoops();
    };
    int H = std::accumulate(faces.begin(), faces.end(), 0, calcHoles);
    float R = S - (V - E + F - H) * 0.5f;

    //Debug output:
    std::cout  << "> S - (V - E + F - H)/2 = " <<
        S << " - (" << V << " - " << E << " + " << F << " - " << H << ") / 2 = " << R << std::endl;
    return R;
}

Loop * HalfEdgeDS::getCommonLoop(Vertex * v1, Vertex * v2)
{
    Loop* commonLoop = nullptr;
    for (auto iter = loops.begin(); iter != loops.end(); ++iter) {
        auto found1 = (*iter)->findHalfedgeStartingAt(v1);
        auto found2 = (*iter)->findHalfedgeStartingAt(v2);
        if ( found1 && found2) {
            commonLoop = *iter;
            break;
        }
    }
    return commonLoop;
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
