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
if(fname == __ToString(name) && argsLen == BOOST_PP_VARIADIC_SIZE params){\
	return this->name(__UCWriteMethodCondsMethParams(BOOST_PP_TUPLE_SIZE(params)));\
}
#define __UCWriteMethodCondsNo(name, ...) \
if(fname == __ToString(name) && argsLen == 0){ return this->name(); }
#define __UCWriteMethodConds(r, data, i, elem)\
BOOST_PP_IIF(\
	BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(elem), 1),\
	__UCWriteMethodCondsNo, \
	__UCWriteMethodCondsMulti)elem
#define __UCWriteMethods(methSeq) \
public:\
BOOST_PP_SEQ_FOR_EACH_I(__UCWriteEachMethod, _, methSeq)\
protected: \
auto callImpl( const ::UC::NatString& fname, const ::UC::NatOVector& args ) -> ::UC::P_Any{\
	const auto& argsLen = args.size();\
	BOOST_PP_SEQ_FOR_EACH_I(__UCWriteMethodConds, _, methSeq)\
	return callImplUpChain(fname, args);\
}\
public:\
virtual ::UC::P_Any Call( const ::UC::NatString& fname, const ::UC::NatOVector& args ) override{return callImpl(fname, args);}

//#define UCInterface __RealInterface
// , Name ,

// (
#   define UC_WhereTypenameIsRealName (1)
	// or
#   define UC_WhereTypenameIs(name) (0)(name)
// )

// ,
#define UC_InheritsUCClasses(...) UC_InheritsUCClasses(__VA_ARGS__)
#define UC_InheritsUCClassesInBraces(...) UC_InheritsUCClassesInBraces(__VA_ARGS__)
#define UC_InheritsNativeClasses(...) UC_InheritsNativeClasses(__VA_ARGS__)
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
protected:\
__UCHasExplicitMakers_Decls(name, hasEmptyMaker, seq)\
public:\
static ::UC::P_Any make_reflective( const ::UC::NatOVector& args ){\
	switch (args.size()){\
		BOOST_PP_EXPR_IF(hasEmptyMaker, case 0:return ::UC::GCP<self>(new self());)\
		BOOST_PP_SEQ_FOR_EACH_I(__UCWriteMakerConds, _, seq)\
		default: throw ::UC::NoSuchConstructor_Exception(::UC::ConcatNatStrings(::UC::NatString("No constructor for type \""),SGetTypeName(),"\" that takes in ", std::to_string(args.size()), " parameters."));\
	}\
}

#   define UC_IsAbstract static ::UC::P_Any make_reflective(const ::UC::NatOVector& args){\
	throw ::UC::NoSuchConstructor_Exception(SGetTypeName() + " is an abstract class that can't be instantiated.");\
}
#   define UC_IsAbstractAndHasCtors(name, hasEmptyCtor,...) protected: __UCHasExplicitMakers_Decls(name, hasEmptyCtor, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) public: static ::UC::P_Any make_reflective(const ::UC::NatOVector& args){\
	throw ::UC::NoSuchConstructor_Exception(SGetTypeName() + " is an abstract class that can't be instantiated.");\
}
#   define UC_OnlyHasNativeCtors static ::UC::P_Any make_reflective(const ::UC::NatOVector& args){\
	throw ::UC::NoSuchConstructor_Exception(SGetTypeName() + " can't be instantiated by reflection.");\
}\
template<typename... Args>\
static ::UC::GCP<self> Make(Args&&... args){return ::UC::GCP<self>(new self(std::forward<Args>(args)...));}\
	// or
#   define UC_OnlyHasEmptyCtor \
static ::UC::GCP<self> Make(){return ::UC::GCP<self>(new self());}\
static ::UC::P_Any make_reflective(const ::UC::NatOVector& args){\
	if(args.size() == 0)return ::UC::P_Any(new self());\
	throw ::UC::NoSuchConstructor_Exception(::UC::ConcatNatStrings(::UC::NatString("No constructor for type \""),SGetTypeName(),"\" that takes in ", std::to_string(args.size()), " parameters."));\
}
	// or
#   define UC_HasNativeCtorsAndEmptyCtor \
template<typename... Args>\
static ::UC::GCP<self> Make(Args&&... args){return ::UC::GCP<self>(new self(std::forward<Args>(args)...));}\
static ::UC::P_Any make_reflective(const ::UC::NatOVector& args){\
	if(args.size() == 0)return ::UC::P_Any(new self());\
	throw ::UC::NoSuchConstructor_Exception(::UC::ConcatNatStrings(::UC::NatString("No constructor for type \""),SGetTypeName(),"\" that takes in ", std::to_string(args.size()), " parameters."));\
}
	// or
#define UC_HasNoEmptyCtor 0
#define UC_AlsoHasEmptyCtor 1
#   define UC_HasExplicitCtors(name, hasEmptyMaker,...) __UCHasExplicitMakers(name, hasEmptyMaker, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
// )

// ,
//#define Method(name, params) ::UC::P_Any name(__UCMethodParams##params)
#define UC_HasMethods(...) __UCWriteMethods(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define UC_HasNoMethods \
protected: \
forceinline auto callImpl( const ::UC::NatString& fname, const ::UC::NatOVector& args ) -> ::UC::P_Any{\
	return callImplUpChain(fname, args);\
}\
public:\
virtual ::UC::P_Any Call( const ::UC::NatString& fname, const ::UC::NatOVector& args ) override{return callImpl(fname, args);}
// ,

#define __UCExpandInheritanceHelper(r, data, i, elem) using BOOST_PP_CAT(base, i) = elem;
#define __UCExpandInheritanceAsUsingsUC_InheritsUCClasses(...) BOOST_PP_SEQ_FOR_EACH_I(__UCExpandInheritanceHelper, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define __UCExpandInheritanceInBracesHelper(r, data, i, elem) using BOOST_PP_CAT(base, i) = __UCEXP elem;
#define __UCExpandInheritanceAsUsingsUC_InheritsUCClassesInBraces(...) BOOST_PP_SEQ_FOR_EACH_I(__UCExpandInheritanceInBracesHelper, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define __UCExpandInheritanceUC_InheritsUCClasses(...) __VA_ARGS__
#define __UCDefine_callUpChainWithInheritsHelperH(baseN, resN) \
try{\
	auto resN = baseN::callImpl(fname, args);\
	if(resN) return resN;\
}\
catch(const ::UC::NoSuchFunction_Exception&){}
#define __UCDefine_callUpChainWithInheritsHelper(r, data, i, elem) \
__UCDefine_callUpChainWithInheritsHelperH(BOOST_PP_CAT(base, i), BOOST_PP_CAT(res, i))
#define __UCDefine_callUpChainWithUC_InheritsUCClasses(...) BOOST_PP_SEQ_FOR_EACH_I(__UCDefine_callUpChainWithInheritsHelper, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define __UCDefineClassRegistererAndTypename(str, Inheritance) \
protected:\
	auto callImplUpChain(const ::UC::NatString& fname, const ::UC::NatOVector& args) -> ::UC::P_Any{__UCDefine_callUpChainWith##Inheritance; throw ::UC::NoSuchFunction_Exception(::UC::ConcatNatStrings(::UC::NatString("No function for type \"" str "\" with name \""), fname, "\" that takes in ", std::to_string(args.size()), " parameters."));}\
private:\
	struct __classRegisterer{\
		__classRegisterer(){\
			::UC::Object::addConstructor(str, &self::make_reflective);\
		}\
	};\
	static __classRegisterer __classRegistererInstance;\
public:\
	static const ::UC::NatString& SGetTypeName( ){\
		static auto nm = ::UC::NatString( str );\
		return nm;\
	}\
	/*Inherited via ::UC::Object*/\
	virtual const ::UC::NatString& GetTypeName( ) const override{\
		return SGetTypeName( );\
	}

#define __UCExpandUC_InheritsUCClassesInBracesHelper(r, data, i, elem) BOOST_PP_COMMA_IF(i) __UCEXP elem
#define __UCExpandUC_InheritsUCClassesInBraces(...) BOOST_PP_SEQ_FOR_EACH_I(__UCExpandUC_InheritsUCClassesInBracesHelper, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define __UCExpandUC_InheritsUCClasses(...) __VA_ARGS__
#define __UCExpandNativeInheritanceHelper(r, data, i, elem) using BOOST_PP_CAT(nbase, i) = elem;
#define __UCExpandUC_InheritsNativeClasses(...) , __VA_ARGS__
#define __UCExpandUC_InheritsNoNativeClasses
#define __UCExpandNativeInheritanceAsUsingsUC_InheritsNativeClasses(...) BOOST_PP_SEQ_FOR_EACH_I(__UCExpandNativeInheritanceHelper, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define __UCExpandNativeInheritanceAsUsingsUC_InheritsNoNativeClasses

// UCInterface is a convenience macro that simplifies much of the complexity and boiler-plate code.
// The first parameter is the name of the UCInterface, here Empty.
// The second parameter is the type-name of the UCInterface, there are 2 valid values for this UC_WhereTypenameIsRealName which assigns the same type-name as the name, or UC_WhereTypenameIs( "?name?" ) where ?name? will become the **_registered_** name of the UCInterface used for reflection.
// The third parameter is the group of UC++ Interfaces the UCInterface inherits, pass in the UC++ Interfaces as wrapped in UC_InheritsUCClasses(?classes?). Each parameter will be expanded into a convenience *using* declaration, corresponding to the 0 based index of the inherited UC++ Interface, e.g. base0, base1, base2, base3,...
// The fourth parameter is the group of native classes the UCInterface inherits, pass in the classes as wrapped in UC_InheritsNativeClasses(?classes?). Each parameter will be expanded into a convenience *using* declaration, corresponding to the 0 based index of the inherited class, e.g. nbase0, nbase1, nbase2, nbase3,... . To inherit no native classes pass in UC_InheritsNoNativeClasses as this parameter.
// The fifth parameter is "*optional*" and can include post modifiers like final, which prevent the UC++ Interface from being inherited.
// Be careful there is a vast difference in the name and the type-name. The name represents the struct identifier of the UCInterface. The type-name is a string that is the return value of T::SGetTypeName() & t.GetTypeName(), also it is the **_registered_** name of the UCInterface, used for reflection.
#define UCInterface(Name, StrName, Inheritance, NativeInheritance, ...)\
struct Name __VA_ARGS__ : __UCExpand##Inheritance __UCExpand##NativeInheritance, ::UC::EnableGCPtrFromMe<Name>{\
	__UCExpandInheritanceAsUsings##Inheritance\
	__UCExpandNativeInheritanceAsUsings##NativeInheritance\
	using self = Name;\
	using pself = ::UC::GCP<self>;\
	using wself = ::UC::WeakPtr<self>;\
	using EGCPFM = ::UC::EnableGCPtrFromMe<Name>;\
	__UCDefineClassRegistererAndTypename(\
		BOOST_PP_IF(BOOST_PP_SEQ_ELEM(0, StrName), \
			__ToString(Name), \
		/*else*/\
			BOOST_PP_SEQ_ELEM(1, StrName) ), UC_InheritsUCClasses( Inheritance) )

#define UCEndInterface(Name) };\
using P_##Name = ::UC::GCP<Name>;\
using W_##Name = ::UC::WeakPtr<Name>

#define UCException(Name) struct Name: public ::UC::Exception{\
using base = ::UC::Exception;\
Name(::UC::NatString&& str)noexcept:base(str){}\
Name(const ::UC::NatString& str)noexcept:base(str){}\
};

#define UCBasedException(Name, Base) struct Name: public Base{\
using base = Base;\
Name(::UC::NatString&& str)noexcept:base(str){}\
Name(const ::UC::NatString& str)noexcept:base(str){}\
};

#define ME EGCPFM::GCFromMe()
#define WME EGCPFM::WeakFromMe()

#define __UCMethod(tuple) auto BOOST_PP_TUPLE_ELEM(0, tuple) (__UCGetGetMethodParamsIfExists(tuple)) -> ::UC::P_Any
#define __UCMTPMethod(tuple) BOOST_PP_TUPLE_ELEM(0, tuple) (__UCGetGetMethodParamsIfExists(tuple)) -> ::UC::P_Any

#define UCMethod(...) __UCMethod((__VA_ARGS__))
#define __UCEXP(...) __VA_ARGS__
#define UC_MTPMethod(ClsT,...) auto __UCEXP ClsT::__UCMTPMethod((__VA_ARGS__))

#define __UCCTor(tuple) BOOST_PP_TUPLE_ELEM(0, tuple) (__UCGetGetMethodParamsIfExists(tuple))

#define UCCtor(...) __UCCTor((__VA_ARGS__))

#define UCRegister(name) name::__classRegisterer name::__classRegistererInstance{}


#define UCAsNotNull(v) ::UC::asNotNull(v, "Variable: \"" __ToString(v) "\" is nullptr." )

#define UCCast(T, v) ::UC::ObjCastThrowing<T>(v, "Variable: \"" __ToString(v) "\" is not of the expected type:" __ToString(T) ".")

#define UCAsInt16(v) ::UC::asInt16(v, "Variable: \"" __ToString(v) "\" doesn't hold a value that can be converted to a int16_t" )
#define UCAsInt32(v) ::UC::asInt32(v, "Variable: \"" __ToString(v) "\" doesn't hold a value that can be converted to a int32_t" )
#define UCAsInt64(v) ::UC::asInt64(v, "Variable: \"" __ToString(v) "\" doesn't hold a value that can be converted to a int64_t" )
#define UCAsUInt16(v) ::UC::asUInt16(v, "Variable: \"" __ToString(v) "\" doesn't hold a value that can be converted to a uint16_t" )
#define UCAsUInt32(v) ::UC::asUInt32(v, "Variable: \"" __ToString(v) "\" doesn't hold a value that can be converted to a uint32_t" )
#define UCAsUInt64(v) ::UC::asUInt64(v, "Variable: \"" __ToString(v) "\" doesn't hold a value that can be converted to a uint64_t" )
#define UCAsByte(v) ::UC::asByte(v, "Variable: \"" __ToString(v) "\" doesn't hold a value that can be converted to a UC::byte" )
#define UCAsSByte(v) ::UC::asSByte(v, "Variable: \"" __ToString(v) "\" doesn't hold a value that can be converted to a UC::sbyte" )
#define UCAsFloat(v) ::UC::asFloat(v, "Variable: \"" __ToString(v) "\" doesn't hold a value that can be converted to a float" )
#define UCAsDouble(v) ::UC::asDouble(v, "Variable: \"" __ToString(v) "\" doesn't hold a value that can be converted to a double" )

#define __UCExpandUCTemplateHelper(r, data, i, elem) BOOST_PP_COMMA_IF(i) typename elem
#define __UCExpandAsTemplate(tup) template<BOOST_PP_SEQ_FOR_EACH_I(__UCExpandUCTemplateHelper, _, BOOST_PP_TUPLE_TO_SEQ(tup))>
#define __UCExpandAsCondensedParametersHelper(r, data, i, elem) BOOST_PP_COMMA_IF(i) elem
#define __UCExpandAsCondensedParameters(...) BOOST_PP_SEQ_FOR_EACH_I(__UCExpandAsCondensedParametersHelper, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define __UCExpandTypesToTypenamesHelper(r, data, i, elem) BOOST_PP_COMMA_IF(i) ::UC::SGetTypeName<elem>()
#define __UCExpandTypesToTypenames(tup) BOOST_PP_SEQ_FOR_EACH_I(__UCExpandTypesToTypenamesHelper, _, BOOST_PP_TUPLE_TO_SEQ(tup))
#define __UCExpandTypesToComparatorsToObjectHelper(r, data, i, elem) BOOST_PP_EXPR_IF(i,&&) ::std::is_same<elem, ::UC::Object>::value
#define __UCExpandTypesToComparatorsToObject(tup) BOOST_PP_SEQ_FOR_EACH_I(__UCExpandTypesToComparatorsToObjectHelper, _, BOOST_PP_TUPLE_TO_SEQ(tup))

#define __UCDefineTemplateTypename(str, Inheritance, TypeParams) \
protected:\
	auto callImplUpChain(const ::UC::NatString& fname, const ::UC::NatOVector& args) -> ::UC::P_Any{__UCDefine_callUpChainWith##Inheritance; throw ::UC::NoSuchFunction_Exception(::UC::ConcatNatStrings(::UC::NatString("No function for type \""), SGetTypeName(), "\" with name \"", fname, "\" that takes in ", std::to_string(args.size()), " parameters."));}\
public:\
	static const ::UC::NatString& SGetSimpleTypeName( ){\
		static auto nm = ::UC::NatString( str );\
		return nm;\
	}\
	static const ::UC::NatString& SGetTypeName( ){\
		static auto nm = ::UC::ConcatNatStrings(::UC::NatString( str ), __UCExpandTypesToTypenames(TypeParams));\
		return nm;\
	}\
	/*Inherited via ::UC::Object*/\
	virtual const ::UC::NatString& GetTypeName( ) const override{\
		return SGetTypeName( );\
	}

#define __UCExpandAsOnlyObjectHelper(r, data, i, elem) BOOST_PP_COMMA_IF(i) ::UC::Object
#define __UCExpandAsOnlyObject(tup) BOOST_PP_SEQ_FOR_EACH_I(__UCExpandAsOnlyObjectHelper, _, BOOST_PP_TUPLE_TO_SEQ(tup))

/// <summary>
/// This macro defines a template interface that inherits UC::Object, it simplifies much of the boiler plate code.
/// Look at the example files to see how to use it.
/// </summary>
#define UCTemplateInterface(Name, TypeParams, StrName, Inheritance, NativeInheritance)\
__UCExpandAsTemplate(TypeParams) struct Name : __UCExpand##Inheritance __UCExpand##NativeInheritance, ::UC::EnableGCPtrFromMe<Name<__UCExpandAsCondensedParameters TypeParams>>{\
	__UCExpandInheritanceAsUsings##Inheritance\
	__UCExpandNativeInheritanceAsUsings##NativeInheritance\
	using self = Name<__UCExpandAsCondensedParameters TypeParams>;\
	using pself = ::UC::GCP<self>;\
	using wself = ::UC::WeakPtr<self>;\
	using EGCPFM = ::UC::EnableGCPtrFromMe<self>;\
	__UCDefineTemplateTypename(\
		BOOST_PP_IF(BOOST_PP_SEQ_ELEM(0, StrName), \
			__ToString(Name), \
		/*else*/\
			BOOST_PP_SEQ_ELEM(1, StrName) ), UC_InheritsUCClasses( Inheritance ), TypeParams )

#define UCEndTemplateInterface(Name, TypeParams) };\
__UCExpandAsTemplate(TypeParams) using P_##Name = ::UC::GCP<Name<__UCExpandAsCondensedParameters TypeParams>>;\
__UCExpandAsTemplate(TypeParams) using W_##Name = ::UC::WeakPtr<Name<__UCExpandAsCondensedParameters TypeParams>>;\


#define __UCDefineTemplateTypenameWithPack(str, Inheritance, TypeParams, PackParam) \
protected:\
	auto callImplUpChain(const ::UC::NatString& fname, const ::UC::NatOVector& args) -> ::UC::P_Any{__UCDefine_callUpChainWith##Inheritance; throw ::UC::NoSuchFunction_Exception(::UC::ConcatNatStrings(::UC::NatString("No function for type \""), SGetTypeName(), "\" with name \"", fname, "\" that takes in ", std::to_string(args.size()), " parameters."));}\
public:\
	static const ::UC::NatString& SGetSimpleTypeName( ){\
		static auto nm = ::UC::NatString( str );\
		return nm;\
	}\
	static const ::UC::NatString& SGetTypeName( ){\
		static auto nm = ::UC::ConcatNatStrings(::UC::NatString( str ), __UCExpandTypesToTypenames(TypeParams), ::UC::SGetTypeName<PackParam>()...);\
		return nm;\
	}\
	/*Inherited via ::UC::Object*/\
	virtual const ::UC::NatString& GetTypeName( ) const override{\
		return SGetTypeName( );\
	}

#define __UCExpandAsTemplateParamPack(tup, PackParamName) template<BOOST_PP_SEQ_FOR_EACH_I(__UCExpandUCTemplateHelper, _, BOOST_PP_TUPLE_TO_SEQ(tup)), typename... PackParamName>

/// <summary>
/// This macro defines a template interface that has a template parameter pack. The template interface defined inherits UC::Object, it simplifies much of the boiler plate code.
/// Look at the example files to see how to use it.
/// </summary>
#define UCTemplateInterfaceWithPack(Name, TypeParams, PackParamName, StrName, Inheritance, NativeInheritance, ...)\
__UCExpandAsTemplateParamPack(TypeParams, PackParamName) struct Name __VA_ARGS__ :  __UCExpand##Inheritance __UCExpand##NativeInheritance, ::UC::EnableGCPtrFromMe<Name<__UCExpandAsCondensedParameters TypeParams, PackParamName...>>{\
	__UCExpandInheritanceAsUsings##Inheritance\
	__UCExpandNativeInheritanceAsUsings##NativeInheritance\
	using self = Name<__UCExpandAsCondensedParameters TypeParams, PackParamName...>;\
	using pself = ::UC::GCP<self>;\
	using wself = ::UC::WeakPtr<self>;\
	using EGCPFM = ::UC::EnableGCPtrFromMe<self>;\
	__UCDefineTemplateTypenameWithPack(\
		BOOST_PP_IF(BOOST_PP_SEQ_ELEM(0, StrName), \
			__ToString(Name), \
		/*else*/\
			BOOST_PP_SEQ_ELEM(1, StrName) ), UC_InheritsUCClasses( Inheritance ), TypeParams, PackParamName )

#define UCEndTemplateInterfaceWithPack(Name, TypeParams, PackParamName) };\
__UCExpandAsTemplateParamPack(TypeParams, PackParamName) using P_##Name = ::UC::GCP<Name<__UCExpandAsCondensedParameters TypeParams, PackParamName...>>;\
__UCExpandAsTemplateParamPack(TypeParams, PackParamName) using W_##Name = ::UC::WeakPtr<Name<__UCExpandAsCondensedParameters TypeParams, PackParamName...>>;\

#define __UCDefineTemplateTypenameWithOnlyPack(str, Inheritance, PackParam) \
protected:\
	auto callImplUpChain(const ::UC::NatString& fname, const ::UC::NatOVector& args) -> ::UC::P_Any{__UCDefine_callUpChainWith##Inheritance; throw ::UC::NoSuchFunction_Exception(::UC::ConcatNatStrings(::UC::NatString("No function for type \""), SGetTypeName(), "\" with name \"", fname, "\" that takes in ", std::to_string(args.size()), " parameters."));}\
public:\
	static const ::UC::NatString& SGetSimpleTypeName( ){\
		static auto nm = ::UC::NatString( str );\
		return nm;\
	}\
	static const ::UC::NatString& SGetTypeName( ){\
		static auto nm = ::UC::ConcatNatStrings(::UC::NatString( str ), ::UC::SGetTypeName<PackParam>()...);\
		return nm;\
	}\
	/*Inherited via ::UC::Object*/\
	virtual const ::UC::NatString& GetTypeName( ) const override{\
		return SGetTypeName( );\
	}

/// <summary>
/// This macro defines a template interface that has a template parameter pack. The template interface defined inherits UC::Object, it simplifies much of the boiler plate code.
/// Look at the example files to see how to use it.
/// </summary>
#define UCTemplateInterfaceWithOnlyPack(Name, PackParam, StrName, Inheritance, NativeInheritance, ...)\
template<typename... PackParam> struct Name __VA_ARGS__ :  __UCExpand##Inheritance __UCExpand##NativeInheritance, ::UC::EnableGCPtrFromMe<Name<PackParam...>>{\
	__UCExpandInheritanceAsUsings##Inheritance\
	__UCExpandNativeInheritanceAsUsings##NativeInheritance\
	using self = Name<PackParam...>;\
	using pself = ::UC::GCP<self>;\
	using wself = ::UC::WeakPtr<self>;\
	using EGCPFM = ::UC::EnableGCPtrFromMe<self>;\
	__UCDefineTemplateTypenameWithOnlyPack(\
		BOOST_PP_IF(BOOST_PP_SEQ_ELEM(0, StrName), \
			__ToString(Name), \
		/*else*/\
			BOOST_PP_SEQ_ELEM(1, StrName) ), UC_InheritsUCClasses( Inheritance ), PackParam )

#define UCEndTemplateInterfaceWithOnlyPack(Name, PackParamName) };\
template<typename... PackParamName> using P_##Name = ::UC::GCP<Name<PackParamName...>>;\
template<typename... PackParamName> using W_##Name = ::UC::WeakPtr<Name<PackParamName...>>;\


#define UCC(var, fname, ...) var->Call(fname, __VA_ARGS__)

#define UCRegisterTemplate(name, ...) struct __##name##_class_registerer_class{\
	__##name##_class_registerer_class(){::UC::Object::addConstructor(name __VA_ARGS__::SGetSimpleTypeName(), &(name __VA_ARGS__::make_reflective));}\
}; static __##name##_class_registerer_class __inst__##name##_class_registerer_class_instance{};
#endif // !__UC__INTERFACE_MACROS__HPP__
