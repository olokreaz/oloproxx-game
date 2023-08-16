module;

export module core.handler.url;
import core.Itype;

namespace core::handlers {
	export class CHandlerUrl final : interface::IHandler {
		friend interface::HandlersRegistry;

	public:
		void handle( ) override {}
	};
}
