#ifndef EP_BASE_H
#define EP_BASE_H

// include specific definition of vertex, halfedge, facet
#include "ep_vertex.h"
#include "ep_halfedge.h"
#include "ep_facet.h"

// CGAL related header files
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

// type declaration
typedef CGAL::Simple_cartesian<double> EP_kernel;

// define wrapper class
struct EP_items : public CGAL::Polyhedron_items_3
{
    // wrap vertex
    template <class Refs, class Traits>
    struct Vertex_wrapper {
        typedef typename Traits::Point_3  Point;
        typedef typename Traits::Vector_3 Vector;
        typedef EP_vertex<Refs, CGAL::Tag_true, Point, Vector> Vertex;
    };

    // wrap halfedge
    template <class Refs, class Traits>
    struct Halfedge_wrapper {
        typedef EP_halfedge<Refs, CGAL::Tag_true, CGAL::Tag_true, CGAL::Tag_true> Halfedge;
    };

    // wrap face
    template <class Refs, class Traits>
    struct Face_wrapper {
        typedef EP_facet<Refs, CGAL::Tag_true> Face;
    };
};

// base class for triangular meshes
class EP_base : public CGAL::Polyhedron_3<EP_kernel, EP_items>
{
public:
    // type declaration
    typedef CGAL::Polyhedron_3<EP_kernel, EP_items> Base;
    typedef EP_kernel Kernel;
    typedef CGAL::Bbox_2 Bbox;

private:
    // TODO: add private member variables here
    Bbox m_bbox;                                            // bounding box of parametric domain
    double m_diameter;                                      // diameter of bounding box

public:
    // life cycle
    EP_base(void) { }
    virtual ~EP_base(void) { }

    // getter and setter member functions
    const Bbox              bbox(void)const                     { return m_bbox; }
    void                    set_bbox(Bbox bbox)                 { m_bbox = bbox; }

    const double            diameter(void)const                 { return m_diameter; }
    void                    set_diameter(double diameter)       { m_diameter = diameter; }
};

#endif // EP_BASE_H