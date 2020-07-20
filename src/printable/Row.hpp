#pragma once

#include <string>
#include <vector>

#include "printable/Cell.hpp"

namespace printable {

class Row {
private:
    // Contains the cells (horizontal lines)
    std::vector<std::string> cells;

public:
    Row(const std::vector<std::string>& cells) : cells(cells) {

    }

    //Row(std::vector<Cell>& cells) : cells(cells) {}

    size_t size() {
        return cells.size();
    }

    decltype(cells) getCells() {
        return cells;
    }
};

} // namespace printable
