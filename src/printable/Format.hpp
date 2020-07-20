#pragma once

#include <optional>
#include <string>
#include <vector>

namespace printable {

// TODO: decide on how I want to link the format to the table
// Options I can think of at this time:
// 1. Every row, cell, and column gets a Format
//      Pros: Unambiguous use
//      Cons: lots of instances, risks conflicts (row, column, and cell could code the same things),
// 2. A single format instance
//      Pros: Single instance to refer to, potentially cleaner interface
//      Cons: still risks conflicts
class Format {
public:
    enum class Align { LEFT, RIGHT };
};

} // namespace printable
