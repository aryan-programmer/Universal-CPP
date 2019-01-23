#pragma once

#include <stdafx.h>
#include <boost\type_traits\remove_cv_ref.hpp>
#include "common_mcr.hpp"
#include "Interface.hpp"
#include "Object.hpp"

namespace UC
{
	namespace _Detail
	{
		template<typename T>
		struct _Generator
		{
			int64_t __uc_coro_last_line;
			_Generator( ) :__uc_coro_last_line( 0 ) { }

			virtual bool operator()( T& __uc_coro_ret_val ) = 0;

			virtual ~_Generator( ) = default;
		};
	};

	template<typename T>
	class Generator final
	{
		std::unique_ptr<_Detail::_Generator<T>> ival;
		void getVal( ) { if ( ival != nullptr && !( ( *ival )( val ) ) ) { ival = nullptr; } }
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

			bool operator ==( const iterator& r ) const { return ( gen->ival == r.gen->ival ); }
			bool operator !=( const iterator& r ) const { return ( gen->ival != r.gen->ival ); }

		private:
			void getVal( )
			{
				if ( gen != nullptr && gen->ival != nullptr && !( ( *gen->ival )( val ) ) )
				{
					gen->ival = nullptr;
					gen = nullptr;
				}
			}
		};

		iterator begin( ) { return iterator( ival == nullptr ? nullptr : this ); }
		iterator end( ) { return iterator( ); }

		template<typename InTy2> Generator( std::unique_ptr<InTy2>&& ival ) :ival( std::move( ival ) ) { }

		Generator& operator()( ) { getVal( ); return *this; }
		___UC_NODISCARD___ const T& operator*( ) const { return val; }
		___UC_NODISCARD___ const T& Get( ) const { return val; }
		bool operator ==( const Generator& r ) const { return ( ival == r.ival ); }
		bool operator !=( const Generator& r ) const { return ( ival != r.ival ); }
	};
}

#define __UC_TUPLE_FOR_EACH_I_IMPL(macro, t) BOOST_PP_SEQ_FOR_EACH_I(macro, _, BOOST_PP_VARIADIC_TO_SEQ t)
#define __UC_TUPLE_FOR_EACH_I_EMPTY(macro, t)

#define __UC_TUPLE_FOR_EACH_I(macro, t) BOOST_PP_IF(BOOST_PP_IS_EMPTY t, __UC_TUPLE_FOR_EACH_I_EMPTY, __UC_TUPLE_FOR_EACH_I_IMPL)(macro, t)

#define __UC_REM(...)
#define __UC_declParams(r, data, i, elem) decltype(__UC_REM elem) __UC_REM elem;
#define __UC_declFwdParams(r, data, i, elem) BOOST_PP_COMMA_IF(i) ::std::forward<decltype(__UC_REM elem)>(__UC_REM elem)
#define __UC_declParamUsings(r, data, i, elem) using __uc_coro_params_pack_type::__UC_REM elem;
#define __UC_genParams(r, data, i, elem) BOOST_PP_COMMA_IF(i) __UCEXP elem

#define UCGenLambda(retType, capture, params, ...) \
BOOST_PP_REMOVE_PARENS(capture) (__UC_TUPLE_FOR_EACH_I(__UC_genParams,params))->::UC::Generator<::boost::remove_cv_ref_t<BOOST_PP_REMOVE_PARENS(retType)>>\
{\
	using __uc_coro_retType = ::boost::remove_cv_ref_t<BOOST_PP_REMOVE_PARENS(retType)>;\
	using __uc_coro_internal_base_gen_retType = ::UC::_Detail::_Generator<__uc_coro_retType>;\
	using __uc_coro_gen_retType = ::UC::Generator<__uc_coro_retType>;\
	struct __uc_coro_params_pack_type{__UC_TUPLE_FOR_EACH_I(__UC_declParams,params)};\
	__uc_coro_params_pack_type __uc_coro_params_pack {__UC_TUPLE_FOR_EACH_I(__UC_declFwdParams,params)};\
	struct __uc_coro_internal_gen_retType: __uc_coro_internal_base_gen_retType, __uc_coro_params_pack_type{\
		__UC_TUPLE_FOR_EACH_I(__UC_declParamUsings,params)\
		__VA_ARGS__\
		__uc_coro_internal_gen_retType(__uc_coro_params_pack_type&& pck): __uc_coro_params_pack_type(std::move(pck)){}\
		bool operator()(__uc_coro_retType& __uc_coro_ret_val){\
			if(__uc_coro_last_line == -1) { return false; }\
			switch(__uc_coro_last_line){\
			case 0:;

#define UCGenEnd\
			}\
			__uc_coro_last_line = -1;\
			return false;\
		};\
	};\
	return __uc_coro_gen_retType(::std::unique_ptr<__uc_coro_internal_base_gen_retType>(new __uc_coro_internal_gen_retType(::std::move(__uc_coro_params_pack))));\
};

#define UCGen(retType, name, params, ...) auto UCGenLambda(retType, name, params, __VA_ARGS__)

#define UCYieldReturn do {\
	__uc_coro_last_line = -1;\
	return false;\
} while (0)
#define UCYield(V)\
do {\
	__uc_coro_last_line=__LINE__;\
	__uc_coro_ret_val = (V); return true; case __LINE__:;\
} while (0)


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
{\
switch (n){BOOST_PP_SEQ_FOR_EACH_I(__UC_SWITCH_CASE, _, seq)}\
if (false){\
	cont_lbl;\
	for (;;on_cont){\
		break;\
		BOOST_PP_SEQ_FOR_EACH_I(__UC_SWITCH_BODY, _, seq)\
		break;\
	}\
}\
}

#define __UC_SWITCH_2(n, label, ...)\
__UC_SWITCH(n, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__), goto label, label: continue)

#define UCGenSwitch(n, ...)\
__UC_SWITCH_2(n, BOOST_PP_CAT(__uc_gen_sw_cont_, __LINE__), __VA_ARGS__)
