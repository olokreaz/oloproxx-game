module;
#include <optional>
#include <vulkan/vulkan.hpp>

export module types.vulkan;
import types;

export namespace type::vulkan
{
	struct QueueFamilyIndices
	{
		std::optional < uint32_t > graphicsFamily;
		std::optional < uint32_t > presentFamily;

		bool isComplete( ) { return graphicsFamily . has_value( ) && presentFamily . has_value( ); }
	};


	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR           capabilities;
		std::vector < vk::SurfaceFormatKHR > formats;
		std::vector < vk::PresentModeKHR >   presentModes;
	};

}
