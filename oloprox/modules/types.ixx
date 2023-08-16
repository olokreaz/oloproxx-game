module;
#include <windows.h>
#include <GLFW/glfw3.h>

#define VULKAN_HPP_NO_SPACESHIP_OPERATOR
#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

export module types;

export using uint8 = unsigned char;
export using int8 = signed char;
export using int16 = __int16;
export using uint16 = unsigned __int16;
export using int32 = __int32;
export using uint32 = unsigned __int32;
export using int64 = __int64;
export using uint64 = unsigned __int64;
export using intp = __int64;
export using uintp = unsigned __int64;
export using float32 = float;
export using float64 = double;

export using byte = uint8;
