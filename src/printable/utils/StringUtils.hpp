#pragma once

#include <iostream>
#include <regex>
#include <string>
#include <vector>

namespace printable {
namespace StringUtils {

inline std::vector<std::string> regexSplit(const std::regex& pattern, const std::string& source) {
    std::sregex_token_iterator it(source.begin(), source.end(), pattern, -1);
    std::sregex_token_iterator end;
    std::vector<std::string> split;
    while (it != end) {
        split.push_back(*it);
        it++;
    }

    return split;
}

inline std::vector<std::string> wrapString(
        const std::string& sourceString, const long long targetLength, bool hasNewline = true) {
    // Target length compensated for hard wrap string additions (-)
    const auto targetLenIdx = targetLength - 1;
    std::vector<std::string> content;

    // String wrap is a bit tricky.
    // In addition to the software wrap, we want to account for user-defined newlines.
    // That means this function has to both split up newlines, as well as do the actual processing
    if (hasNewline) {
        // okay, we have a raw string.
        // this is where stuff gets... "fun".
        // There's effectively 3 different types of separators that may be dished out, depending on the OS:
        // \n, \r, \n\r
        // which means, you gussed it! Regex!
        // One minor caveat: to enable consecutive newlines,
        // this can't be shortened to [\n\r]+. That globs \n\n into a single line,
        // which may not be desired.
        // Admittedly, I don't actually remember whether windows used \n\r or \r\n,
        // sooooooooooo this uses both.
        std::regex newlineRegex("(\n|\r|\n\r|\r\n)");
        auto userLines = regexSplit(newlineRegex, sourceString);
        for (auto& line : userLines) {
            auto resultVec = wrapString(line, targetLength, false);
            content.insert(content.end(), resultVec.begin(), resultVec.end());
        }

        return content;
    }

    // If hasNewline is false, we get here, and actually need to do the hard part: wrapping the string.
    // The print method deals with newlines so we don't have to. Each line is inserted as a new item in
    // the vector, so it should be fine.

    if (sourceString.length() <= targetLength) {
        // If the source string fits within the size requirement, we're just gonna say it's fine and shove
        // it in there
        content.push_back(sourceString);
    } else {
        // Otherwise... bleh
        // I'm not entirely sure how to implement mixed soft and hard wrap, soooo I'm not gonna do that. Lmao
        // Let's use hard wrap all the way
        std::string mutCopy = sourceString;
        do {
            if (mutCopy.at(targetLenIdx) == ' ') {
                // the last character is a space; we're good to break
                // We also
                content.push_back(mutCopy.substr(0, targetLenIdx + 1));
                mutCopy = mutCopy.substr(targetLenIdx + 1);
            } else if (mutCopy.at(targetLenIdx + 1) == ' ') {
                // if the next char is a space, we're good
                content.push_back(mutCopy.substr(0, targetLenIdx + 1));
                mutCopy = mutCopy.substr(
                        targetLenIdx + 2); // +2 trims away the space, which would be the first char on the next line
            } else if (mutCopy.at(targetLenIdx - 1) == ' ') {
                // The previous char was a space, but the current is not and the next one isn't.
                // That means we need to hard wrap at the last space, because we won't have enough
                // space to add the first letter and then a -
                content.push_back(mutCopy.substr(0, targetLenIdx - 1));
                mutCopy = mutCopy.substr(targetLength - 1);
            } else {
                content.push_back(mutCopy.substr(0, targetLenIdx) + "-");
                mutCopy = mutCopy.substr(targetLenIdx);
            }


        } while (mutCopy.length() > targetLength);
        content.push_back(mutCopy);
    }

    return content;
} // namespace StringUtils

} // namespace StringUtils
} // namespace printable
