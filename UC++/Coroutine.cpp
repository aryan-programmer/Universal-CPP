#include "stdafx.h"
#include "Coroutine.hpp"

namespace UC
{
	namespace Coro
	{
		void WhenCalledStop::OnUpdate( P_Coroutine coro ) { Stop( coro ); };

		std::list<P_Coroutine>& getLst( )
		{
			static std::list<P_Coroutine> lst;
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
					boost::this_thread::no_interruption_point::sleep_for( boost::chrono::milliseconds( UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS ) );
					if ( getLst( ).size( ) == 0 )continue;
					boost::lock_guard<boost::recursive_mutex> __lock( getMtx( ) );
					BOOST_SCOPE_EXIT( void )
					{
						try
						{
							auto& lst = getLst( );
							lst.erase( std::remove_if( lst.begin( ) , lst.end( ) , [ ] ( P_Coroutine& coro )
							{
								if ( !coro )return true;
								if ( coro->__finished ) coro->__onStop->Eval( );
								return coro->__finished;
							} ) , lst.end( ) );
						}
						catch ( ... ) { }
					};
					for ( auto& coroutine : getLst( ) )
					{
						bool update = false;
						if ( !coroutine || coroutine->__paused || coroutine->__finished )continue;
						if ( coroutine->__instruction )
						{
							if ( coroutine->__instruction->Finished( ) )
							{
								coroutine->__instruction = nullptr;
								update = true;
							}
							else coroutine->__instruction->OnUpdate( coroutine );
						}
						//initial update
						else update = true;
						if ( update )
						{
							if ( coroutine->__finished )continue;
							try
							{
								auto& fiber = coroutine->__fiber;
								if ( fiber( ) )
								{
									auto yieldCommand = fiber.Get( );

									if ( !yieldCommand ) continue;

									coroutine->__instruction = yieldCommand;
								}
								else
								{
									coroutine->__finished = true;
									continue;
								}
							}
							catch ( ... )
							{
								coroutine->__finished = true;
								throw;
							}
						}
					}
				}
				catch ( const UC::Exception& ex )
				{
					std::cerr << "Exception of type \"" << typeid( ex ).name( ) << "\" in coroutine on separate thread with message: " << std::endl << ex.what( ) << std::endl << "at stack position:" << std::endl << ex.GetStackTrace( );
				}
				catch ( const std::exception& ex )
				{
					std::cerr << "Exception of type \"" << typeid( ex ).name( ) << "\" in coroutine on separate thread with message: \"" << ex.what( ) << "\"." << std::endl;
				}
				catch ( ... )
				{
					std::cerr << "Exception of unknown type in coroutine on separate thread." << std::endl;
				}
			}
		}
		P_Coroutine Start( GeneratorForCoroutine && fiber )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( getMtx( ) );
			if ( fiber == nullptr ) return nullptr;
			auto coroutine = Coroutine::Make( std::move( fiber ) );
			getLst( ).push_back( coroutine );
			return coroutine;
		}
		void Stop( P_Coroutine coroutine ) { if ( coroutine ) coroutine->__finished = true; }
	}
}
