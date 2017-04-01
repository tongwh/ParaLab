#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <iostream>
#include <sstream>
#include <complex>

// System high-level support (e.g. file name manipulation)
namespace Sys
{

// returns whether the file exists
bool FileExists(const char* fname);
bool FileExists(const std::string &fname);
// returns the file size
long FileSize(const char* fname);
long FileSize(const std::string &fname);
// returns the file name prefix (i.e. without the extension)
std::string GetPrefix(const std::string &path);
// returns the lowercase file name extension
std::string GetExtension(const std::string &path);
// returns the filename with the extension changed
std::string ChangeExtension(const std::string &path, const std::string &new_ext);
// returns the directory of the file name
std::string GetDirectory(const std::string &path);
// joins the path together using the appropriate separator
std::string JoinPaths(const char *path_base, const char *path_relative);
std::string JoinPaths(const std::string &path_base, const char *path_relative);
std::string JoinPaths(const std::string &path_base, const std::string &path_relative);

} // namespace Sys

namespace Util
{

// numerics
bool isnan(double v);
bool isnan(std::complex<double> v);
bool isinf(double v);
bool isinf(std::complex<double> v);
bool isfinite(double v);
bool isfinite(std::complex<double> v);

/*
// C++ % is implementation-dependent for negative numerator. This makes it
// consistent.
// XXX Deal with negative denominator?
// Always returns a positive mod
// Note that: (-1*n) % b = (((b-1) - b)) * n) % b = ((b-1)*n) % b
inline int mod(int n, int b) { return n < 0 ? ((1-b)*n) % b : n % b; }
*/

// error and warning output
void error(const char *format, ...);
void warn(const char *format, ...);
void message(const char *format, ...);

// string utilities
// trim whitespace from the right
std::string &rtrim(std::string &s); // in-place

// read line, trimming trailing whitespace
std::istream &readline(std::istream &is, std::string &line);

// read line ignoring comments and blank lines
std::istream &readline_nocomments(std::istream &is, std::string &line, const char *comment_prefix = "#");

// convert an object to a string
template <typename T>
std::string to_string(const T &val)
{
    std::ostringstream os;
    os << val;
    return os.str();
}

} // namespace Util

// For Visual studio
// Note that the return type is not compatible with the gcc snprintf()
#ifdef WIN32
#define snprintf _snprintf
#endif

#endif // UTIL_H

