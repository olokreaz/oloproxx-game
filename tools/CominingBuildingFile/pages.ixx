module;
#include <ftxui/component/animation.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/direction.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/linear_gradient.hpp>
#include <ftxui/screen/box.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>

export module pages;

using namespace ftxui;

export namespace ux = ftxui;
export namespace ui
{
	Component window( std::wstring_view name, Component cmp )
	{
		return ux::Renderer(
				cmp,
				[&name,&cmp]
				{
					return ux::window(
						text( name . data( ) ) | bold | ux::color( Color::HotPink ) | center,
						cmp -> Render( ) ) | color( Color::HotPink
							);
				}
				);
	}
}
