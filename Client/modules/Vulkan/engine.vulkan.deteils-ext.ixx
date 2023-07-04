module;
#include<vector>
export module engine.vulkan.deteils:ext;

namespace engine::vulkan::deteils::ext
{
	export const std::vector< const char * > InstanceExts {
				"VK_KHR_surface"
				, "VK_KHR_win32_surface"
				, "VK_KHR_get_surface_capabilities2"
				, "VK_EXT_swapchain_colorspace"

			};

	export const std::vector< const char * > InstanceLayers
			{
				// "VK_LAYER_VALVE_steam_fossilize"
				// , "VK_LAYER_VALVE_steam_overlay"
				#ifdef _DEBUG
				"VK_LAYER_KHRONOS_validation"
				#endif
			};

	export const std::vector< const char * > DeviceExts
			{
				"VK_KHR_swapchain"
				, "VK_KHR_8bit_storage"
				, "VK_EXT_full_screen_exclusive"
			};
}
