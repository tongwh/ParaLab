#include "stdafx.h"

#include "document.h"
#include "document_impl.h"
#include "render_ui.h"
#include "util.h"
#include "logging.h"

////////////////////////////////////////////////////////////////////////////////
// Constructor and destructor

Document::Document()
{
    m_p = std::unique_ptr<Document_impl>(new Document_impl);
	m_p->m_mesh = NULL;
    RenderUI::InitializeState(m_p->m_state);
}

Document::~Document()
{
    CloseDocument();
}

////////////////////////////////////////////////////////////////////////////////
// Mesh input/output

bool
Document::LoadDocument(const char* fname, bool bMeshOnly)
{
	LOG_MESSAGE_BLOCK(L_MED, "Document::LoadDocument()");

    // get extension
    std::string extension = Sys::GetExtension(fname);
    if( extension != ".obj" && extension != ".off") {
        Log.error("Unknown extension");
        SetStatusMessage("Unknown extension");
        return false;
    }

    // deallocate the previous meshes
    CloseDocument();

    // load the mesh
	m_p->m_mesh = new EP_base();
    SetStatusMessage("Loaded %s", fname);

	return true;
}

bool
Document::SaveDocument(const char* fname, bool save_wrapped_ply)
{
	LOG_MESSAGE_BLOCK(L_MED, "Document::SaveDocument()");

	// save the mesh
    SetStatusMessage("Saved to %s", fname);

    return true;
}

void
Document::CloseDocument()
{
    if (m_p->m_mesh) {
        delete m_p->m_mesh;
        m_p->m_mesh = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Mesh properties
bool Document::HasMesh() const 
{ 
	return m_p->m_mesh != NULL; 
}

bool Document::GLInitialized() const 
{ 
	return m_p->m_state.m_GLInitialized; 
}

////////////////////////////////////////////////////////////////////////////////
// Status string manipulation
////////////////////////////////////////////////////////////////////////////////

const char *
Document::StatusMessage()
{
	return RenderUI::StatusMessage(m_p->m_state);
}

////////////////////////////////////////////////////////////

void
Document::SetStatusMessage(const char* fmt, ...)
{
	// fill buffer
	va_list argptr;
	va_start(argptr, fmt);
	RenderUI::SetStatusMessage(m_p->m_state, fmt, argptr);
	va_end(argptr);
}

////////////////////////////////////////////////////////////////////////////////
// OpenGL framework

void
Document::SetupGL(int width, int height)
{
    RenderUI::SetupGL(m_p->m_state, width, height);
	RenderUI::UpdateSize(m_p->m_state, width, height);
}

void Document::Render()
{
    // nothing to render if no mesh exists
    if (!HasMesh()) return;

    // render scene
    RenderUI::Render(m_p->m_state,  // render state
			m_p.get());	 // meshes
}

void
Document::UpdateSize(int width, int height)
{
    RenderUI::UpdateSize(m_p->m_state, width, height);
}

////////////////////////////////////////////////////////////////////////////////
// Mouse interaction

void
Document::MouseButtonDown(int button_number, int x, int y, int shift_on, int ctrl_on)
{
	int alt_on = 0;
#ifdef _WINDOWS
	alt_on = GetAsyncKeyState(VK_LMENU); // there's also GetKeyState
#endif
    RenderUI::MouseButtonDown(m_p->m_state, button_number, 
		x, y, shift_on, ctrl_on, alt_on);
}

bool
Document::MouseMove(int x, int y)
{
	return RenderUI::MouseMove(m_p->m_state, x, y);
}

void
Document::MouseButtonUp(int button_number, int x, int y)
{
	RenderUI::MouseButtonUp(m_p->m_state, button_number, x, y);
}

void
Document::MouseWheel(short zDelta, int shift_on, int ctrl_on)
{
	RenderUI::MouseWheel(m_p->m_state, zDelta, shift_on, ctrl_on);
}

////////////////////////////////////////////////////////////////////////////////
// Toggle rendering flags 

bool
Document::GetFlag(FlagType p) const
{
	return m_p->m_state.GetFlag(p);
}

void
Document::SetFlag(FlagType p, bool flag)
{
	m_p->m_state.SetFlag(p, flag);
}

bool
Document::ToggleFlag(FlagType p)
{
	SetFlag(p, !GetFlag(p));
	return GetFlag(p);
}

void 
Document::SetShaderIndex(int i)
{
	if (i == 0) {
		m_p->m_state.SetFlag(F_MODE_SHADER, false);
	}
	else {
		m_p->m_state.SetFlag(F_MODE_SHADER, true);
		m_p->m_state.m_iShaderCurrentIndex = i - 1;
	}
}