#pragma once
#ifndef __UC__COROUTINE_HPP__
#define __UC__COROUTINE_HPP__
#include "stdafx.h"
#include "Object.hpp"
#include "Generator.hpp"

#ifndef UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS
#define UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS 10
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
		forceinline YieldInstruction( ) noexcept { }
		virtual bool Finished( P<Coroutine> ) = 0;
		UCEndInterface;
	#pragma endregion


		using GeneratorForCoroutine = Generator<P<YieldInstruction>>;

		enum class CoroutineState { Running , Paused , Failed , Finished };
		using ExceptionDetailsType = std::exception_ptr;


	#pragma region Coroutine
		UCInterface( Coroutine , UC_WhereTypenameIs( "UC::Coro::Coroutine" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;
	public:
		GeneratorForCoroutine __fiber;
		P<YieldInstruction> __instruction;
		P<Event<void>> __onStop;
		CoroutineState __state = CoroutineState::Running;
		ExceptionDetailsType __failureDetails = nullptr;
		mutable boost::recursive_mutex mtx;

		void __Fail( ExceptionDetailsType f )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			__state = CoroutineState::Failed; __failureDetails = std::move( f );
		}
		void __Finish( )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			if ( !Failed( ) ) __state = CoroutineState::Finished;
		}

		bool operator ==( const Coroutine& r )const noexcept
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			return r.__instruction.RefEq( __instruction );
		}
		bool operator !=( const Coroutine& r )const noexcept
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			return !r.__instruction.RefEq( __instruction );
		}

		void Pause( )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			if ( !FinishedOrFailed( ) ) __state = CoroutineState::Paused;
		}
		void Resume( )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			if ( !FinishedOrFailed( ) ) __state = CoroutineState::Running;
		}
		CoroutineState GetSate( ) const noexcept
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			return __state;
		}
		bool Running( ) const noexcept
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			return __state == CoroutineState::Running;
		}
		bool Paused( ) const noexcept
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			return __state == CoroutineState::Paused;
		}
		bool FinishedOrFailed( ) noexcept
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			return __state == CoroutineState::Finished || __state == CoroutineState::Failed;
		}
		bool Finished( ) const noexcept
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			return __state == CoroutineState::Finished;
		}
		bool Finished( P<Coroutine> ) override
		{
			return FinishedOrFailed( );
		}
		bool Failed( ) const noexcept
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			return __state == CoroutineState::Failed;
		}
		const ExceptionDetailsType& Failure( ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			return __failureDetails;
		}

		/// <summary>
		/// Adds a callback for when the Future fails.
		/// Will call immediately if the Future has already failed.
		/// </summary>
		void OnStopF( P<Functor<void>> inCallback )
		{
			if ( !inCallback ) return;
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			__onStop->AddF( inCallback );
		}

		/// <summary>
		/// Adds a callback for when the Future fails.
		/// Will call immediately if the Future has already failed.
		/// </summary>
		template<typename TF> void OnStop( TF&& inCallback )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( mtx );
			__onStop->Add( std::forward<TF>( inCallback ) );
		}
	protected:
		Coroutine( GeneratorForCoroutine&& _fiber ) :__fiber( std::move( _fiber ) ) , __onStop( Event<void>::Make( ) ) { }
		UCEndInterface;
	#pragma endregion


	#pragma region WaitForMillis
		UCInterface( WaitForMillis , UC_WhereTypenameIs( "UC::Coro::WaitForMillis" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;
	public:
		bool Finished( P<Coroutine> ) override { return timeToWaitTo <= std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::steady_clock::now( ).time_since_epoch( ) ); }
	protected:
		std::chrono::milliseconds timeToWaitTo;
		WaitForMillis( int64_t timeToWait ) :timeToWaitTo(
			std::chrono::duration_cast< std::chrono::milliseconds >(
				std::chrono::steady_clock::now( ).time_since_epoch( )
				) + std::chrono::milliseconds( timeToWait ) )
		{ }
		UCEndInterface;
	#pragma endregion


	#pragma region ContinueWhen
		UCTemplateInterface( ContinueWhen , ( TFunc ) , UC_WhereTypenameIs( "UC::Coro::ContinueWhen" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;

		bool Finished( P<Coroutine> ) override
		{
			return std::invoke( func );
		};
	protected:
		TFunc func;

		ContinueWhen( TFunc fun ) :func( fun ) { }
		UCEndInterface;
	#pragma endregion


	#pragma region BlockUntil
		UCTemplateInterface( BlockUntil , ( TFunc ) , UC_WhereTypenameIs( "UC::Coro::BlockUntil" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;

		bool Finished( P<Coroutine> ) override
		{
			return !std::invoke( func );
		};
	protected:
		TFunc func;

		BlockUntil( TFunc fun ) :func( fun ) { }
		UCEndInterface;
	#pragma endregion


	#pragma region WaitForAll
		UCInterface( WaitForAll , UC_WhereTypenameIs( "UC::Coro::WaitForAll" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;
	public:
		bool Finished( P<Coroutine> cor ) override
		{
			for ( auto& i : coros )
				if ( !i->Finished( cor ) )
					return false;
			return true;
		}
		template<typename... Ts> static pself MakeI( Ts&&... vs ) { return pself( new self { std::forward<Ts>( vs )... } ); }
	protected:
		NatVector<P<YieldInstruction>> coros;
		WaitForAll( std::initializer_list<P<YieldInstruction>> ilst ) :coros( ilst ) { }
		WaitForAll( NatVector<P<YieldInstruction>>&& vec ) :coros( std::move( vec ) ) { }
		WaitForAll( const NatVector<P<YieldInstruction>>& vec ) :coros( vec ) { }
		UCEndInterface;
	#pragma endregion


	#pragma region WaitTillUpdate
		UCInterface( WaitTillUpdate , UC_WhereTypenameIs( "UC::Coro::WaitTillUpdate" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_IsSingleton;
		UC_HasNoMethods;
	public:
		bool Finished( P<Coroutine> ) override { return true; }
	protected:
		WaitTillUpdate( ) { }
		UCEndInterface;
	#pragma endregion
	}
}

namespace UC
{
	namespace Coro
	{
		/// <summary>
		/// Starts a coroutine, the coroutine does not run immediately but next time UC_COROUTINE_FIXED_UPDATE_TIME_IN_MS milliseconds have elapsed. <para></para>
		/// The execution of a coroutine can be paused at any point using a Yield statement, but Await is recommended statement. <para></para>
		/// The awaited value specifies when the coroutine should be resumed.
		/// </summary>
		/// <param name="fiber">The generator which will be wrapped as a coroutine</param>
		/// <returns>The coroutine generated.</returns>
		P<Coroutine> Start( GeneratorForCoroutine&& fiber );

		/// <summary>
		/// Removes the specified coroutine from the scheduled coroutine list.
		/// </summary>
		/// <param name="coroutine"></param>
		void Stop( P<Coroutine> coroutine );

		static P<WaitForMillis> WaitForSec( long double param )
		{
			return WaitForMillis::Make( static_cast< unsigned long long int >( param * 1000 ) );
		}

		template<typename T> static P<ContinueWhen<T>> WaitUntil( T&& f ) { return ContinueWhen<T>::Make( std::forward<T>( f ) ); }

		template<typename T> static P<BlockUntil<T>> WaitWhile( T&& f ) { return BlockUntil<T>::Make( std::forward<T>( f ) ); }

		forceinline P<YieldInstruction> AsYieldInstruction( P<YieldInstruction> res ) { return res; }
		forceinline P<Coroutine> AsYieldInstruction( GeneratorForCoroutine&& cor )
		{
			return Start( std::move( cor ) );
		}
		forceinline P<WaitForMillis> AsYieldInstruction( long double time )
		{
			return WaitForMillis::Make( static_cast< unsigned long long int >( time * 1000 ) );
		}

		static forceinline P<WaitTillUpdate> ExecWaitForAll( ) { return WaitTillUpdate::GetI( ); }

		template<typename T>
		static auto ExecWaitForAll( T&& v ) { return AsYieldInstruction( std::forward<T>( v ) ); }

		template<typename... Ts>
		static P<WaitForAll> ExecWaitForAll( Ts&&... vs ) { return WaitForAll::MakeI( AsYieldInstruction( std::forward<Ts>( vs ) )... ); }
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
			Exception ,
			RoutineStopped
		};

		enum class FutureState :char { InProgress , Completed , Failed , Cancelled };

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
			ExceptionDetailsType Details;
		};

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
		FutureState __state = FutureState::InProgress;

		T __val;

		Failure __failure;

		W<Coroutine> __coro;

		mutable boost::recursive_mutex __mtx;
	public:
		/// <summary>
		/// Cancels the Future if not already completed or failed.
		/// </summary>
		~Future( )
		{
			Cancel( );
		}

		/// <summary>
		/// Returns if the Future has been completed, failed, or canceled.
		/// </summary>
		FutureState GetState( ) const { return __state; }

	#pragma region Completion
		/// <summary>
		/// Returns the value, or throws an exception
		/// if the Future has not been completed.
		/// </summary>
		T Get( ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state != FutureState::Completed )
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
			if ( __state == FutureState::Completed )outValue = __val;
			return __state == FutureState::Completed;
		}

		/// <summary>
		/// Sets the value, or throws an exception
		/// if the Future has already been completed or failed.
		/// </summary>
		void Complete( T inValue )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == FutureState::Cancelled )
				return;

			if ( __state != FutureState::InProgress )
				throw FutureException( ConcatNatStrings( "Cannot set value of " , SGetTypeName( ) , " once Future has completed or failed!" ) );
			__state = FutureState::Completed;
			__val = inValue;
		}

		bool Finished( P<Coroutine> ) override { return GetState( ) != FutureState::InProgress; }

	#pragma endregion

	#pragma region Failure
		/// <summary>
		/// Returns the failure object, or throws an exception
		/// if the Future has not failed.
		/// </summary>
		Failure GetFailure( ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state != FutureState::Failed && __state != FutureState::Cancelled )
				throw FutureException( ConcatNatStrings( "Cannot get error of " , SGetTypeName( ) , " before it has failed!" ) );
			return __failure;
		}

		/// <summary>
		/// Attempts to return the failure object.
		/// </summary>
		bool TryGetFailure( Failure& outFailure ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == FutureState::Failed )outFailure = __failure;
			return __state == FutureState::Failed;
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( ExceptionDetailsType arg )
		{
			Fail( Failure { FailureType::Exception , arg } );
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( FailureType type , ExceptionDetailsType arg = nullptr )
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
			if ( __state == FutureState::Cancelled )
				return;

			if ( __state != FutureState::InProgress )
				throw FutureException( ConcatNatStrings( "Cannot fail " , SGetTypeName( ) , " once Future has completed or failed!" ) );
			__state = FutureState::Failed;
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
			if ( __state == FutureState::InProgress )
			{
				Fail( Failure { FailureType::RoutineStopped, nullptr } );
				Stop( __coro.Lock( ) );
				__state = FutureState::Cancelled;
			}
		}

	#pragma endregion

		/// <summary>
		/// Links a Routine to the Future.
		/// If the Routine stops, the Future will fail.
		/// If the Future is canceled, the Routine will Stop.
		/// </summary>
		Future<T>& LinkTo( P<Coroutine> inRoutine )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( !__coro && __state == FutureState::InProgress )
			{
				__coro = inRoutine;
				inRoutine->OnStop( [ wme = WME ] { if ( wme ) wme.Lock( )->OnCoroStopped( ); } );
			}
			return *this;
		}

		P<Coroutine> GetLinked( ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			return __coro.Lock( );
		}

	private:
		void OnCoroStopped( )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == FutureState::InProgress )
				Fail( FailureType::RoutineStopped );
		}
		UCEndInterface;
	#pragma endregion


	#pragma region Future<void>
		template<>
		UCInterface( Future<void> , UC_WhereTypenameIs( "UC::Coro::Future" ) , UC_InheritsUCClasses( YieldInstruction ) , UC_InheritsNoNativeClasses );
		UC_OnlyHasNativeCtors;
		UC_HasNoMethods;
	private:
		Future( ) { }
		FutureState __state = FutureState::InProgress;

		Failure __failure;

		W<Coroutine> __coro;

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
		FutureState GetState( ) const { return __state; }

	#pragma region Completion
		/// <summary>
		/// Sets the value, or throws an exception
		/// if the Future has already been completed or failed.
		/// </summary>
		void Complete( )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == FutureState::Cancelled )
				return;

			if ( __state != FutureState::InProgress )
				throw FutureException( ConcatNatStrings( "Cannot set value of " , SGetTypeName( ) , " once Future has completed or failed!" ) );
			__state = FutureState::Completed;
		}

		bool Finished( P<Coroutine> ) override { return GetState( ) != FutureState::InProgress; }

	#pragma endregion

	#pragma region Failure
		/// <summary>
		/// Returns the failure object, or throws an exception
		/// if the Future has not failed.
		/// </summary>
		Failure GetFailure( ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state != FutureState::Failed && __state != FutureState::Cancelled )
				throw FutureException( ConcatNatStrings( "Cannot get error of " , SGetTypeName( ) , " before it has failed!" ) );
			return __failure;
		}

		/// <summary>
		/// Attempts to return the failure object.
		/// </summary>
		bool TryGetFailure( Failure& outFailure ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == FutureState::Failed )outFailure = __failure;
			return __state == FutureState::Failed;
		}

		/// <summary>
		/// Fails the Future, or throws an exception
		/// if the Future has already been set or failed.
		/// </summary>
		void Fail( ExceptionDetailsType arg )
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
		void Fail( FailureType type , ExceptionDetailsType arg )
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
			if ( __state == FutureState::Cancelled )
				return;

			if ( __state != FutureState::InProgress )
				throw FutureException( ConcatNatStrings( "Cannot fail " , SGetTypeName( ) , " once Future has completed or failed!" ) );
			__state = FutureState::Failed;
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
			if ( __state == FutureState::InProgress )
			{
				Fail( Failure { FailureType::RoutineStopped, nullptr } );
				Stop( __coro.Lock( ) );
				__state = FutureState::Cancelled;
			}
		}

	#pragma endregion

		/// <summary>
		/// Links a Routine to the Future.
		/// If the Routine stops, the Future will fail.
		/// If the Future is canceled, the Routine will Stop.
		/// </summary>
		Future<void>& LinkTo( P<Coroutine> inRoutine )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( !__coro && __state == FutureState::InProgress )
			{
				__coro = inRoutine;
				inRoutine->OnStop( [ wme = WME ] { if ( wme ) wme.Lock( )->OnCoroStopped( ); } );
			}
			return *this;
		}

		P<Coroutine> GetLinked( ) const
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			return __coro.Lock( );
		}

		/// <summary>
		/// Waits for the Future to be completed or failed.
		/// </summary>
		void Wait( ) const
		{
			while ( __state == FutureState::InProgress );
		}

	private:
		void OnCoroStopped( )
		{
			boost::lock_guard<boost::recursive_mutex> __lock( __mtx );
			if ( __state == FutureState::InProgress )
				Fail( FailureType::RoutineStopped );
		}
		UCEndInterface;
	#pragma endregion
	}
}

namespace std
{
	static ostream& operator<<( ostream& o , const UC::Coro::FailureType& ft )
	{
		switch ( ft )
		{
		case UC::Coro::FailureType::Exception:return o << "UC::Coro::FailureType::Exception";
		case UC::Coro::FailureType::RoutineStopped:return o << "UC::Coro::FailureType::RoutineStopped";
		default:return o << "Unknown";
		}
	}

	static ostream& operator<<( ostream& o , const UC::Coro::FutureState& st )
	{
		switch ( st )
		{
		case UC::Coro::FutureState::InProgress:return o << "UC::Coro::FutureState::InProgress";
		case UC::Coro::FutureState::Completed:return o << "UC::Coro::FutureState::Completed";
		case UC::Coro::FutureState::Failed:return o << "UC::Coro::FutureState::Failed";
		case UC::Coro::FutureState::Cancelled:return o << "UC::Coro::FutureState::Cancelled";
		default:return o << "Unknown";
		}
	}

	static ostream& operator<<( ostream& o , const UC::Coro::Failure& fl )
	{
		return o << "UC::Coro::Failure{ Type = " << fl.Type << " , Details = \"" << fl.Details << "\"}";
	}
}

#define UCCoroBeg(params, ...) \
{\
	using __uc_gen_holder_ = ::UC::_Detail::_GeneratorFuncHld<::UC::P<::UC::Coro::YieldInstruction>>;\
	return ::UC::Coro::Start(::UC::Coro::GeneratorForCoroutine( __uc_gen_holder_( [__UC_TUPLE_FOR_EACH_I(__UC_captureParams,params) __uc_coro_last_line = ::UC::_Detail::LineRecordType( 0 ), __uc_coro_makes_non_copyable = ::UC::_Detail::makes_noncopyable(), __uc_exception_info = ::UC::_Detail::ExceptionDetails(), __VA_ARGS__](::UC::P<::UC::Coro::YieldInstruction>& __uc_coro_ret_val) mutable{\
		if(__uc_coro_last_line == -1) { return false; }\
		__uc_coro_start:\
		try{\
			switch(__uc_coro_last_line){\
			case 0:;

#define UCCoroEnd \
			};\
		}\
		catch(...)\
		{\
			if(__uc_exception_info.isInTry)\
			{\
				__uc_coro_last_line = __uc_exception_info.curr;\
				__uc_exception_info.p = ::std::current_exception();\
				goto __uc_coro_start;\
			} else{ __uc_coro_last_line = -1; throw; }\
		}\
		__uc_coro_last_line = -1;\
		return false;\
	})));\
};

#define UCCoro(name, params, ...) ::UC::P<::UC::Coro::Coroutine> name(__UC_TUPLE_FOR_EACH_I(__UC_genParams,params)) UCCoroBeg(params, __VA_ARGS__)
#define UCCoroLambda(params, ...) (__UC_TUPLE_FOR_EACH_I(__UC_genParams,params)) -> ::UC::P<::UC::Coro::Coroutine> UCCoroBeg(params, __VA_ARGS__)


#define UCRCoroBeg(ret, params, ...)\
{\
	using __uc_gen_holder_ = ::UC::_Detail::_GeneratorFuncHld<::UC::P<::UC::Coro::YieldInstruction>>;\
	auto __future__ = ::UC::Coro::Future<BOOST_PP_REMOVE_PARENS(ret)>::Make();\
	auto gen = ::UC::Coro::Start(::UC::Coro::GeneratorForCoroutine( __uc_gen_holder_( [__UC_TUPLE_FOR_EACH_I(__UC_captureParams,params) __uc_coro_last_line = ::UC::_Detail::LineRecordType( 0 ), __uc_coro_makes_non_copyable = ::UC::_Detail::makes_noncopyable(), __uc_exception_info = ::UC::_Detail::ExceptionDetails(), __future__, __VA_ARGS__](::UC::P<::UC::Coro::YieldInstruction>& __uc_coro_ret_val) mutable{\
		if(__uc_coro_last_line == -1) { return false; }\
		__uc_coro_start:\
		try{\
			switch(__uc_coro_last_line){\
			case 0:;

#define UCRCoroEnd\
			}\
		}\
		catch(...)\
		{\
			if(__uc_exception_info.isInTry)\
			{\
				__uc_coro_last_line = __uc_exception_info.curr;\
				__uc_exception_info.p = ::std::current_exception();\
				goto __uc_coro_start;\
			} else __future__->Fail(::std::current_exception());\
		}\
		__uc_coro_last_line = -1;\
		return false;\
	})));\
	__future__->LinkTo(gen);\
	return __future__;\
};

#define UCRCoro(ret, name, params, ...) ::UC::P<::UC::Coro::Future<BOOST_PP_REMOVE_PARENS(ret)>> name(__UC_TUPLE_FOR_EACH_I(__UC_genParams,params)) UCRCoroBeg(ret, params, __VA_ARGS__)
#define UCRCoroLambda(params, ...) (__UC_TUPLE_FOR_EACH_I(__UC_genParams,params)) -> ::UC::P<::UC::Coro::Future<BOOST_PP_REMOVE_PARENS(ret)>> UCRCoroBeg(ret, params, __VA_ARGS__)

#define UCCoroReturn(value) __future__->Complete(value)
#define UCCoroFail UCYieldEsc

#define UCAwait(...) UCYield( ::UC::Coro::ExecWaitForAll( __VA_ARGS__ ) )
#define UCAwaitEsc UCYieldEsc
#endif // !__UC__COROUTINE_HPP__
