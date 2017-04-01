#include "stdafx.h"

#include "gltools.h"
#include "render_ui.h"
#include "render_ui_state.h"
#include "ep_render.h"
#include "document_impl.h"

#include <QPoint>
#include <iostream>
using namespace std;

inline void InitQtResource() { Q_INIT_RESOURCE(paralab); }
inline void ReleaseQtResource() { Q_CLEANUP_RESOURCE(paralab); }

namespace RenderUI {

const int M_LEFT = 1;
const int M_MIDDLE = 2;
const int M_RIGHT = 3;

const double PI = 3.1415926;
const float FTINY = numeric_limits<float>::epsilon();

void
InitializeState(RenderUIState &state)
{
	// OpenGL view parameter
	GLfloat SCENE_SCALE = 10.0f;
	state.m_fViewingDistance = SCENE_SCALE * 2.0f;
	state.m_fFovy = 30.0f; state.m_fNearFactor = 0.1f; state.m_fFarFactor = 10.0f;

	state.m_fZoomScale = state.m_fViewingDistance * state.m_fNearFactor
		* (float)tan(state.m_fFovy * PI / 360.0);
	state.m_fPanOffsetX = 0.0f; state.m_fPanOffsetY = 0.0f;

	// default view transform
	state.m_mxTransform[0][0] = 0.544677f; state.m_mxTransform[0][1] = -0.362249f;
	state.m_mxTransform[0][2] = 0.756375f; state.m_mxTransform[0][3] = 0.0f;
	state.m_mxTransform[1][0] = 0.838644f; state.m_mxTransform[1][1] = 0.233635f;
	state.m_mxTransform[1][2] = -0.492026f; state.m_mxTransform[1][3] = 0.0f;
	state.m_mxTransform[2][0] = 0.001520f; state.m_mxTransform[2][1] = 0.902324f;
	state.m_mxTransform[2][2] = 0.431054f; state.m_mxTransform[2][3] = 0.0f;
	state.m_mxTransform[3][0] = 0.0f; state.m_mxTransform[3][1] = 0.0f;
	state.m_mxTransform[3][2] = 0.0f; state.m_mxTransform[3][3] = 1.0f;

	state.m_vPrevVec[0] = state.m_vPrevVec[1] = state.m_vPrevVec[2] = 0.0f;
	state.m_vCurVec[0] = state.m_vCurVec[1] = state.m_vCurVec[2] = 0.0f;
	state.m_fTrackingAngle = 0.0f;

	state.m_fRxSpeed = 0.0f; state.m_fRySpeed = 0.0f; state.m_fRzSpeed = 0.0f;

	// set variables by default value
	state.m_bIsTracking = false;
	state.m_eManipulationMode = RenderUIState::None;

	// set shader objects
	state.m_iShaderCurrentIndex = 0;
	for (int i = 0; i < 5; ++i) state.m_GLShaderProgramObjects[i] = NULL;

	// rendering options
	state.m_flags.reset();
	state.SetFlag(Document::F_DRAW_SMOOTH, true);
	state.m_GLInitialized = false;
}

void
GLSetupDef(RenderUIState &state)
{
	// set white background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// reset modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set lighting
	GLfloat light_position1[4] = { -52.0f, -16.0f, -50.0f, 0.0f };
	GLfloat light_position2[4] = { -26.0f, -48.0f, -50.0f, 0.0f };
	GLfloat light_position3[4] = { 16.0f, -52.0f, -50.0f, 0.0f };

	GLfloat direction1[3] = { 52.0f, 16.0f, 50.0f };
	GLfloat direction2[3] = { 26.0f, 48.0f, 50.0f };
	GLfloat direction3[3] = { -16.0f, 52.0f, 50.0f };

	GLfloat light_position4[4] = { 52.0f, 16.0f, 50.0f, 0.0f };
	GLfloat light_position5[4] = { 26.0f, 48.0f, 50.0f, 0.0f };
	GLfloat light_position6[4] = { -16.0f, 52.0f, 50.0f, 0.0f };

	GLfloat direction4[3] = { -52.0f, -16.0f, -50.0f };
	GLfloat direction5[3] = { -26.0f, -48.0f, -50.0f };
	GLfloat direction6[3] = { 16.0f, -52.0f, -50.0f };

	GLfloat color1[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat color2[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	GLfloat color3[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

	GLfloat color4[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat color5[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	GLfloat color6[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color1);
	glLightfv(GL_LIGHT0, GL_SPECULAR, color1);

	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, color2);

	glLightfv(GL_LIGHT2, GL_POSITION, light_position3);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direction3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, color3);
	glLightfv(GL_LIGHT2, GL_SPECULAR, color3);

	glLightfv(GL_LIGHT3, GL_POSITION, light_position4);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, direction4);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, color4);
	glLightfv(GL_LIGHT3, GL_SPECULAR, color4);

	glLightfv(GL_LIGHT4, GL_POSITION, light_position5);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, direction5);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, color5);
	glLightfv(GL_LIGHT4, GL_SPECULAR, color5);

	glLightfv(GL_LIGHT5, GL_POSITION, light_position6);
	glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, direction6);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, color6);
	glLightfv(GL_LIGHT5, GL_SPECULAR, color6);

	glEnable(GL_LIGHTING);

	/**
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	/**/

	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHT5);

	// set material
	GLfloat ambient[4] = { 0.3f, 0.3f, 0.3f, 0.5f };
	GLfloat material_ambient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat material_diffuse[4] = { 1.0f, 1.0f, 1.0f, 0.3f };
	GLfloat material_specular[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	GLfloat material_shininess = 51.2f;

	/**
	GLfloat ambient[4] = {0.3f, 0.3f, 0.3f, 0.5f};
	GLfloat material_ambient[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat material_diffuse[] = {0.4, 0.4, 0.4, 1.0};
	GLfloat material_specular[] = {0.8, 0.8, 0.8, 1.0};
	GLfloat material_shininess = 128.0f;
	/**/

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	// set cull bake face mode
	//	glCullFace(GL_BACK);
	//	glEnable(GL_CULL_FACE);

	// set point, line, polygon draw mode
	glPointSize(1.0f);
	glLineWidth(1.0f);

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	// set blend mode
//	glEnable(GL_BLEND);    
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // set polygon faceing mode
	glFrontFace(GL_CCW);
//	glPolygonMode(GL_BACK, GL_LINE);
}

void
GLSetupShader(RenderUIState &state)
{
	// load Qt resource
	InitQtResource();

	// setup shader and their parameters
	const GLubyte *extstrGL;
	extstrGL = glGetString(GL_EXTENSIONS);

	// check for GL_ARB_shading_language_100 extension.
	if (GL_TRUE != GL::gltCheckExt(extstrGL, (const GLubyte *)"GL_ARB_shading_language_100"))
	{
		cerr << "Not support GL_ARB_shading_language_100!" << endl;
		return;
	}

	// check for GL_ARB_shader_objects extension.
	if (GL_TRUE != GL::gltCheckExt(extstrGL, (const GLubyte *)"GL_ARB_shader_objects"))
	{
		cerr << "Not support GL_ARB_shader_objects!" << endl;
		return;
	}

	// check for GL_ARB_vertex_shader extension.
	if (GL_TRUE != GL::gltCheckExt(extstrGL, (const GLubyte *)"GL_ARB_vertex_shader"))
	{
		cerr << "Not support GL_WIN_swap_hint!" << endl;
		return;
	}

	// check for GL_ARB_fragment_shader extension.
	if (GL_TRUE != GL::gltCheckExt(extstrGL, (const GLubyte *)"GL_ARB_fragment_shader"))
	{
		cerr << "Not support GL_ARB_fragment_shader!" << endl;
		return;
	}

	// load Phone shader
	char *vFile = ":/shaders/vGL2_Phong1.vert";
	char *fFile = ":/shaders/fGL2_Phong1.frag";
	GL::gltInitShaders(vFile, fFile, state.m_GLShaderProgramObjects[0]);

	// load Golden shader
	vFile = ":/shaders/vGL2_Golden.vert";
	fFile = ":/shaders/fGL2_Golden.frag";
	GL::gltInitShaders(vFile, fFile, state.m_GLShaderProgramObjects[1]);
	glUseProgram(state.m_GLShaderProgramObjects[1]);
	glUniform3f(GL::gltGetUniLoc(state.m_GLShaderProgramObjects[1], "LightPosition"), 0.0f, 0.0f, 5.0f);

	// load Checkerboard shader
	vFile = ":/shaders/vGL2_checkerboard.vert";
	fFile = ":/shaders/fGL2_checkerboard.frag";
	GL::gltInitShaders(vFile, fFile, state.m_GLShaderProgramObjects[2]);
	glUseProgram(state.m_GLShaderProgramObjects[2]);
	glUniform3f(GL::gltGetUniLoc(state.m_GLShaderProgramObjects[2], "lightPos"), 0.0f, 0.0f, 5.0f);

	// load Cartoons shader
	vFile = ":/shaders/vGL2_toons.vert";
	fFile = ":/shaders/fGL2_toons.frag";
	GL::gltInitShaders(vFile, fFile, state.m_GLShaderProgramObjects[3]);
	glUseProgram(state.m_GLShaderProgramObjects[3]);
	glUniform4f(GL::gltGetUniLoc(state.m_GLShaderProgramObjects[3], "cmate"), 0.2f, 0.4f, 0.6f, 1.0f);
	glUniform4f(GL::gltGetUniLoc(state.m_GLShaderProgramObjects[3], "csilh"), 0.1f, 0.1f, 0.1f, 1.0f);
	glUniform4f(GL::gltGetUniLoc(state.m_GLShaderProgramObjects[3], "cspec"), 0.2f, 0.4f, 0.8f, 1.0f);

	// load Contours shader
	vFile = ":/shaders/vGL2_contours.vert";
	fFile = ":/shaders/fGL2_contours.frag";
	GL::gltInitShaders(vFile, fFile, state.m_GLShaderProgramObjects[4]);

	// release Qt resource
	ReleaseQtResource();
}

void
GLSetupFrustum(RenderUIState &state)
{
	// TODO: setup the projection matrix
	double aspectRatio = (double)state.m_vViewport[3] / (double)state.m_vViewport[2];
	float nearPlane = state.m_fNearFactor * state.m_fViewingDistance;
	float farPlane = state.m_fFarFactor * state.m_fViewingDistance;

	float left = -(float)(state.m_fZoomScale / aspectRatio) + state.m_fPanOffsetX;
	float right = (float)(state.m_fZoomScale / aspectRatio) + state.m_fPanOffsetX;
	float bottom = -state.m_fZoomScale + state.m_fPanOffsetY;
	float top = state.m_fZoomScale + state.m_fPanOffsetY;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(left, right, bottom, top, nearPlane, farPlane);

	return;
}

void
GLSetupTransform(RenderUIState &state)
{
	// TODO: setup the view transform matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -state.m_fViewingDistance);
	if ((state.m_eManipulationMode == RenderUIState::Trackball && state.m_bIsTracking == true)
		|| (state.GetFlag(Document::F_MODE_ANIMATE) == true))
	{
		glPushMatrix();
		glLoadIdentity();
		glRotatef(state.m_fTrackingAngle, state.m_vAxis[0], state.m_vAxis[1], state.m_vAxis[2]);
		glMultMatrixf((GLfloat *)state.m_mxTransform);
		glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)state.m_mxTransform);
		glPopMatrix();
	}
	glMultMatrixf((GLfloat *)state.m_mxTransform);
}

void
SetupGL(RenderUIState &state, int width, int height)
{
	// initialize GLEW library
	GL::gltGLEWSetup();

	// setup shader
	GLSetupShader(state);

	// setup default
	GLSetupDef(state);

	// check for error
	GL::gltCheckErrors();

	// set the status
	state.m_GLInitialized = true;
}

void
Render(RenderUIState &state, Document_impl *dimpl)
{
	// the main render function
	//EP_render *render = dimpl->m_render;
	//if (render == NULL || render->Mesh() == NULL) return;
	//EP_base *mesh = render->Mesh();

	// set shader for rendering
	if (state.GetFlag(Document::F_MODE_SHADER))
		glUseProgram(state.m_GLShaderProgramObjects[state.m_iShaderCurrentIndex]);
	else
		glUseProgram(NULL);

	// clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set OpenLG view Parameters 
	GLSetupFrustum(state);
	GLSetupTransform(state);

	// save matrix state and do the rotation
	glPushMatrix();

	// render scene
	glutSolidTorus(1.0f, 5.0f, 10, 50);

	// restore transformations
	glPopMatrix();

	// flush drawing commands
	glFlush();
}

void
UpdateSize(RenderUIState &state, int width, int height)
{
	// set the viewport in terms of window dimensions
	state.m_vViewport[0] = 0; state.m_vViewport[1] = 0;
	state.m_vViewport[2] = width; state.m_vViewport[3] = height;
	glViewport(state.m_vViewport[0], state.m_vViewport[1],
		state.m_vViewport[2], state.m_vViewport[3]);
}

void
GLPtTo3DVec(RenderUIState &state, QPoint point, GLfloat *pVec)
{
	// transform point to vector
	float dis, a;

	// project x,y onto semi-sphere centered within width, height
	pVec[0] = (2.0f*point.x() - state.m_vViewport[2]) / state.m_vViewport[2];
	pVec[1] = (state.m_vViewport[3] - 2.0f*point.y()) / state.m_vViewport[3];
	dis = (float)sqrt(pVec[0] * pVec[0] + pVec[1] * pVec[1]);
	pVec[2] = (float)cos((PI / 2.0f) * ((dis < 1.0f) ? dis : 1.0f));
	a = (float)sqrt(pVec[0] * pVec[0] + pVec[1] * pVec[1] + pVec[2] * pVec[2]);
	pVec[0] /= a;
	pVec[1] /= a;
	pVec[2] /= a;
}

void
GLPtTo3DPos(RenderUIState &state, QPoint point, bool isPickModel, 
	GLdouble &x, GLdouble &y, GLdouble &z)
{
	// transform the windows coordinate back to 3D world coordinate
	GLint    viewport[4];
	GLdouble modelview[16], projection[16];
	GLfloat  winX, winY, winZ;

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	winX = (GLfloat)(point.x() - viewport[0]);
	winY = (GLfloat)(viewport[3] - point.y());
	if (isPickModel == true)
		glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_DOUBLE, &winZ);
	else
		winZ = 10 * FTINY;

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &x, &y, &z);
}

void
MouseButtonDown(RenderUIState &state, int button_number,
	int x, int y, int shift_on, int ctrl_on, int alt_on)
{
	// implementation of mouse button down event
	assert(1 <= button_number && button_number <= 3);
	QPoint point(x, y);

	if (button_number == M_LEFT)
	{
		// left button down
		state.m_eManipulationMode = RenderUIState::Trackball;
		state.m_ptPrev = point;

		GLPtTo3DVec(state, point, state.m_vPrevVec);
		state.m_bIsTracking = true;
	}
	else if (button_number == M_RIGHT)
	{
		// Right button down
		state.m_eManipulationMode = RenderUIState::Panning;
		state.m_ptPrev = point;

		GLPtTo3DVec(state, point, state.m_vPrevVec);
		state.m_bIsTracking = true;
	}
}

bool
MouseMove(RenderUIState &state, int x, int y)
{
	// implementation of mouse move Event
	QPoint point(x, y);

	if (state.m_eManipulationMode == RenderUIState::Trackball)
	{
		GLPtTo3DVec(state, point, state.m_vCurVec);

		GLfloat dx, dy, dz;
		dx = state.m_vCurVec[0] - state.m_vPrevVec[0];
		dy = state.m_vCurVec[1] - state.m_vPrevVec[1];
		dz = state.m_vCurVec[2] - state.m_vPrevVec[2];

		if (!state.GetFlag(Document::F_MODE_ANIMATE))
		{
			state.m_fTrackingAngle = 130.0f * (float)sqrt(dx*dx + dy*dy + dz*dz);

			state.m_vAxis[0] = state.m_vPrevVec[1] * state.m_vCurVec[2] - state.m_vPrevVec[2] * state.m_vCurVec[1];
			state.m_vAxis[1] = state.m_vPrevVec[2] * state.m_vCurVec[0] - state.m_vPrevVec[0] * state.m_vCurVec[2];
			state.m_vAxis[2] = state.m_vPrevVec[0] * state.m_vCurVec[1] - state.m_vPrevVec[1] * state.m_vCurVec[0];

			state.m_vPrevVec[0] = state.m_vCurVec[0];
			state.m_vPrevVec[1] = state.m_vCurVec[1];
			state.m_vPrevVec[2] = state.m_vCurVec[2];
		}
		else
		{
			state.m_fRxSpeed = dx;
			state.m_fRySpeed = dy;
			state.m_fRzSpeed = dz;
		}

		return true;
	}
	else if (state.m_eManipulationMode == RenderUIState::Panning)
	{
		state.m_fPanOffsetX += (float)(state.m_ptPrev.x() - point.x()) * state.m_fZoomScale / state.m_vViewport[3] * 2;
		state.m_fPanOffsetY -= (float)(state.m_ptPrev.y() - point.y()) * state.m_fZoomScale / state.m_vViewport[3] * 2;
		state.m_ptPrev = point;

		return true;
	}

	return false;
}

void
MouseButtonUp(RenderUIState &state, int button_number, int x, int y)
{
	// implementation of mouse release event
	assert(1 <= button_number && button_number <= 3);
	QPoint point(x, y);

	if (button_number == M_LEFT)
	{
		state.m_eManipulationMode = RenderUIState::None;
		state.m_bIsTracking = false;
	}
	else if (button_number == M_RIGHT) 
	{
		state.m_eManipulationMode = RenderUIState::None;
		state.m_bIsTracking = false;
	}
}

void
MouseWheel(RenderUIState &state, short zDelta, int shift_on, int ctrl_on)
{
	// implementation of mouse wheel event
	state.m_eManipulationMode = RenderUIState::Zooming;
	state.m_fZoomScale *= 1.f + (float)((zDelta / 1.0) / state.m_vViewport[2]);
}

const char *
StatusMessage(RenderUIState &state)
{
	return state.m_status.c_str();
}

void
SetStatusMessage(RenderUIState &state, const char* fmt, va_list argptr)
{
	const int STRMAX = 1024;
	char buffer[STRMAX + 1];

	// fill buffer
	vsnprintf(buffer, STRMAX, fmt, argptr);
	state.m_status = buffer;
}

void
SetStatusMessage(RenderUIState &state, const char* fmt, ...)
{
	// fill buffer
	va_list argptr;
	va_start(argptr, fmt);
	SetStatusMessage(state, fmt, argptr);
	va_end(argptr);
}

} // namespace RenderUI
