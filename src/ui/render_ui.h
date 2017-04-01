#ifndef RENDER_UI_H
#define RENDER_UI_H

struct Document_impl;
struct RenderUIState;

class EP_base;
class QPoint;

// Could be turned into a class if needed
namespace RenderUI
{
	// Initialize UI state
    void InitializeState(RenderUIState &state);

    // OpenGL framework
    void SetupGL(RenderUIState &state, int width, int height);
	void Render(RenderUIState &state, Document_impl *dimpl);
    void UpdateSize(RenderUIState &state, int width, int height);

	// OpenGL auxiliary stuff
	void GLPtTo3DVec(RenderUIState &state, QPoint point, GLfloat *pVec);
	void GLPtTo3DPos(RenderUIState &state, QPoint point, bool isPickModel, GLdouble &x, GLdouble &y, GLdouble &z);

	// Mouse interaction
    void MouseButtonDown(RenderUIState &state, int button_number, int x, int y, int shift_on, int ctrl_on, int alt_on);
	bool MouseMove(RenderUIState &state, int x, int y);
	void MouseButtonUp(RenderUIState &state, int button_number, int x, int y);
	void MouseWheel(RenderUIState &state, short zDelta, int shift_on, int ctrl_on);

	// status bar
	const char *StatusMessage(RenderUIState &state);
	void SetStatusMessage(RenderUIState &state, const char* fmt, va_list argptr);
	void SetStatusMessage(RenderUIState &state, const char* fmt, ...);
};

#endif // RENDER_UI_H