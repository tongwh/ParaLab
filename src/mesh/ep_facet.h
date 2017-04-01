#ifndef EP_FACET_H
#define EP_FACET_H

#include <CGAL/HalfedgeDS_face_base.h>

template <class Refs, class T>
class EP_facet : public CGAL::HalfedgeDS_face_base<Refs, T>
{
public:
    // type declaration
    typedef typename CGAL::HalfedgeDS_face_base<Refs, T> Base;

private:
    // TODO: add private member variables here
    int m_index;                                                // index of facet
    int m_tag;                                                  // more of a temporary index

public:
    // life cycle    
	EP_facet(void) { default_initialize(); }    
	
	// copy assignment operator
    EP_facet& operator=(const EP_facet& f) {        
		this->m_index = f.m_index;        
		this->m_tag = f.m_tag;        
		return *this;    
	}

    // repeat mandatory constructors
    void default_initialize() { 
		m_index = -1;
		m_tag = -1;
	}

    // getter and setter member functions
    const int               index(void)const                     { return m_index; }
    void                    set_index(int i)                     { m_index = i; }

    int                     tag(void)const                       { return m_tag; }
    void                    set_tag(int i)                       { m_tag = i; }
};

#endif // EP_FACET_H