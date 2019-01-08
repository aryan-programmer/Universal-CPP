#pragma once
#ifndef __UC__INTERFACE_MACROS__HPP__
#define __UC__INTERFACE_MACROS__HPP__
#include "common_mcr.hpp"
#include <boost\preprocessor\arithmetic\inc.hpp>
#include <boost\preprocessor\comparison\less.hpp>
#include <boost\preprocessor\tuple\to_list.hpp>
#include <boost\preprocessor\list\for_each_i.hpp>

#define __UCGetGetMethodParams(tup) __UCMethodParams(BOOST_PP_TUPLE_TO_LIST(BOOST_PP_TUPLE_ELEM(1, tup)))
#define __UCNothing(...)
#define __UCGetGetMethodParamsIfExists(tup) \
BOOST_PP_IIF(BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(tup), 2),\
__UCGetGetMethodParams,\
__UCNothing)(tup)

#define __UCMethodParamsHelper(r, data, i, elem) BOOST_PP_COMMA_IF(i) ::UC::P_Any elem
#define __UCMethodParams(list) BOOST_PP_LIST_FOR_EACH_I(__UCMethodParamsHelper,_,list)
#define __UCWriteEachMethod(r, data, i, elem) virtual __UCMethod(elem);
#define __UCWriteMethodCondsMethParamsHelper(z, i, data) BOOST_PP_COMMA_IF(i) args[i]
#define __UCWriteMethodCondsMethParams(sz) BOOST_PP_REPEAT(sz, __UCWriteMethodCondsMethParamsHelper, _)
#define __UCWriteMethodCondsMulti(name, params, ...) \
if(fname == __ToString(name) && args.size() == BOOST_PP_VARIADIC_SIZE params){\
	return name(__UCWriteMethodCondsMethParams(BOOST_PP_TUPLE_SIZE(params)));\
}
#define __UCWriteMethodCondsNo(name, ...) \
if(fname == __ToString(name) && args.size() == 0){ return name(); }
#define __UCWriteMethodConds(r, data, i, elem)\
BOOST_PP_IIF(\
	BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(elem), 1),\
	__UCWriteMethodCondsNo, \
	__UCWriteMethodCondsMulti)elem
#define __UCWriteMethods(methSeq) \
public:\
BOOST_PP_SEQ_FOR_EACH_I(__UCWriteEachMethod, _, methSeq)\
protected: \
auto callImpl( const ::UC::NatString& fname, const ::UC::NatODeque& args ) -> ::UC::P_Any{\
	BOOST_PP_SEQ_FOR_EACH_I(__UCWriteMethodConds, _, methSeq)\
	return callImplUpChain(fname, args);\
}\
public:\
virtual ::UC::P_Any Call( const ::UC::NatString& fname, const ::UC::NatODeque& args ) override{return callImpl(fname, args);}

//#define UCInterface __RealInterface
// , Name ,

// (
#   define UC_WhereTypenameIsRealName (1)
	// or
#   define UC_WhereTypenameIs(name) (0)(BOOST_STRINGIZE(name))
// )

// ,
#define UC_InheritsUCClasses(...) __VA_ARGS__
#define UC_InheritsNativeClasses(...) __VA_ARGS__
#define UC_InheritsNoNativeClasses
//#undef UC_InheritsUCClasses
// , (...Inheritance...), 

// (
#define __UCWriteEachMaker(r, data, i, elem) data(__UCMethodParams(BOOST_PP_TUPLE_TO_LIST(elem)));
#define __UCWriteMakerConds(r, data, i, params) \
case BOOST_PP_VARIADIC_SIZE params:\
	return ::UC::GCP<self>(new self(__UCWriteMethodCondsMethParams(BOOST_PP_TUPLE_SIZE(params))));
#define __UCHasExplicitMakers_Decls(name, hasEmptyMaker, seq)\
protected:\
BOOST_PP_EXPR_IF(hasEmptyMaker, name();)\
BOOST_PP_SEQ_FOR_EACH_I(__UCWriteEachMaker, name, seq)
#define __UCHasExplicitMakers(name, hasEmptyMaker, seq) \
template<typename... Args>\
static ::UC::GCP<self> Make(Args&&... args){return ::UC::GCP<self>(new self(std::forward<Args>(args)...));}\
__UCHasExplicitMakers_Decls(name, hasEmptyMaker, seq)\
public:\
static ::UC::P_Any make_reflective( const ::UC::NatODeque& args ){\
	switch (args.size()){\
		BOOST_PP_EXPR_IF(hasEmptyMaker, case 0:return ::UC::GCP<self>(new self());)\
		BOOST_PP_SEQ_FOR_EACH_I(__UCWriteMakerConds, _, seq)\
		default: return nullptr;\
	}\
}\
public:

#   define UC_IsAbstract(name)
#   define UC_IsAbstractAndHasCtors(name, hasEmptyMaker,...) __UCHasExplicitMakers(name, hasEmptyMaker, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
	// or
#   define UC_OnlyHasEmptyCtor \
static ::UC::GCP<self> Make(){return ::UC::GCP<self>(new self());}\
static ::UC::P_Any make_reflective(const ::UC::NatODeque& args){\
	if(args.size() == 0)return ::UC::P_Any(new self());\
	return nullptr;\
}
	// or
#   define UC_HasNativeCtorsAndEmptyCtor \
template<typename... Args>\
static ::UC::GCP<self> Make(Args&&... args){return ::UC::GCP<self>(new self(std::forward<Args>(args)...));}\
static ::UC::P_Any make_reflective(const ::UC::NatODeque& args){\
	if(args.size() == 0)return ::UC::P_Any(new self());\
	return nullptr;\
}
	// or
#define UC_HasNoEmptyMaker 0
#define UC_AlsoHasEmptyMaker 1
#   define UC_HasExplicitCtors(name, hasEmptyMaker,...) __UCHasExplicitMakers(name, hasEmptyMaker, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
// )

// ,
//#define Method(name, params) ::UC::P_Any name(__UCMethodParams##params)
#define UC_HasMethods(...) __UCWriteMethods(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define UC_HasNoMethods \
protected: \
forceinline auto callImpl( const ::UC::NatString& fname, const ::UC::NatODeque& args ) -> ::UC::P_Any{\
	return callImplUpChain(fname, args);\
}\
public:\
virtual ::UC::P_Any Call( const ::UC::NatString& fname, const ::UC::NatODeque& args ) override{return callImpl(fname, args);}
// ,

#define __UCExpandInheritanceHelper(r, data, i, elem) using BOOST_PP_CAT(base, i) = elem;
#define __UCExpandInheritanceAsUsingsUC_InheritsUCClasses(...) BOOST_PP_SEQ_FOR_EACH_I(__UCExpandInheritanceHelper, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define __UCExpandInheritanceUC_InheritsUCClasses(...) __VA_ARGS__
#define __UCDefine_callUpChainWithInheritsHelperH(baseN, resN) \
auto resN = baseN::callImpl(fname, args);\
if(resN) return resN;
#define __UCDefine_callUpChainWithInheritsHelper(r, data, i, elem) \
__UCDefine_callUpChainWithInheritsHelperH(BOOST_PP_CAT(base, i), BOOST_PP_CAT(res, i))
#define __UCDefine_callUpChainWithUC_InheritsUCClasses(...) BOOST_PP_SEQ_FOR_EACH_I(__UCDefine_callUpChainWithInheritsHelper, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define __UCDefineClassRegistererAndTypename(str) \
private:\
	struct __classRegisterer{\
		__classRegisterer(){\
			::UC::Object::addConstructor(str, &self::make_reflective);\
		}\
	};\
	static __classRegisterer __classRegistererInstance;\
public:\
	static const char * SGetTypeName( ){\
		static auto nm = str;\
		return nm;\
	}\
	/*Inherited via ::UC::Object*/\
	virtual const char * GetTypeName( ) override{\
		return SGetTypeName( );\
	}

#define __UCExpandNativeInheritanceHelper(r, data, i, elem) using BOOST_PP_CAT(nbase, i) = elem;
#define __UCExpandUC_InheritsNativeClasses(...) , __VA_ARGS__
#define __UCExpandUC_InheritsNoNativeClasses
#define __UCExpandNativeInheritanceAsUsingsUC_InheritsNativeClasses(...) BOOST_PP_SEQ_FOR_EACH_I(__UCExpandNativeInheritanceHelper, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define __UCExpandNativeInheritanceAsUsingsUC_InheritsNoNativeClasses

/// <summary>
/// This macro defines a class that inherits UC::Object, it simplifies much of the boiler plate code.
/// Look at the example files to see how to use it.
/// </summary>
#define UCInterface(Name, StrName, Inheritance, NativeInheritance, ...)\
struct Name;\
using P_##Name = ::UC::GCP<Name>;\
using W_##Name = ::UC::WeakPtr<Name>;\
struct Name __VA_ARGS__ : Inheritance __UCExpand##NativeInheritance, ::UC::EnableGCPtrFromMe<Name>{\
	__UCExpandInheritanceAsUsings##Inheritance\
	__UCExpandNativeInheritanceAsUsings##NativeInheritance\
	using self = Name;\
	using pself = UC::GCP<self>;\
	using EGCPFM = ::UC::EnableGCPtrFromMe<Name>;\
protected:\
	::UC::P_Any callImplUpChain(const ::UC::NatString& fname, const ::UC::NatODeque& args){__UCDefine_callUpChainWith##Inheritance return nullptr;}\
	__UCDefineClassRegistererAndTypename(\
		BOOST_PP_IF(BOOST_PP_SEQ_ELEM(0, StrName), \
			__ToString(Name), \
		/*else*/\
			__ToString(BOOST_PP_SEQ_ELEM(1, StrName))))


#define UCEndInterface }

#define UCException(Name) struct Name: public ::UC::Exception{\
using base = ::UC::Exception;\
Name(::UC::NatString&& str)noexcept:base(str){}\
Name(const ::UC::NatString& str)noexcept:base(str){}\
};

#define ME EGCPFM::GCFromMe()
#define WME EGCPFM::WeakFromMe()

#define __UCMethod(tuple) auto BOOST_PP_TUPLE_ELEM(0, tuple) (__UCGetGetMethodParamsIfExists(tuple)) -> ::UC::P_Any

#define UCMethod(...) __UCMethod((__VA_ARGS__))

#define __UCCTor(tuple) BOOST_PP_TUPLE_ELEM(0, tuple) (__UCGetGetMethodParamsIfExists(tuple))

#define UCCtor(...) __UCCTor((__VA_ARGS__))

#define UCRegisterClass(name) name::__classRegisterer name::__classRegistererInstance{}


#define UCAssertNotNull(v) if(v==nullptr)throw PreNullPointerException( "Variable: " __ToString(v) " is nullptr." )

#define UCCast(T, v) ::UC::ObjCastThrowing<T>(v, "Variable: " __ToString(v) " is not of the expected type:" __ToString(T) ".")

#define UCAsInt16(v) ::UC::asInt16(v, "Variable: " __ToString(v) " doesn't hold a value that can be converted to a int16_t" )
#define UCAsInt32(v) ::UC::asInt32(v, "Variable: " __ToString(v) " doesn't hold a value that can be converted to a int32_t" )
#define UCAsInt64(v) ::UC::asInt64(v, "Variable: " __ToString(v) " doesn't hold a value that can be converted to a int64_t" )
#define UCAsUInt16(v) ::UC::asUInt16(v, "Variable: " __ToString(v) " doesn't hold a value that can be converted to a uint16_t" )
#define UCAsUInt32(v) ::UC::asUInt32(v, "Variable: " __ToString(v) " doesn't hold a value that can be converted to a uint32_t" )
#define UCAsUInt64(v) ::UC::asUInt64(v, "Variable: " __ToString(v) " doesn't hold a value that can be converted to a uint64_t" )
#define UCAsByte(v) ::UC::asByte(v, "Variable: " __ToString(v) " doesn't hold a value that can be converted to a UC::byte" )
#define UCAsSByte(v) ::UC::asSByte(v, "Variable: " __ToString(v) " doesn't hold a value that can be converted to a UC::sbyte" )
#define UCAsFloat(v) ::UC::asFloat(v, "Variable: " __ToString(v) " doesn't hold a value that can be converted to a float" )
#define UCAsDouble(v) ::UC::asDouble(v, "Variable: " __ToString(v) " doesn't hold a value that can be converted to a double" )
#endif // !__UC__INTERFACE_MACROS__HPP__
