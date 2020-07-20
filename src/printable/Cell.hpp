#pragma once

#include <vector>
#include <string>
#include <optional>

#include "printable/Format.hpp"

namespace printable {

/**
 * Currently unused; may be removed if I don't see a use for it.
 * Depends heavily on how I deal with the format issue;
 * see Format.hpp for that whole clusterfuck
 */
class Cell {
private:
    std::string content;
    std::vector<std::string> wrapped;
public:
    Cell(const std::string& content) : content(content) {}
    
    const std::string& getContent() {
        return content;
    }

    const decltype(wrapped) getWrappedContent() {
        return wrapped;
    }

    void wrap(int maxWidth) {

    }
};

}
