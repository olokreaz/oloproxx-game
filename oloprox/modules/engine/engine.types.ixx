module;
#include <oloproxx/config>

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.hpp>

export module engine.types;
import types;

namespace engine::type
{
	export class CMonitor
	{
		std::string_view m_name;
		glm::i32vec2     m_position;
		glm::i32vec2     m_physical_size;
		glm::vec2        m_content_scale;
		glm::i32vec2     m_size;

		glm::i32vec3 m_rgbBits;

		int32 refresh_rate;

		GLFWmonitor *m_pMonitor;

	public:
		CMonitor( ) = default;

		CMonitor( GLFWmonitor *monitor )
		{
			m_pMonitor = monitor;
			m_name     = glfwGetMonitorName( monitor );
			glfwGetMonitorPos( monitor, &m_position . x, &m_position . y );
			glfwGetMonitorContentScale( monitor, &m_content_scale . x, &m_content_scale . y );
			glfwGetMonitorPhysicalSize( monitor, &m_physical_size . x, &m_physical_size . y );
			{
				const GLFWvidmode *mode = glfwGetVideoMode( monitor );
				m_size . x              = mode -> width;
				m_size . y              = mode -> height;
				refresh_rate            = mode -> refreshRate;

				m_rgbBits . r = mode -> redBits;
				m_rgbBits . g = mode -> greenBits;
				m_rgbBits . b = mode -> blueBits;
			}
		}

		[[nodiscard]] const std::string_view& getName( ) const { return m_name; }
		[[nodiscard]] const glm::i32vec2&     getPosition( ) const { return m_position; }
		[[nodiscard]] const glm::i32vec2&     getPhysical_size( ) const { return m_physical_size; }
		[[nodiscard]] const glm::vec2&        getContent_scale( ) const { return m_content_scale; }
		[[nodiscard]] const glm::i32vec2&     getSize( ) const { return m_size; }
		[[nodiscard]] const glm::i32vec3&     getRgbBits( ) const { return m_rgbBits; }
		[[nodiscard]] int32                   getRefresh_rate( ) const { return refresh_rate; }

		__declspec(property(get = getName)) std::string_view      Name;
		__declspec(property(get = getPosition)) glm::i32vec2      Position;
		__declspec(property(get = getPhysical_size)) glm::i32vec2 PhysicalSize;
		__declspec(property(get = getContent_scale)) glm::vec2    ContentScale;
		__declspec(property(get = getSize)) glm::i32vec2          Size;
		__declspec(property(get = getRgbBits)) glm::i32vec3       RgbBits;
		__declspec(property(get = getRefresh_rate)) int32         RefreshRate;
	};
}
