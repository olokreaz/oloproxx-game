#include <app/SyncBinaryWatcher.hpp>

#include <magic_enum_all.hpp>

const fs::path& CSyncBinaryWatcher::context( ) const { return dest_context; }

void CSyncBinaryWatcher::update( cppfs::FileHandle &fh )
{
	auto oDest = this -> try_take_specific_path( fh . path( ) );
	if ( oDest ) dest_context = m_pConfig -> destination / *oDest;
	else dest_context         = m_pConfig -> destination / relative( fh . path( ), m_pConfig -> source );
	dest_context = m_pConfig -> destination / ( oDest ? *oDest : relative( fh . path( ), m_pConfig -> source ) );
}

void CSyncBinaryWatcher::onFileEvent( cppfs::FileHandle &fh, cppfs::FileEvent event )
{
	if ( !this -> valide_on_ignore( fh . path( ) ) ) return;
	m_logger -> info( "{} {}", magic_enum::enum_name( event ), fh . path( ) );
	update( fh );

	switch ( event )
	{
		case cppfs::FileCreated: this -> onFileCreated( fh );
			break;
		case cppfs::FileRemoved: this -> onFileRemoved( fh );
			break;
		case cppfs::FileModified: this -> onFileModified( fh );
			break;
		case cppfs::FileAttrChanged: this -> onFileAttrChanged( fh );
			break;
		default: spdlog::warn( "can't find Event {{ {} }}", fmt::underlying( event ) );
	}
	dest_context . clear( );
}

void CSyncBinaryWatcher::onFileCreated( cppfs::FileHandle &fh )
{
	m_logger -> debug( "onFileCreated"" {}", fh . path( ) );
	const auto dest = context( );
	utils::ufs::copy_to( fh . path( ), dest );
	m_logger -> info( "File Coppied {}", dest );
}

void CSyncBinaryWatcher::onFileRemoved( cppfs::FileHandle &fh )
{
	m_logger -> debug( "onFileRemoved: {}", fh . path( ) );
	const auto dest = context( );
	utils::ufs::remove( dest );
	m_logger -> info( "Removed {}", dest );
}

void CSyncBinaryWatcher::onFileModified( cppfs::FileHandle &fh )
{
	m_logger -> debug( "onFileModified: {}", fh . path( ) );
	const auto dest  = context( );
	auto       hDest = cppfs::fs::open( dest . string( ) );
	utils::ufs::copy_to( fh . path( ), dest );
	m_logger -> info( "File Coppied {}", dest );
}

void CSyncBinaryWatcher::onFileAttrChanged( cppfs::FileHandle &fh )
{
	m_logger -> debug( "onFileAttrChanged: {}", fh . path( ) );
	const auto dest = context( );
	utils::ufs::copy_to( fh . path( ), dest );
	m_logger -> info( "File Coppied {}", dest );
}
