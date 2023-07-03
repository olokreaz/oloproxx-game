module;
#include <optional>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.h>
export module types;

export typedef unsigned char    uint8;
export typedef signed char      int8;
export typedef __int16          int16;
export typedef unsigned __int16 uint16;
export typedef __int32          int32;
export typedef unsigned __int32 uint32;
export typedef __int64          int64;
export typedef unsigned __int64 uint64;
export typedef __int64          intp;
export typedef unsigned __int64 uintp;

export typedef struct QueueFamilyIndices {
	std::optional< uint32_t > graphicsFamily;
	std::optional< uint32_t > presentFamily;

	bool isComplete( ) { return graphicsFamily . has_value( ) && presentFamily . has_value( ); }
}            QueueFamilyIndices;

export typedef struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR          capabilities;
	std::vector< VkSurfaceFormatKHR > formats;
	std::vector< VkPresentModeKHR >   presentModes;
}                                         SwapChainSupportDetails;
