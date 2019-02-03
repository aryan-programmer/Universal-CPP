#pragma once
#ifndef __UC__COROUTINE_HPP__
#define __UC__COROUTINE_HPP__
#include "stdafx.h"
#include "Object.hpp"
#include "Generator.hpp"

#ifndef UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS
#define UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS 1
#endif // !UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS

namespace UC
{
	namespace Coro
	{
		struct Coroutine;

	#pragma region YieldInstruction
		UCInterface( YieldInstruction , UC_WhereTypenameIs( "UC::Coro::YieldInstruction" ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );
		UC_IsAbstract;
		UC_HasNoMethods;
	public:
		virtual bool Finished( ) = 0;
		virtual void OnUpdate( GCP<Coroutine> ) { }
		UCEndInterface( YieldInstruction );
	#pragma endregion


		using GeneratorForCoroutine = Generator<P_YieldInstruction>;


	#pragma region Coroutine
		UCInterface( Coroutine , UC_WhereTypenameIs( "UC::Coro::Coroutine" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;
	public:
		GeneratorForCoroutine __fiber;
		bool __finished = false;
		bool __paused = false;
		P_YieldInstruction __instruction;
		P_Event<void> __onStop;
		bool operator ==( const Coroutine& r )const { return r.__instruction.RefEq( __instruction ); }
		bool operator !=( const Coroutine& r )const { return !r.__instruction.RefEq( __instruction ); }

		void Pause( ) { __paused = true; }
		void Resume( ) { __paused = false; }
		bool IsPaused( ) const { return __paused; }
		bool Finished( ) { return __finished; }

		/// <summary>
		/// Adds a callback for when the Future fails.
		/// Will call immediately if the Future has already failed.
		/// </summary>
		void OnStopF( P_Functor<void> inCallback )
		{
			if ( !inCallback ) return;
			__onStop->AddF( inCallback );
		}

		/// <summary>
		/// Adds a callback for when the Future fails.
		/// Will call immediately if the Future has already failed.
		/// </summary>
		template<typename TF> void OnStop( TF&& inCallback )
		{
			__onStop->Add( std::forward<TF>( inCallback ) );
		}
	protected:
		Coroutine( GeneratorForCoroutine&& _fiber ) :__fiber( std::move( _fiber ) ) , __onStop( Event<void>::Make( ) ) { }
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


	#pragma region WaitForAll
		UCInterface( WaitForAll , UC_WhereTypenameIs( "UC::Coro::WaitForAll" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;
	public:
		bool Finished( )
		{
			for ( auto& i : coros ) if ( !i->Finished( ) )return false;
			return true;
		}
		void OnUpdate( P_Coroutine cor )
		{
			for ( auto& i : coros )i->OnUpdate( cor );
		}
		template<typename... Ts> static pself MakeI( Ts&&... vs ) { return pself( new self { std::forward<Ts>( vs )... } ); }
	protected:
		NatVector<P_YieldInstruction> coros;
		WaitForAll( std::initializer_list<P_YieldInstruction> ilst ) :coros( ilst ) { }
		WaitForAll( NatVector<P_YieldInstruction>&& vec ) :coros( std::move( vec ) ) { }
		WaitForAll( const NatVector<P_YieldInstruction>& vec ) :coros( vec ) { }
		UCEndInterface( WaitForAll );
	#pragma endregion


	#pragma region WhenCalledStop
		UCInterface( WhenCalledStop , UC_WhereTypenameIs( "UC::Coro::WhenCalledStop" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_IsSingleton;
		UC_HasNoMethods;
	public:
		void OnUpdate( P_Coroutine coro );
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
		/// Starts a coroutine, the coroutine does not run immediately but on the
		/// next automatic call to Update. The execution of a coroutine can
		/// be paused at any point using the yield statement. The yield return value
		/// specifies when the coroutine is resumed.
		/// </summary>
		/// <param name="fiber">The generator which will be wrapped as a coroutine</param>
		/// <returns>The coroutine generated.</returns>
		P_Coroutine Start( GeneratorForCoroutine&& fiber );

		/// <summary>
		/// Removes the specified coroutine from the scheduled coroutine list.
		/// </summary>
		/// <param name="coroutine"></param>
		void Stop( P_Coroutine coroutine );

		static P_WaitForMillis WaitForSec( long double param )
		{
			return WaitForMillis::Make( static_cast< unsigned long long int >( param * 1000 ) );
		}

		template<typename T> static P_ContinueWhen<T> WaitUntil( T&& f ) { return ContinueWhen<T>::Make( std::forward<T>( f ) ); }

		template<typename T> static P_BlockUntil<T> WaitWhile( T&& f ) { return BlockUntil<T>::Make( std::forward<T>( f ) ); }

		static P_WhenCalledStop Stop( ) { return WhenCalledStop::Make( ); }

		template<typename T>
		static auto ExecWaitForAll( T&& v )->decltype( std::forward<T>( v ) ) { return std::forward<T>( v ); }
		template<typename... Ts>
		static P_WaitForAll ExecWaitForAll( Ts&&... vs ) { return WaitForAll::MakeI( Start( std::move( vs ) )... ); }
	}
}

namespace UC
{
	namespace Coro
	{
		/// <summary>
		/// This exception is thrown on failure by the functions in the coroutine library that deal with shared states i.e. functions in the class Future.
		/// </summary>
		UCException( FutureException );

		/// <summary>
		/// Common error codes for Fail calls.
		/// </summary>
		enum class FailureType :char
		{
			Unknown ,

			Exception ,
			RoutineStopped ,
			NullReference
		};

		enum class State :char { InProgress , Completed , Failed , Cancelled };

		/// <summary>
		/// Failure state for a future.
		/// </summary>
		struct Failure
		{
			/// <summary>
			/// How a future has failed.
			/// </summary>
			FailureType Type;

			/// <summary>
			/// Failure details.
			/// </summary>
			boost::variant<nullptr_t , std::exception , Exception> Details;
		};

		template<typename T>
		struct Future;

	#pragma region Future<T>
		/// <summary>
		/// Represents a value that will be set at some point
		/// in the future. Can be either completed or failed.
		/// </summary>
		UCTemplateInterface( Future , ( T ) , UC_WhereTypenameIs( "UC::Coro::Future" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;
	private:
		Future( ) { }
		State __state = State::InProgress;

		T __val;

		Failure __failure;

		W_Coroutine __coro;

		mutable boost::recursive_mutex __mtx;
	public:
		/// <summary>
		/// Cancels the Future if not already completed or failed,
		/// and cleans up references.
		/// </summary>
		~Future( )
		{
			Cancel( );
		}

		/// <summary>
		/// Returns if the Future has been completed, failed, or canceled.
		/// </summary>
		State GetState( ) const { return __state; }

	#pragma region Completion
		/// <summary>
		/// Returns the value, or throws an exception
		/// if the Future has not been completed.
		/// </summary>
		T Get( ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state != State::Completed )
				throw FutureException( ConcatNatStrings(
					"Cannot get value of " , SGetTypeName( ) , " before it is completed!" ) );
			return __val;
		}

		/// <summary>
		/// Attempts to return the value.
		/// </summary>
		bool TryGet( T& outValue ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == State::Completed )outValue = __val;
			return __state == State::Completed;
		}

		/// <summary>
		/// Sets the value, or throws an exception
		/// if the Future has already been completed or failed.
		/// </summary>
		void Complete( T inValue )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == State::Cancelled )
				return;

			if ( __state != State::InProgress )
				throw FutureException( ConcatNatStrings( "Cannot set value of " , SGetTypeName( ) , " once Future has completed or failed!" ) );
			__state = State::Completed;
			__val = inValue;
		}

		bool Finished( ) { return GetState( ) != State::InProgress; }

	#pragma endregion

	#pragma region Failure
		/// <summary>
		/// Returns the failure object, or throws an exception
		/// if the Future has not failed.
		/// </summary>
		Failure GetFailure( ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state != State::Failed && __state != State::Cancelled )
				throw FutureException( ConcatNatStrings( "Cannot get error of " , SGetTypeName( ) , " before it has failed!" ) );
			return __failure;
		}

		/// <summary>
		/// Attempts to return the failure object.
		/// </summary>
		bool TryGetFailure( Failure& outFailure ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == State::Failed )outFailure = __failure;
			return __state == State::Failed;
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( )
		{
			Fail( Failure { FailureType::Unknown , nullptr } );
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( boost::variant<nullptr_t , std::exception , Exception> arg )
		{
			Fail( Failure { FailureType::Exception , arg } );
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( FailureType type )
		{
			Fail( Failure { type , nullptr } );
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( FailureType type , boost::variant<nullptr_t , std::exception , Exception> arg )
		{
			Fail( Failure { type , arg } );
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( Failure failure )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == State::Cancelled )
				return;

			if ( __state != State::InProgress )
				throw FutureException( ConcatNatStrings( "Cannot fail " , SGetTypeName( ) , " once Future has completed or failed!" ) );
			__state = State::Failed;
			__failure = std::move( failure );
		}
	#pragma endregion

	#pragma region Cancellation
		/// <summary>
		/// Cancels the Future. It will no longer receive Complete or Fail calls.
		/// </summary>
		void Cancel( )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == State::InProgress )
			{
				Fail( Failure { FailureType::RoutineStopped, nullptr } );
				Stop( __coro.Lock( ) );
				__state = State::Cancelled;
			}
		}

	#pragma endregion

		/// <summary>
		/// Links a Routine to the Future.
		/// If the Routine stops, the Future will fail.
		/// If the Future is canceled, the Routine will Stop.
		/// </summary>
		Future<T>& LinkTo( P_Coroutine inRoutine )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( !__coro && __state == State::InProgress )
			{
				__coro = inRoutine;
				inRoutine->OnStop( [ wme = WME ] { if ( wme ) wme.Lock( )->OnCoroStopped( ); } );
			}
			return *this;
		}

		P_Coroutine GetLinked( ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			return __coro.Lock( );
		}

		/// <summary>
		/// Waits for the Future to be completed or failed.
		/// </summary>
		void Wait( ) const
		{
			while ( __state == State::InProgress );
		}

	private:
		void OnCoroStopped( )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == State::InProgress )
				Fail( FailureType::RoutineStopped );
		}
		UCEndTemplateInterface( Future , ( T ) );
	#pragma endregion


	#pragma region Future<void>
		template<>
		UCInterface( Future<void> , UC_WhereTypenameIs( "UC::Coro::Future" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;
	private:
		Future( ) { }
		State __state = State::InProgress;

		Failure __failure;

		W_Coroutine __coro;

		mutable boost::recursive_mutex __mtx;
	public:
		/// <summary>
		/// Cancels the Future if not already completed or failed,
		/// and cleans up references.
		/// </summary>
		~Future( )
		{
			Cancel( );
		}

		/// <summary>
		/// Returns if the Future has been completed, failed, or canceled.
		/// </summary>
		State GetState( ) const { return __state; }

	#pragma region Completion
		/// <summary>
		/// Sets the value, or throws an exception
		/// if the Future has already been completed or failed.
		/// </summary>
		void Complete( )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == State::Cancelled )
				return;

			if ( __state != State::InProgress )
				throw FutureException( ConcatNatStrings( "Cannot set value of " , SGetTypeName( ) , " once Future has completed or failed!" ) );
			__state = State::Completed;
		}

		bool Finished( ) { return GetState( ) != State::InProgress; }

	#pragma endregion

	#pragma region Failure
		/// <summary>
		/// Returns the failure object, or throws an exception
		/// if the Future has not failed.
		/// </summary>
		Failure GetFailure( ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state != State::Failed && __state != State::Cancelled )
				throw FutureException( ConcatNatStrings( "Cannot get error of " , SGetTypeName( ) , " before it has failed!" ) );
			return __failure;
		}

		/// <summary>
		/// Attempts to return the failure object.
		/// </summary>
		bool TryGetFailure( Failure& outFailure ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == State::Failed )outFailure = __failure;
			return __state == State::Failed;
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( )
		{
			Fail( Failure { FailureType::Unknown , nullptr } );
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( boost::variant<nullptr_t , std::exception , Exception> arg )
		{
			Fail( Failure { FailureType::Exception , arg } );
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( FailureType type )
		{
			Fail( Failure { type , nullptr } );
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( FailureType type , boost::variant<nullptr_t , std::exception , Exception> arg )
		{
			Fail( Failure { type , arg } );
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( Failure failure )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == State::Cancelled )
				return;

			if ( __state != State::InProgress )
				throw FutureException( ConcatNatStrings( "Cannot fail " , SGetTypeName( ) , " once Future has completed or failed!" ) );
			__state = State::Failed;
			__failure = std::move( failure );
		}
	#pragma endregion

	#pragma region Cancellation
		/// <summary>
		/// Cancels the Future. It will no longer receive Complete or Fail calls.
		/// </summary>
		void Cancel( )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == State::InProgress )
			{
				Fail( Failure { FailureType::RoutineStopped, nullptr } );
				Stop( __coro.Lock( ) );
				__state = State::Cancelled;
			}
		}

	#pragma endregion

		/// <summary>
		/// Links a Routine to the Future.
		/// If the Routine stops, the Future will fail.
		/// If the Future is canceled, the Routine will Stop.
		/// </summary>
		Future<void>& LinkTo( P_Coroutine inRoutine )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( !__coro && __state == State::InProgress )
			{
				__coro = inRoutine;
				inRoutine->OnStop( [ wme = WME ] { if ( wme ) wme.Lock( )->OnCoroStopped( ); } );
			}
			return *this;
		}

		P_Coroutine GetLinked( ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			return __coro.Lock( );
		}

		/// <summary>
		/// Waits for the Future to be completed or failed.
		/// </summary>
		void Wait( ) const
		{
			while ( __state == State::InProgress );
		}

	private:
		void OnCoroStopped( )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == State::InProgress )
				Fail( FailureType::RoutineStopped );
		}
		UCEndHiddenInterface;
	#pragma endregion
	}
}

namespace std
{
	static ostream& operator<<( ostream& o , const UC::Coro::FailureType& ft )
	{
		switch ( ft )
		{
		case UC::Coro::FailureType::Unknown:return o << "UC::Coro::FailureType::Unknown";
		case UC::Coro::FailureType::Exception:return o << "UC::Coro::FailureType::Exception";
		case UC::Coro::FailureType::RoutineStopped:return o << "UC::Coro::FailureType::RoutineStopped";
		case UC::Coro::FailureType::NullReference:return o << "UC::Coro::FailureType::NullReference";
		default:return o << "Unknown";
		}
	}

	static ostream& operator<<( ostream& o , const UC::Coro::State& st )
	{
		switch ( st )
		{
		case UC::Coro::State::InProgress:return o << "UC::Coro::State::InProgress";
		case UC::Coro::State::Completed:return o << "UC::Coro::State::Completed";
		case UC::Coro::State::Failed:return o << "UC::Coro::State::Failed";
		case UC::Coro::State::Cancelled:return o << "UC::Coro::State::Cancelled";
		default:return o << "Unknown";
		}
	}

	static ostream& operator<<( ostream& o , const UC::Coro::Failure& fl )
	{
		return o << "UC::Coro::Failure{ Type = " << fl.Type << " , Details = \"" << fl.Details << "\"}";
	}
}

#define UCCoro(name, params, ...) UCGen(::UC::Coro::P_YieldInstruction, name, params, __VA_ARGS__) using namespace ::UC::Coro::CoroLiterals;
#define UCCoroLambda(params, ...) UCGenLambda(::UC::Coro::P_YieldInstruction, params, __VA_ARGS__) using namespace ::UC::Coro::CoroLiterals;
#define UCCoroBeg(params, ...) UCGenBeg(::UC::Coro::P_YieldInstruction, params, __VA_ARGS__) using namespace ::UC::Coro::CoroLiterals;
#define UCCoroEnd UCBDGenEnd

#define UCRCoroBeg(ret, params, ...)\
{\
	using __uc_gen_holder_ = ::UC::_Detail::_GeneratorFuncHld<::UC::Coro::P_YieldInstruction>;\
	auto __future__ = ::UC::Coro::Future<BOOST_PP_REMOVE_PARENS(ret)>::Make();\
	auto gen = ::UC::Coro::Start(::UC::Coro::GeneratorForCoroutine( __uc_gen_holder_( [__UC_TUPLE_FOR_EACH_I(__UC_captureParams,params) __uc_coro_last_line = ::UC::_Detail::LineRecordType( 0 ), __uc_coro_makes_non_copyable = ::UC::_Detail::makes_noncopyable(), __future__, __VA_ARGS__](::UC::Coro::P_YieldInstruction& __uc_coro_ret_val) mutable{\
		using namespace ::UC::Coro::CoroLiterals;\
		if(__uc_coro_last_line == -1) { return false; }\
		switch(__uc_coro_last_line){\
		case 0:;\

#define UCRCoroEnd\
		};\
		__uc_coro_last_line = -1;\
		return false;\
	})));\
	__future__->LinkTo(gen);\
	return __future__;\
};

#define UCRCoro(ret, name, params, ...) ::UC::Coro::P_Future<BOOST_PP_REMOVE_PARENS(ret)> name(__UC_TUPLE_FOR_EACH_I(__UC_genParams,params)) UCRCoroBeg(ret, params, __VA_ARGS__)
#define UCRCoroLambda(params, ...) (__UC_TUPLE_FOR_EACH_I(__UC_genParams,params)) -> ::UC::Coro::P_Future<BOOST_PP_REMOVE_PARENS(ret)> UCRCoroBeg(ret, params, __VA_ARGS__)

#define UCCoroReturn(value) __future__->Complete(value)
#define UCCoroFail UCYieldEsc

#define UCAwait(...) UCYield( ::UC::Coro::ExecWaitForAll( __VA_ARGS__ ) )
#endif // !__UC__COROUTINE_HPP__
