#ifndef DOCUMENT_IMPL_H
#define DOCUMENT_IMPL_H

#include "ep_base.h"
#include "ep_render.h"
#include "render_ui_state.h"

struct Document_impl
{
	// Pointers to classes doing actual work
	EP_base*         m_mesh;
    EP_render*       m_render;

	// UI states
	RenderUIState	 m_state;

	// Constructor/destructor
	Document_impl()
    {
		m_mesh = NULL;
		m_render = NULL;
	}

	~Document_impl()
	{
		if (m_mesh != NULL) {
			delete m_mesh;
		}

		if (m_render != NULL) {
			delete m_render;
		}
	}
};

#endif // DOCUMENT_IMPL_H