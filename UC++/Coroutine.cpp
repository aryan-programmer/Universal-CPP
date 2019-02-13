#include "stdafx.h"
#include "Coroutine.hpp"

namespace UC
{
	namespace Coro
	{
		std::list<P<Coroutine>>& getLst( ) noexcept
		{
			static std::list<P<Coroutine>> lst;
			return lst;
		}

		void Update( );

		boost::recursive_mutex& getMtx( )
		{
			static boost::recursive_mutex mtx;
			static boost::thread thread( &Update );
			return mtx;
		}

		void Update( )
		{
			while ( true )
			{
				try
				{
					boost::this_thread::sleep_for( boost::chrono::milliseconds( UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS ) );
					if ( getLst( ).size( ) == 0 )continue;
					boost::lock_guard<boost::recursive_mutex> __lock( getMtx( ) );
					BOOST_SCOPE_EXIT( void )
					{
						try
						{
							auto& lst = getLst( );
							lst.erase( std::remove_if( lst.begin( ) , lst.end( ) , [ ] ( P<Coroutine>& coro )
							{
								if ( !coro )return true;
								if ( coro->FinishedOrFailed( ) ) coro->__onStop->EvalAll( );
								return coro->FinishedOrFailed( );
							} ) , lst.end( ) );
						}
						catch ( ... ) { }
					};
					for ( auto& coroutine : getLst( ) )
					{
						bool update = false;
						if ( !coroutine || !coroutine->Running( ) )continue;
						try
						{
							if ( coroutine->__instruction )
							{
								if ( coroutine->__instruction->Finished( coroutine ) )
								{
									coroutine->__instruction = nullptr;
									update = true;
								}
							}
							//initial update
							else update = true;
							if ( update )
							{
								if ( !coroutine->Running( ) )continue;
								auto& fiber = coroutine->__fiber;
								if ( fiber( ) )
									coroutine->__instruction = fiber.Get( ) | WaitTillUpdate::Make( );
								else
								{
									coroutine->__Finish( );
									continue;
								}
							}
						}
						catch ( ... ) { coroutine->__Fail( ::std::current_exception( ) ); }
					}
				}
				catch ( const UC::Exception& ex )
				{
					std::cerr << "Exception of type \"" << boost::typeindex::type_id_runtime( ex ).pretty_name( ) << "\" in coroutine on separate thread with message: " << std::endl << ex.what( ) << std::endl << "at stack position:" << std::endl << ex.GetStackTrace( );
				}
				catch ( const std::exception& ex )
				{
					std::cerr << "Exception of type \"" << boost::typeindex::type_id_runtime( ex ).pretty_name( ) << "\" in coroutine on separate thread with message: \"" << ex.what( ) << "\"." << std::endl;
				}
				catch ( ... )
				{
					std::cerr << "Exception of unknown type in coroutine on separate thread." << std::endl;
				}
			}
		}
		P<Coroutine> Start( GeneratorForCoroutine && fiber )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( getMtx( ) );
			if ( fiber == nullptr ) return nullptr;
			auto coroutine = Coroutine::Make( std::move( fiber ) );
			getLst( ).push_back( coroutine );
			return coroutine;
		}
		void Stop( P<Coroutine> coroutine ) { if ( coroutine ) coroutine->__Finish( ); }
	}
}
