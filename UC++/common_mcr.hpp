#pragma once
#ifndef __UC__INTERNAL__COMMON_MACROS__HPP
#define __UC__INTERNAL__COMMON_MACROS__HPP
#include <boost\preprocessor\cat.hpp>
#include <boost\preprocessor\repeat.hpp>
#include <boost\preprocessor\expr_if.hpp>
#include <boost\preprocessor\seq\elem.hpp>
#include <boost\preprocessor\comma_if.hpp>
#include <boost\preprocessor\stringize.hpp>
#include <boost\preprocessor\control\if.hpp>
#include <boost\preprocessor\seq\for_each.hpp>
#include <boost\preprocessor\variadic\size.hpp>
#include <boost\preprocessor\tuple\to_list.hpp>
#include <boost\preprocessor\seq\for_each_i.hpp>
#include <boost\preprocessor\arithmetic\mod.hpp>
#include <boost\preprocessor\arithmetic\dec.hpp>
#include <boost\preprocessor\arithmetic\inc.hpp>
#include <boost\preprocessor\variadic\to_seq.hpp>
#include <boost\preprocessor\comparison\less.hpp>
#include <boost\preprocessor\list\for_each_i.hpp>
#include <boost\preprocessor\comparison\equal.hpp>
#include <boost\preprocessor\facilities\is_empty.hpp>
#include <boost\preprocessor\punctuation\remove_parens.hpp>

#define __ToStringHHHHHHHHHH(v) #v
#define __ToStringHHHHHHHHH(v) __ToStringHHHHHHHHHH(v)
#define __ToStringHHHHHHHH(v) __ToStringHHHHHHHHH(v)
#define __ToStringHHHHHHH(v) __ToStringHHHHHHHH(v)
#define __ToStringHHHHHH(v) __ToStringHHHHHHH(v)
#define __ToStringHHHHH(v) __ToStringHHHHHH(v)
#define __ToStringHHHH(v) __ToStringHHHHH(v)
#define __ToStringHHH(v) __ToStringHHHH(v)
#define __ToStringHH(v) __ToStringHHH(v)
#define __ToStringH(v) __ToStringHH(v)
#define __ToString(v) __ToStringH(v)
#endif // !__UC__INTERNAL__COMMON_MACROS__HPP

