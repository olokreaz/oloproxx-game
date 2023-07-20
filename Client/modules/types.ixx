module;
#include <windows.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR
#include <vulkan/vulkan.hpp>
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

namespace types {
	export enum class EWindowFlag : byte {
		None       = 0 ,
		Fullscreen = 1 << 0 ,
		Borderless = 1 << 1 ,
		Decorated  = 1 << 4 ,
	};

	export using InitRender = struct InitRender {
		glm::ivec2   size;
		EWindowFlag flags;
	};

	export using RenderConfig = struct RenderConfig {
		GLFWwindow *pWindow;
		HWND        hWindow;

		vk::Instance       Instance;
		vk::PhysicalDevice PhysicDevice;
		vk::Device         Device;
		vk::SurfaceKHR     surface;

		glm::ivec2 size;
	};
}
