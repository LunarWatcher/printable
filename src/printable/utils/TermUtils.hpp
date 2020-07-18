#pragma once

#include <iostream>
#include <ostream>
#include <filesystem>

#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#include <windows.h>

#define WINDOZE
// TTY check
#define ISATTY _isatty
#define FILENO _fileno

// ANSI support check
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
// Hard-code the variable.
// We can use this to tell if ANSI is supported or not
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#else
#include <sys/ioctl.h>
#include <unistd.h>

// TTY check
#define ISATTY isatty
#define FILENO fileno
#endif

// End IO includes
#include <memory>
#include "EnvVariableHelper.hpp"

namespace printable {

namespace TermUtils {

inline void enableTermColors();
/**
 * This contains static information used to indentify certain ANSI-related
 * values.
 */
class TermData {
private:
    static inline std::shared_ptr<TermData> INSTANCE = nullptr;
public:
    
    /**
     * Somewhat poorly named, but this variable indicates whether or not there's a special
     * reason ANSI will not be supported.
     * Internally, this is only used in the supportsAnsi() function.
     */
    bool specialSupportAnsi;
    bool hasDevNull;

    static std::shared_ptr<TermData> getInstance() {
        if (INSTANCE == nullptr) {
            INSTANCE = std::make_shared<TermData>();
            std::filesystem::path devnull("/dev/null");
            if (std::filesystem::exists(devnull)) {
                INSTANCE->hasDevNull = true;
            }
#ifdef WINDOZE
            else {
                enableTermColors();
            }
#endif
        }
        return INSTANCE;
    }
};

/**
 * Checks whether a given ostream is a terminal or not.
 * To be more specific, it also checks whether std::cout
 * and std::cerr have been redirected, and therefore
 * aren't "terminals". (Failing hard on the right words
 * to use here, but you hopefully get the idea :eyes:)
 *
 * Possibly broken in MinGW and/or Cygwin.
 */
inline bool isTerminal(std::ostream* stream) {
    return (stream == &std::cout && ISATTY(FILENO(stdout))) || (stream == &std::cerr && ISATTY(FILENO(stderr)));
}

/**
 * Returns whether the terminal supports ansi or not.
 * Always returns false if the provided ostream
 * isn't a terminal.
 *
 */
inline bool supportsAnsi(std::ostream* stream) {
    // Main check; if we're not in a terminal, there's a 100% chance we don't support ANSI
    // This is also in part because adding ANSI to a file, if piped properly, is really not a good idea
    // Also means any hooks or other programs trying to read Taskranger's output doesn't get useless bytes
    if (!isTerminal(stream)) {
        return false;
    }
    // There's about half a billion approaches to this, but there's one thing I'm extremely
    // brutely going to assume:
    // ANYONE with a C++17 compiler also has a modern enough terminal to support ANSI,
    // provided /dev/null exists. No, really.
    // /dev/null indicates Linux, Cygwin, or MinGW at runtime, Cygwin and MinGW both
    // support ANSI, Linux falls back to the previous statement.
    // MinGW and Cygwin should, IIRC, be able to run Windows executables without any
    // problem, which means it's perfectly possible to build Taskranger with MSVC, and
    // run in CMD, MinGW, or Cygwin.
    // This causes a few challenges, but it's really the runtime environment that matters.
    // If it has /dev/null, it's probably made by people who didn't insist on their own
    // shit system for a few years, and probably supports ANSI
    if (TermData::getInstance()->hasDevNull) {
        return true;
    }

    // Windoze, specifically MinGW/Cygwin. Also enables an override for, well, everything
    if (EnvVariable::getEnv("ANSICON", "nej") != "nej") {
        return true;
    }

    // This is put far down to make sure we can run cygwin and mingw tests first
    if (!TermData::getInstance()->specialSupportAnsi) {
        return false;
    }
    // In the rest of the cases, let's just assume we're good
    return true;
}

inline int getWidth() {
#ifdef WINDOZE
    // Windows likes to yell. {{{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    // }}}
#else
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return size.ws_col;
#endif
}

#ifdef WINDOZE


/**
 * Checks whether or not the given HANDLE is a console output stream,
 * or if it's a file. This exists alongside isTerminal because HANDLE
 * AND OTHER WINDOWS API COMPONENTS THAT JUST REALLY LIKE SHOUTING.
 * KINDA LOOKS LIKE A TERMS OF SERVICE WITH ALL THAT CAPS LOCK
 *
 * Returns true if console, false otherwise
 */
inline bool winIsConsole(HANDLE file) {
    return (GetFileType(file) & 3) == FILE_TYPE_CHAR;
}

inline void setTermMode(HANDLE handle) {
    if (!TermData::getInstance()->specialSupportAnsi) {
        return;
    }
    // THE WINDOWS API REALLY LIKES TO SHOUT
    // SOMEONE SHOULD TELL THEM IT'S POSSIBLE TO TURN OFF CAPS-LOCK
    // Note to self: LPDWORD is a pointer to a DWORD
    // So is PDWORD, because Windows :facepaw:
    DWORD mode;
    if (!GetConsoleMode(handle, &mode)) {
        // We failed to get the console mode; abort.
        return;
    }
    if (!SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        auto e = GetLastError();
        if (e == STATUS_INVALID_PARAMETER) {
            // This should be the code triggered by a lack of
            // ANSI support in CMD
            TermData::getInstance()->specialSupportAnsi = false;
        }
    }
}


inline void enableTermColors() {
    // https://docs.microsoft.com/en-us/windows/console/getstdhandle
    HANDLE outStream = GetStdHandle(-11);
    HANDLE outErrStream = GetStdHandle(-12);

    if (isConsole(outStream)) {
        setTermMode(outStream);
    }
    if (isConsole(outErrStream)) {
        setTermMode(outErrStream);
    }
}

#endif


}
}
