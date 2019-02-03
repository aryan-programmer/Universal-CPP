#pragma once
#ifndef __UC__GENERATOR_HPP__
#define __UC__GENERATOR_HPP__
#include "stdafx.h"
#include "common_mcr.hpp"
#include "Interface.hpp"
#include "Object.hpp"

// From Boost.ScopeExit: MSVC has problems expanding __LINE__ so use (the non standard) __COUNTER__.
#ifdef __COUNTER__
#   define __UC_MostlyUniquePreprocessingTimeInt __COUNTER__
#else
#   define __UC_MostlyUniquePreprocessingTimeInt __LINE__
#endif

namespace UC
{
	struct FakeVoid
	{
		forceinline static FakeVoid& I( )
		{
			static FakeVoid s;
			return s;
		};

		forceinline FakeVoid( ) { }
		forceinline FakeVoid( nullptr_t ) { }
		forceinline FakeVoid( int16_t ) { }
		forceinline FakeVoid( int32_t ) { }
		forceinline FakeVoid( int64_t ) { }
		forceinline operator nullptr_t( ) { return nullptr; }
		forceinline operator int16_t( ) { return 0i16; }
		forceinline operator int32_t( ) { return 0i32; }
		forceinline operator int64_t( ) { return 0i64; }
	};

	namespace _Detail
	{
		struct makes_noncopyable
		{
			makes_noncopyable( ) = default;
			makes_noncopyable( makes_noncopyable&& ) = default;
			makes_noncopyable& operator =( makes_noncopyable&& ) = default;
		};

		template<typename T , typename... TInp>
		struct _GeneratorFuncHld :makes_noncopyable
		{
			struct _1
			{
				virtual bool operator()( T& , TInp... ) = 0;

				virtual ~_1( ) = default;
			};

			template<typename I>
			struct _2 :_1
			{
				I i;
				_2( I&& _i ) : i( std::move( _i ) ) { }

				bool operator()( T& t , TInp... tinp ) { return i( t , tinp... ); }
			};

			std::unique_ptr<_1> ptr;

			template<typename I , typename = std::enable_if_t<!std::is_same_v<boost::remove_cv_ref_t<I> , _GeneratorFuncHld>>>
			_GeneratorFuncHld( I&& i ) : ptr( new _2<I>( std::move( i ) ) ) { }
			_GeneratorFuncHld( ) : ptr( ) { }
			_GeneratorFuncHld( nullptr_t ) : ptr( ) { }

			_GeneratorFuncHld( _GeneratorFuncHld&& ) = default;
			_GeneratorFuncHld& operator =( _GeneratorFuncHld&& ) = default;
			_GeneratorFuncHld& operator =( nullptr_t ) { ptr = nullptr; return *this; }

			bool operator ==( nullptr_t ) const { return ptr == nullptr; }
			bool operator !=( nullptr_t ) const { return ptr != nullptr; }
			bool operator ==( const _GeneratorFuncHld& p ) const { return ptr == p.ptr; }
			bool operator !=( const _GeneratorFuncHld& p ) const { return ptr != p.ptr; }

			bool operator()( T& t , TInp... tinp )
			{
				return ptr->operator()( t , tinp... );
			}
		};

		using LineRecordType = int64_t;
	};

	template<typename T , typename... TInp>
	class Generator final : boost::noncopyable
	{
		using Internal = _Detail::_GeneratorFuncHld<T , TInp...>;
		Internal ival;
		T val;
	public:
		Generator( ) :ival( ) { }
		Generator( Internal&& ival2 ) :ival( std::move( ival2 ) ) { }
		Generator& operator=( Internal&& ival2 ) { ival = std::move( ival2 ); }
		Generator( Generator&& g ) :ival( std::move( g.ival ) ) { }
		Generator& operator=( Generator&& g ) { ival = std::move( g.ival ); }

		explicit operator bool( ) { return ival != nullptr; }
		Generator& operator()( TInp... params )
		{
			if ( ival != nullptr && !( ival( val , params... ) ) ) { ival = false; };
			return *this;
		}
		___UC_NODISCARD___ const T& operator*( ) const { return val; }
		___UC_NODISCARD___ const T& Get( ) const { return val; }

		bool operator ==( nullptr_t ) const { return ( ival == nullptr ); }
		bool operator !=( nullptr_t ) const { return ( ival != nullptr ); }
		template<typename Internal2> bool operator ==( const Generator<Internal2 , T , TInp...>& r ) const { return ( ival == r.ival ); }
		template<typename Internal2> bool operator !=( const Generator<Internal2 , T , TInp...>& r ) const { return ( ival != r.ival ); }
	};

	template<typename T>
	class Generator<T> final : boost::noncopyable
	{
		using Internal = _Detail::_GeneratorFuncHld<T>;
		Internal ival;
		void getVal( ) { if ( ival != nullptr && !( ival( val ) ) ) { ival = nullptr; } }
		T val;
	public:
		struct iterator
		{
			using iterator_category = std::input_iterator_tag;
			using value_type = T;
			using pointer = const T *;
			using reference = const T&;

			Generator* gen;

			iterator( ) :gen( nullptr ) { }
			iterator( Generator* gen ) :gen( gen ) { }

			___UC_NODISCARD___ const T& operator*( ) const { return gen->val; }
			___UC_NODISCARD___ const T* operator->( ) const { return std::addressof( gen->val ); }

			iterator& operator++( ) { getVal( ); return *this; }

			bool operator ==( const iterator& r ) const { return ( gen->ival == &r.gen->ival ); }
			bool operator !=( const iterator& r ) const { return ( gen->ival != &r.gen->ival ); }
			bool operator ==( nullptr_t ) const { return ( gen->ival == nullptr ); }
			bool operator !=( nullptr_t ) const { return ( gen->ival != nullptr ); }

		private:
			void getVal( )
			{
				if ( gen != nullptr && gen->ival != nullptr && !( gen->ival( gen->val ) ) )
				{
					gen->ival = nullptr;
					gen = nullptr;
				}
			}
		};

		iterator begin( ) { return iterator( !ival != nullptr ? nullptr : this ); }
		iterator end( ) { return iterator( ); }

		Generator( ) :ival( ) { }
		Generator( Internal&& ival2 ) :ival( std::move( ival2 ) ) { }
		Generator& operator=( Internal&& ival2 ) { ival = std::move( ival2 ); }
		Generator( Generator&& g ) :ival( std::move( g.ival ) ) { }
		Generator& operator=( Generator&& g ) { ival = std::move( g.ival ); }

		explicit operator bool( ) { return ival != nullptr; }
		Generator& operator()( ) { getVal( ); return *this; }
		___UC_NODISCARD___ const T& operator*( ) const { return val; }
		___UC_NODISCARD___ const T& Get( ) const { return val; }
		bool operator ==( nullptr_t ) const { return ( ival == nullptr ); }
		bool operator !=( nullptr_t ) const { return ( ival != nullptr ); }
		template<typename Internal2> bool operator ==( const Generator<Internal2 , T>& r ) const { return ( ival == r.ival ); }
		template<typename Internal2> bool operator !=( const Generator<Internal2 , T>& r ) const { return ( ival != r.ival ); }
	};

	UCException( ContinueStatementInSwitchStatementInAGenerator );
}

#define __UC_TUPLE_FOR_EACH_I_IMPL(macro, t) BOOST_PP_SEQ_FOR_EACH_I(macro, _, BOOST_PP_VARIADIC_TO_SEQ t)
#define __UC_TUPLE_FOR_EACH_I_EMPTY(macro, t)

#define __UC_TUPLE_FOR_EACH_I(macro, t) BOOST_PP_IF(BOOST_PP_IS_EMPTY t, __UC_TUPLE_FOR_EACH_I_EMPTY, __UC_TUPLE_FOR_EACH_I_IMPL)(macro, t)

#define __UC_REM(...)
#define __UC_captureParams(r, data, i, elem) __UC_REM elem = std::forward<decltype(__UC_REM elem)>( __UC_REM elem ),
#define __UC_genParams(r, data, i, elem) BOOST_PP_COMMA_IF(i) __UCEXP elem
#define __UC_genInvocParams(r, data, i, elem) BOOST_PP_COMMA_IF(i) BOOST_PP_SEQ_ELEM(0, elem ) BOOST_PP_SEQ_ELEM(1, elem )
#define __UC_genInvocTypeParams(r, data, i, elem) BOOST_PP_COMMA_IF(i) BOOST_PP_SEQ_ELEM(0, elem )

#define UCGenBeg(retType, params, ...) \
{\
	using __uc_gen_holder_ = ::UC::_Detail::_GeneratorFuncHld<BOOST_PP_REMOVE_PARENS(retType)>;\
	return ::UC::Generator<BOOST_PP_REMOVE_PARENS(retType)>( __uc_gen_holder_( [__UC_TUPLE_FOR_EACH_I(__UC_captureParams,params) __uc_coro_last_line = ::UC::_Detail::LineRecordType( 0 ), __uc_coro_makes_non_copyable = ::UC::_Detail::makes_noncopyable(), __VA_ARGS__](BOOST_PP_REMOVE_PARENS(retType)& __uc_coro_ret_val) mutable{\
		if(__uc_coro_last_line == -1) { return false; }\
		switch(__uc_coro_last_line){\
		case 0:;

#define UCGenEnd\
		};\
		__uc_coro_last_line = -1;\
		return false;\
	}));\
};

#define UCBDGenBeg(retType, params, invocParams, ...) \
{\
	using __uc_gen_holder_ = ::UC::_Detail::_GeneratorFuncHld<BOOST_PP_REMOVE_PARENS(retType), __UC_TUPLE_FOR_EACH_I(__UC_genInvocTypeParams,invocParams)>;\
	return ::UC::Generator<BOOST_PP_REMOVE_PARENS(retType),__UC_TUPLE_FOR_EACH_I(__UC_genInvocTypeParams,invocParams)>(__uc_gen_holder_([__UC_TUPLE_FOR_EACH_I(__UC_captureParams,params) __uc_coro_last_line = ::UC::_Detail::LineRecordType( 0 ), __uc_coro_makes_non_copyable = ::UC::_Detail::makes_noncopyable(), __VA_ARGS__](BOOST_PP_REMOVE_PARENS(retType)& __uc_coro_ret_val, __UC_TUPLE_FOR_EACH_I(__UC_genInvocParams,invocParams)) mutable\
	{\
		if(__uc_coro_last_line == -1) { return false; }\
		switch(__uc_coro_last_line){\
		case 0:;

#define UCBDGenEnd\
		}\
		__uc_coro_last_line = -1;\
		return false;\
	}));\
};

#define UCBDGenLambda(retType, params, invocParams, ...) (__UC_TUPLE_FOR_EACH_I(__UC_genParams,params)) -> ::UC::Generator<BOOST_PP_REMOVE_PARENS(retType),__UC_TUPLE_FOR_EACH_I(__UC_genInvocTypeParams,invocParams)> UCBDGenBeg(retType, params, invocParams, __VA_ARGS__)

#define UCBDGen(retType, name, params, invocParams, ...) ::UC::Generator<BOOST_PP_REMOVE_PARENS(retType),__UC_TUPLE_FOR_EACH_I(__UC_genInvocTypeParams,invocParams)> name (__UC_TUPLE_FOR_EACH_I(__UC_genParams,params)) UCBDGenBeg(retType, params, invocParams, __VA_ARGS__)

#define UCGenLambda(retType, params, ...) (__UC_TUPLE_FOR_EACH_I(__UC_genParams,params)) -> ::UC::Generator<BOOST_PP_REMOVE_PARENS(retType)> UCGenBeg(retType, params, __VA_ARGS__)

#define UCGen(retType, name, params, ...) ::UC::Generator<BOOST_PP_REMOVE_PARENS(retType)> name (__UC_TUPLE_FOR_EACH_I(__UC_genParams,params)) UCGenBeg(retType, params, __VA_ARGS__)

#define UCYieldEsc do {\
	__uc_coro_last_line = -1;\
	return false;\
} while (0)

#define __UCYield(id, ...)\
do {\
	__uc_coro_last_line=id;\
	__uc_coro_ret_val = (__VA_ARGS__); return true; case id:;\
} while (0)

#define UCYield(...) __UCYield(__UC_MostlyUniquePreprocessingTimeInt, __VA_ARGS__)

#define __UC_SWITCH_LABEL(n) BOOST_PP_CAT(BOOST_PP_CAT(_uc_gen_sw_case_, __LINE__), n)

#define __UC_SWITCH_CASE(r, _, i, e)\
BOOST_PP_IF(BOOST_PP_MOD(i, 2), , e: goto __UC_SWITCH_LABEL(i);)\
/***/

#define __UC_SWITCH_BODY_TRUE(i, e) __UC_SWITCH_LABEL(BOOST_PP_DEC(i)): __UCEXP e
#define __UC_SWITCH_BODY_FALSE(i, e)

#define __UC_SWITCH_BODY(r, _, i, e)\
BOOST_PP_IF(BOOST_PP_MOD(i, 2), __UC_SWITCH_BODY_TRUE, __UC_SWITCH_BODY_FALSE)(i, e)\
/***/

#define __UC_SWITCH(n, seq, on_cont, cont_lbl)\
do{\
switch (n){BOOST_PP_SEQ_FOR_EACH_I(__UC_SWITCH_CASE, _, seq)}\
if (false){\
	cont_lbl;\
	for (;;on_cont){\
		break;\
		BOOST_PP_SEQ_FOR_EACH_I(__UC_SWITCH_BODY, _, seq)\
		break;\
	}\
}\
}while (0)

#define __UC_SWITCH_2(n, label, ...)\
__UC_SWITCH(n, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__), goto label, label: continue)

#define UCGenSwitchWithCont(n, ...)\
__UC_SWITCH_2(n, BOOST_PP_CAT(__uc_gen_sw_cont_, __LINE__), __VA_ARGS__)

#define UCGenSwitch(n, ...) __UC_SWITCH(n, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__), throw ::UC::ContinueStatementInSwitchStatementInAGenerator("There can't be a continue statement in a switch statement in a generator."), )
#endif // !__UC__GENERATOR_HPP__
