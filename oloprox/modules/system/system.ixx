module;

#define NODISCARD [[nodiscard]]
#define EXPORT export NODISCARD

export module system;

import app.config;
import types;

using namespace fmt::literals;
using namespace std::literals;

namespace fs = std::filesystem;

namespace sys {}
