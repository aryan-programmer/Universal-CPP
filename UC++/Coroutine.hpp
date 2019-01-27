#pragma once

#include "stdafx.h"
#include "Object.hpp"
#include "Generator.hpp"
#include <boost\thread.hpp>
#include <chrono>

#ifndef UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS
#define UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS 20
#endif // !UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS

namespace UC
{
	namespace Coro
	{
		struct Coroutine;
		class Scheduler;

	#pragma region YieldInstruction
		UCInterface( YieldInstruction , UC_WhereTypenameIs( "UC::Coro::YieldInstruction" ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );
		UC_IsAbstract;
		UC_HasNoMethods;
	public:
		virtual bool Finished( ) = 0;
		virtual void OnUpdate( Scheduler& , GCP<Coroutine> ) { }
		UCEndInterface( YieldInstruction );
	#pragma endregion


		using GeneratorForCoroutine = Generator<P_YieldInstruction , Scheduler& , GCP<Coroutine>>;


	#pragma region Coroutine
		UCInterface( Coroutine , UC_WhereTypenameIs( "UC::Coro::Coroutine" ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;
	public:
		GeneratorForCoroutine __fiber;
		bool __finished = false;
		bool __paused = false;
		P_YieldInstruction __instruction;
		bool operator ==( const Coroutine& r )const { return r.__instruction.RefEq( __instruction ); }
		bool operator !=( const Coroutine& r )const { return !r.__instruction.RefEq( __instruction ); }

		void Pause( ) { __paused = true; }
		void Resume( ) { __paused = false; }
	protected:
		Coroutine( GeneratorForCoroutine&& _fiber ) :__fiber( std::move( _fiber ) ) { }
		UCEndInterface( Coroutine );
	#pragma endregion


	#pragma region WaitForMillis
		UCInterface( WaitForMillis , UC_WhereTypenameIs( "UC::Coro::WaitForMillis" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;
	public:
		bool Finished( ) { return timeToWaitTo <= std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::steady_clock::now( ).time_since_epoch( ) ); }
	protected:
		std::chrono::milliseconds timeToWaitTo;
		WaitForMillis( int64_t timeToWait ) :timeToWaitTo(
			std::chrono::duration_cast< std::chrono::milliseconds >(
				std::chrono::steady_clock::now( ).time_since_epoch( )
				) + std::chrono::milliseconds( timeToWait ) )
		{ }
		UCEndInterface( WaitForMillis );
	#pragma endregion


	#pragma region ContinueWhen
		UCTemplateInterface( ContinueWhen , ( TFunc ) , UC_WhereTypenameIs( "UC::Coro::ContinueWhen" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;

		bool Finished( ) override
		{
			return std::invoke( func );
		};
	protected:
		TFunc func;

		ContinueWhen( TFunc fun ) :func( fun ) { }
		UCEndTemplateInterface( ContinueWhen , ( TFunc ) );
	#pragma endregion


	#pragma region BlockUntil
		UCTemplateInterface( BlockUntil , ( TFunc ) , UC_WhereTypenameIs( "UC::Coro::BlockUntil" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;

		bool Finished( ) override
		{
			return !std::invoke( func );
		};
	protected:
		TFunc func;

		BlockUntil( TFunc fun ) :func( fun ) { }
		UCEndTemplateInterface( BlockUntil , ( TFunc ) );
	#pragma endregion


	#pragma region TransferToAll
		UCInterface( TransferToAll , UC_WhereTypenameIs( "UC::Coro::TransferToAll" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;
	public:
		bool Finished( ) { return std::all_of( coros.begin( ) , coros.end( ) , [ ] ( const P_Coroutine& c ) { return c->__finished; } ); }
		template<typename... Ts> static pself MakeI( Ts&&... vs ) { return pself( new self { std::forward<Ts>( vs )... } ); }
	protected:
		NatVector<P_Coroutine> coros;
		TransferToAll( std::initializer_list<P_Coroutine> ilst ) :coros( ilst ) { }
		TransferToAll( NatVector<P_Coroutine>&& vec ) :coros( std::move( vec ) ) { }
		TransferToAll( const NatVector<P_Coroutine>& vec ) :coros( vec ) { }
		UCEndInterface( TransferToAll );
	#pragma endregion


	#pragma region WhenCalledStop
		UCInterface( WhenCalledStop , UC_WhereTypenameIs( "UC::Coro::WhenCalledStop" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_IsSingleton;
		UC_HasNoMethods;
	public:
		void OnUpdate( Scheduler& sched , P_Coroutine coro );
		bool Finished( ) { return false; }
	protected:
		WhenCalledStop( ) { }
		UCEndInterface( WhenCalledStop );
	#pragma endregion

		namespace CoroLiterals
		{
			static P_WaitForMillis operator""_wms( unsigned long long int param )
			{
				return WaitForMillis::Make( param );
			}

			static P_WaitForMillis operator""_ws( long double param )
			{
				return WaitForMillis::Make( static_cast< unsigned long long int >( param * 1000 ) );
			}

			static P_WaitForMillis operator""_ws( unsigned long long int param )
			{
				return WaitForMillis::Make( param * 1000 );
			}

			static P_WaitForMillis operator""_wait( long double param )
			{
				return WaitForMillis::Make( static_cast< unsigned long long int >( param * 1000 ) );
			}

			static P_WaitForMillis operator""_wait( unsigned long long int param )
			{
				return WaitForMillis::Make( param * 1000 );
			}
		}
	}
}

namespace UC
{
	namespace Coro
	{
		/// <summary>
		/// This class manages the creation, destruction, and updation of coroutines. It is required if you need fine grained control over when the coroutines are updated.
		/// <para>
		/// NOTE: You will need to call .Update to update the coroutines, if you want .Update to be called automatically use Threader instead.
		/// </para>
		/// </summary>
		class Scheduler :boost::noncopyable
		{
			std::list<P_Coroutine> lst;

			/// <summary>
			/// Executes coroutine until next yield. If coroutine has finished, flags
			/// it as finished and removes it from scheduler list.
			/// </summary>
			/// <param name="coroutine"></param>
			void UpdateCoroutine( P_Coroutine coroutine );
		public:

			/// <summary>
			/// Starts a coroutine, the coroutine does not run immediately but on the
			/// next call to Update. The execution of a coroutine can
			/// be paused at any point using the yield statement. The yield return value
			/// specifies when the coroutine is resumed.
			/// </summary>
			/// <param name="fiber">The generator which will be wrapped as a coroutine</param>
			/// <returns>The coroutine generated.</returns>
			P_Coroutine Start( GeneratorForCoroutine&& fiber );

			/// <summary>
			/// Returns true if this scheduler has any coroutines. You can use this to
			/// check if all coroutines have finished or been stopped.
			/// </summary>
			/// <returns>true if this scheduler has any coroutines.</returns>
			bool HasCoroutines( ) const noexcept;

			/// <summary>
			/// Runs all active coroutines until their next yield.
			/// </summary>
			void Update( );

			/// <summary>
			/// Removes the specified coroutine from the scheduled coroutine list.
			/// </summary>
			/// <param name="coroutine"></param>
			void Stop( P_Coroutine coroutine );
		};

		/// <summary>
		/// This class is a wrapper around Scheduler that automatically calls .Update on a separate thread each UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS.
		/// </summary>
		class Threader :boost::noncopyable
		{
			Scheduler sched;
			boost::mutex mtx;
			boost::thread thread;
		public:
			static Threader& I( );

			Threader( );

			/// <summary>
			/// Starts a coroutine, the coroutine does not run immediately but on the
			/// next automatic call to Update. The execution of a coroutine can
			/// be paused at any point using the yield statement. The yield return value
			/// specifies when the coroutine is resumed.
			/// </summary>
			/// <param name="fiber">The generator which will be wrapped as a coroutine</param>
			/// <returns>The coroutine generated.</returns>
			P_Coroutine Start( GeneratorForCoroutine&& fiber );

			/// <summary>
			/// Returns true if this scheduler has any coroutines. You can use this to
			/// check if all coroutines have finished or been stopped.
			/// </summary>
			/// <returns>true if this scheduler has any coroutines.</returns>
			bool HasCoroutines( ) const noexcept;

			/// <summary>
			/// Removes the specified coroutine from the scheduled coroutine list.
			/// </summary>
			/// <param name="coroutine"></param>
			void Stop( P_Coroutine coroutine );
		};

		/// <summary>
		/// Starts a coroutine, the coroutine does not run immediately but on the
		/// next automatic call to Update. The execution of a coroutine can
		/// be paused at any point using the yield statement. The yield return value
		/// specifies when the coroutine is resumed.
		/// </summary>
		/// <param name="fiber">The generator which will be wrapped as a coroutine</param>
		/// <returns>The coroutine generated.</returns>
		forceinline P_Coroutine Start( GeneratorForCoroutine&& fiber ) { return Threader::I( ).Start( std::move( fiber ) ); }

		/// <summary>
		/// Removes the specified coroutine from the scheduled coroutine list.
		/// </summary>
		/// <param name="coroutine"></param>
		forceinline void Stop( P_Coroutine coroutine ) { Threader::I( ).Stop( coroutine ); }

		static P_WaitForMillis WaitForSec( long double param )
		{
			return WaitForMillis::Make( static_cast< unsigned long long int >( param * 1000 ) );
		}

		template<typename T> static P_ContinueWhen<T> WaitUntil( T&& f ) { return ContinueWhen<T>::Make( std::forward<T>( f ) ); }

		template<typename T> static P_BlockUntil<T> WaitWhile( T&& f ) { return BlockUntil<T>::Make( std::forward<T>( f ) ); }

		static P_WhenCalledStop Stop( ) { return WhenCalledStop::Make( ); }

		template<typename... Ts>
		static P_TransferToAll ExecTransferToAll( Scheduler& sched , Ts&&... vs ) { return TransferToAll::MakeI( sched.Start( std::move( vs ) )... ); }
	}
}

#define UCCoro(name, params, ...) UCBDGen(::UC::Coro::P_YieldInstruction, name, params, ((::UC::Coro::Scheduler&)(UCScheduler) , (::UC::Coro::P_Coroutine)(UCCoroutineID)), __VA_ARGS__) using namespace ::UC::Coro::CoroLiterals;
#define UCCoroLambda(params, ...) UCBDGenLambda(::UC::Coro::P_YieldInstruction, params, ((::UC::Coro::Scheduler&)(UCScheduler) , (::UC::Coro::P_Coroutine)(UCCoroutineID)), __VA_ARGS__) using namespace ::UC::Coro::CoroLiterals;
#define UCCoroBeg(params, ...) UCBDGenBeg(::UC::Coro::P_YieldInstruction, params, ((::UC::Coro::Scheduler&)(UCScheduler) , (::UC::Coro::P_Coroutine)(UCCoroutineID)), __VA_ARGS__) using namespace ::UC::Coro::CoroLiterals;
#define UCCoroEnd UCBDGenEnd

#define UCAwait(...) UCYield( ::UC::Coro::ExecTransferToAll( UCScheduler, __VA_ARGS__ ) )
