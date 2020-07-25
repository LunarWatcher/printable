#pragma once

#include <algorithm>
#include <iomanip>
#include <string>
#include <vector>

#include "printable/Cell.hpp"
#include "printable/Format.hpp"
#include "printable/Row.hpp"
#include "printable/utils/StringUtils.hpp"
#include "printable/utils/TermUtils.hpp"

namespace printable {

class Table {
public:
    // Contains the rows (vertical lines)
    std::vector<Row> rows;
    // Table width; stored as a separate variable
    // because it's equivalent to the longest inner
    // vector in $rows.
    // If rows is:
    // [
    //   [<len=3>],
    //   [<len=5>],
    //   [<len=1>]
    // ]
    // width is 5, because that's the widest point.
    // We can determine the length by using rows.size(),
    // so that doesn't need to be stored separately.
    size_t width = 0;

    Format format;

public:
    Table() {}

    Format& getFormat() {
        return format;
    }

    void addRow(const std::vector<std::string>& row) {
        // This calls the Row constructor
        Row rInstance(row);
        if (rInstance.size() > width) {
            this->width = rInstance.size();
        }
        this->rows.push_back(rInstance);
    }

    size_t getWidth() {
        return width;
    }

    size_t getHeight() {
        return rows.size();
    }

    std::string print() {
        if (rows.size() == 0) {
            // Minor optimization; no rows, no table.
            return "";
        }
        // This is gonna get fun xd
        // Okay, so first, we need to determine the maximum size the table can have.
        // TermUtils::getWidth() returns 0 if it can't determine, in which case,
        // we just assume we're good and rather deal with issues where getWidth()
        // fails when we cross that bridge.
        // This shouldn't cause issues with piping, but it's assuming the output is as
        // wide as the terminal.
        // Not sure if this is a good idea yet, but we'll cross that bridge when we get
        // there.
        auto maxWidth = TermUtils::getWidth();
        // This is the max column width.
        // This should prevent terminal-related issues, but it might still cause issues
        // in cases where the output is indented.
        auto maxColumnWidth = maxWidth / width;

        std::stringstream output;
        output << std::setfill(' ');

        std::vector<int> colWidths;
        std::vector<std::vector<std::vector<std::string>>> rowLineOutputMatrix;

        for (auto& row : this->rows) {
            // TODO: top border

            rowLineOutputMatrix.push_back({});
            auto& lineOutputMatrix = rowLineOutputMatrix.back();

            auto cells = row.getCells();

            size_t maxHeight;
            // We iterate by the index because we need the index anyway
            // Specifically, it lets us later calculate the matrix positions.
            for (auto i = 0; i < cells.size(); i++) {
                auto cell = cells.at(i);
                auto rawCellLength = cell.length();

                int lostWidth = 4; // Cell border + padding. TODO: replace with a calculation
                std::vector<std::string> lines;
                if (rawCellLength < maxColumnWidth - lostWidth && cell.find("\n") == std::string::npos) {
                    lines.push_back(cell);
                } else {
                    lines = StringUtils::wrapString(
                            cell, maxColumnWidth - lostWidth); // - 4: accounting for padding and borders
                }
                if (lines.size() > lineOutputMatrix.size()) {
                    size_t oldSize = lineOutputMatrix.size();
                    lineOutputMatrix.resize(lines.size());

                    for (auto rowIdx = oldSize; rowIdx < lineOutputMatrix.size(); rowIdx++) {
                        lineOutputMatrix[rowIdx].resize(i);
                    }
                }

                for (auto lineIdx = 0; lineIdx < lineOutputMatrix.size(); lineIdx++) {

                    if (lineIdx < lines.size()) {
                        auto& line = lines.at(lineIdx);
                        auto lineLen = line.length();
                        if (i >= colWidths.size()) {
                            colWidths.push_back(lineLen);
                        } else {
                            if (colWidths.at(i) < lineLen) {
                                colWidths[i] = lineLen;
                            }
                        }

                        // If we have a line for the position, add it
                        lineOutputMatrix[lineIdx].push_back(line);
                    } else {
                        // Otherwise, add an empty item so we know that line is empty
                        lineOutputMatrix[lineIdx].resize(i + 1);
                    }
                }
            }
        }

        // The output system got messy fast... Here's a crash course
        // A vector contains the rows (vertical units)
        for (auto& row : rowLineOutputMatrix) {
            // Each row contains lines (vertical subunit)
            for (auto& line : row) {
                output << "|";
                // Each line contains columns (horizontal units)
                for (auto colIdx = 0; colIdx < line.size(); colIdx++) {
                    auto& column = line.at(colIdx);
                    auto targetWidth = colWidths.at(colIdx);
                    std::cout << targetWidth << std::endl;
                    output << " " // TODO: replace with format (padding and borders)
                           << std::left // TODO: replace with format (align)
                           // targetWidth only factors in the length of the content, so it ignores padding
                           << std::setw(targetWidth) << column << " |";
                }
                output << "\n";
            }
        }

        return output.str();
    }
};

inline std::ostream& operator<<(std::ostream& stream, Table& table) {
    stream << table.print();
    return stream;
}

} // namespace printable
