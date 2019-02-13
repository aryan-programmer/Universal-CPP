#pragma once
#ifndef __UC__OBJECT__HPP__
#define __UC__OBJECT__HPP__
#include "stdafx.h"
#include "Interface.hpp"
#include "byte.hpp"
#include <boost\type_index.hpp>

namespace UC
{
	template<class T> boost::add_reference_t<boost::add_const<T>> cdeclref( ) noexcept;
	template<typename T> using HasGHC = decltype( cdeclref<T>( ).GetHashCode( ) );
	template<typename T> using HasPGHC = decltype( ( *cdeclref<T>( ) ).GetHashCode( ) );
	template<typename T> using HasSGTN = decltype( T::SGetTypeName( ) );

#pragma region Hashing
	template<typename... Ts>forceinline void forceRepeatedEval( Ts&&... ) { }

	template<typename TColl>
	size_t CombineHashCodesInColl( const TColl& coll )
	{
		size_t hash1 = ( 0x1505UL << 0x10UL ) + 0x1505UL;
		size_t hash2 = hash1;

		size_t i = 0UL;
		for ( const auto& hashCode : coll )
		{
			if ( i % 0x2 == 0x0 )
				hash1 = ( ( hash1 << 0x5 ) + hash1 + ( hash1 >> 0x1b ) ) ^ hashCode;
			else
				hash2 = ( ( hash2 << 0x5 ) + hash2 + ( hash2 >> 0x1b ) ) ^ hashCode;

			++i;
		};

		return hash1 + ( hash2 * 0x5d588b65 );
	}

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

			return ' ';
		};
		forceRepeatedEval( f( hashCodes )... );

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
		___UC_NODISCARD___ forceinline size_t operator()( const T& v ) const noexcept { return _Hasher( )( v ); }
	};

	template<typename T> forceinline size_t Hash( const T& val ) { return _Hasher( )( val ); }
#pragma endregion


	class Object;


#pragma region Native String & Concatenation
	/// <summary>
	/// Represents text as a _mutable_ sequence of ASCII code units.
	/// </summary>
	using NatString = std::string;

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
#pragma endregion


	template<typename T> forceinline const NatString& SGetTypeName( std::true_type ) { return T::SGetTypeName( ); }
	template<typename T> const NatString& SGetTypeName( std::false_type )
	{ static NatString s( boost::typeindex::type_id<T>( ).pretty_name( ) ); return s; }
	template<typename T> forceinline const NatString& SGetTypeName( )
	{ return SGetTypeName<boost::remove_cv_ref_t<T>>( std::bool_constant<boost::is_detected_v<HasSGTN , boost::remove_cv_ref_t<T>>>( ) ); }

#pragma region Exceptions
	class Exception : public std::exception
	{
		using base = std::exception;
		NatString str;
		boost::stacktrace::stacktrace stackTrace;
	public:
		Exception( NatString&& str ) noexcept : base( ) , str( str ) , stackTrace( boost::stacktrace::stacktrace( ) ) { }
		Exception( const NatString& str ) noexcept : base( ) , str( str ) { }
		virtual char const* what( ) const override { return str.c_str( ); }
		const NatString& Message( ) const { return str; }
		const boost::stacktrace::stacktrace& GetStackTrace( ) const { return stackTrace; }
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

	/// <summary>
	/// This exception is thrown when invalid parameters are specified for collection indexing.
	/// </summary>
	UCException( IndexOutOfRangeException );

	/// <summary>
	/// This exception is thrown when the value that has been asked for is not found.
	/// </summary>
	UCException( ValueNotFoundException );

	/// <summary>
	/// This exception is thrown when an event that has to return a value has no functions added to it.
	/// </summary>
	UCException( NoFunctorsAddedToEvent_Exception );
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

		static const NatString& SGetTypeName( );

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
		forceinline bool RefNotEq( const GCPtr<T2>& ptr2 )const noexcept { return ptr != ptr2.ptr; }

		template<typename T>
		friend struct WeakPtr;
		template<typename T>
		friend class EnableGCPtrFromMe;
	};

	template<typename T>
	inline const NatString & UC::GCPtr<T>::SGetTypeName( )
	{
		static auto s = ConcatNatStrings( "GCPtr<" , UC::SGetTypeName<T>( ) , ">" );
		return s;
	};

	template<typename T1 , typename T2>
	GCPtr<T1> operator |( const GCPtr<T1>& v1 , const GCPtr<T2>& v2 ) { return v1 != nullptr ? v1 : GCPtr<T1>( v2 ); }
	template<typename T1 , typename T2>
	forceinline bool operator ==( const GCPtr<T1>& ptr1 , const GCPtr<T2>& ptr2 ) { return *ptr1 == *ptr2; }
	template<typename T1 , typename T2>
	forceinline bool operator !=( const GCPtr<T1>& ptr1 , const GCPtr<T2>& ptr2 ) { return *ptr1 != *ptr2; }

	template<typename T> forceinline bool operator!=( nullptr_t , const GCPtr<T>& val )
	{ return val != nullptr; }

	template<typename T> forceinline bool operator==( nullptr_t , const GCPtr<T>& val )
	{ return val == nullptr; }

	/// <summary>
	/// WeakPtr is a smart pointer that holds a non-owning ("weak") reference to an object that is managed by GCPtr. 
	/// It must be converted to GCPtr (by Lock, LockIfNotThrow/operator *) in order to access the referenced object.
	/// </summary>
	template<typename T> struct WeakPtr
	{
		std::weak_ptr<T> wptr;

		forceinline WeakPtr( std::weak_ptr<T>&& p ) :wptr( std::move( p ) ) { }
		forceinline WeakPtr( const std::weak_ptr<T>& p ) : wptr( p ) { }

		forceinline WeakPtr( ) noexcept :wptr( ) { }
		forceinline WeakPtr( nullptr_t ) noexcept :wptr( ) { }
		forceinline WeakPtr( const GCPtr<T>& ptr ) noexcept :wptr( ptr.ptr ) { }

		forceinline WeakPtr<T>& Reset( ) noexcept { wptr.reset( ); return *this; }
		forceinline bool Expired( ) const { return wptr.expired( ); }

		forceinline GCPtr<T> Lock( ) const { return GCPtr<T>( wptr.lock( ) ); }
		GCPtr<T> LockIfNotThrow( ) const
		{
			auto lockedVal = wptr.lock( );
			if ( lockedVal == nullptr ) throw BadWeakPtrException( "Trying to dereference an expired WeakPtr." );
			return GCPtr<T>( std::move( lockedVal ) );
		}
		forceinline GCPtr<T> operator*( ) { return LockIfNotThrow( ); }

		forceinline explicit operator bool( ) const noexcept { return *this != nullptr || !wptr.expired( ); }
		forceinline WeakPtr<T>& operator=( const GCPtr<T>& ptr ) { wptr = ptr.ptr; return *this; }

		forceinline bool operator!=( nullptr_t )const noexcept { return !( *this == nullptr ); }
		bool operator==( nullptr_t )const noexcept { return !wptr.owner_before( std::weak_ptr<T>{} ) && !std::weak_ptr<T>{}.owner_before( wptr ); }

		template<typename T>
		friend class EnableGCPtrFromMe;
	};

	template<typename T> forceinline bool operator!=( nullptr_t , const WeakPtr<T>& val )
	{ return val != nullptr; }

	template<typename T> forceinline bool operator==( nullptr_t , const WeakPtr<T>& val )
	{ return val == nullptr; }

	struct virtual_enable_shared_from_this_base :
		std::enable_shared_from_this<virtual_enable_shared_from_this_base>
	{
		virtual ~virtual_enable_shared_from_this_base( ) { }
	};
	template<typename T>
	struct virtual_enable_shared_from_this : virtual virtual_enable_shared_from_this_base
	{
		std::shared_ptr<T> shared_from_this( )
		{
			return std::dynamic_pointer_cast< T >(
				virtual_enable_shared_from_this_base::shared_from_this( ) );
		}

		std::shared_ptr<const T> shared_from_this( ) const
		{
			return std::dynamic_pointer_cast< const T >(
				virtual_enable_shared_from_this_base::shared_from_this( ) );
		}
	};

	template<typename T> class EnableGCPtrFromMe : public virtual_enable_shared_from_this<T>
	{
		using base_t = virtual_enable_shared_from_this<T>;
		base_t& base( ) { return *this; }
		const base_t& base( ) const { return *this; }
	public:
		forceinline GCPtr<T> GCFromMe( )
		{
			return GCPtr<T>( base( ).shared_from_this( ) );
		}

		forceinline GCPtr<const T> GCFromMe( ) const
		{ return GCPtr<const T>( base( ).shared_from_this( ) ); }

		forceinline WeakPtr<T> WeakFromMe( )noexcept
		{
			try { return WeakPtr<T>( base( ).shared_from_this( ) ); }
			catch ( const std::bad_weak_ptr& ) { return  WeakPtr<T>( ); }
		}

		forceinline WeakPtr<const T> WeakFromMe( ) const noexcept
		{
			try { return WeakPtr<const T>( base( ).shared_from_this( ) ); }
			catch ( const std::bad_weak_ptr& ) { return  WeakPtr<const T>( ); }
		}
	};
#pragma endregion

	/// <summary>
	/// Represents a strongly referenced garbage collectible object of type T.
	/// </summary>
	template<typename T>using P = GCPtr<T>;

	/// <summary>
	/// Represents a weakly referenced garbage collectible object of type T.
	/// </summary>
	template<typename T>using W = WeakPtr<T>;

	// NatVector = Native Vector
	template<typename T>
	using NatVector = boost::container::vector<T>;

	// NatDeque = Native Deque (Deque = Double Ended Queue)
	template<typename T>
	using NatDeque = boost::container::deque<T>;

	// NatOVector = Native Object Vector
	using NatOVector = NatVector<P<Object>>;

	template<typename TKey , typename TVal>
	using NatMap = boost::unordered_map<TKey , TVal , Hasher<TKey>>;

	/// <summary>
	/// This is the base class of all <seealso cref="UCInterface"/>s.
	/// It is the root of the UC++ type hierarchy.
	/// </summary>
	/// <seealso cref="EnableGCPtrFromMe{Object}" />
	class Object : public EnableGCPtrFromMe<Object>
	{
	public:
		using CtorT = P<Object>( *)( const NatOVector& args );
		virtual ~Object( );

		static const NatString& SGetTypeName( ) { static NatString s = "UC::Object"; return s; }
		virtual const NatString& GetTypeName( ) const { return SGetTypeName( ); };
		virtual P<Object> Call( const NatString& fname , const NatOVector& args ) = 0;
		virtual NatString ToString( ) const;
		virtual int64_t GetHashCode( ) const;

		Object( Object&& ) = delete;
		Object& operator=( Object&& ) = delete;

		static P<Object> CreateInstance( const NatString& className , const NatOVector& args );

		static void addConstructor( const NatString& className , CtorT ctor );

		using EGCPFM = EnableGCPtrFromMe<Object>;
	protected:
		Object( );
		forceinline P<Object> callImpl( const NatString& fname , const NatOVector& args ) { return nullptr; }
	};

#pragma region Casting & Checking
	/// <summary>
	/// Casts `value` to a value of type P{T}.
	/// </summary>
	/// <param name="v">The value.</param>
	/// <returns></returns>
	template<typename T , typename T2>forceinline P<T> ObjCast( const P<T2>& v ) { return std::dynamic_pointer_cast< T >( v.ptr ); }

	template<typename T , typename T2>
	P<T> ObjCastThrowing( const P<T2>& v , const char* msg )
	{
		auto ret = ObjCast<T>( v );
		if ( ret == nullptr )throw InvalidCastException( msg );
		return ret;
	}

	template<typename T , typename T2>
	P<T> ObjCastThrowingNatStr( const P<T2>& v , const NatString& msg )
	{
		auto ret = ObjCast<T>( v );
		if ( ret == nullptr )throw InvalidCastException( msg );
		return ret;
	}

	template<typename T>
	P<T> asNotNull( const P<T>& v , const char* msg )
	{
		if ( v == nullptr )throw PreNullPointerException( msg );
		return v;
	}

	template<typename T>
	P<T> asNotNull( P<T>&& v , const char* msg )
	{
		if ( v == nullptr )throw PreNullPointerException( msg );
		return std::move( v );
	}
#pragma endregion


#pragma region Integral Placeholder Interfaces
#define __DEFINE_integralPlaceHolderInterfaces(name, underlyingType)\
UCInterface( name , UC_WhereTypenameIs( "UC::" __ToString(name) ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );\
	UC_OnlyHasEmptyCtor;\
	UC_HasNoMethods;\
public:\
	using int_t = underlyingType;\
	const int_t value;\
	forceinline name():value{ }{}\
	bool operator==( const self& o ) const{return value == o.value;}\
	bool operator!=( const self& o ) const{return value != o.value;}\
	virtual NatString ToString( ) const{return std::to_string(value);}\
	virtual int64_t GetHashCode( ) const{return Hash(value);}\
	forceinline name(int_t value):value{value }{}\
	forceinline static P<name> Make( int_t value ) { return P<name>( new name(value) ); }\
UCEndInterface

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
	bool operator==( const self& o ) const{return value == o.value;}\
	bool operator!=( const self& o ) const{return value != o.value;}\
	virtual NatString ToString( ) const{return std::to_string(value);}\
	virtual int64_t GetHashCode( ) const{return Hash(value);}\
	forceinline name(underlyingType value=default_):value{value }{}\
	forceinline static P<name> Make( underlyingType value ) { return P<name>( new name(value) ); }\
UCEndInterface


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
	bool operator==( const self& o ) const{return value == o.value;}\
	bool operator!=( const self& o ) const{return value != o.value;}\
	forceinline name():value{}{}\
	virtual NatString ToString( ) const{return std::to_string(value);}\
	virtual int64_t GetHashCode( ) const{return Hash(value);}\
	forceinline name(underlyingType value):value{value }{}\
	forceinline static P<name> Make( underlyingType value ) { return P<name>( new name(value) ); }\
UCEndInterface

	__DEFINE_byteTypePlaceHolderInterfaces( Byte , byte );
	__DEFINE_byteTypePlaceHolderInterfaces( SByte , sbyte );
	__DEFINE_byteTypePlaceHolderInterfaces( Bool , bool );

#undef __DEFINE_byteTypePlaceHolderInterfaces  
#pragma endregion


#pragma region Integral Limits & Floating Point Constants
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

	inline constexpr double Pi = 3.14159265358979323846;
	inline constexpr double Tau = 6.283185307179586;
	inline constexpr double SqrtPi = 1.77245385091;
	inline constexpr double PiOver2 = 1.57079632679489661923;
	inline constexpr double PiOver4 = 0.785398163397448309616;
	inline constexpr double OneOverPi = 0.318309886183790671538;
	inline constexpr double TwoOverPi = 0.636619772367581343076;
	inline constexpr double TwoOverSqrtPi = 1.12837916709551257390;
	inline constexpr double Sqrt2 = 1.41421356237309504880;
	inline constexpr double OneOverSqrt2 = 0.707106781186547524401;
	inline constexpr double E = 2.71828182845904523536;
	inline constexpr double Log2OfE = 1.44269504088896340736;
	inline constexpr double Log10OfE = 0.434294481903251827651;
	inline constexpr double LnOf2 = 0.693147180559945309417;
	inline constexpr double LnOf10 = 2.30258509299404568402;
	inline constexpr double NanD = std::numeric_limits<double>::signaling_NaN( );
	inline constexpr float NanF = std::numeric_limits<float>::signaling_NaN( );
	inline constexpr double InfD = std::numeric_limits<double>::infinity( );
	inline constexpr float InfF = std::numeric_limits<float>::infinity( );
	inline constexpr double NegInfD = -std::numeric_limits<double>::infinity( );
	inline constexpr float NegInfF = -std::numeric_limits<float>::infinity( );
#pragma endregion


#pragma region Integral Conversion Functions
#define __DEFINE_asTForLowerHelper(r, data, i, elem) if ( auto res = ObjCast<elem>( it ) ) return static_cast<data>(res->value);
#define __DEFINE_asTForLower(curr, name, ...) \
static curr as##name(const P<Object>& it , const char* msg){\
	BOOST_PP_SEQ_FOR_EACH_I(__DEFINE_asTForLowerHelper, curr, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	throw InvalidCastException(msg);\
}
	__DEFINE_asTForLower( int16_t , Int16 , Int16 , Byte , SByte );
	__DEFINE_asTForLower( int32_t , Int32 , Int32 , UInt16 , Int16 , Byte , SByte );
	__DEFINE_asTForLower( int64_t , Int64 , Int64 , UInt32 , Int32 , UInt16 , Int16 , Byte , SByte );
	__DEFINE_asTForLower( uint16_t , UInt16 , UInt16 , Byte );
	__DEFINE_asTForLower( uint32_t , UInt32 , UInt32 , UInt16 , Byte );
	__DEFINE_asTForLower( uint64_t , UInt64 , UInt64 , UInt32 , UInt16 , Byte );
	__DEFINE_asTForLower( byte , Byte , Byte );
	__DEFINE_asTForLower( sbyte , SByte , SByte );


	__DEFINE_asTForLower( float , Float , UInt64 , Int64 , Int64 , UInt32 , Int32 , UInt16 , Int16 , Byte , SByte );
	__DEFINE_asTForLower( double , Double , Float , UInt64 , Int64 , Int64 , UInt32 , Int32 , Int16 , UInt16 , Byte , SByte );
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
	using pcself = P<const self>;
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

	NatString ToString( ) const override { return ConcatNatStrings( "\"" , value , "\"" ); }
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
	forceinline bool operator==( const self& s ) const { return value == s.value; }
	forceinline bool operator!=( const self& s ) const { return value != s.value; }
	forceinline bool operator==( const char* s ) const { return value == s; }
	forceinline bool operator!=( const char* s ) const { return value != s; }

	template<typename... Args> static pself Concat( Args&&... args )
	{ return Make( ConcatNatStrings( GetNativeFrom( GetFrom( std::forward<Args>( args ) ) )... ) ); }

	static pself GetFrom( self& s ) { return s.ME; }
	static pcself GetFrom( const self& s ) { return s.ME; }
	static pself GetFrom( self&& s ) { return s.ME; }
	static const pself& GetFrom( const pself& s ) { return s; }
	static pcself GetFrom( const pcself& s ) { return s; }
	static pself GetFrom( str_t&& s ) { return Make( s ); }
	static pself GetFrom( const str_t& s ) { return Make( s ); }
	static pself GetFrom( const char* s ) { return Make( s ); }
	static const str_t& GetNativeFrom( const pself& s ) { return s->value; }
	static const str_t& GetNativeFrom( const P<const self>& s ) { return s->value; }

private:
	forceinline String( const std::string_view& v ) :value( v ) { }
	forceinline String( const str_t& v ) : value( v ) { }
	forceinline String( str_t&& v ) : value( std::move( v ) ) { }
	forceinline String( ) : value( ) { }

	template<typename Iter> String( Iter beg , Iter end ) : value( beg , end ) { }
	forceinline String( const char* value_ ) : value( value_ ) { }
	forceinline String( const char* value_ , size_t startIndex , size_t length ) : value( value_ , startIndex , length ) { }
	UCEndInterface;
#pragma endregion


#pragma region Containers
#pragma region Non-Associative Containers
#define __DEFINE_Container(name, underly_t,startBrace,endBrace, emplaceFront)\
UCTemplateInterface( name , ( T ) , UC_WhereTypenameIs( "UC::" __ToString(name) ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );\
	UC_HasExplicitCtors( name , UC_AlsoHasEmptyCtor , ( size ) );\
	UC_HasMethods(\
		( OpGet , ( start , end ) ) ,\
		( OpLen ) ,\
		( OpClear ) ,\
		( OpReverse , ( from , to ) ) ,\
		( OpReverse ) ,\
		( OpResize , ( idx ) ) ,\
		( OpTrimExcess ) ,\
		( OpRemAt , ( idx ) )\
	);\
\
public:\
	using col_t = underly_t<T>;\
	col_t coll;\
	typedef typename col_t::value_type value_type;\
	typedef typename col_t::reference reference;\
	typedef typename col_t::const_reference const_reference;\
	typedef typename col_t::iterator iterator;\
	typedef typename col_t::const_iterator const_iterator;\
	typedef typename col_t::reverse_iterator reverse_iterator;\
	typedef typename col_t::const_reverse_iterator const_reverse_iterator;\
	typedef typename col_t::size_type size_type;\
	typedef typename col_t::difference_type difference_type;\
\
	forceinline iterator begin( ) { return		coll.begin( ); }\
	forceinline const_iterator begin( ) const { return coll.begin( ); }\
	forceinline iterator end( ) { return		 coll.end( ); }\
	forceinline const_iterator end( ) const { return coll.end( ); }\
	forceinline const_iterator cbegin( ) const { return coll.begin( ); }\
	forceinline const_iterator cend( ) const { return coll.end( ); }\
\
	forceinline reverse_iterator rbegin( ) { return coll.rbegin( ); }\
	forceinline const_reverse_iterator rbegin( ) const { return coll.rbegin( ); }\
	forceinline reverse_iterator rend( ) { return coll.rend( ); }\
	forceinline const_reverse_iterator rend( ) const { return coll.rend( ); }\
	forceinline const_reverse_iterator crbegin( ) const { return coll.rbegin( ); }\
	forceinline const_reverse_iterator crend( ) const { return coll.rend( ); }\
\
	T& operator[]( const uint64_t& idx )\
	{\
		if ( Size( ) <= idx )\
			throw IndexOutOfRangeException(\
				ConcatNatStrings(\
					"Index: " ,\
					std::to_string( idx ) ,\
					" is out of range for a " __ToString(name) " of size: " ,\
					std::to_string( Size( ) ) ) );\
		return coll[ idx ];\
	}\
	const T& operator[]( const uint64_t& idx ) const\
	{\
		if ( Size( ) <= idx )\
			throw IndexOutOfRangeException(\
				ConcatNatStrings(\
					"Index: " ,\
					std::to_string( idx ) ,\
					" is out of range for a " __ToString(name) " of size: " ,\
					std::to_string( Size( ) ) ) );\
		return coll[ idx ];\
	}\
	const T& Get( const uint64_t& idx ) const { return ( *this )[ idx ]; }\
	forceinline void Set( const uint64_t& idx , T value ) { coll[ idx ] = value; }\
\
	NatString ToString( ) const override\
	{\
		NatString s( startBrace );\
		for ( size_t i = 0; i < Size( ); i++ )\
		{\
			if ( i != 0 )s += ", ";\
			s += coll[ i ]->ToString();\
		}\
		s += endBrace;\
		return std::move( s );\
	}\
	int64_t GetHashCode( ) const override\
	{\
		NatVector<size_t> hashes( Size( ) );\
		for ( size_t i = 0; i < Size( ); i++ ) hashes[ i ] = Hash( coll[ i ] );\
		return CombineHashCodesInColl( hashes );\
	}\
\
	forceinline uint64_t Size( ) const { return coll.size( ); }\
	forceinline uint64_t Length( ) const { return coll.size( ); }\
	template<typename T2> forceinline void AddFront( T2&& item ) { emplaceFront(coll, std::forward<T2>( item ) ); }\
	template<typename T2> forceinline void Add( T2&& item ) { coll.emplace_back( std::forward<T2>( item ) ); }\
	template<typename T2> forceinline void AddBefore( uint64_t index , T2&& item )\
	{ coll.emplace( begin( ) + index , std::forward<T2>( item ) ); }\
	template<typename T2 , typename... Args> forceinline void AddFront( T2&& item , Args&&... args )\
	{\
		AddFront( std::forward<Args>( args )... );\
		emplaceFront(coll, std::forward<T2>( item ) );\
	}\
	template<typename T2 , typename... Args> forceinline void Add( T2&& item , Args&&... args )\
	{\
		coll.emplace_back( std::forward<T2>( item ) );\
		Add( std::forward<Args>( args )... );\
	}\
	template<typename T2 , typename... Args> forceinline void AddBefore( uint64_t index , T2&& item , Args&&... args )\
	{\
		AddBefore( index , std::forward<Args>( args )... );\
		coll.emplace( begin( ) + index , std::forward<T2>( item ) );\
	}\
	forceinline void Clear( ) { coll.clear( ); }\
	template<typename TF>\
	forceinline bool Exists( TF&& func ) const { return std::any_of( begin( ) , end( ) , std::forward<TF>( func ) ); }\
	template<typename TF>\
	T& Find( TF&& func )\
	{\
		auto iter = std::find_if( begin( ) , end( ) , std::forward<TF>( func ) );\
		if ( iter == end( ) )throw ValueNotFoundException( "value not found" );\
		return *iter;\
	}\
	template<typename TF>\
	const T& Find( TF&& func ) const\
	{\
		auto iter = std::find_if( begin( ) , end( ) , std::forward<TF>( func ) );\
		if ( iter == end( ) )throw ValueNotFoundException( "value not found" );\
		return *iter;\
	}\
	template<typename TF>\
	pself FindAll( TF&& func )\
	{\
		pself retVal = Make( );\
		std::copy_if( begin( ) , end( ) , std::back_inserter( retVal->coll ) , std::forward<TF>( func ) );\
		return retVal;\
	}\
	template<typename TF>\
	pself FindAll( TF&& func ) const\
	{\
		pself retVal = Make( );\
		std::copy_if( begin( ) , end( ) , std::back_inserter( retVal->coll ) , std::forward<TF>( func ) );\
		return retVal;\
	}\
	template<typename TF> iterator FindIndex( uint64_t startIndex , uint64_t count , TF&& match )\
	{ return std::find_if( begin( ) + startIndex , begin( ) + startIndex + count , std::forward<TF>( match ) ); }\
	template<typename TF> iterator FindIndex( uint64_t startIndex , TF&& match )\
	{ return std::find_if( begin( ) + startIndex , end( ) , std::forward<TF>( match ) ); }\
	template<typename TF> iterator FindIndex( TF&& match )\
	{ return std::find_if( begin( ) , end( ) , std::forward<TF>( match ) ); }\
	template<typename TF> const_iterator FindIndex( uint64_t startIndex , uint64_t count , TF&& match ) const\
	{ return std::find_if( begin( ) + startIndex , begin( ) + startIndex + count , std::forward<TF>( match ) ); }\
	template<typename TF> const_iterator FindIndex( uint64_t startIndex , TF&& match ) const\
	{ return std::find_if( begin( ) + startIndex , end( ) , std::forward<TF>( match ) ); }\
	template<typename TF> const_iterator FindIndex( TF&& match ) const\
	{ return std::find_if( begin( ) , end( ) , std::forward<TF>( match ) ); }\
	template<typename TF> iterator FindLastIndex( TF&& match )\
	{ return std::find_if( rbegin( ) , rend( ) , std::forward<TF>( match ) ); }\
	template<typename TF>\
	forceinline void ForEach( TF&& f ) { std::for_each( begin( ) , end( ) , std::forward<TF>( f ) ); }\
	template<typename TF>\
	forceinline void ForEach( TF&& f ) const { std::for_each( begin( ) , end( ) , std::forward<TF>( f ) ); }\
	template<typename TF>\
	forceinline void RevForEach( TF&& f ) { std::for_each( rbegin( ) , rend( ) , std::forward<TF>( f ) ); }\
	template<typename TF>\
	forceinline void RevForEach( TF&& f ) const { std::for_each( rbegin( ) , rend( ) , std::forward<TF>( f ) ); }\
	forceinline pself GetRange( uint64_t index , uint64_t count ) { return Make( begin( ) + index , begin( ) + index + count ); }\
	template<typename TF>\
	bool Remove( TF&& match )\
	{\
		auto iter = std::find_if( begin( ) , end( ) , std::forward<TF>( match ) );\
		if ( iter == end( ) )return false;\
		coll.erase( iter );\
		return true;\
	}\
	template<typename TF>\
	uint64_t RemoveAll( TF match )\
	{\
		uint64_t no = 0;\
		for ( size_t i = 0; i < coll.size( ); i++ )\
			if ( match( ( *this )[ i ] ) )\
			{\
				++no;\
				RemoveAt( begin( ) + i );\
				--i;\
			}\
		return no;\
	}\
	forceinline void Reverse( uint64_t index , uint64_t count ) { std::reverse( begin( ) + index , begin( ) + index + count ); }\
	forceinline void Reverse( ) { std::reverse( begin( ) , end( ) ); }\
	forceinline void Resize( uint64_t size ) { coll.resize( size ); }\
\
	template<typename TF>\
	forceinline void Sort( TF&& comparer ) { std::sort( begin( ) , end( ) , std::forward<TF>( comparer ) ); }\
	forceinline void TrimExcess( ) { coll.shrink_to_fit( ); }\
	template<typename TF>\
	forceinline bool TrueForAll( TF&& match ) { return std::all_of( begin( ) , end( ) , std::forward<TF>( match ) ); }\
\
	forceinline void RemoveAt( uint64_t index )\
	{\
		if ( Size( ) <= index )\
			throw IndexOutOfRangeException(\
				ConcatNatStrings(\
					"Index: " ,\
					std::to_string( index ) ,\
					" is out of range for a " __ToString(name) " of size: " ,\
					std::to_string( Size( ) ) ) );\
		coll.erase( begin( ) + index );\
	}\
	template<typename... Args>\
	pself MakeI(Args&&... args){return pself(new self{std::forward<Args>(args)...});}\
protected:\
	inline name( uint64_t len ) : coll( len ) { }\
	template<typename TIter>\
	forceinline name( const TIter& beg , const TIter& end ) : coll( beg , end ) { }\
	forceinline name( std::initializer_list<T> init ) : coll( init ) { }\
UCEndInterface;\
\
template<typename _T>\
UCCtor( name<_T>::name ) :coll( ) { }\
template<typename _T>\
UCCtor( name<_T>::name , ( size ) ) : coll( UCAsInt64( size ) ) { }\
template<typename _T>\
UCMethod( name<_T>::OpGet , ( start , end ) ) { return this->GetRange( UCAsUInt64( start ) , UCAsUInt64( end ) ); }\
template<typename _T>\
UCMethod( name<_T>::OpLen ) { return UInt64::Make( Size( ) ); }\
template<typename _T>\
UCMethod( name<_T>::OpClear ) { Clear( ); return nullptr; }\
template<typename _T>\
UCMethod( name<_T>::OpReverse , ( from , to ) ) { Reverse( UCAsUInt64( from ) , UCAsUInt64( to ) ); return nullptr; }\
template<typename _T>\
UCMethod( name<_T>::OpReverse ) { Reverse( ); return nullptr; }\
template<typename _T>\
UCMethod( name<_T>::OpResize , ( idx ) ) { Resize( UCAsUInt64( idx ) ); return nullptr; }\
template<typename _T>\
UCMethod( name<_T>::OpTrimExcess ) { TrimExcess( ); return nullptr; }\
template<typename _T>\
UCMethod( name<_T>::OpRemAt , ( idx ) ) { RemoveAt( UCAsUInt64( idx ) ); return nullptr; }\

#define __CONTAINER_NatEmplaceFront(coll, itm) coll.emplace_front(itm);
#define __CONTAINER_WrkAroundEmplaceFront(coll, itm) coll.emplace(coll.begin(), itm);

	__DEFINE_Container( Deque , std::deque , "<| " , " |>" , __CONTAINER_NatEmplaceFront );
	__DEFINE_Container( Vector , std::vector , "[[ " , " ]]" , __CONTAINER_WrkAroundEmplaceFront );
	__DEFINE_Container( BstDeque , boost::container::deque , "<< " , " >>" , __CONTAINER_NatEmplaceFront );
	__DEFINE_Container( BstVector , boost::container::vector , "[| " , " |]" , __CONTAINER_WrkAroundEmplaceFront );
#pragma endregion


#pragma region Associative Containers
#define __DEFINE_UnorderedContainer(name, underly_t, startBrace, endBrace)\
UCTemplateInterface( name , ( TKey , TVal ) , UC_WhereTypenameIs( "UC::" __ToString(name) ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );\
	UC_HasNativeCtorsAndEmptyCtor;\
	UC_HasMethods( ( OpClear ) , ( OpSize ) , ( OpLen ) );\
public:\
	using coll_t = underly_t<TKey , TVal, Hasher<TKey>>;\
	coll_t coll;\
\
	using hasher = typename coll_t::hasher;\
	using key_type = typename coll_t::key_type;\
	using mapped_type = typename coll_t::mapped_type;\
	using key_equal = typename coll_t::key_equal;\
\
	using value_type = typename coll_t::value_type;\
	using allocator_type = typename coll_t::allocator_type;\
	using size_type = typename coll_t::size_type;\
	using difference_type = typename coll_t::difference_type;\
	using pointer = typename coll_t::pointer;\
	using const_pointer = typename coll_t::const_pointer;\
	using reference = value_type & ;\
	using const_reference = const value_type&;\
	using iterator = typename coll_t::iterator;\
	using const_iterator = typename coll_t::const_iterator;\
\
	forceinline iterator begin( ) { return		coll.begin( ); }\
	forceinline const_iterator begin( ) const { return coll.begin( ); }\
	forceinline iterator end( ) { return		 coll.end( ); }\
	forceinline const_iterator end( ) const { return coll.end( ); }\
	forceinline const_iterator cbegin( ) const { return coll.begin( ); }\
	forceinline const_iterator cend( ) const { return coll.end( ); }\
\
	name( ) :coll {} { }\
	name( std::initializer_list<std::pair<const TKey , TVal>> ilist ) :coll(ilist) { }\
\
	const TVal& Get( const TKey& key ) const\
	{\
		auto itr = coll.find( key );\
		if ( itr == coll.end( ) )\
			throw IndexOutOfRangeException( ConcatNatStrings( "Key: \"" , key->ToString( ) , "\" is invalid for " , SGetTypeName( ) ) );\
		return itr->second;\
	}\
	TVal& Get( const TKey& key )\
	{\
		auto itr = coll.find( key );\
		if ( itr == coll.end( ) )\
			throw IndexOutOfRangeException( ConcatNatStrings( "Key: \"" , key->ToString( ) , "\" is invalid for " , SGetTypeName( ) ) );\
		return itr->second;\
	}\
	forceinline void Set( const TKey& key , TVal& val ) { coll[ key ] = val; }\
\
	template<typename _TKey , typename _TVal>\
	forceinline std::pair<iterator , bool> Add( _TKey&& key , _TVal&& val )\
	{ return coll.emplace( std::forward<_TKey>( key ) , std::forward<_TVal>( val ) ); }\
\
	NatString ToString( ) const override\
	{\
		NatString s( startBrace );\
		bool first = true;\
		for ( auto& i : coll )\
		{\
			if ( first ) { first = false; s += ConcatNatStrings( "[ " , i.first->ToString( ) , " ] = " , i.second->ToString( ) ); }\
			else s += ConcatNatStrings( ", [ " , i.first->ToString( ) , " ] = " , i.second->ToString( ) );\
		}\
		s += endBrace;\
		return std::move( s );\
	}\
	int64_t GetHashCode( ) const override\
	{\
		NatVector<size_t> hashes( Size( ) * 2 );\
		size_t idx = 0;\
		for ( auto& i : coll )\
		{\
			hashes[ idx ] = i.first->GetHashCode( );\
			++idx;\
			hashes[ idx ] = i.second->GetHashCode( );\
			++idx;\
		}\
		return CombineHashCodesInColl( hashes );\
	}\
\
	forceinline void Clear( ) { coll.clear( ); }\
	forceinline iterator Find( const TKey& key ) { return coll.find( key ); }\
	forceinline uint64_t Size( ) const { return coll.size( ); }\
	forceinline uint64_t Length( ) const { return coll.length( ); }\
\
	bool Remove( const TKey& key )\
	{\
		auto iter = coll.find( key );\
		if ( iter == coll.end( ) )return false;\
		coll.erase( iter );\
		return true;\
	}\
\
	template<typename... Args>\
	static pself MakeI(std::initializer_list<std::pair<const TKey , TVal>> ilist){return pself(new self(ilist));}\
\
UCEndInterface;\
\
template<typename TKey , typename TVal>\
UC_MTPMethod( ( name<TKey , TVal> ) , OpClear ) { Clear( ); return nullptr; }\
template<typename TKey , typename TVal>\
UC_MTPMethod( ( name<TKey , TVal> ) , OpSize ) { return Int64::Make( Size( ) ); }\
template<typename TKey , typename TVal>\
UC_MTPMethod( ( name<TKey , TVal> ) , OpLen ) { return Int64::Make( Size( ) ); }\


	__DEFINE_UnorderedContainer( UnorderedMap , std::unordered_map , "{|" , "|}" );
	__DEFINE_UnorderedContainer( BstUnorderedMap , boost::unordered_map , "{{" , "}}" );
#pragma endregion
#pragma endregion

	template<typename T , typename TFunctionToUse = void> struct _FunctionTypeDeducerImpl;

	template<typename TAssign , typename TFunc>
	typename _FunctionTypeDeducerImpl<TAssign , std::decay_t<TFunc>>::pbase MakeFunc( TFunc&& func )
	{
		using Deducer = _FunctionTypeDeducerImpl<TAssign , std::decay_t<TFunc>>;
		using TRet = typename Deducer::impl;
		using TRRet = typename Deducer::pbase;
		return TRRet( TRet::Make( std::forward<TFunc>( func ) ) );
	}


#pragma region Functor
	UCTemplateInterfaceWithPack( Functor , ( TReturn ) , TParams , UC_WhereTypenameIs( "UC::Functor" ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );
	UC_IsAbstract;
	UC_HasNoMethods;

	virtual TReturn Eval( TParams... params ) = 0;

	TReturn operator()( TParams... params )
	{
		if constexpr ( std::is_void_v<TReturn> ) Eval( params... );
		else return Eval( params... );
	};
	UCEndInterface;
#pragma endregion


#pragma region FunctorImpl
	UCTemplateInterfaceWithPack( FunctorImpl , ( TFunc , TReturn ) , TParams , UC_WhereTypenameIs( base0::SGetTypeName( ) ) , UC_InheritsUCClassesInBraces( ( Functor<TReturn , TParams...> ) ) , UC_InheritsNoNativeClasses , final );
	UC_OnlyHasNativeCtors;
	UC_HasNoMethods;

	virtual TReturn Eval( TParams... params ) override
	{
		if constexpr ( std::is_void_v<TReturn> ) std::invoke( func , params... );
		else return std::invoke( func , params... );
	};
protected:
	TFunc func;

	FunctorImpl( TFunc fun ) :func( fun ) { }
	UCEndInterface;
#pragma endregion


#pragma region Event
	UCTemplateInterfaceWithPack( Event , ( TReturn ) , TParams , UC_WhereTypenameIs( "UC::Event" ) , UC_InheritsUCClassesInBraces( ( Functor<TReturn , TParams...> ) ) , UC_InheritsNoNativeClasses , final );
	UC_OnlyHasNativeCtors;
	UC_HasNoMethods;

	using func_t = P<Functor<TReturn , TParams...>>;

	using col_t = std::list<func_t>;
	typedef typename col_t::value_type value_type;
	typedef typename col_t::reference reference;
	typedef typename col_t::const_reference const_reference;
	typedef typename col_t::iterator iterator;
	typedef typename col_t::const_iterator const_iterator;
	typedef typename col_t::reverse_iterator reverse_iterator;
	typedef typename col_t::const_reverse_iterator const_reverse_iterator;
	typedef typename col_t::size_type size_type;
	typedef typename col_t::difference_type difference_type;

	using func_id = iterator;

	forceinline iterator begin( ) { return		lst.begin( ); }
	forceinline const_iterator begin( ) const { return lst.begin( ); }
	forceinline iterator end( ) { return		 lst.end( ); }
	forceinline const_iterator end( ) const { return lst.end( ); }
	forceinline const_iterator cbegin( ) const { return lst.begin( ); }
	forceinline const_iterator cend( ) const { return lst.end( ); }

	forceinline reverse_iterator rbegin( ) { return lst.rbegin( ); }
	forceinline const_reverse_iterator rbegin( ) const { return lst.rbegin( ); }
	forceinline reverse_iterator rend( ) { return lst.rend( ); }
	forceinline const_reverse_iterator rend( ) const { return lst.rend( ); }
	forceinline const_reverse_iterator crbegin( ) const { return lst.rbegin( ); }
	forceinline const_reverse_iterator crend( ) const { return lst.rend( ); }

	virtual TReturn Eval( TParams... params ) override
	{
		if constexpr ( std::is_void_v<TReturn> )
		{
			for ( auto& func : lst ) func->Eval( params... );
		}
		else
		{
			const auto sz = Size( );
			const auto szm1 = sz - 1;
			auto it = lst.begin( );
			for ( size_t i = 0; i < sz; i++ , it++ )
			{
				if ( szm1 == i )return ( *it )->Eval( params... );
				( *it )->Eval( params... );
			}
			throw NoFunctorsAddedToEvent_Exception( ConcatNatStrings( SGetTypeName( ) , " has no added functors that can return a value that can be returned." ) );
		}
	};

	forceinline void __evalAll( std::true_type , TParams... params )
	{
		for ( auto& func : lst ) func->Eval( params... );
	}
	forceinline NatVector<TReturn> __evalAll( std::false_type , TParams... params )
	{
		NatVector<TReturn> vec;
		vec.reserve( Size( ) );
		for ( auto& func : lst ) vec.emplace_back( func->Eval( params... ) );
		return std::move( vec );
	}

	std::conditional_t<std::is_void_v<TReturn> , void , NatVector<TReturn>> EvalAll( TParams... params )
	{
		return __evalAll( std::is_void<TReturn>( ) , std::forward<TParams>( params )... );
	}

	template<typename TFunc> func_id Add( TFunc&& func )
	{
		using TRet = typename _FunctionTypeDeducerImpl<func_t , std::decay_t<TFunc>>::impl;
		lst.emplace_back( TRet::Make( std::forward<TFunc>( func ) ) );
		return --lst.end( );
	}

	func_id AddF( func_t func )
	{
		lst.emplace_back( std::move( func ) );
		return --lst.end( );
	}

	forceinline uint64_t Size( ) { return lst.size( ); }

	forceinline void Clear( ) { lst.clear( ); }

	forceinline void Remove( func_id id ) { lst.erase( id ); }

protected:
	Event( ) :lst( ) { }
	std::list<func_t> lst;
	UCEndInterface;
#pragma endregion


#pragma region _FunctionTypeDeducerImpl
#define __DEFINE__FunctionTypeDeducerImpl(...)\
template<typename TFunctionToUse , typename TReturn , typename... TParams>\
struct _FunctionTypeDeducerImpl<__VA_ARGS__ , TFunctionToUse>\
{\
	using impl = FunctorImpl<TFunctionToUse , TReturn , TParams...>;\
	using base = Functor<TReturn , TParams...>;\
	using evnt = Event<TReturn , TParams...>;\
	using pevnt = P<Event<TReturn , TParams...>>;\
	using pbase = P<Functor<TReturn , TParams...>>;\
};
	__DEFINE__FunctionTypeDeducerImpl( W<Functor<TReturn , TParams...>> );
	__DEFINE__FunctionTypeDeducerImpl( P<Functor<TReturn , TParams...>> );
	__DEFINE__FunctionTypeDeducerImpl( Functor<TReturn , TParams...> );
	__DEFINE__FunctionTypeDeducerImpl( W<Event<TReturn , TParams...>> );
	__DEFINE__FunctionTypeDeducerImpl( P<Event<TReturn , TParams...>> );
	__DEFINE__FunctionTypeDeducerImpl( Event<TReturn , TParams...> );
	__DEFINE__FunctionTypeDeducerImpl( TReturn( TParams... ) );
	__DEFINE__FunctionTypeDeducerImpl( TReturn( *)( TParams... ) );
	__DEFINE__FunctionTypeDeducerImpl( TReturn( &)( TParams... ) );

#undef __DEFINE__FunctionTypeDeducerImpl

#define __DEFINE__FunctionTypeDeducerImplWithVoidTFunctionToUse(...)\
template<typename TReturn , typename... TParams>\
struct _FunctionTypeDeducerImpl<__VA_ARGS__>\
{\
	using base = Functor<TReturn , TParams...>;\
	using evnt = Event<TReturn , TParams...>;\
	using pevnt = P<Event<TReturn , TParams...>>;\
	using pbase = P<Functor<TReturn , TParams...>>;\
};

	__DEFINE__FunctionTypeDeducerImplWithVoidTFunctionToUse( W<Functor<TReturn , TParams...>> );
	__DEFINE__FunctionTypeDeducerImplWithVoidTFunctionToUse( P<Functor<TReturn , TParams...>> );
	__DEFINE__FunctionTypeDeducerImplWithVoidTFunctionToUse( Functor<TReturn , TParams...> );
	__DEFINE__FunctionTypeDeducerImplWithVoidTFunctionToUse( W<Event<TReturn , TParams...>> );
	__DEFINE__FunctionTypeDeducerImplWithVoidTFunctionToUse( P<Event<TReturn , TParams...>> );
	__DEFINE__FunctionTypeDeducerImplWithVoidTFunctionToUse( Event<TReturn , TParams...> );
	__DEFINE__FunctionTypeDeducerImplWithVoidTFunctionToUse( TReturn( TParams... ) );
	__DEFINE__FunctionTypeDeducerImplWithVoidTFunctionToUse( TReturn( *)( TParams... ) );
	__DEFINE__FunctionTypeDeducerImplWithVoidTFunctionToUse( TReturn( &)( TParams... ) );

#undef __DEFINE__FunctionTypeDeducerImplWithVoidTFunctionToUse
#pragma endregion

	template<typename TFunc> using EventFrom = typename _FunctionTypeDeducerImpl<TFunc>::evnt;
	template<typename TFunc> using FuncFrom = typename _FunctionTypeDeducerImpl<TFunc>::base;

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

	namespace IntLiterals
	{
	#define __DEFINE_IntLiterals(name, cname, undTupe) \
		static P<name> operator""_##cname(unsigned long long int param){\
			return name::Make(static_cast<undTupe>(param));\
		}

		__DEFINE_IntLiterals( Byte , b , UC::byte );
		__DEFINE_IntLiterals( SByte , sb , UC::sbyte );
		__DEFINE_IntLiterals( Int16 , i16 , int16_t );
		__DEFINE_IntLiterals( Int32 , i32 , int32_t );
		__DEFINE_IntLiterals( Int64 , i64 , int64_t );
		__DEFINE_IntLiterals( UInt16 , ui16 , uint16_t );
		__DEFINE_IntLiterals( UInt32 , ui32 , uint32_t );
		__DEFINE_IntLiterals( UInt64 , ui64 , uint64_t );

	#undef __DEFINE_IntLiterals
	}

	namespace FltLiterals
	{
	#define __DEFINE_FltLiterals(name, cname, undTupe) \
		static P<name> operator""_##cname(long double param){return name::Make(static_cast<undTupe>(param));}

		__DEFINE_FltLiterals( Float , flt , float );
		__DEFINE_FltLiterals( Double , dbl , double );

	#undef __DEFINE_FltLiterals
	}

	namespace StrLiterals
	{
		forceinline static P<String> operator""_us( const char* param , std::size_t ) { return String::Make( param ); }
	}

	namespace AnalogLiterals
	{
		template<uint32_t i , uint16_t s>struct Accumulator { };
		template<uint16_t s>
		struct Delimiter { };

		template<uint64_t i> struct Line
		{
			enum : int64_t { v = i };

			template<uint64_t i2>
			forceinline constexpr Line<i + i2> operator+( const Line<i2>& ) const
			{ return Line<i + i2>; }
		};

		template<uint64_t i , uint16_t s>
		forceinline constexpr static Accumulator<i + 1 , s> operator*( const Accumulator<i , s>& )
		{ return Accumulator<i + 1 , s>( ); }

		template<uint64_t i , uint16_t s>
		forceinline constexpr static Line<( i + 1 )*s> operator*( const Delimiter<s>& , const Accumulator<i , s>& )
		{ return Line<( i + 1 )*s>( ); }

		template<uint16_t s>
		forceinline constexpr static Line<s> operator*( const Delimiter<s>& , const Delimiter<s>& )
		{ return Line<s>( ); }
		template<uint16_t s>
		forceinline constexpr static Accumulator<1 , s> operator*( const Delimiter<s>& )
		{ return Accumulator<1 , s>( ); }

		constexpr static Line<0> ii;
		constexpr static Line<0> vv;
		constexpr static Line<0> xx;
		constexpr static Line<0> ll;
		constexpr static Line<0> cc;
		constexpr static Line<0> dd;
		constexpr static Line<0> mm;
		constexpr static Delimiter<1> i;
		constexpr static Delimiter<5> v;
		constexpr static Delimiter<10> x;
		constexpr static Delimiter<50> l;
		constexpr static Delimiter<100> c;
		constexpr static Delimiter<500> d;
		constexpr static Delimiter<1000> m;
	}

	forceinline static NatString to_string( const Object& obj ) { return obj.ToString( ); }
}

namespace std
{
	template<typename T> struct hash<UC::GCPtr<T>>
	{
		typedef UC::GCPtr<T> argument_type;
		typedef size_t result_type;
		___UC_NODISCARD___ size_t operator()( const UC::GCPtr<T> v ) const noexcept { return UC::_HashGCP( )( v ); }
	};

	template<typename T> inline ostream& operator<<( ostream& o , const UC::GCPtr<T>& p )
	{ return o << to_string( *p ); }

	static inline ostream& operator<<( ostream& o , nullptr_t )
	{ return o << "nullptr"; }

	static inline ostream& operator<<( ostream& o , const std::exception& ex )
	{ return o << boost::typeindex::type_id_runtime( ex ).pretty_name( ) << ":" << ex.what( ); }

	static inline ostream& operator<<( ostream& o , const UC::Exception& ex )
	{ return o << boost::typeindex::type_id_runtime( ex ).pretty_name( ) << ":" << ex.Message( ) << endl << "at stack position:" << endl << ex.GetStackTrace( ); }

	static ostream& operator<<( ostream& o , const exception_ptr& ex )
	{
		if ( !ex ) { return o << "null exception"; }
		try
		{
			std::rethrow_exception( ex );
		}
		catch ( const UC::Exception& ex )
		{
			return o << ex;
		}
		catch ( const std::exception& ex )
		{
			return o << ex;
		}
		catch ( ... )
		{
			return o << "std::exception_ptr @ " << &ex;
		}
	}
}
#endif // !__UC__OBJECT__HPP__
