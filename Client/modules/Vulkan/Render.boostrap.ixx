module;
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <functional>
#include <unordered_map>
#include<vector>
#include<GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include<fmt/format.h>
#include <glm/glm.hpp>
#include<vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

export module vulkan.bootstrap;
import types;

namespace Vulkan::bootstrap
{
	export [[nodiscard]] constexpr std::vector< const char * > getConstExtensions( )
	{
		return {
					VK_KHR_SURFACE_EXTENSION_NAME
					, VK_KHR_WIN32_SURFACE_EXTENSION_NAME
					, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME
					, VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME

				};;
	}

	export [[nodiscard]] constexpr std::vector< const char * > getConstLayers( )
	{
		return {
					// "VK_LAYER_VALVE_steam_fossilize"
					// , "VK_LAYER_VALVE_steam_overlay"
					#ifdef _DEBUG
					"VK_LAYER_KHRONOS_validation"
					#endif
				};
	}

	export [[nodiscard]] constexpr std::vector< const char * > getConstDeviceExtensions( )
	{
		return {
					VK_KHR_SWAPCHAIN_EXTENSION_NAME
					, VK_KHR_8BIT_STORAGE_EXTENSION_NAME
					, VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME
				};
	}

	

}
