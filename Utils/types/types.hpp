#include <string>
#include <vcruntime_typeinfo.h>
#include <steam/steamtypes.h>
using namespace std;

namespace types
{
	template< class TyPkg > class IPackage {
		const uint32  m_TypeID = { typeid( TyPkg ).hash_code( ) };
		const uint32  m_TypeSize { sizeof TyPkg };
		mutable TyPkg m_Package;

	public:
		IPackage( ) = default;
		IPackage( TyPkg &t ) : m_Package { } {}

		TyPkg& operator->( ) { return m_Package; }

		~IPackage( ) = default;
	};
}
