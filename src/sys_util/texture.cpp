/***************************************************************************
texture.cpp
----------------------------------------------------------------------------
begin                : june 2003
copyright            : (C) 2003 by Pierre Alliez - INRIA
email                : pierre.alliez@sophia.inria.fr
***************************************************************************/

#include <cmath>
#include <cstring>
#include <algorithm>
#include <cassert>

#include "util.h"
#include "texture.h"
#include "texture_helper.h"
#include "logging.h"

using namespace std;
using namespace GL;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

static unsigned char JetMap[] = { 
0,0,131,0,0,135,0,0,139,0,0,143,0,0,147,0,0,151,0,0,155,0,0,159,0,0,163,0,0,167,0,0,171,0,0,175,0,0,179,0,0,183,0,0,187,0,0,191,0,0,195,0,0,199,0,0,203,0,0,207,0,0,211,0,0,215,0,0,219,0,0,223,0,0,227,0,0,231,0,0,235,0,0,239,0,0,243,0,0,247,0,0,251,0,0,255,0,4,255,0,8,255,0,12,255,0,16,255,0,20,255,0,24,255,0,28,255,0,32,255,0,36,255,0,40,255,0,44,255,0,48,255,0,52,255,0,56,255,0,60,255,0,64,255,0,68,255,0,72,255,0,76,255,0,80,255,0,84,255,0,88,255,0,92,255,0,96,255,0,100,255,0,104,255,0,108,255,0,112,255,0,116,255,0,120,255,0,124,255,0,128,255,0,131,255,0,135,255,0,139,255,0,143,255,0,147,255,0,151,255,0,155,255,0,159,255,0,163,255,0,167,255,0,171,255,0,175,255,0,179,255,0,183,255,0,187,255,0,191,255,0,195,255,0,199,255,0,203,255,0,207,255,0,211,255,0,215,255,0,219,255,0,223,255,0,227,255,0,231,255,0,235,255,0,239,255,0,243,255,0,247,255,0,251,255,0,255,255,4,255,251,8,255,247,12,255,243,16,255,239,20,255,235,24,255,231,28,255,227,32,255,223,36,255,219,40,255,215,44,255,211,48,255,207,52,255,203,56,255,199,60,255,195,64,255,191,68,255,187,72,255,183,76,255,179,80,255,175,84,255,171,88,255,167,92,255,163,96,255,159,100,255,155,104,255,151,108,255,147,112,255,143,116,255,139,120,255,135,124,255,131,128,255,128,131,255,124,135,255,120,139,255,116,143,255,112,147,255,108,151,255,104,155,255,100,159,255,96,163,255,92,167,255,88,171,255,84,175,255,80,179,255,76,183,255,72,187,255,68,191,255,64,195,255,60,199,255,56,203,255,52,207,255,48,211,255,44,215,255,40,219,255,36,223,255,32,227,255,28,231,255,24,235,255,20,239,255,16,243,255,12,247,255,8,251,255,4,255,255,0,255,251,0,255,247,0,255,243,0,255,239,0,255,235,0,255,231,0,255,227,0,255,223,0,255,219,0,255,215,0,255,211,0,255,207,0,255,203,0,255,199,0,255,195,0,255,191,0,255,187,0,255,183,0,255,179,0,255,175,0,255,171,0,255,167,0,255,163,0,255,159,0,255,155,0,255,151,0,255,147,0,255,143,0,255,139,0,255,135,0,255,131,0,255,128,0,255,124,0,255,120,0,255,116,0,255,112,0,255,108,0,255,104,0,255,100,0,255,96,0,255,92,0,255,88,0,255,84,0,255,80,0,255,76,0,255,72,0,255,68,0,255,64,0,255,60,0,255,56,0,255,52,0,255,48,0,255,44,0,255,40,0,255,36,0,255,32,0,255,28,0,255,24,0,255,20,0,255,16,0,255,12,0,255,8,0,255,4,0,255,0,0,251,0,0,247,0,0,243,0,0,239,0,0,235,0,0,231,0,0,227,0,0,223,0,0,219,0,0,215,0,0,211,0,0,207,0,0,203,0,0,199,0,0,195,0,0,191,0,0,187,0,0,183,0,0,179,0,0,175,0,0,171,0,0,167,0,0,163,0,0,159,0,0,155,0,0,151,0,0,147,0,0,143,0,0,139,0,0,135,0,0,131,0,0,128,0
};


//////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////

//********************************************
// Constructor
//********************************************
CTexture::CTexture()
{
  m_pData = NULL;
  m_Width = 0;
  m_WidthByte32 = 0;
  m_Height = 0;
  m_Depth = 0;
}

//********************************************
// Destructor
//********************************************
CTexture::~CTexture()
{
  Free();
}


//////////////////////////////////////////////
// DATA
//////////////////////////////////////////////

//********************************************
// Alloc
//********************************************
int CTexture::Alloc(unsigned int width, unsigned int height, unsigned int depth)
{
  Free();

  unsigned int Width32 = WidthByte32(width,depth);

  m_pData = (unsigned char*)malloc(Width32 * height * sizeof(unsigned char));
  if(m_pData == NULL)
  {
    Log.error("CTexture::Alloc: Insufficient memory");
    return 0;
  }

  // Set members variables
  m_Width       = width;
  m_WidthByte32 = Width32;
  m_Height      = height;
  m_Depth       = depth;
  UpdateHeader();

  return 1;
}

//********************************************
// Free
//********************************************
// Using malloc and free to be compatible with texture_helper.*
void CTexture::Free()
{
  if(m_pData != NULL)
  {
    free(m_pData);
    m_pData = NULL;
  }
  m_Width = 0;
  m_Height = 0;
  m_Depth = 0;
}


//////////////////////////////////////////////
//////////////////////////////////////////////
// FILE READING
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// ReadFile (dispatch function)
//********************************************
int CTexture::ReadFile(const char *filename)
{
  // Extension
  Log.println("CTexture::ReadFile : file : %s",filename);

  // Read the file.
  int width, height, n_components_in_file;
  unsigned char *pData = stbi_load(filename, &width, &height, &n_components_in_file, 0);
  if (pData == NULL)
  {
      Log.error("Unable to read from %s", filename);
      return 0;
  }
  // Require RGB or RGBA mode (i.e. not allowed grey or gray+alpha).
  assert(3 <= n_components_in_file && n_components_in_file <= 4);

  // Update data.
  Free();
  m_pData = pData;
  m_Width = width;
  m_Height = height;
  m_Depth = n_components_in_file * 8;
  m_pFileName = filename;
  UpdateWidthByte32();

  // Switch to BGR mode.
//BGRtoRGB();
 
  return 1;
}

/*
// ********************************************
// ReadFile (dispatch function)
// ********************************************
int CTexture::ReadFile(const char *filename, unsigned int width, unsigned int height, unsigned int depth)
{
  // Cleanup
  Free();

  // Storage
  m_pFileName = filename;

  // Extension
  Log.println("CTexture::ReadFile : file : %s",filename);

  CString extension = filename;
  extension = extension.Right(4);
  extension.MakeLower();

  if(extension == ".bmp") return ReadFileBMP(filename);
  if(extension == ".raw") return ReadFileRAW(filename,width,height,depth);
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);

  return 0;
}
*/


// XXX No longer works because 4-byte alignment removed (see WidthByte32()).
/*
// ********************************************
// ReadFileBMP (*.bmp)
// ********************************************
// Read windows bmp files
// Accept only 24 bits
// Size : 2^n x 2^m
// ********************************************
int CTexture::ReadFileBMP(const char *filename)
{
  
  // Check for valid bmp file
  CFile file;
  CFileException ex;

  // Try to open file
  if(!file.Open(filename, CFile::modeRead | CFile::typeBinary,&ex))
  {
    Util::error("Unable to open file for reading");
    return 0;
  }

  // File header
  BitmapFileHeader FileHeader;
  file.Read(&FileHeader,sizeof(BitmapFileHeader));
  Log.println("FileHeader.bfType : %d",FileHeader.bfType);
  Log.println("FileHeader.bfSize : %d",FileHeader.bfSize);
  Log.println("FileHeader.bfReserved1 : %d",FileHeader.bfReserved1);
  Log.println("FileHeader.bfReserved2 : %d",FileHeader.bfReserved2);
  Log.println("FileHeader.bfOffBits : %d",FileHeader.bfOffBits);

  // Is it a Windows BMP file ? (BM)
  WORD sign = ((WORD) ('M' << 8) | 'B');
  if(FileHeader.bfType != sign)
  {
    Util::error("Invalid BMP file");
    file.Close();
    return 0;
  }

  file.Read(&m_Header,sizeof(BitmapInfoHeader));
  Log.println("");
  Log.println("IMAGE HEADER :");
  Log.println("biSize : %d",m_Header.biSize);
  Log.println("biWidth : %d",m_Header.biWidth);
  Log.println("biHeight : %d",m_Header.biHeight);
  Log.println("biPlanes : %d",m_Header.biPlanes);
  Log.println("biBitCount : %d",m_Header.biBitCount);
  Log.println("biCompression : %d",m_Header.biCompression);
  Log.println("biSizeImage : %d",m_Header.biSizeImage);
  Log.println("biXPelsPerMeter : %d",m_Header.biXPelsPerMeter);
  Log.println("biYPelsPerMeter : %d",m_Header.biYPelsPerMeter);
  Log.println("biClrUsed : %d",m_Header.biClrUsed);
  Log.println("biClrImportant : %d",m_Header.biClrImportant);

  // 24 bits ?
  if(m_Header.biPlanes != 1 ||
     m_Header.biBitCount != 24)
  {
    Util::error("Texture file must have 24 bits depth");
    file.Close();
    return 0;
  }

  // Alloc (does call Free before)
  Free();
  m_pData = (unsigned char*)malloc(m_Header.biSizeImage * sizeof(unsigned char));
  if(m_pData == NULL)
  {
    Util::error("Insufficiant memory");
    file.Close();
    return 0;
  }

  // Update datas
  m_Width = m_Header.biWidth;
  m_Height = m_Header.biHeight;
  m_Depth = m_Header.biBitCount;

  // Image reading
  file.Read(m_pData,m_Header.biSizeImage);
  BGRtoRGB();
  // Close file
  file.Close();

  UpdateWidthByte32();
 
  return 1;
}
*/

//********************************************
// UpdateWidthByte32
//********************************************
void CTexture::UpdateWidthByte32()
{
  m_WidthByte32 = WidthByte32(m_Width,m_Depth);
}

//********************************************
// WidthByte32
//********************************************
unsigned int CTexture::WidthByte32(unsigned int width, unsigned int depth)
{
//// 32 bits alignment (4 bytes)
//int rest=(width*depth/8)%4;
//if(rest != 0)
//  return (width*depth/8 + 4-rest);
//else
//  return (width*depth/8);

    // Ignore alignment.
    // This messes up the BMP and RAW loaders in this file (commented out)
    // but it gets stbi_write_bmp and stbi_write_tga (from texture_helper.h)
    // working.
    return (width*depth/8);
}

//********************************************
// UpdateHeader
//********************************************
void CTexture::UpdateHeader()
{
  UpdateWidthByte32();

/*
  m_Header.biWidth = m_Width;
  m_Header.biHeight = m_Height;
  m_Header.biSizeImage = m_WidthByte32 * m_Height;

  m_Header.biSize = 40;
  m_Header.biPlanes = 1;
  m_Header.biBitCount = m_Depth;
  m_Header.biCompression = (WORD)0;
  m_Header.biXPelsPerMeter = 0;
  m_Header.biYPelsPerMeter = 0;
  m_Header.biClrUsed = 0;
  m_Header.biClrImportant = 0;
*/
}


// XXX No longer works because 4-byte alignment removed (see WidthByte32()).
/*
// ********************************************
// ReadFileRAW (*.raw)
// ********************************************
// Read raw files
// Accept only 24 or 32 bits
// Size : 2^n x 2^m
// ********************************************
int CTexture::ReadFileRAW(const char *filename, unsigned int width, unsigned int height, unsigned int depth)
{
  // Check for valid file
  FILE *fp = fopen(filename,"rb");

  // Try to open file
  if(!fp)
  {
    Util::error("Unable to open file for reading");
    return 0;
  }

  // Alloc (does call Free before)
  if(!Alloc(width,height,depth))
  {
    Util::error("Insufficiant memory");
    fclose(fp);
    return 0;
  }

  fread(m_pData,sizeof(unsigned char),m_Width*m_Height*depth/8,fp);

  // Close file
  fclose(fp);

  // Success, also set FileName
  m_pFileName = filename;

  return 1;
}
*/

//////////////////////////////////////////////
//////////////////////////////////////////////
// FILE SAVING
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// SaveFile (dispatch function)
//********************************************
int CTexture::SaveFile(const char *filename)
{
//Util::error("CTexture::SaveFile : file : %s",filename);
  string extension = Sys::GetExtension(filename);

  // Test the BMP writer and orientation.
//unsigned char img[] = {
//      0,   0,   0,      0,   0,   0,      0,   0,   0,      0,   0,   0,
//    120,   0,   0,    120,   0,   0,    120,   0,   0,    120,   0,   0,
//    255, 255, 255,    255, 255, 255,    255, 255, 255,    255, 255, 255,
//    255,   0,   0,    255,   0,   0,    255,   0,   0,    255,   0,   0 };
//stbi_write_bmp("tmp.bmp", 4, 4, 3, img);

  BGRtoRGB();
  if(extension == ".bmp")
    return stbi_write_bmp(filename, m_Width, m_Height, m_Depth/8, m_pData);
  if(extension == ".tga")
    return stbi_write_tga(filename, m_Width, m_Height, m_Depth/8, m_pData);
  BGRtoRGB();

  return 0;
}

/*
// ********************************************
// SaveFile (dispatch function)
// ********************************************
int CTexture::SaveFile(const char *filename)
{
  Log.println("CTexture::SaveFile : file : %s",filename);
  int len = strlen(filename);
  char extension[10];
  strcpy(extension,&(filename[len-4]));

  if(extension == ".raw")
    return SaveFileRAW(filename);
  if(extension == ".bmp")
    return SaveFileBMP(filename);

  return 0;
}
*/


// XXX No longer works because 4-byte alignment removed (see WidthByte32()).
/*
// ********************************************
// SaveFileRAW
// ********************************************
int CTexture::SaveFileRAW(const char *filename)
{
  // Check for valid image
  if((m_Width * m_Height * m_Depth) == 0)
  {
    Util::error("CTexture::SaveFileRAW : invalid image");
    return 0;
  }

  // Check for valid file
  FILE *fp = fopen(filename,"wb");

  // Try to open file
  if(!fp)
  {
    Util::error("Unable to open file for writing");
    return 0;
  }

  // Image writing
  fwrite(m_pData,sizeof(unsigned char),m_Width*m_Height*m_Depth/8,fp);

  // Close file
  fclose(fp);

  return 1;
}
*/


// XXX No longer works because 4-byte alignment removed (see WidthByte32()).
/*
// ********************************************
// SaveFileBMP (*.bmp)
// ********************************************
// Save windows bmp files
// Accept only 24 bits
// ********************************************
int CTexture::SaveFileBMP(const char *filename)
{
  if(!IsValid())
    return 0;

  if(m_Depth != 24)
    return 0;

  // Check for valid bmp file
  CFile file;
  CFileException ex;

  // Try to open file
  if(!file.Open(filename,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary,&ex))
  {
    Util::error("Unable to open file for writing");
    return 0;
  }

  // File header
  BitmapFileHeader FileHeader;
  WORD sign = ((WORD) ('M' << 8) | 'B');
  FileHeader.bfType = sign;
  FileHeader.bfSize = 14 + 40 + m_WidthByte32*m_Height; 
  FileHeader.bfReserved1 = 0; 
  FileHeader.bfReserved2 = 0; 
  FileHeader.bfOffBits = 54; 

  Log.println("\nSave BMP File...");
  Log.println("FileHeader.bfType : %d",FileHeader.bfType);
  Log.println("FileHeader.bfSize : %d",FileHeader.bfSize);
  Log.println("FileHeader.bfReserved1 : %d",FileHeader.bfReserved1);
  Log.println("FileHeader.bfReserved2 : %d",FileHeader.bfReserved2);
  Log.println("FileHeader.bfOffBits : %d",FileHeader.bfOffBits);

  file.Write(&FileHeader,sizeof(BitmapFileHeader));

  file.Write(&m_Header,sizeof(BitmapInfoHeader));

  // DEBUG
  Log.println("");
  Log.println("IMAGE HEADER :");
  Log.println("biSize : %d",m_Header.biSize);
  Log.println("biWidth : %d",m_Header.biWidth);
  Log.println("biHeight : %d",m_Header.biHeight);
  Log.println("biPlanes : %d",m_Header.biPlanes);
  Log.println("biBitCount : %d",m_Header.biBitCount);
  Log.println("biCompression : %d",m_Header.biCompression);
  Log.println("biSizeImage : %d",m_Header.biSizeImage);
  Log.println("biXPelsPerMeter : %d",m_Header.biXPelsPerMeter);
  Log.println("biYPelsPerMeter : %d",m_Header.biYPelsPerMeter);
  Log.println("biClrUsed : %d",m_Header.biClrUsed);
  Log.println("biClrImportant : %d",m_Header.biClrImportant);

  // Image writing
  file.Write(m_pData,m_Header.biSizeImage);

  // Close file
  file.Close();

  return 1;
}
*/

//////////////////////////////////////////////
//////////////////////////////////////////////
// CHECKING
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// IsValid
//********************************************
int CTexture::IsValid()
{
  int success = 0;
  success = (m_Depth == 24) || (m_Depth == 32);
  success &= (m_Width != 0);
  success &= (m_Height != 0);
  success &= (m_pData != NULL);
  if(!success)
  {
    Log.error("");
    Log.error("Invalid texture");
    Log.error("Width  : %d",m_Width);
    Log.error("Height : %d",m_Height);
    Log.error("Depth  : %d",m_Depth);
    Log.error("Data   : %x",m_pData);
  }
  return success;
}


//********************************************
// HigherPowerOfTwo
//********************************************
int CTexture::HigherPowerOfTwo(int value)
{
  if(value <= 0)
    return value;

  int power = 1;
  int x = 0;

  while(1)
  {
    x = (int)pow(2.0,(double)power);
    if(x >= value)
      return x;
    power++;
  }
}

//********************************************
// LowerPowerOfTwo
//********************************************
int CTexture::LowerPowerOfTwo(int value)
{
  if(value <= 0)
    return value;

  int power = 1;
  int x = 0;

  while(1)
  {
    x = (int)pow(2.0,(double)power);
    if(x >= value)
      return (int)pow(2.0,(double)power-1);
    power++;
  }
}

//********************************************
// SameSize
//********************************************
int CTexture::SameSize(CTexture *pTexture)
{
  int success = (m_Width == pTexture->GetWidth());
  success &= (m_Height == pTexture->GetHeight());
  return success;
}


//********************************************
// Flip BGR to RGB
//********************************************
int CTexture::BGRtoRGB()
{
  if(!IsValid())
    return 0;

  unsigned char pixel;
  int BytePerPixel = m_Depth/8;
  for(unsigned int j=0;j<m_Height;j++)
    for(unsigned int i=0;i<m_Width;i++)
    {
      pixel = m_pData[m_WidthByte32*j+i*BytePerPixel+2];
      m_pData[m_WidthByte32*j+i*BytePerPixel+2] = m_pData[m_WidthByte32*j+i*BytePerPixel];
      m_pData[m_WidthByte32*j+i*BytePerPixel] = pixel;
    }
  return 1;
}

//////////////////////////////////////////////
//////////////////////////////////////////////
// DUPLICATE
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// Extract
//********************************************
int CTexture::Extract(int left, int top, int right, int bottom)
{
  // Saturate
  if(right == -1)
    right = m_Width-1;
  if(bottom == -1)
    bottom = m_Height-1;

  // Check
  if(left >= right || top >= bottom)
    return 0;
  if(left < 0  || left >= (int)m_Width || 
     right < 0 || right >= (int)m_Width)
    return 0;
  if(top < 0  || top >= (int)m_Height || 
     bottom < 0 || bottom >= (int)m_Height)
    return 0;

  int NewWidth = right-left+1;
  int NewWidthByte32 = WidthByte32(NewWidth,m_Depth);
  int NewHeight = bottom-top+1;
  int BytePerPixel = m_Depth / 8;
  int i,j,k;

  Log.println("Start extracting...");
  Log.println("New width : %d",NewWidth);
  Log.println("New height : %d",NewHeight);

  // Alloc
  unsigned char *pData = (unsigned char*)malloc(NewWidthByte32*NewHeight * sizeof(unsigned char));
  if(pData == NULL)
  {
    Util::error("Insufficiant memory");
    return 0;
  }

  for(j=0;j<NewHeight;j++)
    for(i=0;i<NewWidth;i++)
      for(k=0;k<BytePerPixel;k++)
    pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(m_Height-1-(j+top))+(i+left)*BytePerPixel+k];

  // Replace datas
  free(m_pData);
  m_pData = pData;
  m_Width = NewWidth;
  m_WidthByte32 = NewWidthByte32;
  m_Height = NewHeight;

  UpdateHeader();

  return 1;
}


//********************************************
// DuplicateMirror
//********************************************
int CTexture::DuplicateMirror(int left, int top, int right, int bottom)
{

  if(!Extract(left,top,right,bottom))
    return 0;

  left = 0;
  right = m_Width-1;
  top = 0;
  bottom = m_Height-1;

  int NewWidth = 2*m_Width;
  int NewWidthByte32 = WidthByte32(NewWidth,m_Depth);
  int NewHeight = 2*m_Height;
  int BytePerPixel = m_Depth / 8;
  int i,j,k;

  Log.println("Start duplicate mirror...");
  Log.println("New width : %d",NewWidth);
  Log.println("New widthbyte32 : %d",NewWidthByte32);
  Log.println("New height : %d",NewHeight);

  // Alloc
  unsigned char *pData = (unsigned char*)malloc(NewWidthByte32*NewHeight * sizeof(unsigned char));
  if(pData == NULL)
  {
    Util::error("Insufficiant memory");
    return 0;
  }

  // o o
  // x o
  for(j=0;j<NewHeight/2;j++)
    for(i=0;i<NewWidth/2;i++)
      for(k=0;k<BytePerPixel;k++)
    pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(bottom-(j+top))+(i+left)*BytePerPixel+k];
  // o o
  // o x
  for(j=0;j<NewHeight/2;j++)
    for(i=NewWidth/2;i<NewWidth;i++)
      for(k=0;k<BytePerPixel;k++)
    pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(bottom-(j+top))+(right-(i-NewWidth/2+left))*BytePerPixel+k];
  // x o
  // o o
  for(j=NewHeight/2;j<NewHeight;j++)
    for(i=0;i<NewWidth/2;i++)
      for(k=0;k<BytePerPixel;k++)
    pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(j-NewHeight/2+top)+(i+left)*BytePerPixel+k];
  // o x
  // o o
  for(j=NewHeight/2;j<NewHeight;j++)
    for(i=NewWidth/2;i<NewWidth;i++)
      for(k=0;k<BytePerPixel;k++)
    pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(j-NewHeight/2+top)+(right-(i-NewWidth/2+left))*BytePerPixel+k];

  // Replace datas
  free(m_pData);
  m_pData = pData;
  m_Width = NewWidth;
  m_WidthByte32 = NewWidthByte32;
  m_Height = NewHeight;

  UpdateHeader();

  return 1;
}




//********************************************
// DuplicateRepeatWidth
//********************************************
int CTexture::DuplicateRepeatWidth(int left, int top, int right, int bottom)
{
  if(!Extract(left,top,right,bottom))
    return 0;

  left = 0;
  right = m_Width-1;
  top = 0;
  bottom = m_Height-1;

  int NewWidth = 2*m_Width;
  int NewWidthByte32 = WidthByte32(NewWidth,m_Depth);
  int NewHeight = m_Height;
  int BytePerPixel = m_Depth / 8;
  int i,j,k;

  Log.println("Start duplicate repeat width...");
  Log.println("New width : %d",NewWidth);
  Log.println("New widthbyte32 : %d",NewWidthByte32);
  Log.println("New height : %d",NewHeight);

  // Alloc
  unsigned char *pData = (unsigned char*)malloc(NewWidthByte32*NewHeight * sizeof(unsigned char));
  if(pData == NULL)
  {
    Util::error("Insufficiant memory");
    return 0;
  }

  // x o
  for(j=0;j<NewHeight;j++)
    for(i=0;i<NewWidth/2;i++)
      for(k=0;k<BytePerPixel;k++)
    pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(bottom-(j+top))+(i+left)*BytePerPixel+k];
  // o x
  for(j=0;j<NewHeight;j++)
    for(i=NewWidth/2;i<NewWidth;i++)
      for(k=0;k<BytePerPixel;k++)
    pData[NewWidthByte32*j+i*BytePerPixel+k] = m_pData[m_WidthByte32*(bottom-(j+top))+(i-NewWidth/2+left)*BytePerPixel+k];

  // Replace datas
  free(m_pData);
  m_pData = pData;
  m_Width = NewWidth;
  m_WidthByte32 = NewWidthByte32;
  m_Height = NewHeight;

  UpdateHeader();

  return 1;
}


//********************************************
// Fill
//********************************************
void CTexture::Fill(unsigned char r,
            unsigned char g,
            unsigned char b)
{
  if(!IsValid()) return;
  if(m_Depth != 24) return;
  for(unsigned int j=0;j<m_Height;j++)
    for(unsigned int i=0;i<m_Width;i++)
      {
    m_pData[m_WidthByte32*j+i*3] = b;
    m_pData[m_WidthByte32*j+i*3+1] = g;
    m_pData[m_WidthByte32*j+i*3+2] = r;
      }
}


//***************************************
// GreyToColor
//***************************************
void CTexture::GreyToColor(unsigned char grey,
               unsigned char r, 
               unsigned char g,
               unsigned char b)
{
  if(!IsValid()) return;
  if(m_Depth != 24) return;
  for(unsigned int j=0;j<m_Height;j++)
    for(unsigned int i=0;i<m_Width;i++)
      {
    if(m_pData[m_WidthByte32*j+i*3] == grey)
      {
        m_pData[m_WidthByte32*j+i*3] = b;
        m_pData[m_WidthByte32*j+i*3+1] = g;
        m_pData[m_WidthByte32*j+i*3+2] = r;
      }
      }
}

//***************************************
// ColorToColor
//***************************************
void CTexture::ColorToColor(unsigned char r1,
                unsigned char g1,
                unsigned char b1,
                unsigned char r2, 
                unsigned char g2,
                unsigned char b2)
{
  if(!IsValid()) return;
  if(m_Depth != 24) return;
  for(unsigned int j=0;j<m_Height;j++)
    for(unsigned int i=0;i<m_Width;i++)
      {
    if(m_pData[m_WidthByte32*j+i*3] == b1 &&
       m_pData[m_WidthByte32*j+i*3+1] == g1 &&
       m_pData[m_WidthByte32*j+i*3+2] == r1)
      {
        m_pData[m_WidthByte32*j+i*3]   = b2;
        m_pData[m_WidthByte32*j+i*3+1] = g2;
        m_pData[m_WidthByte32*j+i*3+2] = r2;
      }
      }
}

//////////////////////////////////////////////
//////////////////////////////////////////////
// ALPHA
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// SetAlphaLayer
//********************************************
int CTexture::SetAlphaLayer(unsigned char alpha) // 0 - 255
{
  // Check
  if(!IsValid())
    return 0;

  if(m_Depth != 32)
    return 0;

  // Fill alpha layer 
  int size = m_Width * m_Height;
  for(int i=0;i<4*size;i+=4)
    m_pData[i+3] = alpha;

  return 1;
}

//********************************************
// AddAlphaLayer
//********************************************
int CTexture::AddAlphaLayer(unsigned char alpha) // 0 - 255
{
  // Check
  if(!IsValid())
    return 0;

  // Has soon alpha
  if(HasAlpha())
    return SetAlphaLayer(alpha);

  // Alloc memory
  unsigned char *pData = (unsigned char*)malloc(4*m_Width*m_Height * sizeof(unsigned char));
  if(pData == NULL)
  {
    Util::error("CTexture::AddAlphaLayer : insufficiant memory");
    return 0;
  }

  // Fill new data
  int size = m_Width * m_Height;
  int BytePerPixel = m_Depth / 8;
  for(int i=0;i<size;i++)
  {
    pData[4*i+0] = m_pData[BytePerPixel*i+0];
    pData[4*i+1] = m_pData[BytePerPixel*i+1];
    pData[4*i+2] = m_pData[BytePerPixel*i+2];
    pData[4*i+3] = alpha;
  }

  // Set new depth
  m_Depth = 32;

  // Replace datas
  free(m_pData);
  m_pData = pData;

  return 1;
}


//********************************************
// SetAlpha
// From RGB to grey scales, then alpha layer
//********************************************
int CTexture::SetAlphaLayer(CTexture *pTexture) 
{
  // Check
  if(!IsValid())
    return 0;
  if(!pTexture->IsValid())
    return 0;

  if(!SameSize(pTexture))
    return 0;

  if(!AddAlphaLayer(0))
    return 0;

  // Fill new data
  unsigned char *pData = pTexture->GetData();
  int size = m_Width * m_Height;
  int BytePerPixel = pTexture->GetDepth() / 8;
  for(int i=0;i<size;i++)
    m_pData[4*i+3] = (unsigned char)((int)pData[BytePerPixel*i+0]+
                     (int)pData[BytePerPixel*i+1]+
                     (int)pData[BytePerPixel*i+2])/3;

  return 1;
}

//////////////////////////////////////////////
//////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////
//////////////////////////////////////////////

// //********************************************
// // Draw
// //********************************************
// int CTexture::Draw(CDC *pDC, 
// 		   int xOffset, 
// 		   int yOffset, 
// 		   int width, 
// 		   int height)
// {
//   // Checking
//   if(!IsValid())
//     return 0;
// 
//   // Flags
//   if(width == -1)
//     width = m_Width;
//   if(height == -1)
//     height = m_Height;
// 
//   // Painting
//   return SetDIBitsToDevice(pDC->m_hDC, xOffset, yOffset, width, height, 0, 0, 0, 
// 			   m_Height, GetData(), (CONST BITMAPINFO *)&m_Header, DIB_RGB_COLORS);
// }
// 
// //********************************************
// // Stretch
// //********************************************
// int CTexture::Stretch(CDC *pDC,
// 		      CRect *pRect)
// {
//   // Checking
//   if(!IsValid())
//     return 0;
// 	
//   SetStretchBltMode(pDC->m_hDC,COLORONCOLOR);
// 	
//   // Painting
//   return StretchDIBits(pDC->m_hDC,
// 		       pRect->left,
// 		       pRect->top,
// 		       pRect->Width(),
// 		       pRect->Height(),
// 		       0,
// 		       0,
// 		       m_Width,
// 		       m_Height,
// 		       GetData(),
// 		       (CONST BITMAPINFO *)&m_Header,
// 		       DIB_RGB_COLORS,SRCCOPY);
// }

//********************************************
// ReadBuffer
//********************************************
// Flips in the vertical direction
int CTexture::ReadBufferVFlip(unsigned char *buffer, 
             int width, 
             int height, 
             int depth)
{
  if(buffer == NULL)
    return 0;

  if(!Alloc(width,height,depth))
    return 0;

  int BytePerPixel = depth / 8;

  int heightm = height-1;
  for(int j=0;j<height;j++)
    for(int i=0;i<width;i++)
      for(int k=0;k<BytePerPixel;k++)
    m_pData[m_WidthByte32*(heightm-j) + i*BytePerPixel+k] = 
      buffer[(width*j+i)*BytePerPixel+k];

  return 1;
}


//********************************************
// ReadBuffer
//********************************************
int CTexture::ReadBuffer(unsigned char *buffer, 
             int width, 
             int height, 
             int depth)
{
  if(buffer == NULL)
    return 0;

  if(!Alloc(width,height,depth))
    return 0;

  int BytePerPixel = depth / 8;

  for(int j=0;j<height;j++)
    for(int i=0;i<width;i++)
      for(int k=0;k<BytePerPixel;k++)
    m_pData[m_WidthByte32*j + i*BytePerPixel+k] = 
      buffer[(width*j+i)*BytePerPixel+k];

  return 1;
}


//********************************************
// ReadBufferByte32
//********************************************
int CTexture::ReadBufferByte32(unsigned char *pData, 
                   int width, 
                   int height)
{
  // alloc 32 bits buffer
  if(!Alloc(width,height,32))
    return 0;

  if(pData == NULL)
    return 0;

  memcpy(m_pData,pData,height*m_WidthByte32);
  return 1;
}


//***************************************
// Copy
//***************************************
void CTexture::Copy(CTexture *pTexture)
{
  unsigned char *pBuffer = pTexture->GetData();
  if(pBuffer == NULL)
    return;

  unsigned int width = pTexture->GetWidth();
  unsigned int height = pTexture->GetHeight();
  unsigned int depth = pTexture->GetDepth();
  if(!Alloc(width,height,depth))
    return;

  unsigned int BytePerPixel = depth / 8;

  for(unsigned int j=0;j<height;j++)
    for(unsigned int i=0;i<width;i++)
      for(unsigned int k=0;k<BytePerPixel;k++)
    m_pData[m_WidthByte32*j + i*BytePerPixel+k] = 
      pBuffer[(width*j+i)*BytePerPixel+k];
}


//********************************************
// ReadBuffer
//********************************************
int CTexture::ReadBuffer(float *buffer, 
             int width, 
             int height, 
             int depth)
{
  if(buffer == NULL)
    return 0;

  if(!Alloc(width,height,depth))
    return 0;

  int BytePerPixel = depth / 8;

  for(int j=0;j<height;j++)
    for(int i=0;i<width;i++)
      for(int k=0;k<BytePerPixel;k++)
    m_pData[m_WidthByte32*j + i*BytePerPixel+k] = 
      (byte)(255.0f * buffer[(width*j+i)*BytePerPixel+k]);

  return 1;
}

//********************************************
// ReadBuffer
//********************************************
int CTexture::ReadBuffer(float **ppBuffer, 
             int width, 
             int height,
             float ratio)
{
  if(ppBuffer == NULL)
    return 0;

  if(!Alloc(width,height,24))
    return 0;

  for(int j=0;j<height;j++)
    for(int i=0;i<width;i++)
      {
    m_pData[m_WidthByte32*j + i*3]   = (unsigned char)(ratio*ppBuffer[j][i]);
    m_pData[m_WidthByte32*j + i*3+1] = (unsigned char)(ratio*ppBuffer[j][i]);
    m_pData[m_WidthByte32*j + i*3+2] = (unsigned char)(ratio*ppBuffer[j][i]);
      }
  return 1;
}

//********************************************
// WriteBuffer
//********************************************
int CTexture::WriteBuffer(float **ppBuffer, 
              int width, 
              int height)
{
  if(ppBuffer == NULL)
    return 0;

  for(int j=0;j<height;j++)
    for(int i=0;i<width;i++) // only first channel
      ppBuffer[j][i] = m_pData[m_WidthByte32*j + i*3];
  return 1;
}

//********************************************
// WriteBuffer32
//********************************************
int CTexture::WriteBuffer32(float **ppBuffer, 
                int width, 
                int height)
{
  if(ppBuffer == NULL)
    return 0;
  assert(m_Depth == 32);
  unsigned int r,g,b;
  int tmp;
  for(int j=0;j<height;j++)
    {
      tmp = m_WidthByte32*j;
      for(int i=0;i<width;i++) // from 3 channels
    {
      b = m_pData[tmp + i*4];
      g = m_pData[tmp + i*4+1];
      r = m_pData[tmp + i*4+2];
      unsigned int value = (r << 16) + (g << 8) + b;
      ppBuffer[j][i] = (float)value;
    }
    }
  return 1;
}

//********************************************
// ReadBuffer
//********************************************
int CTexture::ReadBuffer(double *buffer, 
             int width, 
             int height, 
             int depth)
{
  if(buffer == NULL)
    return 0;

  if(!Alloc(width,height,depth))
    return 0;

  int BytePerPixel = depth / 8;

  for(int j=0;j<height;j++)
    for(int i=0;i<width;i++)
      {
    m_pData[m_WidthByte32*j + i*BytePerPixel]   = (unsigned char)buffer[width*j+i];
    m_pData[m_WidthByte32*j + i*BytePerPixel+1] = (unsigned char)buffer[width*j+i];
    m_pData[m_WidthByte32*j + i*BytePerPixel+2] = (unsigned char)buffer[width*j+i];
      }

  return 1;
}


//********************************************
// Grey
//********************************************
int CTexture::Grey(unsigned int x, unsigned int y)
{
  int BytePerPixel = m_Depth / 8;
  // Grey scale
  if(BytePerPixel == 1)
    return (int)m_pData[m_WidthByte32*y + x]; 
  else // 24 or 32 bits (alpha layer)
    return (int)((int)m_pData[m_WidthByte32*y + x*BytePerPixel+0]+ 
         (int)m_pData[m_WidthByte32*y + x*BytePerPixel+1]+
         (int)m_pData[m_WidthByte32*y + x*BytePerPixel+2])/3;
}

//********************************************
// Color
//********************************************
void CTexture::Color(unsigned int x, unsigned int y, 
             unsigned char *pRed, unsigned char *pGreen, unsigned char *pBlue)
{
  int BytePerPixel = m_Depth / 8;
  // Grey scale
  if(BytePerPixel == 1)
    {
      *pRed = m_pData[m_WidthByte32*y + x]; 
      *pGreen = m_pData[m_WidthByte32*y + x]; 
      *pBlue = m_pData[m_WidthByte32*y + x]; 
    }
  else // 24 or 32 bits (alpha layer)
    {
      *pRed = m_pData[m_WidthByte32*y + x*BytePerPixel]; 
      *pGreen = m_pData[m_WidthByte32*y + x*BytePerPixel+1]; 
      *pBlue = m_pData[m_WidthByte32*y + x*BytePerPixel+2]; 
    }
}



/*
// ***************************************
// ExportHandle
// ***************************************
HANDLE CTexture::ExportHandle()
{ 
  HANDLE handle;

  Util::error("Export handle...");
  // Process source handle size
  int size = sizeof(BitmapInfoHeader) + m_WidthByte32 * m_Height;
  // Alloc memory
  Util::error("alloc...");
  handle = (HANDLE)::GlobalAlloc (GHND,size);
  if(handle != NULL)
    {
      char *pData = (char *) ::GlobalLock((HGLOBAL)handle);
      Util::error("lock...");
      // Copy header
      Util::error("header...");
      memcpy(pData,&m_Header,sizeof(BitmapInfoHeader));
      // Copy datas
      Util::error("datas...");
      memcpy(pData+sizeof(BitmapInfoHeader),m_pData,m_WidthByte32*m_Height);
      // Unlock
      Util::error("unlock...");
      ::GlobalUnlock((HGLOBAL)handle);
    }
  Log.println("ok");
  ASSERT(handle);
  return handle;
}

// ********************************************
// ImportHandle
// ********************************************
int CTexture::ImportHandle(HANDLE handle)
{
  Util::error("Import handle...");
  ASSERT(handle != NULL);
  char *pData = (char *) ::GlobalLock((HGLOBAL)handle);
    
  // Header
  memcpy(&m_Header,pData,sizeof(BitmapInfoHeader));
    
  Log.println("");
  Log.println("IMAGE HEADER :");
  Log.println("biSize : %d",m_Header.biSize);
  Log.println("biWidth : %d",m_Header.biWidth);
  Log.println("biHeight : %d",m_Header.biHeight);
  Log.println("biPlanes : %d",m_Header.biPlanes);
  Log.println("biBitCount : %d",m_Header.biBitCount);
  Log.println("biCompression : %d",m_Header.biCompression);
  Log.println("biSizeImage : %d",m_Header.biSizeImage);
  Log.println("biXPelsPerMeter : %d",m_Header.biXPelsPerMeter);
  Log.println("biYPelsPerMeter : %d",m_Header.biYPelsPerMeter);
  Log.println("biClrUsed : %d",m_Header.biClrUsed);
  Log.println("biClrImportant : %d",m_Header.biClrImportant);
    
  // 24 bits ?
  if(m_Header.biPlanes != 1 ||
     m_Header.biBitCount != 24)
    {
      AfxMessageBox("Texture file must have 24 bits depth");
      return 0;
    }
    
  // Alloc (does call Free before)
  Alloc(m_Header.biWidth,m_Header.biHeight,m_Header.biBitCount);
  memcpy(m_pData,pData+sizeof(BitmapInfoHeader),m_WidthByte32*m_Height);
    
  ::GlobalUnlock((HGLOBAL)handle);
  return 1;
}
*/

void CTexture::GenerateMirrorV(unsigned int width,
                   unsigned int height, 
                   unsigned char r,
                   unsigned char g,
                   unsigned char b,
                   unsigned char rb,
                   unsigned char gb,
                   unsigned char bb)
{
  Alloc(width,height,24);

  for(unsigned j=0;j<height;j++)
    for(unsigned int i=0;i<width/4;i++)
      {
    m_pData[m_WidthByte32*j + i*3]   = r;
    m_pData[m_WidthByte32*j + i*3+1] = g;
    m_pData[m_WidthByte32*j + i*3+2] = b;
      }
  for(unsigned j=0;j<height;j++)
    for(unsigned int i=width/4;i<3*width/4;i++)
      {
    m_pData[m_WidthByte32*j + i*3]   = rb;
    m_pData[m_WidthByte32*j + i*3+1] = gb;
    m_pData[m_WidthByte32*j + i*3+2] = bb;
      }
  for(unsigned j=0;j<height;j++)
    for(unsigned int i=3*width/4;i<width;i++)
      {
    m_pData[m_WidthByte32*j + i*3]   = r;
    m_pData[m_WidthByte32*j + i*3+1] = g;
    m_pData[m_WidthByte32*j + i*3+2] = b;
      }
}

void CTexture::GenerateMirrorH(unsigned int width,
                   unsigned int height, 
                   unsigned char r,
                   unsigned char g,
                   unsigned char b,
                   unsigned char rb,
                   unsigned char gb,
                   unsigned char bb)
{
  Alloc(width,height,24);

  for(unsigned j=0;j<height/4;j++)
    for(unsigned int i=0;i<width;i++)
      {
    m_pData[m_WidthByte32*j + i*3]   = r;
    m_pData[m_WidthByte32*j + i*3+1] = g;
    m_pData[m_WidthByte32*j + i*3+2] = b;
      }
  for(unsigned j=height/4;j<3*height/4;j++)
    for(unsigned int i=0;i<width;i++)
      {
    m_pData[m_WidthByte32*j + i*3]   = rb;
    m_pData[m_WidthByte32*j + i*3+1] = gb;
    m_pData[m_WidthByte32*j + i*3+2] = bb;
      }
  for(unsigned j=3*height/4;j<height;j++)
    for(unsigned int i=0;i<width;i++)
      {
    m_pData[m_WidthByte32*j + i*3]   = r;
    m_pData[m_WidthByte32*j + i*3+1] = g;
    m_pData[m_WidthByte32*j + i*3+2] = b;
      }
}

////////////////////////////////////////////////////////////////////////////////

// generate checkboard grid
// The two background colors are rgb1 and rgb2, while the grid line color is
// grid_rgb.
//
// If symmetric is on, then the left pattern is chosen. Otherwise, the right
// one is.
//  XOOX  XXOO
//  OXXO  XXOO
//  OXXO  OOXX
//  XOOX  OOXX
void
CTexture::GenerateCheckerBoardGrid(bool symmetric,
        unsigned int wid, unsigned int hgt,
        unsigned char rgb1[3], unsigned char rgb2[3],
        unsigned char grid_rgb1[3],
        unsigned char grid_rgb2[3],
        int thickness)
{
    Alloc(wid,hgt,24);

    // use ints to avoid negative overflow bugs
    const int width = wid;
    const int height = hgt;
    const int hsize = height/2;
    const int wsize = width/2;

    const double offset = symmetric ? 0.5 : 0.0;

    // checkerboard pattern
    for (int j = 0 ; j < height ; ++j)
    {
        for (int i = 0 ; i < width ; ++i)
        {
            int h = (int)((double)i/(double)hsize + offset);
            int v = (int)((double)j/(double)wsize + offset);
            bool h_odd = (h%2) == 0;
            bool v_odd = (v%2) == 0;
            if ((h_odd + v_odd)%2 == 0)
            {
                m_pData[m_WidthByte32*j + i*3    ] = rgb1[0];
                m_pData[m_WidthByte32*j + i*3 + 1] = rgb1[1];
                m_pData[m_WidthByte32*j + i*3 + 2] = rgb1[2];
            }
            else
            {
                m_pData[m_WidthByte32*j + i*3    ] = rgb2[0];
                m_pData[m_WidthByte32*j + i*3 + 1] = rgb2[1];
                m_pData[m_WidthByte32*j + i*3 + 2] = rgb2[2];
            }
        }
    }

    if (thickness <= 0)
        return;

    const int symm_hlocs[2] = { height/4, 3*height/4 };
    const int symm_wlocs[2] = {  width/4, 3* width/4 };
    const int asymm_hlocs[3] = { 0, height/2, height };
    const int asymm_wlocs[3] = { 0,  width/2,  width };
    const int nlines = symmetric ? sizeof(symm_hlocs)/sizeof(symm_hlocs[0]) : sizeof(asymm_hlocs)/sizeof(asymm_hlocs[0]);
    const int *hlocs = symmetric ? symm_hlocs : asymm_hlocs;;
    const int *wlocs = symmetric ? symm_wlocs : asymm_wlocs;;

    // horizontal grid lines
    for (int k = 0; k < nlines; ++k) {
        for (int j = hlocs[k] - thickness/2; j < hlocs[k] + thickness/2 ; ++j)
        {
            if (j < 0 || j >= height) continue;

            for (int i = 0 ; i < width ; ++i)
            {
                m_pData[m_WidthByte32*j + i*3    ] = grid_rgb1[0];
                m_pData[m_WidthByte32*j + i*3 + 1] = grid_rgb1[1];
                m_pData[m_WidthByte32*j + i*3 + 2] = grid_rgb1[2];
            }
        }
    }

    // vertical grid lines
    for (int k = 0; k < nlines; ++k) {
        for (int i = wlocs[k] - thickness/2; i < wlocs[k] + thickness/2 ; ++i)
        {
            if (i < 0 || i >= width) continue;

            for (int j = 0 ; j < height ; ++j)
            {
                m_pData[m_WidthByte32*j + i*3    ] = grid_rgb2[0];
                m_pData[m_WidthByte32*j + i*3 + 1] = grid_rgb2[1];
                m_pData[m_WidthByte32*j + i*3 + 2] = grid_rgb2[2];
            }
        }
    }

    // grid junctions
    for (int k1 = 0; k1 < nlines; ++k1)
        for (int k2 = 0; k2 < nlines; ++k2)
            for (int j = hlocs[k1] - thickness/2; j < hlocs[k1] + thickness/2 ; ++j)
            {
                if (j < 0 || j >= height) continue;

                for (int i = hlocs[k2] - thickness/2; i < hlocs[k2] + thickness/2 ; ++i)
                {
                    if (i < 0 || i >= width) continue;

                    m_pData[m_WidthByte32*j + i*3    ] = (unsigned char)( ((int)grid_rgb1[0] + (int)grid_rgb2[0])/2 );
                    m_pData[m_WidthByte32*j + i*3 + 1] = (unsigned char)( ((int)grid_rgb1[1] + (int)grid_rgb2[1])/2 );
                    m_pData[m_WidthByte32*j + i*3 + 2] = (unsigned char)( ((int)grid_rgb1[2] + (int)grid_rgb2[2])/2 );
                }
            }
}

////////////////////////////////////////////////////////////////////////////////

// generates pattern
//  XXOO
//  XXOO
//  OOXX
//  OOXX
void CTexture::GenerateCheckerBoard(unsigned int width,
                    unsigned int height, 
                    int size,
                    unsigned char r,
                    unsigned char g,
                    unsigned char b,
                    unsigned char rb,
                    unsigned char gb,
                    unsigned char bb)
{
  Alloc(width,height,24);

  for(unsigned j=0;j<height;j++)
    for(unsigned int i=0;i<width;i++)
      {
    int h = (int)((double)i/(double)size);
    int v = (int)((double)j/(double)size);
    bool h_odd = (h%2) == 0;
    bool v_odd = (v%2) == 0;
    if((h_odd	+ v_odd)%2 == 0)
      {
        m_pData[m_WidthByte32*j + i*3]   = rb;
        m_pData[m_WidthByte32*j + i*3+1] = gb;
        m_pData[m_WidthByte32*j + i*3+2] = bb;
      }
    else
      {
        m_pData[m_WidthByte32*j + i*3]   = r;
        m_pData[m_WidthByte32*j + i*3+1] = g;
        m_pData[m_WidthByte32*j + i*3+2] = b;
      }
      }
}

// generates pattern
//  XOOX
//  OXXO
//  OXXO
//  XOOX
void CTexture::GenerateSymmetricCheckerBoard(unsigned int width,
                    unsigned int height, 
                    int size,
                    unsigned char r,
                    unsigned char g,
                    unsigned char b,
                    unsigned char rb,
                    unsigned char gb,
                    unsigned char bb)
{
  Alloc(width,height,24);

  for(unsigned j=0;j<height;j++)
    for(unsigned int i=0;i<width;i++)
      {
    int h = (int)((double)i/(double)size + 0.5);
    int v = (int)((double)j/(double)size + 0.5);
    bool h_odd = (h%2) == 0;
    bool v_odd = (v%2) == 0;
    if((h_odd	+ v_odd)%2 == 0)
      {
        m_pData[m_WidthByte32*j + i*3]   = rb;
        m_pData[m_WidthByte32*j + i*3+1] = gb;
        m_pData[m_WidthByte32*j + i*3+2] = bb;
      }
    else
      {
        m_pData[m_WidthByte32*j + i*3]   = r;
        m_pData[m_WidthByte32*j + i*3+1] = g;
        m_pData[m_WidthByte32*j + i*3+2] = b;
      }
      }
}

void CTexture::GenerateVStripes(unsigned int width,
                unsigned int height, 
                int size,
                unsigned char r,
                unsigned char g,
                unsigned char b,
                unsigned char rb,
                unsigned char gb,
                unsigned char bb)
{
  Alloc(width,height,24);

  for(unsigned j=0;j<height;j++)
    for(unsigned int i=0;i<width;i++)
      {
    int h = (int)((double)i/(double)size);
    bool h_odd = (h%2) == 0;
    if(h_odd)
      {
        m_pData[m_WidthByte32*j + i*3]   = rb;
        m_pData[m_WidthByte32*j + i*3+1] = gb;
        m_pData[m_WidthByte32*j + i*3+2] = bb;
      }
    else
      {
        m_pData[m_WidthByte32*j + i*3]   = r;
        m_pData[m_WidthByte32*j + i*3+1] = g;
        m_pData[m_WidthByte32*j + i*3+2] = b;
      }
      }
}

void CTexture::GenerateHStripes(unsigned int width,
                unsigned int height, 
                int size,
                unsigned char r,
                unsigned char g,
                unsigned char b,
                unsigned char rb,
                unsigned char gb,
                unsigned char bb)
{
  Alloc(width,height,24);

  for(unsigned j=0;j<height;j++)
    for(unsigned int i=0;i<width;i++)
      {
    int v = (int)((double)j/(double)size);
    bool v_odd = (v%2) == 0;
    if(v_odd)
      {
        m_pData[m_WidthByte32*j + i*3]   = rb;
        m_pData[m_WidthByte32*j + i*3+1] = gb;
        m_pData[m_WidthByte32*j + i*3+2] = bb;
      }
    else
      {
        m_pData[m_WidthByte32*j + i*3]   = r;
        m_pData[m_WidthByte32*j + i*3+1] = g;
        m_pData[m_WidthByte32*j + i*3+2] = b;
      }
      }
}


void CTexture::Jet() 
{
    Alloc(256,1,24);
  for(unsigned int i = 0; i < 3*256; i++) 
    m_pData[i] = JetMap[i];

}

void CTexture::GenerateRamp(unsigned int width,
                unsigned char r,
                unsigned char g,
                unsigned char b,
                unsigned char rb,
                unsigned char gb,
                unsigned char bb)
{
  const unsigned int height = 1;
  Alloc(width,height,24);

  for(unsigned int i=0;i<width;i++){
    unsigned char ir =
      char(min(max(0.,double(r)+double(i)/(width-1)*(rb-r)),255.));
    unsigned char ig =
      char(min(max(0.,double(g)+double(i)/(width-1)*(gb-g)),255.));
    unsigned char ib =
      char(min(max(0.,double(b)+double(i)/(width-1)*(bb-b)),255.));
    m_pData[i*3]   = ir;
    m_pData[i*3+1] = ig;
    m_pData[i*3+2] = ib;
  }
}

//***************************************
// GenerateGrid
//***************************************
void CTexture::GenerateGrid(unsigned int width,
                unsigned int height, 
                int size,
                unsigned int thickness,
                unsigned char r,
                unsigned char g,
                unsigned char b,
                unsigned char rb,
                unsigned char gb,
                unsigned char bb)
{
  Alloc(width,height,24);

  // fill background
  unsigned int i,j;
  for(j=0;j<height;j++)
    for(i=0;i<width;i++)
      {
    m_pData[m_WidthByte32*j + i*3]   = rb;
    m_pData[m_WidthByte32*j + i*3+1] = gb;
    m_pData[m_WidthByte32*j + i*3+2] = bb;
      }

  // horizontal
  for(j=height/2-thickness/2;j<height/2+thickness/2;j++)
    {
      for(i=0;i<width;i++)
    {
      m_pData[m_WidthByte32*j + i*3]   = r;
      m_pData[m_WidthByte32*j + i*3+1] = g;
      m_pData[m_WidthByte32*j + i*3+2] = b;
    }
    }

  // vertical
  for(unsigned int i=width/2-thickness/2;i<width/2+thickness/2;i++)
    for(unsigned int j=0;j<height;j++)
      {
    m_pData[m_WidthByte32*j + i*3]   = r;
    m_pData[m_WidthByte32*j + i*3+1] = g;
    m_pData[m_WidthByte32*j + i*3+2] = b;
      }
}

void CTexture::GenerateGradientH(unsigned int width,
                 unsigned int height,
                 int size)
{
  Alloc(width,height,24);

  // fill background
  for(unsigned int i=0;i<width;i++)
    {
      unsigned char g = (unsigned char)(255.0 * (double)i / (double)(width-1));
      for(unsigned int j=0;j<height;j++)
    {
      m_pData[m_WidthByte32*j + i*3]   = g;
      m_pData[m_WidthByte32*j + i*3+1] = g;
      m_pData[m_WidthByte32*j + i*3+2] = g;
    }
    }
}
void CTexture::GenerateGradientV(unsigned int width,
                 unsigned int height,
                 int size)
{
  Alloc(width,height,24);

  // fill background
  for(unsigned int j=0;j<height;j++)
    {
      unsigned char g = (unsigned char)(255.0 * (double)j / (double)(height-1));
      for(unsigned int i=0;i<width;i++)
    {
      m_pData[m_WidthByte32*j + i*3]   = g;
      m_pData[m_WidthByte32*j + i*3+1] = g;
      m_pData[m_WidthByte32*j + i*3+2] = g;
    }
    }
}

//***************************************
// GenerateLines
//***************************************
void CTexture::GenerateLines(unsigned int width,
                 unsigned int height, 
                 int size,
                 unsigned char r,
                 unsigned char g,
                 unsigned char b,
                 unsigned char rb,
                 unsigned char gb,
                 unsigned char bb)
{
  Alloc(width,height,24);

  // fill background
  unsigned int i,j;
  for(j=0;j<height;j++)
    for(i=0;i<width;i++)
      {
    m_pData[m_WidthByte32*j + i*3]   = rb;
    m_pData[m_WidthByte32*j + i*3+1] = gb;
    m_pData[m_WidthByte32*j + i*3+2] = bb;
      }

  // horizontal zebra
  for(j=0;j<height;j++)
    {
      if((j/size)%2 == 0)
    for(i=0;i<width;i++)
      {
        m_pData[m_WidthByte32*j + i*3]   = r;
        m_pData[m_WidthByte32*j + i*3+1] = g;
        m_pData[m_WidthByte32*j + i*3+2] = b;
      }
    }
}

/*
// ********************************************
// ReadFromResource
// ********************************************
int CTexture::ReadFromResource(const WORD ResourceId)
{
  // Check
  ASSERT(ResourceId != 0);

  // Secret hand-made decomp. method
  HINSTANCE hInst = AfxGetResourceHandle();
  HRSRC hrsrc = ::FindResource(hInst,MAKEINTRESOURCE(ResourceId),"DIB");
  if(hrsrc == NULL) 
    {
      Util::error("DIB resource not found");
      return 0;
    }

  // Famous decomp. algorithm
  HGLOBAL hg = LoadResource(hInst, hrsrc); // you do not need to call any free
  if(hg == NULL) 
    {
      Util::error("Failed to load DIB resource");
      return FALSE;
    }

  void* pRes = LockResource(hg);
  ASSERT(pRes != NULL);
  int size = ::SizeofResource(hInst, hrsrc);

  // This must be a windows .BMP file
  BitmapFileHeader FileHeader;
  memcpy(&FileHeader,pRes,sizeof(BitmapFileHeader));

  // Is it a Windows BMP file ? (BM)
  if(FileHeader.bfType != 0x4D42)
    {
      AfxMessageBox("Invalid BMP file");
      return 0;
    }

  // Image header
  memcpy(&m_Header,(byte *)pRes+sizeof(BitmapFileHeader),sizeof(BitmapInfoHeader));

  // 24 bits ?
  if(m_Header.biPlanes != 1 ||
     m_Header.biBitCount != 24)
    {
      AfxMessageBox("Image file must have 24 bits depth");
      return 0;
    }

  // Alloc (does call Free before)
  Free();
  unsigned char *m_pData = (unsigned char*)malloc(m_Header.biSizeImage sizeof(unsigned char));
  if(m_pData == NULL)
    {
      AfxMessageBox("Insufficient memory");
      return 0;
    }

  // Update datas
  m_Width = m_Header.biWidth;
  m_Height = m_Header.biHeight;
  m_Depth = m_Header.biBitCount;
  Log.println("Image : (%d x %d x %d bits)",m_Width,m_Height,m_Depth);

  // Image reading
  CMemoryException e;
  memcpy(m_pData,(byte *)((byte *)pRes+FileHeader.bfOffBits),m_Header.biSizeImage);

  // Success, also set FileName
  //m_FileName = _T("FromResource.bmp");

  UpdateWidthByte32();

  return 1;
}
*/
// ** EOF **

////////////////////////////////////////////////////////////////////////////////

