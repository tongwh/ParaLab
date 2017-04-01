#ifndef EP_VERTEX_H
#define EP_VERTEX_H

#include <CGAL/HalfedgeDS_vertex_base.h>

#include <complex>

template <class Refs, class T, class Point, class Vector>
class EP_vertex : public CGAL::HalfedgeDS_vertex_base<Refs, T, Point> 
{
public:
    // type declaration 
    typedef CGAL::HalfedgeDS_vertex_base<Refs, T, Point> Base;
    typedef std::complex<double> Complex;

private:
    // TODO: add private member variables here
    int m_index;                                    // index of vertex
    int m_tag;                                                  // more of a temporary index

public:   
	// life cycle    
	EP_vertex(void) { default_initialize(); }    
	EP_vertex(const Point& pt) : Base(pt) { default_initialize(); }

    // copy assignment operator
    EP_vertex& operator=(const EP_vertex& v) {
        this->m_index = v.m_index;
		this->m_tag = f.m_tag;
        return *this;
    }

	// default initialization
    void default_initialize() {
		m_index = -1;
		m_tag = -1;
	}
			
	// getter and setter member functions
    const int           index(void)const                { return m_index; }
    void                set_index(int i)                { m_index = i; }

    int                 tag(void)const                  { return m_tag; }
    void                set_tag(int i)                  { m_tag = i; }
};

#endif // EP_VERTEX_H