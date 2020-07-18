#pragma once

#include <string>
#include <stdlib.h>

namespace printable {

/**
 * This namespace exports a single function: getEnv(const char*, const std::string& fail (optional))
 * that may or may not be secure, dependong on the platform.
 * On Windows, the function uses _dupenv_s under the hood.
 * On Linux and other operating systems, it depends on availability.
 * At this time, only Linux is supported with a secure getenv function - the rest fall back
 * to getenv.
 */
namespace EnvVariable {

#if defined(_WIN32) || defined(_WIN64)
inline std::string getEnv(const char* name, const std::string& fail = "") {
    char* value = nullptr;
    size_t len = 0;
    errno_t err = _dupenv_s(&value, &len, name);
    if (err != 0 || value == nullptr)
        return fail;

    std::string retVal(value);
    return retVal;
}
#else
inline std::string getEnv(const char* name, const std::string& fail = "") {
#ifdef _GNU_SOURCE
    // _GNU_SOURCE is predefined with libstdc++ (GCC's stdlib),
    // and when it's defined, it grants access to secure_getenv.
    // Dunno if this works outside linux, hence why there's no
    // forced enablign of _GNU_SOURCE
    char* output = secure_getenv(name);
    if (!output)
        return fail;
    return std::string(output);
#else
#pragma message("WARNING: your OS doesn't have secure_getenv(). If this is a mistake, " \
                "please open an issue on GitHub (https://github.com/lunarwatcher/printable. "\
                " To make it easier for me to add, " \
                "documentation links to the function are appreciated.")
    char* output = getenv(name);
    if (!output)
        return fail;
    return std::string(output);
#endif
}

#endif

}
}
