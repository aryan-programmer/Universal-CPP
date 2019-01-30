#include "stdafx.h"
#include "Coroutine.hpp"

namespace UC
{
	namespace Coro
	{
		void WhenCalledStop::OnUpdate( P_Coroutine coro ) { Stop( coro ); };

		static std::list<P_Coroutine> lst;
		static boost::recursive_mutex mtx;
		static boost::thread thread( [ ] ( )
		{
			while ( true )
			{
				boost::this_thread::sleep_for( boost::chrono::milliseconds( UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS ) );
				if ( lst.size( ) == 0 )continue;
				boost::lock_guard<boost::recursive_mutex> __lock( mtx );
				try
				{
					auto iter = lst.begin( );
					while ( iter != lst.end() )
					{
						auto coroutine = *iter;
						bool update = false;
						if ( !coroutine )
						{
							lst.erase( iter++ );
							continue;
						}
						if ( coroutine->__paused )continue;
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
							auto& fiber = coroutine->__fiber;
							if ( fiber( coroutine ) )
							{
								auto yieldCommand = fiber.Get( );

								if ( !yieldCommand ) continue;

								coroutine->__instruction = yieldCommand;
							}
							else
							{
								coroutine->__finished = true;
								lst.erase( iter++ );
								continue;
							}
						}
						++iter;
					}
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
		} );

		P_Coroutine Start( GeneratorForCoroutine && fiber )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			// if function has exited or is invalid, fiber will be null and we no-op
			if ( fiber == nullptr ) return nullptr;
			// create coroutine node and run until we reach first yield
			auto coroutine = Coroutine::Make( std::move( fiber ) );
			lst.push_back( coroutine );
			return coroutine;
		}
		void Stop( P_Coroutine coroutine )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			lst.remove( coroutine );
		}
	}
}
