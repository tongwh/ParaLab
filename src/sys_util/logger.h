/**
 * Simple formatted-logger class with built-in support for indentation and
 * outputting to a user-specified stream.
 *
 * To see an example of how it works, check out test/test_logger.cpp.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <memory>
#include <sstream>

class Logger
{
public:
    Logger();
    Logger(int log_level);
    ~Logger();

    int level() const;
    int set_level(int log_level);
	void restore_cout();
	bool quiet() const;
	bool quiet(bool q);
    std::string &set_normal_prefix(const char *log_prefix);
    const std::string &normal_prefix() const;
    std::string &set_warn_prefix(const char *log_prefix);
    const std::string &warn_prefix() const;
    std::string &set_error_prefix(const char *log_prefix);
    const std::string &error_prefix() const;
    bool auto_flush() const;
    bool set_auto_flush(bool auto_flush);
    Logger& flush();

    // returns 0 on success
    int use_file(const char *fname, bool append = false);
    void use_stream(std::ostream &out);

    // formatted output versions
    Logger& println(int level, const char *format, ...);
    Logger& start_block(int level, const char *format, ...);
    Logger& end_block(int level, const char *format, ...);
    Logger& println(const char *format, ...);
    Logger& start_block(const char *format, ...);
    Logger& end_block(const char *format, ...);
	Logger& emphasize(const char *format, ...);
    Logger& warn(const char *format, ...);
    Logger& error(const char *format, ...);

    // ostream versions
    std::ostream& print(int level);
    std::ostream& print();
    std::ostream& start_block(int level);
    std::ostream& start_block();
    std::ostream& end_block(int level);
    std::ostream& end_block();
	std::ostream& emphasize();
    std::ostream& warn();
    std::ostream& error();

    // This class is useful for auto-indenting within code blocks and
    // methods.
    class Block {
    protected:
        Block(Logger &log, int log_level, const char *message);
        //Block(Block &block);
        Logger *m_pLog;
        bool m_enabled;
        char m_message[1025];

        friend class Logger;
    public:
        ~Block();
    };

    // create a block object
    Block block(int log_level, const char *message);
    Block block(int log_level, const std::string &message);

protected:
    enum { MSG_NORMAL = 0, MSG_WARN = 1, MSG_ERROR = 2 };

    int m_indent;  // indentation level
    int m_logging_level;  // logging level
	bool m_quiet; // be quiet
	std::streambuf *m_cout_sbuf, *m_cerr_sbuf;
    std::string m_normal_prefix; // prefix (displayed before indentation)
    std::string m_warn_prefix; // prefix (displayed before indentation)
    std::string m_error_prefix; // prefix (displayed before indentation)
    std::ostream *m_out;  // logging destination
	std::stringstream m_stbuf;
    std::auto_ptr<std::ostream> m_null; // null IO stream (useful for discarding)
    bool m_clean_up_file; // whether the file needs to be closed and delete-ed
    bool m_auto_flush;    // whether to flush the output every time

    void initialize();
    void deinitialize();
    void clean_up_file();
    void output_header();
    void vprintf_helper(const char *message_prefix, int type, const char *format, va_list ap);
    void output_prefix(const char *message_prefix, int type);

private:
    // disallow copying
    Logger(const Logger &log);
    Logger &operator=(const Logger &log);
};

#endif // LOGGER_H

