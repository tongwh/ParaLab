#ifndef __EP_RENDER_H__
#define __EP_RENDER_H__

#include "ep_base.h"

// for rendering EP_base
class EP_render
{
private:
	EP_base* m_mesh;

public:
	EP_render(EP_base* m): m_mesh(m) { }
	virtual ~EP_render() { }

	// getter and setter member functions
	EP_base *Mesh() { return m_mesh; }

	// draw using OpenGL commands (display lists)
};

#endif // __EP_RENDER_H__
