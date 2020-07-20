# printable

C++17 header-only table printing library with terminal IO and environment utils.

## Features

Note that until everything has been added, not all of these will actually be done. These will still be in place soon:

* [x] Word wrap
* [x] Respecting terminal width in known systems
* [x] Enabling ANSI on Windows, where supported
* [x] General ANSI detection elsewhere (note: makes an extremely basic assumption that may produce ANSI in terminals that doesn't support it)
* [ ] Stylizable tables
* [ ] Stylize tables with ANSI (where supported)
* [ ] Basic UTF-8 support
* ... and some more stuff I'll probably decide to randomly add in the dev process

## Installing

Printable, being a header-only library without any configuration or anything requiring the build system to do anything, is completely build system agnostic.

`git clone https://github.com/LunarWatcher/printable`, make sure your build system or command line arguments adds printable/src as an include directory, make sure you're using C++17, and you're good to go.

Note that due to the nature of the library, I will not be adding support for C++14 or older. One of the files also relies on `std::filesystem`, and additionally (note: at the time of writing, this hasn't been implemented) uses [`__has_include`](https://en.cppreference.com/w/cpp/preprocessor/include) to add support for `std::experimental::filesystem`.

To top that off, `<optional>` is used in a few places, and the only way around that is implementing a new `<optional>`, or using a library, which I don't want to do. If you need C++11 or C++14 support, [tabulate](https://github.com/p-ranav/tabulate) might be what you're looking for.

This should still be compatible with C++20, and if it isn't, I'll be adding support for that as soon as I get my hands on a C++20 compiler and stdlib that implements a decent amount of C++20's features. That's probably gonna take a while.

## Usage

You only need to include one file to use the table aspects of the library:
```cpp
#include "printable/Table.hpp"
```

This will also include the utilities, but the environment and IO utilities can also be included separately:
```cpp
#include "printable/utils/EnvVariableHelper.hpp"
#include "printable/utils/TermUtils.hpp"
```

Documentation will be made available in separate files in the docs/ directory when there actually is stuff to document.

## License

MIT. See the LICENSE file.
