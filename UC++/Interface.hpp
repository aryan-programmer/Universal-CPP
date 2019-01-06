#pragma once

#pragma once
#include "common_mcr.hpp"
#include <boost\preprocessor\arithmetic\inc.hpp>
#include <boost\preprocessor\comparison\less.hpp>
#include <boost/preprocessor/tuple/to_list.hpp>
#include <boost/preprocessor/list/for_each_i.hpp>

#define __UCMethodParamsHelper(r, data, i, elem) BOOST_PP_COMMA_IF(i) ::UC::PAny elem
#define __UCMethodParams(list) BOOST_PP_LIST_FOR_EACH_I(__UCMethodParamsHelper,_,list)
#define __UCWriteEachMethod(r, data, i, elem) __UCMethod(elem);
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
auto callImpl( ::UC::String fname , ::UC::GCOVect args ) -> ::UC::PAny{\
	BOOST_PP_SEQ_FOR_EACH_I(__UCWriteMethodConds, _, methSeq)\
	return callImplUpChain(fname, args);\
}\
public:\
virtual ::UC::PAny Call( ::UC::String fname , ::UC::GCOVect args ) override{return callImpl(fname, args);}

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
static ::UC::GCP<self> make(Args&&... args){return ::UC::GCP<self>(new self(std::forward<Args>(args)...));}\
__UCHasExplicitMakers_Decls(name, hasEmptyMaker, seq)\
public:\
static ::UC::PAny make_reflective( ::UC::GCOVect args ){\
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
static ::UC::GCP<self> make(){return ::UC::GCP<self>(new self());}\
static ::UC::PAny make_reflective(::UC::GCOVect args){\
	if(args.size() == 0)return ::UC::PAny(new self());\
	return nullptr;\
}
	// or
#define UC_HasNoEmptyMaker 0
#define UC_AlsoHasEmptyMaker 1
#   define UC_HasExplicitCtors(name, hasEmptyMaker,...) __UCHasExplicitMakers(name, hasEmptyMaker, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
// )

// ,
//#define Method(name, params) ::UC::PAny name(__UCMethodParams##params)
#define UC_HasMethods(...) __UCWriteMethods(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
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
	/*Inherited via ::UC::Object*/\
	virtual const char * GetTypeName( ) override{\
		static auto nm = str;\
		return nm;\
	}

#define __UCExpandNativeInheritanceHelper(r, data, i, elem) using BOOST_PP_CAT(nbase, i) = elem;
#define __UCExpandUC_InheritsNativeClasses(...) , __VA_ARGS__
#define __UCExpandUC_InheritsNoNativeClasses
#define __UCExpandNativeInheritanceAsUsingsUC_InheritsNativeClasses(...) BOOST_PP_SEQ_FOR_EACH_I(__UCExpandNativeInheritanceHelper, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define __UCExpandNativeInheritanceAsUsingsUC_InheritsNoNativeClasses

#define UCInterface(Name, StrName, Inheritance, NativeInheritance)\
struct Name: Inheritance __UCExpand##NativeInheritance{\
	__UCExpandInheritanceAsUsings##Inheritance\
	__UCExpandNativeInheritanceAsUsings##NativeInheritance\
	using self = Name;\
protected:\
	::UC::PAny callImplUpChain(::UC::String fname , ::UC::GCOVect args){__UCDefine_callUpChainWith##Inheritance}\
	__UCDefineClassRegistererAndTypename(BOOST_PP_IF(BOOST_PP_SEQ_ELEM(0, StrName), \
				__ToString(Name), \
			/*else*/\
				__ToString(BOOST_PP_SEQ_ELEM(1, StrName))))


#define UCEndInterface }

#define ME this->shared_from_this()
#define WME this->weak_from_this()

#define __UCMethod(tuple) auto BOOST_PP_TUPLE_ELEM(0, tuple) (\
BOOST_PP_EXPR_IF(\
	BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(tuple), 2), \
	__UCMethodParams(BOOST_PP_TUPLE_TO_LIST(BOOST_PP_TUPLE_ELEM(1, tuple)))\
	)\
) -> ::UC::PAny

#define UCMethod(...) __UCMethod((__VA_ARGS__))

#define __UCCTor(tuple) BOOST_PP_TUPLE_ELEM(0, tuple) (\
BOOST_PP_EXPR_IF(\
	BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(tuple), 2), \
	__UCMethodParams(BOOST_PP_TUPLE_TO_LIST(BOOST_PP_TUPLE_ELEM(1, tuple)))\
	)\
)

#define UCCTor(...) __UCCTor((__VA_ARGS__))

#define UCRegisterClass(name) name::__classRegisterer name::__classRegistererInstance{}
