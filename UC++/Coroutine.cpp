#include "stdafx.h"
#include "Coroutine.hpp"

namespace UC
{
	namespace Coro
	{
		void WhenCalledStop::OnUpdate( Scheduler& sched , P_Coroutine coro ) { sched.Stop( coro ); };

		P_WaitForMillis CoroYield( long double param ) { return WaitForMillis::Make( static_cast< unsigned long long int >( param * 1000 ) ); };

		P_Coroutine Scheduler::Start( GeneratorForCoroutine&& fiber )
		{
			// if function has exited or is invalid, fiber will be null and we no-op
			if ( fiber == nullptr ) return nullptr;
			// create coroutine node and run until we reach first yield
			auto coroutine = Coroutine::Make( std::move( fiber ) );
			lst.push_back( coroutine );
			return coroutine;
		}

		bool Scheduler::HasCoroutines( ) const noexcept { return lst.size( ) != 0; }

		void Scheduler::Update( )
		{
			if ( !HasCoroutines( ) )return;
			for ( auto& coroutine : lst )
			{
				if ( coroutine->__paused )continue;
				if ( coroutine->__instruction )
				{
					if ( coroutine->__instruction->Finished( ) )
					{
						coroutine->__instruction = nullptr;
						UpdateCoroutine( coroutine );
					}
					else coroutine->__instruction->OnUpdate( *this , coroutine );
				}
				else
					// initial update
					UpdateCoroutine( coroutine );
			}
		}

		void Scheduler::UpdateCoroutine( P_Coroutine coroutine )
		{
			auto& fiber = coroutine->__fiber;
			if ( fiber( *this , coroutine ) )
			{
				auto yieldCommand = fiber.Get( );

				UCAsNotNull( yieldCommand );

				coroutine->__instruction = yieldCommand;
			}
			else
			{
				coroutine->__finished = true;
				Stop( coroutine );
			}
		}
		void Scheduler::Stop( P_Coroutine coroutine ) { lst.remove( coroutine ); }

		Threader::Threader( ) : thread(
			[ this ] ( )
		{
			while ( true )
			{
				boost::this_thread::sleep_for( boost::chrono::milliseconds( UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS ) );
				if ( !sched.HasCoroutines( ) )continue;
				boost::lock_guard<boost::mutex> __lock( mtx );
				try { sched.Update( ); }
				catch ( const std::exception& ex )
				{
					std::cerr << "Exception of type \"" << typeid( ex ).name( ) << "\" in coroutine on separate thread with message: \"" << ex.what( ) << "\"." << std::endl;
				}
				catch ( ... )
				{
					std::cerr << "Exception of unknown type in coroutine on separate thread." << std::endl;
				}
			}
		} )
		{ }
		P_Coroutine Threader::Start( GeneratorForCoroutine&& fiber )
		{
			boost::lock_guard<boost::mutex> __lock( mtx );
			return sched.Start( std::move( fiber ) );
		}
		bool Threader::HasCoroutines( ) const noexcept { return sched.HasCoroutines( ); }
		void Threader::Stop( P_Coroutine coroutine )
		{
			boost::lock_guard<boost::mutex> __lock( mtx );
			sched.Stop( coroutine );
		}

		Threader& Threader::I( )
		{
			static Threader handler;
			return handler;
		}
	}
}
