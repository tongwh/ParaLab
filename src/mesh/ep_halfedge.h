#ifndef EP_HALFEDGE_H
#define EP_HALFEDGE_H

#include <CGAL/HalfedgeDS_halfedge_base.h>

#include <complex>

template <class Refs, class Tag_prev, class Tag_vertex, class Tag_face>
class EP_halfedge : public CGAL::HalfedgeDS_halfedge_base<Refs, Tag_prev, Tag_vertex, Tag_face>
{
public:
	// type declaration    
	typedef CGAL::HalfedgeDS_halfedge_base<Refs, Tag_prev, Tag_vertex, Tag_face> Base;

private:		
	// TODO: add private member variables here    
	int m_index;                                        // index of halfedge    
	int m_tag;                                          // more of a temporary index

public:		
	// life cycle    
	EP_halfedge(void) { default_initialize(); }    
		
	// copy assignment operator
    EP_halfedge& operator=(const EP_halfedge& h) {
        this->m_index = h.m_index;
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

template<class Handle> inline bool is_handle_assigned(Handle h) { Handle null; return h != null; }

#endif // PD_HALFEDGE_H