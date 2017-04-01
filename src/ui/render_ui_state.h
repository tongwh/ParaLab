#ifndef RENDER_UI_STATE_H
#define RENDER_UI_STATE_H

#include "document.h"
#include "texture.h"

#include <QPoint>

#include <bitset>
#include <string>

struct RenderUIState
{
	// Mouse interaction
	enum ManipulMode
	{
		None = 0,
		Trackball,
		Zooming,
		Panning
	};

	ManipulMode m_eManipulationMode;
	bool m_ButtonDown[4];
    bool m_Moving;

	// OpenGL framework
	bool m_GLInitialized;
	std::bitset<Document::F_NUMBER_OF_FLAGS> m_flags; // set of flags

	QPoint m_ptPrev;
	GLfloat m_vPrevVec[3], m_vCurVec[3];
	GLfloat m_fViewingDistance, m_fFovy;
	GLfloat m_fNearFactor, m_fFarFactor;
	GLfloat m_fZoomScale, m_fPanOffsetX, m_fPanOffsetY;
	bool m_bIsTracking;
	GLfloat m_vAxis[3];
	GLfloat m_fTrackingAngle;
	GLfloat m_mxTransform[4][4];
	GLint m_vViewport[4];
	GLfloat m_fRxSpeed, m_fRySpeed, m_fRzSpeed;

	int m_iShaderCurrentIndex;
	GLuint m_GLShaderProgramObjects[5];	// OpenGL shader program objects

    // Status bar information
    std::string m_status;
    std::string m_status_mesh_info;

    // Toggle rendering flags
    bool GetFlag(Document::FlagType p) const { assert(0 <= p && p < Document::F_NUMBER_OF_FLAGS); return m_flags[p]; }
    void SetFlag(Document::FlagType p, bool flag) { assert(0 <= p && p < Document::F_NUMBER_OF_FLAGS); m_flags.set(p, flag); }
};

#endif // RENDER_UI_STATE_H