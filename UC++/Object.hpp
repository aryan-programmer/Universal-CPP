#pragma once
#ifndef __UC__OBJECT__HPP__
#define __UC__OBJECT__HPP__
#include "stdafx.h"
#include "Interface.hpp"
#include "byte.hpp"
#include <sstream>
#include <iostream>
#include <boost\type_traits\remove_reference.hpp>
#include <boost\type_traits\is_detected.hpp>
#include <boost\type_traits\add_const.hpp>
#include <boost\type_traits\add_reference.hpp>
#include <boost\type_traits\remove_cv_ref.hpp>

namespace UC
{
	template<class T> boost::add_reference_t<boost::add_const<T>> cdeclref( ) noexcept;
	template<typename T> using HasGHC = decltype( cdeclref<T>( ).GetHashCode( ) );
	template<typename T> using HasPGHC = decltype( ( *cdeclref<T>( ) ).GetHashCode( ) );

	template<typename... Args>
	size_t CombineHashCodes( Args&&... hashCodes )
	{
		size_t hash1 = ( 0x1505UL << 0x10UL ) + 0x1505UL;
		size_t hash2 = hash1;

		size_t i = 0UL;
		auto f = [ &hash1 , &hash2 , &i ] ( auto&& hashCode )
		{
			if ( i % 0x2 == 0x0 )
				hash1 = ( ( hash1 << 0x5 ) + hash1 + ( hash1 >> 0x1b ) ) ^ hashCode;
			else
				hash2 = ( ( hash2 << 0x5 ) + hash2 + ( hash2 >> 0x1b ) ) ^ hashCode;

			++i;
		};
		( f( hashCodes ) , ... );

		return hash1 + ( hash2 * 0x5d588b65 );
	}

	struct _Hasher
	{
		template<typename T>
		forceinline size_t operator()( const T& val ) const
		{ return ( *this )( val , std::bool_constant<boost::is_detected_v<HasGHC , boost::remove_cv_ref_t<T>>>( ) ); }

	private:
		template<typename T>
		forceinline size_t operator()( const T& val , std::true_type ) const
		{ return val.GetHashCode( ); }

		template<typename T>
		forceinline size_t operator()( const T& val , std::false_type ) const
		{ return std::hash<boost::remove_cv_ref_t<T>>( )( val ); }
	};

	template<typename T>
	struct Hasher
	{
		typedef T argument_type;
		typedef size_t result_type;
		___UC_NODISCARD___ size_t operator()( const T v ) const noexcept { return _Hasher( )( v ); }
	};

	template<typename T> forceinline size_t Hash( const T& val ) { return _Hasher( )( val ); }

	class Object;

	/// <summary>
	/// Represents text as a mutable sequence of ASCII code units.
	/// </summary>
	using NatString = std::string;

	template<typename Arg1>
	forceinline void ConcatNatStringsHelper( std::ostringstream& s , Arg1&& arg1 )
	{ s << std::forward<Arg1>( arg1 ); }
	template<typename Arg1 , typename... Args>
	forceinline void ConcatNatStringsHelper( std::ostringstream& s , Arg1&& arg1 , Args&&... args )
	{ s << std::forward<Arg1>( arg1 ); ConcatNatStringsHelper( s , std::forward<Args>( args )... ); }

	static NatString ConcatNatStringsI( std::initializer_list<NatString> args )
	{
		NatString s;
		size_t sz = 0;
		for ( auto& i : args ) sz += i.size( );
		s.reserve( sz + 1 );
		for ( auto& i : args ) s += i;
		return std::move( s );
	}

	template<typename... Args> forceinline NatString ConcatNatStrings( Args&&... args )
	{ return ConcatNatStringsI( { NatString( args )... } ); }

#pragma region Exceptions
	class Exception : public std::exception
	{
		using base = std::exception;
		const NatString str;
	public:
		Exception( NatString&& str ) noexcept : base( ) , str( str ) { }
		Exception( const NatString& str ) noexcept : base( ) , str( str ) { }
		virtual char const* what( ) const override { return str.c_str( ); }
		const NatString& Message( ) const { return str; }
	};

	/// <summary>
	/// This exception is thrown when 2 classes are added for reflection which have the same name, to resolve this error simply include the full namespace name in front of the name of the class.
	/// </summary>
	/// <seealso cref="std::Exception"/>
	UCException( RepeatingClassNameException );

	/// <summary>
	/// This exception is thrown for invalid casting or explicit conversion.
	/// </summary>
	UCException( InvalidCastException );

	/// <summary>
	/// This exception is thrown when one of the arguments provided to a method is not valid.
	/// </summary>
	UCException( InvalidArgumentException );

	/// <summary>
	/// This exception is thrown when there is an attempt to dereference a null GCPtr object.
	/// </summary>
	UCException( NullPointerException );

	/// <summary>
	/// This exception is thrown when the check for an object GCPtr being non-null fails.
	/// This exception is the same as NullPointerException.
	/// </summary>
	using PreNullPointerException = NullPointerException;

	/// <summary>
	/// This exception is thrown for invalid use of expired WeakPtr object.
	/// </summary>
	UCException( BadWeakPtrException );

	/// <summary>
	/// This exception is thrown when invalid parameters are specified for function reflection.
	/// </summary>
	UCException( NoSuchFunction_Exception );

	/// <summary>
	/// This exception is thrown when invalid parameters are specified for reflective construction.
	/// </summary>
	UCException( NoSuchConstructor_Exception );

	/// <summary>
	/// This exception is thrown when the name of a non-existent type is specified for reflective construction.
	/// </summary>
	UCBasedException( NoSuchRegisteredType_Exception , NoSuchConstructor_Exception );
#pragma endregion

#pragma region Smart Pointers
	/// <summary>
	/// GCPtr is a class for reference counted resource management/ARC (Automatic Reference Counting).
	/// It holds a strong reference to the object inside it.
	/// </summary>
	template<typename T> struct GCPtr
	{
		std::shared_ptr<T> ptr;
		using element_type = T;

		GCPtr<T>& Get( ) { if ( !HasValue( ) )throw NullPointerException( "Trying to dereference a null pointer." ); return *this; }
		const GCPtr<T>& Get( ) const { if ( !HasValue( ) )throw NullPointerException( "Trying to dereference a null pointer." ); return *this; }

		template<class T2>
		forceinline GCPtr( const GCPtr<T2>& p ) :ptr( p.ptr ) { }
		template<class T2>
		forceinline GCPtr( const std::shared_ptr<T2>& p ) : ptr( p ) { }
		forceinline GCPtr( std::shared_ptr<T>&& p ) : ptr( p ) { }
		forceinline GCPtr( const std::shared_ptr<T>& p ) : ptr( p ) { }

		forceinline GCPtr( ) noexcept :ptr {  } { }
		forceinline GCPtr( nullptr_t ) noexcept :ptr { } { }
		forceinline explicit GCPtr( T* value ) : ptr( value ) { }

		forceinline explicit operator bool( ) const noexcept { return ptr != nullptr; }
		forceinline GCPtr<T>& Reset( ) noexcept { ptr.reset( ); return *this; }
		forceinline GCPtr<T>& Reset( T* value ) { ptr.reset( value ); return *this; }
		forceinline GCPtr<T>& Reset( const T& value )
		{
			ptr.reset( new T( value ) );
			return *this;
		}

		forceinline T& operator*( ) { Get( ); return *ptr; }
		forceinline const T& operator*( ) const { Get( ); return *ptr; }
		forceinline T* operator->( ) { Get( ); return ptr.operator->( ); }
		forceinline const T* operator->( ) const { Get( ); return ptr.operator->( ); }

		forceinline bool HasValue( ) const noexcept { return ptr != nullptr; }

		forceinline bool operator!=( nullptr_t )const noexcept { return ptr != nullptr; }
		forceinline bool operator==( nullptr_t )const noexcept { return ptr == nullptr; }

		template<typename T2>
		forceinline bool RefEq( const GCPtr<T2>& ptr2 )const noexcept { return ptr == ptr2.ptr; }
		template<typename T2>
		forceinline bool RefNotEq( const GCPtr<T2>& ptr2 )const noexcept { return ptr == ptr2.ptr; }

		template<typename T>
		friend class WeakPtr;
		template<typename T>
		friend class IEnableGCPtrFromMe;
	};

	template<typename T> forceinline bool operator!=( nullptr_t , const GCPtr<T>& val )
	{ return val != nullptr; }

	template<typename T> forceinline bool operator==( nullptr_t , const GCPtr<T>& val )
	{ return val == nullptr; }

	/// <summary>
	/// WeakPtr is a smart pointer that holds a non-owning ("weak") reference to an object that is managed by GCPtr. 
	/// It must be converted to GCPtr (by Lock, LockIfNotThrow/operator *) in order to access the referenced object.
	/// </summary>
	template<typename T> class WeakPtr
	{
		std::weak_ptr<T> wptr;
	public:
		forceinline WeakPtr( std::weak_ptr<T>&& p ) :wptr( std::move( p ) ) { }
		forceinline WeakPtr( const std::weak_ptr<T>& p ) : wptr( p ) { }

		forceinline WeakPtr( ) noexcept :wptr( ) { }
		forceinline WeakPtr( nullptr_t ) noexcept :wptr( ) { }
		forceinline WeakPtr( const GCPtr<T>& ptr ) noexcept :wptr( ptr.ptr ) { }

		forceinline WeakPtr<T>& Reset( ) noexcept { wptr.reset( ); return *this; }
		forceinline bool Expired( ) { return wptr.expired( ); }

		forceinline GCPtr<T> Lock( ) { return GCPtr<T>( wptr.lock( ) ); }
		GCPtr<T> LockIfNotThrow( )
		{
			auto lockedVal = wptr.lock( );
			if ( lockedVal == nullptr ) throw BadWeakPtrException( "Trying to dereference an expired WeakPtr." );
			return GCPtr<T>( std::move( lockedVal ) );
		}
		forceinline GCPtr<T> operator*( ) { return LockIfNotThrow( ); }

		forceinline WeakPtr<T>& operator=( const GCPtr<T>& ptr ) { wptr = ptr.ptr; return *this; }
		forceinline WeakPtr<T>& operator=( GCPtr<T>&& ptr ) { wptr = ptr.ptr; return *this; }

		forceinline bool operator!=( nullptr_t )const noexcept { return wptr != nullptr; }
		forceinline bool operator==( nullptr_t )const noexcept { return wptr == nullptr; }

		template<typename T>
		friend class IEnableGCPtrFromMe;
	};

	template<typename T> forceinline bool operator!=( nullptr_t , const WeakPtr<T>& val )
	{ return val != nullptr; }

	template<typename T> forceinline bool operator==( nullptr_t , const WeakPtr<T>& val )
	{ return val == nullptr; }

	template<typename T> class EnableGCPtrFromMe : public std::enable_shared_from_this<T>
	{
		using base_t = std::enable_shared_from_this<T>;
		base_t& base( ) { return *this; }
		const base_t& base( ) const { return *this; }
	public:
		forceinline GCPtr<T> GCFromMe( ) { return GCPtr<T>( base( ).shared_from_this( ) ); }

		forceinline GCPtr<const T> GCFromMe( ) const
		{ return GCPtr<const T>( base( ).shared_from_this( ) ); }

		forceinline WeakPtr<T> WeakFromMe( )noexcept { return WeakPtr<T>( base( ).weak_from_this( ) ); }

		forceinline WeakPtr<const T> WeakFromMe( ) const noexcept
		{ return WeakPtr<const T>( base( ).weak_from_this( ) ); }
	};
#pragma endregion

	// P_Any = Pointer to Any (U++) value
	using P_Any = GCPtr<Object>;
	// W_Any = Weak pointer to Any (emc2) value
	using W_Any = WeakPtr<Object>;

	// NatDeque = Native Deque (Deque = Double Ended Queue)
	template<typename T>
	using NatDeque = std::deque<T>;

	// NatODeque = Native Object Deque (Deque = Double Ended Queue)
	using NatODeque = std::deque<P_Any>;

	/// <summary>
	/// This is the base class of all <seealso cref="UCInterface"/>s.
	/// It is the root of the UC++ type hierarchy.
	/// </summary>
	/// <seealso cref="EnableGCPtrFromMe{Object}" />
	class Object : public EnableGCPtrFromMe<Object>
	{
	public:
		virtual ~Object( );

		virtual const NatString& GetTypeName( ) const = 0;
		virtual P_Any Call( const NatString& fname , const NatODeque& args ) = 0;
		virtual NatString ToString( ) const;
		virtual int64_t GetHashCode( ) const;

		Object( Object&& ) = delete;
		Object& operator=( Object&& ) = delete;

		static P_Any CreateInstance( const NatString& className , const NatODeque& args );

		using EGCPFM = ::UC::EnableGCPtrFromMe<Object>;
	protected:
		Object( );
		forceinline P_Any callImpl( const NatString& fname , const NatODeque& args ) { return nullptr; }
		static void addConstructor( const NatString& className , P_Any( *ctor )( const NatODeque& args ) );
	private:
		static std::unordered_map<NatString , P_Any( *)( const NatODeque& args )>& getClassCtors( );
	};

	/// <summary>
	/// Represents a garbage collectible object of type T.
	/// </summary>
	template<typename T>using GCP = GCPtr<T>;

	/// <summary>
	/// Casts `value` to a value of type GCP{T}.
	/// </summary>
	/// <param name="v">The value.</param>
	/// <returns></returns>
	template<typename T , typename T2>forceinline GCP<T> ObjCast( const GCP<T2>& v ) { return std::dynamic_pointer_cast< T >( v.ptr ); }

	template<typename T , typename T2>
	GCP<T> ObjCastThrowing( const GCP<T2>& v , const char* msg )
	{
		auto ret = ObjCast<T>( v );
		if ( ret == nullptr )throw InvalidCastException( msg );
		return ret;
	}

	template<typename T , typename T2>
	GCP<T> ObjCastThrowingNatStr( const GCP<T2>& v , const NatString& msg )
	{
		auto ret = ObjCast<T>( v );
		if ( ret == nullptr )throw InvalidCastException( msg );
		return ret;
	}

	template<typename T>
	GCP<T> asNotNull( const GCP<T>& v , const char* msg )
	{
		if ( v == nullptr )throw PreNullPointerException( msg );
		return v;
	}

	template<typename T>
	GCP<T> asNotNull( GCP<T> v , const char* msg )
	{
		if ( v == nullptr )throw PreNullPointerException( msg );
		return std::move( v );
	}

#pragma region Integral Placeholder Interfaces
#define __DEFINE_integralPlaceHolderInterfaces(name, underlyingType)\
UCInterface( name , UC_WhereTypenameIs( "UC::" __ToString(name) ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );\
	UC_OnlyHasEmptyCtor;\
	UC_HasNoMethods;\
public:\
	using int_t = underlyingType;\
	const int_t value;\
	forceinline name():value{ }{}\
	virtual NatString ToString( ) const{return std::to_string(value);}\
	virtual int64_t GetHashCode( ) const{return Hash(value);}\
	forceinline name(int_t value):value{value }{}\
	forceinline static GCP<name> Make( int_t value ) { return GCP<name>( new name(value) ); }\
UCEndInterface(name)

	__DEFINE_integralPlaceHolderInterfaces( Int16 , int16_t );
	__DEFINE_integralPlaceHolderInterfaces( Int32 , int32_t );
	__DEFINE_integralPlaceHolderInterfaces( Int64 , int64_t );
	__DEFINE_integralPlaceHolderInterfaces( UInt16 , uint16_t );
	__DEFINE_integralPlaceHolderInterfaces( UInt32 , uint32_t );
	__DEFINE_integralPlaceHolderInterfaces( UInt64 , uint64_t );

	static_assert( std::is_same<size_t , uint64_t>::value , "Platform must be x64" );
#undef __DEFINE_integralPlaceHolderInterfaces
#pragma endregion


#pragma region Floating Point Number Placeholder Interfaces
#define __DEFINE_floatingPointPlaceHolderInterfaces(name, underlyingType, default_)\
UCInterface( name , UC_WhereTypenameIs( "UC::" __ToString(name) ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );\
	UC_OnlyHasEmptyCtor;\
	UC_HasNoMethods;\
public:\
	const underlyingType value;\
	virtual NatString ToString( ) const{return std::to_string(value);}\
	virtual int64_t GetHashCode( ) const{return Hash(value);}\
	forceinline name(underlyingType value=default_):value{value }{}\
	forceinline static GCP<name> Make( underlyingType value ) { return GCP<name>( new name(value) ); }\
UCEndInterface(name)

	__DEFINE_floatingPointPlaceHolderInterfaces( Float , float , 0.0f );
	__DEFINE_floatingPointPlaceHolderInterfaces( Double , double , 0.0 );

#undef __DEFINE_floatingPointPlaceHolderInterfaces
#pragma endregion


#pragma region byte and sbyte Placeholder Interfaces
#define __DEFINE_byteTypePlaceHolderInterfaces(name, underlyingType)\
UCInterface( name , UC_WhereTypenameIs( "UC::" __ToString(name) ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );\
	UC_OnlyHasEmptyCtor;\
	UC_HasNoMethods;\
public:\
	const underlyingType value;\
	forceinline name():value{}{}\
	virtual NatString ToString( ) const{return std::to_string(value);}\
	virtual int64_t GetHashCode( ) const{return Hash(value);}\
	forceinline name(underlyingType value):value{value }{}\
	forceinline static GCP<name> Make( underlyingType value ) { return GCP<name>( new name(value) ); }\
UCEndInterface(name)

	__DEFINE_byteTypePlaceHolderInterfaces( Byte , byte );
	__DEFINE_byteTypePlaceHolderInterfaces( SByte , sbyte );
	__DEFINE_byteTypePlaceHolderInterfaces( Bool , bool );

#undef __DEFINE_byteTypePlaceHolderInterfaces  
#pragma endregion


#pragma region Intergal Limits
	inline constexpr const int16_t	Int16Min = INT16_MIN;
	inline constexpr const int16_t	Int16Max = INT16_MAX;
	inline constexpr const int32_t	Int32Min = INT32_MIN;
	inline constexpr const int32_t	Int32Max = INT32_MAX;
	inline constexpr const int64_t	Int64Min = INT64_MIN;
	inline constexpr const int64_t	Int64Max = INT64_MAX;
	inline constexpr const uint16_t	UInt16Min = 0;
	inline constexpr const uint16_t	UInt16Max = UINT16_MAX;
	inline constexpr const uint32_t	UInt32Min = 0;
	inline constexpr const uint32_t	UInt32Max = UINT32_MAX;
	inline constexpr const uint64_t	UInt64Min = 0;
	inline constexpr const uint64_t	UInt64Max = UINT64_MAX;
	inline constexpr const byte		ByteMin = ( byte ) 0;
	inline constexpr const byte		ByteMax = ( byte ) UCHAR_MAX;
	inline constexpr const sbyte	SByteMin = ( sbyte ) CHAR_MIN;
	inline constexpr const sbyte	SByteMax = ( sbyte ) CHAR_MAX;
#pragma endregion


#pragma region Integral Conversion Functions
#define __DEFINE_asTForLowerHelper(r, data, i, elem) if ( auto res = ObjCast<elem>( it ) ) return static_cast<data>(res->value);
#define __DEFINE_asTForLower(curr, name, ...) \
static curr as##name(const P_Any& it , const char* msg){\
	BOOST_PP_SEQ_FOR_EACH_I(__DEFINE_asTForLowerHelper, curr, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	throw InvalidCastException(msg);\
}
	__DEFINE_asTForLower( int16_t , Int16 , Int16 , Byte , SByte );
	__DEFINE_asTForLower( int32_t , Int32 , Int32 , UInt16 , UInt16 , Byte , SByte );
	__DEFINE_asTForLower( int64_t , Int64 , Int64 , UInt32 , Int32 , UInt16 , UInt16 , Byte , SByte );
	__DEFINE_asTForLower( uint16_t , UInt16 , UInt16 , Byte );
	__DEFINE_asTForLower( uint32_t , UInt32 , UInt32 , UInt16 , Byte );
	__DEFINE_asTForLower( uint64_t , UInt64 , UInt64 , UInt32 , UInt16 , Byte );
	__DEFINE_asTForLower( byte , Byte , Byte );
	__DEFINE_asTForLower( sbyte , SByte , SByte );


	__DEFINE_asTForLower( float , Float , UInt64 , Int64 , Int64 , UInt32 , Int32 , UInt16 , UInt16 , Byte , SByte );
	__DEFINE_asTForLower( double , Double , Float , UInt64 , Int64 , Int64 , UInt32 , Int32 , UInt16 , UInt16 , Byte , SByte );
#undef __DEFINE_asTForLowerHelper
#undef __DEFINE_asTForLower
#pragma endregion


#pragma region String
	/// <summary>
	/// Represents text as a non-mutable sequence of ASCII code units.
	/// </summary>
	UCInterface( String , UC_WhereTypenameIs( "UC::String" ) , UC_InheritsUCClasses( Object ) ,
				 UC_InheritsNoNativeClasses , final );
	UC_HasNativeCtorsAndEmptyCtor;
	UC_HasMethods(
		( ULength ) ,
		( UContains , ( s ) ) ,
		( UEndsWith , ( s ) ) ,
		( UStartsWith , ( s ) ) ,
		( UEquals , ( s ) ) ,
		( USubstring , ( startIdx ) ) ,
		( USubstring , ( startIdx , count ) ) ,
		( Get , ( Idx ) ) ,
		( OpAdd , ( _1 ) ) ,
		( OpAdd , ( _1 , _2 ) ) ,
		( OpAdd , ( _1 , _2 , _3 ) ) ,
		( OpAdd , ( _1 , _2 , _3 , _4 ) ) ,
		( OpAdd , ( _1 , _2 , _3 , _4 , _5 ) ) ,
		( OpAdd , ( _1 , _2 , _3 , _4 , _5 , _6 ) ) ,
		( OpAdd , ( _1 , _2 , _3 , _4 , _5 , _6 , _7 ) ) ,
		( OpAdd , ( _1 , _2 , _3 , _4 , _5 , _6 , _7 , _8 ) ) ,
		( OpAdd , ( _1 , _2 , _3 , _4 , _5 , _6 , _7 , _8 , _9 ) ) ,
		( OpAdd , ( _1 , _2 , _3 , _4 , _5 , _6 , _7 , _8 , _9 , _10 ) )
	);
public:
	using pcself = GCP<const self>;
	using str_t = NatString;
	using value_type = char;
	using reference = char &;
	using const_reference = const char&;
	using iterator = typename str_t::const_iterator;
	using const_iterator = typename str_t::const_iterator;
	using reverse_iterator = typename str_t::const_reverse_iterator;
	using const_reverse_iterator = typename str_t::const_reverse_iterator;

	const str_t value;

	forceinline iterator begin( ) { return std::begin( value ); }
	forceinline const_iterator begin( ) const { return std::begin( value ); }
	forceinline iterator end( ) { return std::end( value ); }
	forceinline const_iterator end( ) const { return std::end( value ); }
	forceinline const_iterator cbegin( ) const { return std::cbegin( value ); }
	forceinline const_iterator cend( ) const { return std::cend( value ); }

	forceinline reverse_iterator rbegin( ) { return std::rbegin( value ); }
	forceinline const_reverse_iterator rbegin( ) const { return std::rbegin( value ); }
	forceinline reverse_iterator rend( ) { return std::rend( value ); }
	forceinline const_reverse_iterator rend( ) const { return std::rend( value ); }
	forceinline const_reverse_iterator crbegin( ) const { return std::crbegin( value ); }
	forceinline const_reverse_iterator crend( ) const { return std::crend( value ); }

	NatString ToString( ) const override { return value; }
	int64_t GetHashCode( ) const override { return Hash( value ); }

	forceinline const char& Get( size_t idx ) const { return value[ ( size_t ) idx ]; }

	forceinline int64_t NLength( ) const noexcept { return value.length( ); }

	forceinline bool NContains( const pself& s )const { return ( s->NLength( ) == 0 ) || boost::contains( value , s->value ); }
	forceinline bool NEndsWith( const pself& s )const { return ( s->NLength( ) == 0 ) || boost::ends_with( value , s->value ); }
	forceinline bool NStartsWith( const pself& s )const { return ( s->NLength( ) == 0 ) || boost::starts_with( value , s->value ); }
	forceinline bool NEquals( const pself& s )const { return value == s->value; }
	forceinline size_t NGetHashCode( ) const { return std::hash<std::string>( )( value ); }
	forceinline pself NSubstring( size_t startIndex , size_t length ) const { return Make( value.substr( startIndex , length ) ); }
	forceinline pself NSubstring( size_t startIndex ) const { return Make( value.substr( startIndex ) ); }

	forceinline pself operator+( const self& s )const { return Make( value + s.value ); }
	forceinline bool operator==( const pself& s ) const { return value == s->value; }
	forceinline bool operator!=( const pself& s ) const { return value != s->value; }
	forceinline bool operator==( const char* s ) const { return value == s; }
	forceinline bool operator!=( const char* s ) const { return value != s; }

	template<typename... Args>
	static pself Concat( Args&&... args )
	{
		return Make( ConcatNatStrings( GetNativeFrom( GetFrom( std::forward<Args>( args ) ) )... ) );
	}

	static pself GetFrom( self& s ) { return s.ME; }
	static pcself GetFrom( const self& s ) { return s.ME; }
	static pself GetFrom( self&& s ) { return s.ME; }
	static const pself& GetFrom( const pself& s ) { return s; }
	static pcself GetFrom( const pcself& s ) { return s; }
	static pself GetFrom( str_t&& s ) { return Make( s ); }
	static pself GetFrom( const str_t& s ) { return Make( s ); }
	static pself GetFrom( const char* s ) { return Make( s ); }
	static const str_t& GetNativeFrom( const pself& s ) { return s->value; }
	static const str_t& GetNativeFrom( const GCP<const self>& s ) { return s->value; }

private:
	forceinline String( const std::string_view& v ) :value( v ) { }
	forceinline String( const str_t& v ) : value( v ) { }
	forceinline String( str_t&& v ) : value( std::move( v ) ) { }
	forceinline String( ) : value( ) { }

	template<typename Iter> String( Iter beg , Iter end ) : value( beg , end ) { }
	forceinline String( const char* value_ ) : value( value_ ) { }
	forceinline String( const char* value_ , size_t startIndex , size_t length ) : value( value_ , startIndex , length ) { }
	UCEndInterface( String );
#pragma endregion

	struct _HashGCP
	{
		template<typename T>
		forceinline size_t operator()( const T& val ) const
		{ return ( *this )( val , std::bool_constant<std::is_base_of_v<Object , typename T::element_type>>( ) ); }

	private:
		template<typename T>
		forceinline size_t operator()( const T& val , std::true_type ) const
		{ return val->GetHashCode( ); }

		template<typename T>
		forceinline size_t operator()( const T& val , std::false_type ) const
		{ return std::hash<std::shared_ptr<typename T::element_type>>( )( val.ptr ); }
	};
}

namespace std
{
	template<typename T>
	struct hash<UC::GCPtr<T>>
	{
		typedef UC::GCPtr<T> argument_type;
		typedef size_t result_type;
		___UC_NODISCARD___ size_t operator()( const UC::GCPtr<T> v ) const noexcept { return UC::_HashGCP( )( v ); }
	};

	template<typename T> inline ostream& operator<<( ostream& o , const UC::GCPtr<T>& p )
	{ return o << p->ToString( ); }
}
#endif // !__UC__OBJECT__HPP__
