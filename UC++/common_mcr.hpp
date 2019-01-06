#pragma once
#include <boost\preprocessor\cat.hpp>
#include <boost\preprocessor\expr_if.hpp>
#include <boost\preprocessor\seq\elem.hpp>
#include <boost\preprocessor\comma_if.hpp>
#include <boost\preprocessor\stringize.hpp>
#include <boost\preprocessor\variadic\size.hpp>
#include <boost\preprocessor\seq\for_each_i.hpp>
#include <boost\preprocessor\variadic\to_seq.hpp>
#include <boost\preprocessor\repeat.hpp>
#include <boost\preprocessor\comparison\equal.hpp>

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

#define __DoubleOpenBrace(...) ()

#define EXPAND(...) __VA_ARGS__
#define EMPTY()
#define DEFER(x) x EMPTY()
