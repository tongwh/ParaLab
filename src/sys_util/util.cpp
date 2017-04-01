#include "util.h"
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <sys/stat.h>

////////////////////////////////////////////////////////////
// System high-level support (e.g. file name manipulation)
////////////////////////////////////////////////////////////

namespace Sys
{

#ifdef WIN32
const char PATH_SEPARATOR = '\\';
#else
const char PATH_SEPARATOR = '/';
#endif
const char EXTENSION_SEPARATOR = '.';

////////////////////////////////////////

// returns whether the file exists
bool FileExists(const char* fname)
{
  struct stat finfo;
  int ret;
  ret = stat(fname,&finfo); 
  // 0 means the file is there
  return (ret == 0); 
}

bool FileExists(const std::string &fname)
{
    return FileExists(fname.c_str());
}

////////////////////////////////////////

// from http://www.codeproject.com/KB/files/filesize.aspx
long FileSize(const char* fname)
{
    struct stat finfo;
    if (stat(fname, &finfo) == 0)
        return finfo.st_size;
    else
        return 0;
}

long FileSize(const std::string &fname)
{
    return FileSize(fname.c_str());
}

////////////////////////////////////////

// returns the file name prefix (i.e. without the extension)
std::string
GetPrefix(const std::string &path)
{
    if (path.size() > 0)
        return path.substr(0, path.find_last_of('.'));
    else
        return std::string("");
}

////////////////////////////////////////

// returns the lowercase file name extension
std::string
GetExtension(const std::string &path)
{
    std::string extension;
    size_t ext_start = path.find_last_of(EXTENSION_SEPARATOR);
    if (ext_start < path.length())
        extension = path.substr(ext_start, path.length());
    for(std::string::iterator i = extension.begin(); i != extension.end(); ++i)
        *i = (std::string::value_type)tolower(*i);
    return extension;
}

////////////////////////////////////////

// returns the filename with the extension changed
std::string ChangeExtension(const std::string &path, const std::string &new_ext)
{
    return Sys::GetPrefix(path).append(".").append(new_ext);
}

////////////////////////////////////////

// returns the directory of the file name
std::string
GetDirectory(const std::string &path)
{
    if (path.size() > 0)
        return path.substr(0, path.find_last_of(PATH_SEPARATOR));
    else
        return std::string("");
}

////////////////////////////////////////

// joins the path together
std::string
JoinPaths(const char *path_base, const char *path_relative)
{
    return (std::string(path_base) += PATH_SEPARATOR) += path_relative;
}

std::string
JoinPaths(const std::string &path_base, const char *path_relative)
{
    return JoinPaths(path_base.c_str(), path_relative);
}

std::string
JoinPaths(const std::string &path_base, const std::string &path_relative)
{
    return JoinPaths(path_base.c_str(), path_relative.c_str());
}

////////////////////////////////////////////////////////////

} // namespace Sys

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

namespace Util
{

bool isnan(double v) { return !(v == v); }
bool isnan(std::complex<double> v) { return isnan(v.real()) || isnan(v.imag()); }
bool isinf(double v) { return (v != 0) && (v*2 == v); }
bool isinf(std::complex<double> v) { return isinf(v.real()) || isinf(v.imag()); }
bool isfinite(double v) { return !isnan(v) && !isinf(v); }
bool isfinite(std::complex<double> v) { return !isnan(v) && !isinf(v); }

////////////////////////////////////////////////////////////
// error and warning output

void
error(const char *format, ...)
{
    fprintf(stdout, "*** ERROR: ");
    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
    fflush(stdout);
    fprintf(stdout, "\n");
}

void
warn(const char *format, ...)
{
    fprintf(stdout, "*** WARNING: ");
    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
    fflush(stdout);
    fprintf(stdout, "\n");
}

void
message(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
    fflush(stdout);
    fprintf(stdout, "\n");
}

////////////////////////////////////////////////////////////
// string utilities

const char WHITESPACE[] = " \t\r\n";

std::string &
rtrim(std::string &s)
{
    size_t pos = s.find_last_not_of(WHITESPACE);
    s.erase(pos == std::string::npos ? 0 : pos+1);
    return s;
}

////////////////////////////////////////////////////////////

std::istream &
readline(std::istream &is, std::string &line)
{
    if (getline(is, line)) {
        rtrim(line);
    }
    return is;
}

////////////////////////////////////////////////////////////

std::istream &
readline_nocomments(std::istream &is, std::string &line, const char *comment_prefix)
{
    const size_t comment_len = strlen(comment_prefix);
    while (getline(is, line)) {
        rtrim(line);
        if (line.size() != 0 && (line.substr(0, comment_len) != comment_prefix))
            break;
    }
    return is;
}

////////////////////////////////////////////////////////////

} // namespace Util

