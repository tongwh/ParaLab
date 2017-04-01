// The classes here are designed to AVOID PASS-BY-REFERENCE for easier
// interfacing with python.

#ifdef SWIG

%module pyparalab
%{
#define SWIG_FILE_WITH_INIT

#include "document.h"
%}

%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
namespace std {
    %template(StringVector) vector<string>;
    %template(IntVector) vector<int>;
	%template(DoubleVector) vector<double>;
    %template(PairSD) pair<string, double>;
    %template(PairID) pair<int, double>;
}

#endif	// SWIG

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Document
////////////////////////////////////////////////////////////////////////////////

struct Document_impl;

class Document
{
private:
    std::unique_ptr<Document_impl> m_p;   // the only data member, all actual members are hidden here 

public:
    // This definition would work even if these constants weren't explicitly set.
	enum FlagType {
		F_MODE_SELECT = 0,
		F_MODE_ANIMATE,
		F_MODE_SHADER,
		F_DRAW_SMOOTH,
		F_DRAW_FLAT,
		F_DRAW_WIRE,
		F_SHOW_TEXTURE,
		F_SHOW_VERTEX_TAG,
		F_SHOW_FACET_TAG,
		F_NUMBER_OF_FLAGS	// the number of flags
	};

public:
    // Constructor/destructor
	Document();
    ~Document();

    // Mesh input/output
    bool LoadDocument(const char* fname, bool bMeshOnly = false); 
	bool SaveDocument(const char* fname, bool save_wrapped_ply = true);
    void CloseDocument();

	// Mesh properties
	bool HasMesh() const;
	bool GLInitialized() const;

	// Status string manipulation
	const char *StatusMessage();
	void SetStatusMessage(const char* fmt, ...);

	// OpenGL framework: all of these functions need a VALID OPENGL CONTEXT 
	// which needs to be set before they are called.
    void SetupGL(int width, int height);
	void Render(); 
    void UpdateSize(int width, int height);

    // Mouse interaction
    // button numbers: 1 = left, 2 = middle, 3 = right
    void MouseButtonDown(int button_number, int x, int y, int shift_on, int ctrl_on);
	bool MouseMove(int x, int y);
	void MouseButtonUp(int button_number, int x, int y);
	void MouseWheel(short zDelta, int shift_on, int ctrl_on);

	// Toggle rendering flags 
	bool GetFlag(FlagType p) const;
	void SetFlag(FlagType p, bool flag);
	bool ToggleFlag(FlagType p);

	// Set shader index
	void SetShaderIndex(int i);
};

#endif // DOCUMENT_H

