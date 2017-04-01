#include "logger.h"

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <assert.h>

#include <boost/test/utils/nullstream.hpp>

#ifdef _WINDOWS
#include <windows.h>
#endif

// output ap to file
#define OUTPUT_VA_LIST(VA_message_prefix, type, VA_format) \
    va_list ap; \
    va_start(ap, VA_format); \
    vprintf_helper(VA_message_prefix, type, VA_format, ap); \
    va_end(ap); \


static const int  INDENT_SIZE = 2;
static const int  BUFFER_SIZE = 4096;
static const char START_PREFIX[] = ">> ";
static const char END_PREFIX[] = "<< ";
static const char WARNING_PREFIX[] = "!!! WARNING !!! ";
static const char ERROR_PREFIX[] = "!!! ERROR !!! ";


Logger::Logger()
{
    initialize();
}


Logger::Logger(int log_level)
{
    initialize();
    m_logging_level = log_level;
}


Logger::~Logger()
{
    deinitialize();
}


// Note that this should not be called except by the construction or after
// an object had already been deinitialized. Otherwise, it is possible for a
// file to a file to not be closed.
void Logger::initialize()
{
	m_quiet = false;
	m_cout_sbuf = m_cerr_sbuf = NULL;
    m_indent = 0;  // indentation level
    m_logging_level = 0;   // logging level
    m_normal_prefix = "~~ ";  // prefix (displayed before indentation)
    m_warn_prefix = "WW ";  // prefix (displayed before indentation)
    m_error_prefix = "EE ";  // prefix (displayed before indentation)
    m_out = dynamic_cast<std::ostream *>(&std::cerr);  // logging destination
    m_clean_up_file = false;
    m_auto_flush = false;
    m_null = std::auto_ptr<std::ostream>(new boost::onullstream()); // acts like /dev/null
}


// must have been initialized first
void Logger::deinitialize()
{
	restore_cout();
    m_indent = 0;  // indentation level
    m_logging_level = 0;   // logging level
    m_normal_prefix = "~~ ";  // prefix (displayed before indentation)
    m_warn_prefix = "WW ";  // prefix (displayed before indentation)
    m_error_prefix = "EE ";  // prefix (displayed before indentation)
    clean_up_file();   // close the file if opened by Logger
    m_auto_flush = false;
}


void Logger::output_header()
{
    (*m_out) << "============================================================\n";
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    (*m_out) << asctime(timeinfo) << std::endl;
    (*m_out) << "============================================================\n";
}


void Logger::clean_up_file()
{
    if (m_clean_up_file)
    {
        m_out->flush();
        delete m_out;
    }
    m_out = &std::cerr;
    m_clean_up_file = false;
}


// returns 0 on success
int Logger::use_file(const char *fname, bool append)
{
    std::ofstream *pfout = append ? new std::ofstream(fname, std::ios::app)
                                  : new std::ofstream(fname);
    if (pfout->fail()) {
        std::cerr << "Unable to open file fname for writing." << std::endl;
        delete pfout;
        return 1;
    }

    clean_up_file(); // close current logging file if needed

    m_out = dynamic_cast<std::ostream *>(pfout); // set new logging file
    m_clean_up_file = true;
    return 0;
}


void Logger::use_stream(std::ostream &out)
{
    clean_up_file();
    m_out = dynamic_cast<std::ostream *>(&out); // set new logging file
    m_clean_up_file = false;
}


// logging level
int Logger::level() const { return m_logging_level; }
int Logger::set_level(int log_level) { m_logging_level = log_level; return m_logging_level; }

// logging quiet
void Logger::restore_cout()
{
	if ( m_cerr_sbuf ) {
		std::cerr.rdbuf(m_cerr_sbuf);
		m_cerr_sbuf = NULL;
	}
	if ( m_cout_sbuf ) {
		std::cout.rdbuf(m_cout_sbuf);
		m_cout_sbuf = NULL;
	}
}

bool Logger::quiet() const { return m_quiet; }
bool Logger::quiet(bool q) 
{ 
	if ( q == m_quiet )
		return q;
	bool oldq = m_quiet;
	m_quiet = q;
	m_out = q ? &m_stbuf : dynamic_cast<std::ostream *>(&std::cerr);

	// Redirect for all those who don't use logger...
	// Muffling cout doesn't muffle printf.
	if ( m_quiet ) {
		assert(m_cerr_sbuf == NULL && m_cout_sbuf == NULL);
		m_cerr_sbuf = std::cerr.rdbuf();
		m_cout_sbuf = std::cout.rdbuf();
		std::cout.rdbuf(m_stbuf.rdbuf()); // redirect
		std::cerr.rdbuf(m_stbuf.rdbuf()); // redirect
	} else {
		restore_cout();
	}

	return oldq;
}

// logging prefix
std::string &Logger::set_normal_prefix(const char *log_prefix) { m_normal_prefix = log_prefix; return m_normal_prefix; }
const std::string &Logger::normal_prefix() const { return m_normal_prefix; }
std::string &Logger::set_warn_prefix(const char *log_prefix) { m_warn_prefix = log_prefix; return m_warn_prefix; }
const std::string &Logger::warn_prefix() const { return m_warn_prefix; }
std::string &Logger::set_error_prefix(const char *log_prefix) { m_error_prefix = log_prefix; return m_error_prefix; }
const std::string &Logger::error_prefix() const { return m_error_prefix; }

// auto-flush
bool Logger::auto_flush() const { return m_auto_flush; }
bool Logger::set_auto_flush(bool auto_flush) { m_auto_flush = auto_flush; return m_auto_flush; }

// force-flush the output
Logger& Logger::flush()
{
    m_out->flush();
    return *this;
}

// output the indented prefix
void Logger::output_prefix(const char *message_prefix, int type)
{
    (*m_out) << (
            (type == MSG_WARN) ? m_warn_prefix :
            (type == MSG_ERROR) ? m_error_prefix : m_normal_prefix
            );

    (*m_out) << std::setw(m_indent * INDENT_SIZE) << "" << message_prefix;
}

////////////////////////////////////////////////////////////
// FORMATTED OUTPUT VERSIONS

// output functions
void
Logger::vprintf_helper(const char *message_prefix, int type, const char *format, va_list ap)
{
	if ( m_quiet )
		return;
    output_prefix(message_prefix, type);

    char str[BUFFER_SIZE+1];
    ::vsnprintf(str, BUFFER_SIZE, format, ap);

    (*m_out) << str << std::endl;
    if (m_auto_flush) m_out->flush();
}


Logger& Logger::println(int level, const char *format, ...)
{
    if (m_logging_level < level) return *this;
    OUTPUT_VA_LIST("", MSG_NORMAL, format);
    return *this;
}


Logger& Logger::start_block(int level, const char *format, ...)
{
    if (m_logging_level < level) return *this;
    OUTPUT_VA_LIST(START_PREFIX, MSG_NORMAL, format);
    ++m_indent;
    return *this;
}


Logger& Logger::end_block(int level, const char *format, ...)
{
    if (m_logging_level < level) return *this;
    --m_indent;
    OUTPUT_VA_LIST(END_PREFIX, MSG_NORMAL, format);
    return *this;
}


Logger& Logger::println(const char *format, ...)
{
    OUTPUT_VA_LIST("", MSG_NORMAL, format);
    return *this;
}


Logger& Logger::start_block(const char *format, ...)
{
    OUTPUT_VA_LIST(START_PREFIX, MSG_NORMAL, format);
    ++m_indent;
    return *this;
}


Logger& Logger::end_block(const char *format, ...)
{
    --m_indent;
    OUTPUT_VA_LIST(END_PREFIX, MSG_NORMAL, format);
    return *this;
}


Logger& Logger::emphasize(const char *format, ...)
{
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
#endif
    OUTPUT_VA_LIST("", MSG_NORMAL, format);
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
    return *this;
}


Logger& Logger::warn(const char *format, ...)
{
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
#endif
    OUTPUT_VA_LIST(WARNING_PREFIX, MSG_WARN, format);
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
    return *this;
}


Logger& Logger::error(const char *format, ...)
{
	bool q = quiet(false);
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
#endif
    OUTPUT_VA_LIST(ERROR_PREFIX, MSG_ERROR, format);
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
	quiet(q);
    return *this;
}


////////////////////////////////////////////////////////////
// OSTREAM VERSIONS

std::ostream& Logger::print(int level)
{
    if (m_logging_level < level) return *m_null;
    output_prefix("", MSG_NORMAL);
    return *m_out;
}


std::ostream& Logger::print()
{
    output_prefix("", MSG_NORMAL);
    return *m_out;
}


std::ostream& Logger::start_block(int level)
{
    if (m_logging_level < level) return *m_null;
    output_prefix(START_PREFIX, MSG_NORMAL);
    ++m_indent;
    return *m_out;
}


std::ostream& Logger::start_block()
{
    output_prefix(START_PREFIX, MSG_NORMAL);
    ++m_indent;
    return *m_out;
}


std::ostream& Logger::end_block(int level)
{
    if (m_logging_level < level) return *m_null;
    --m_indent;
    output_prefix(END_PREFIX, MSG_NORMAL);
    return *m_out;
}


std::ostream& Logger::end_block()
{
    --m_indent;
    output_prefix(END_PREFIX, MSG_NORMAL);
    return *m_out;
}


std::ostream& Logger::emphasize()
{
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
#endif
    output_prefix("", MSG_NORMAL);
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
    return *m_out;
}


std::ostream& Logger::warn()
{
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
#endif
    output_prefix(WARNING_PREFIX, MSG_WARN);
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
    return *m_out;
}


std::ostream& Logger::error()
{
	bool q = quiet(false);
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
#endif
    output_prefix(ERROR_PREFIX, MSG_ERROR);
#ifdef _WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
	quiet(q);
    return *m_out;
}


////////////////////////////////////////////////////////////
// Block object (RAII-based indentation block)

Logger::Block Logger::block(int log_level, const char *message)
{
    return Block(*this, log_level, message);
}
Logger::Block Logger::block(int log_level, const std::string &message)
{
    return Block(*this, log_level, message.c_str());
}

Logger::Block::Block(Logger &log, int log_level, const char *message)
{
    m_pLog = &log;
    strncpy(m_message, message, sizeof(m_message)/sizeof(char)); // not automatically null-terminated if overflowing
    m_message[sizeof(m_message)/sizeof(char)-1] = '\0';

    m_enabled = (log_level <= m_pLog->m_logging_level);
    if (m_enabled)
        m_pLog->start_block(m_message);
}

/*
// A block that is copied using a copy-constructor is disabled since control
// is passed to the block that got the copy. This avoids outputting stuff
// twice when using the factory pattern to create an object.
Logger::Block::Block(Block &block)
{
    m_enabled = block.m_enabled;
    block.m_enabled = false;

    // copy the contents
    m_pLog = block.m_pLog;
    strncpy(m_message, block.message, sizeof(m_message)/sizeof(char)); // not automatically null-terminated if overflowing
    m_message[sizeof(m_message)/sizeof(char)-1] = '\0';

    // no need to output again since block's construct would have already
    // output the header
}
*/

Logger::Block::~Block()
{
    if (m_enabled)
        m_pLog->end_block(m_message);
}

