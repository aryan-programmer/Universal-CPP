#pragma once
#ifndef __UC__BYTE_SBYTE__HPP__
#define __UC__BYTE_SBYTE__HPP__

#include "stdafx.h"

#if _HAS_CXX17
#define ___NODISCARD___ [[nodiscard]]
#else
#define ___NODISCARD___
#endif /* _HAS_NODISCARD */

namespace UC
{
	// ENUM CLASS sbyte
	enum class sbyte : char { };

	template<class _IntType ,
		std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
		___NODISCARD___ forceinline constexpr sbyte operator<<( const sbyte _Arg , const _IntType _Shift ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >( static_cast< int >( _Arg ) << _Shift ) ) );
	}

	template<class _IntType ,
		std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
		___NODISCARD___ forceinline constexpr sbyte operator>>( const sbyte _Arg , const _IntType _Shift ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >( static_cast< int >( _Arg ) >> _Shift ) ) );
	}

	___NODISCARD___ forceinline constexpr sbyte operator|( const sbyte _Left , const sbyte _Right ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >(
			static_cast< int >( _Left ) | static_cast< int >( _Right ) ) ) );
	}

	___NODISCARD___ forceinline constexpr sbyte operator&( const sbyte _Left , const sbyte _Right ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >(
			static_cast< int >( _Left ) & static_cast< int >( _Right ) ) ) );
	}

	___NODISCARD___ forceinline constexpr sbyte operator^( const sbyte _Left , const sbyte _Right ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >(
			static_cast< int >( _Left ) ^ static_cast< int >( _Right ) ) ) );
	}

	___NODISCARD___ forceinline constexpr sbyte operator~( const sbyte _Arg ) noexcept
	{	// bitwise NOT, every static_cast is intentional
		return ( static_cast< sbyte >( static_cast< char >( ~static_cast< int >( _Arg ) ) ) );
	}

	template<class _IntType , std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
	forceinline constexpr sbyte& operator<<=( sbyte& _Arg , const _IntType _Shift ) noexcept { return ( _Arg = _Arg << _Shift ); }

	template<class _IntType , std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
	forceinline constexpr sbyte& operator>>=( sbyte& _Arg , const _IntType _Shift ) noexcept { return ( _Arg = _Arg >> _Shift ); }

	forceinline constexpr sbyte& operator|=( sbyte& _Left , const sbyte _Right ) noexcept { return ( _Left = _Left | _Right ); }

	forceinline constexpr sbyte& operator&=( sbyte& _Left , const sbyte _Right ) noexcept { return ( _Left = _Left & _Right ); }

	forceinline constexpr sbyte& operator^=( sbyte& _Left , const sbyte _Right ) noexcept { return ( _Left = _Left ^ _Right ); }

	template<class _IntType ,
		std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
		___NODISCARD___ forceinline constexpr sbyte operator+( const sbyte _Arg , const _IntType _Shift ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >( static_cast< int >( _Arg ) + _Shift ) ) );
	}

	template<class _IntType ,
		std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
		___NODISCARD___ forceinline constexpr sbyte operator-( const sbyte _Arg , const _IntType _Shift ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >( static_cast< int >( _Arg ) - _Shift ) ) );
	}

	___NODISCARD___ forceinline constexpr sbyte operator/( const sbyte _Left , const sbyte _Right ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >(
			static_cast< int >( _Left ) / static_cast< int >( _Right ) ) ) );
	}

	___NODISCARD___ forceinline constexpr sbyte operator*( const sbyte _Left , const sbyte _Right ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >(
			static_cast< int >( _Left ) * static_cast< int >( _Right ) ) ) );
	}

	___NODISCARD___ forceinline constexpr sbyte operator%( const sbyte _Left , const sbyte _Right ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >(
			static_cast< int >( _Left ) % static_cast< int >( _Right ) ) ) );
	}

	___NODISCARD___ forceinline constexpr sbyte operator-( const sbyte _Arg ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >( -static_cast< int >( _Arg ) ) ) );
	}

	___NODISCARD___ forceinline constexpr sbyte operator+( const sbyte _Arg ) noexcept
	{
		return ( static_cast< sbyte >( static_cast< char >( +static_cast< int >( _Arg ) ) ) );
	}

	template<class _IntType , std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
	forceinline constexpr sbyte& operator+=( sbyte& _Arg , const _IntType _Shift ) noexcept { return ( _Arg = _Arg + _Shift ); }

	template<class _IntType , std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
	forceinline constexpr sbyte& operator-=( sbyte& _Arg , const _IntType _Shift ) noexcept { return ( _Arg = _Arg - _Shift ); }

	forceinline constexpr sbyte& operator/=( sbyte& _Left , const sbyte _Right ) noexcept { return ( _Left = _Left / _Right ); }

	forceinline constexpr sbyte& operator*=( sbyte& _Left , const sbyte _Right ) noexcept { return ( _Left = _Left * _Right ); }

	forceinline constexpr sbyte& operator%=( sbyte& _Left , const sbyte _Right ) noexcept { return ( _Left = _Left % _Right ); }


	// ENUM CLASS byte
	enum class byte : unsigned char { };

	template<class _IntType ,
		std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
		_NODISCARD constexpr byte operator<<( const byte _Arg , const _IntType _Shift ) noexcept
	{	// bitwise LEFT SHIFT, every static_cast is intentional
		return ( static_cast< byte >( static_cast< unsigned char >( static_cast< unsigned int >( _Arg ) << _Shift ) ) );
	}

	template<class _IntType ,
		std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
		_NODISCARD constexpr byte operator>>( const byte _Arg , const _IntType _Shift ) noexcept
	{	// bitwise RIGHT SHIFT, every static_cast is intentional
		return ( static_cast< byte >( static_cast< unsigned char >( static_cast< unsigned int >( _Arg ) >> _Shift ) ) );
	}

	_NODISCARD constexpr byte operator|( const byte _Left , const byte _Right ) noexcept
	{	// bitwise OR, every static_cast is intentional
		return ( static_cast< byte >( static_cast< unsigned char >(
			static_cast< unsigned int >( _Left ) | static_cast< unsigned int >( _Right ) ) ) );
	}

	_NODISCARD constexpr byte operator&( const byte _Left , const byte _Right ) noexcept
	{	// bitwise AND, every static_cast is intentional
		return ( static_cast< byte >( static_cast< unsigned char >(
			static_cast< unsigned int >( _Left ) & static_cast< unsigned int >( _Right ) ) ) );
	}

	_NODISCARD constexpr byte operator^( const byte _Left , const byte _Right ) noexcept
	{	// bitwise XOR, every static_cast is intentional
		return ( static_cast< byte >( static_cast< unsigned char >(
			static_cast< unsigned int >( _Left ) ^ static_cast< unsigned int >( _Right ) ) ) );
	}

	_NODISCARD constexpr byte operator~( const byte _Arg ) noexcept
	{	// bitwise NOT, every static_cast is intentional
		return ( static_cast< byte >( static_cast< unsigned char >( ~static_cast< unsigned int >( _Arg ) ) ) );
	}

	template<class _IntType ,
		std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
		forceinline constexpr byte& operator<<=( byte& _Arg , const _IntType _Shift ) noexcept
	{	// bitwise LEFT SHIFT
		return ( _Arg = _Arg << _Shift );
	}

	template<class _IntType ,
		std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
		forceinline constexpr byte& operator>>=( byte& _Arg , const _IntType _Shift ) noexcept
	{	// bitwise RIGHT SHIFT
		return ( _Arg = _Arg >> _Shift );
	}

	forceinline constexpr byte& operator|=( byte& _Left , const byte _Right ) noexcept
	{	// bitwise OR
		return ( _Left = _Left | _Right );
	}

	forceinline constexpr byte& operator&=( byte& _Left , const byte _Right ) noexcept
	{	// bitwise AND
		return ( _Left = _Left & _Right );
	}

	forceinline constexpr byte& operator^=( byte& _Left , const byte _Right ) noexcept
	{	// bitwise XOR
		return ( _Left = _Left ^ _Right );
	}

	template<class _IntType ,
		std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
		_NODISCARD constexpr _IntType to_integer( const byte _Arg ) noexcept
	{	// convert byte to integer
		return ( static_cast< _IntType >( _Arg ) );
	}

	template<class _IntType ,
		std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
		___NODISCARD___ forceinline constexpr byte operator+( const byte _Arg , const _IntType _Shift ) noexcept
	{
		return ( static_cast< byte >( static_cast< char >( static_cast< int >( _Arg ) + _Shift ) ) );
	}

	template<class _IntType ,
		std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
		___NODISCARD___ forceinline constexpr byte operator-( const byte _Arg , const _IntType _Shift ) noexcept
	{
		return ( static_cast< byte >( static_cast< char >( static_cast< int >( _Arg ) - _Shift ) ) );
	}

	___NODISCARD___ forceinline constexpr byte operator/( const byte _Left , const byte _Right ) noexcept
	{
		return ( static_cast< byte >( static_cast< char >(
			static_cast< int >( _Left ) / static_cast< int >( _Right ) ) ) );
	}

	___NODISCARD___ forceinline constexpr byte operator*( const byte _Left , const byte _Right ) noexcept
	{
		return ( static_cast< byte >( static_cast< char >(
			static_cast< int >( _Left ) * static_cast< int >( _Right ) ) ) );
	}

	___NODISCARD___ forceinline constexpr byte operator%( const byte _Left , const byte _Right ) noexcept
	{
		return ( static_cast< byte >( static_cast< char >(
			static_cast< int >( _Left ) % static_cast< int >( _Right ) ) ) );
	}

	___NODISCARD___ forceinline constexpr byte operator-( const byte _Arg ) noexcept
	{
		return ( static_cast< byte >( static_cast< char >( -static_cast< int >( _Arg ) ) ) );
	}

	___NODISCARD___ forceinline constexpr byte operator+( const byte _Arg ) noexcept
	{
		return ( static_cast< byte >( static_cast< char >( +static_cast< int >( _Arg ) ) ) );
	}

	template<class _IntType , std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
	forceinline constexpr byte& operator+=( byte& _Arg , const _IntType _Shift ) noexcept { return ( _Arg = _Arg + _Shift ); }

	template<class _IntType , std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
	forceinline constexpr byte& operator-=( byte& _Arg , const _IntType _Shift ) noexcept { return ( _Arg = _Arg - _Shift ); }

	forceinline constexpr byte& operator/=( byte& _Left , const byte _Right ) noexcept { return ( _Left = _Left / _Right ); }

	forceinline constexpr byte& operator*=( byte& _Left , const byte _Right ) noexcept { return ( _Left = _Left * _Right ); }

	forceinline constexpr byte& operator%=( byte& _Left , const byte _Right ) noexcept { return ( _Left = _Left % _Right ); }

	template<class _IntType , std::enable_if_t<std::is_integral_v<_IntType> , int> = 0>
	___NODISCARD___ forceinline constexpr _IntType to_integer( const sbyte _Arg ) noexcept { return ( static_cast< _IntType >( _Arg ) ); }

	___NODISCARD___ forceinline std::string to_string( byte b ) { return std::to_string( static_cast< int16_t >( b ) ); }
	___NODISCARD___ forceinline std::string to_string( sbyte b ) { return std::to_string( static_cast< int16_t >( b ) ); }
}

namespace std
{
	___NODISCARD___ forceinline string to_string( UC::byte b ) { return to_string( static_cast< int16_t >( b ) ); }
	___NODISCARD___ forceinline string to_string( UC::sbyte b ) { return to_string( static_cast< int16_t >( b ) ); }
}
#endif // !__UC__BYTE_SBYTE__HPP__

