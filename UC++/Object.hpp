#pragma once
#include "stdafx.h"

#include "Interface.hpp"
#include "byte.hpp"


namespace UC
{
	/// <summary>
	/// This is the base class of all <seealso cref="UCInterface"/>s.
	/// It is the root of the UC++ type hierarchy.
	/// </summary>
	/// <seealso cref="EnableGCPtrFromMe{Object}" />
	class Object : public EnableGCPtrFromMe<Object>
	{
	public:
		virtual ~Object( );

		virtual const char* GetTypeName( ) = 0;
		virtual PAny Call( NatString fname , NatODeque args ) = 0;

		Object( Object&& ) = delete;
		Object& operator=( Object&& ) = delete;

		static PAny CreateInstance( NatString className , NatODeque args );
	protected:
		Object( );
		forceinline PAny callImpl( NatString fname , NatODeque args ) { return nullptr; }
		static void addConstructor( NatString className , PAny( *ctor )( NatODeque args ) );
	private:
		static std::unordered_map<NatString , PAny( *)( NatODeque args )>& getClassCtors( );
	};

	/// <summary>
	/// "Native String"
	/// </summary>
	using NatString = std::string;

	class Exception : public std::exception
	{
		using base = std::exception;
		NatString str;
	public:
		Exception( NatString&& str ) noexcept : base( ) , str( str ) { }
		Exception( const NatString& str ) noexcept : base( ) , str( str ) { }
		virtual char const* what( ) const override { return str.c_str( ); }
		const NatString& Message( ) { return str; }
	};


#pragma region Exceptions
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
#pragma endregion


#pragma region Smart Pointers
	/// <summary>
	/// GCPtr is a class for reference counted resource management/ARC (Automatic Reference Counting).
	/// It holds a strong reference to the object inside it.
	/// </summary>
	template<typename T> class GCPtr
	{
	public:
		std::shared_ptr<T> ptr;
		using element_type = T;

		void NoValueThenThrow( ) const { if ( !HasValue( ) )throw NullPointerException( "Trying to dereference a null pointer." ); }

		template<class T2>
		forceinline GCPtr( const GCPtr<T2>& p ) noexcept :ptr { p.ptr } { }
		forceinline GCPtr( std::shared_ptr<T>&& p ) : ptr { p } { }
		forceinline GCPtr( const std::shared_ptr<T>& p ) : ptr { p } { }

		forceinline GCPtr( ) noexcept :ptr {  } { }
		forceinline GCPtr( nullptr_t ) noexcept :ptr { } { }
		forceinline explicit GCPtr( T* value ) : ptr { value } { }
		forceinline explicit GCPtr( const T& value ) : ptr { new T( value ) } { }

		forceinline explicit operator bool( ) const noexcept { return ptr != nullptr; }
		forceinline GCPtr<T>& Reset( ) noexcept { ptr.reset( ); return *this; }
		forceinline GCPtr<T>& Reset( T* value ) { ptr.reset( value ); return *this; }
		forceinline GCPtr<T>& Reset( const T& value )
		{
			ptr.reset( new T( value ) );
			return *this;
		}

		forceinline GCPtr<T>& operator*( ) { NoValueThenThrow( ); return *this; }
		forceinline const GCPtr<T>& operator*( ) const { NoValueThenThrow( ); return *this; }
		forceinline T* operator->( ) { NoValueThenThrow( ); return ptr.operator->( ); }
		forceinline const T* operator->( ) const { NoValueThenThrow( ); return ptr.operator->( ); }

		forceinline bool HasValue( ) const noexcept { return ptr != nullptr; }

		forceinline bool operator!=( nullptr_t )const noexcept { return ptr != nullptr; }
		forceinline bool operator==( nullptr_t )const noexcept { return ptr == nullptr; }

		forceinline bool operator ==( GCPtr<T> ptr2 ) { return Equals( ptr2 ); }
		forceinline bool operator !=( GCPtr<T> ptr2 ) { return !Equals( ptr2 ); }

		forceinline bool Equals( GCPtr<T> other ) { return ptr == other.ptr; }
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


	// PAny = Pointer to Any (U++) value
	using PAny = GCPtr<Object>;
	// WAny = Weakly pointer to Any (emc2) value
	using WAny = WeakPtr<Object>;

	// NatODeque = Native Object Deque (Deque = Double Ended Queue)
	using NatODeque = std::deque<PAny>;

	/// <summary>
	/// Represents a garbage collectible object of type T.
	/// </summary>
	template<typename T>using GCP = GCPtr<T>;

#pragma region Integral Placeholder Interfaces
#define __DEFINE_integralPlaceHolderInterfaces(name, underlyingType)\
UCInterface( name , UC_WhereTypenameIs( "UC::" __ToString(name) ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );\
	UC_OnlyHasEmptyCtor;\
	UC_HasNoMethods;\
public:\
	underlyingType value;\
	forceinline name(underlyingType value=0):value{value }{}\
	forceinline static GCP<name> make( underlyingType value ) { return GCP<name>( new name(value) ); }\
UCEndInterface

	__DEFINE_integralPlaceHolderInterfaces( Int16 , int16_t );
	__DEFINE_integralPlaceHolderInterfaces( Int32 , int32_t );
	__DEFINE_integralPlaceHolderInterfaces( Int64 , int64_t );
	__DEFINE_integralPlaceHolderInterfaces( UInt16 , uint16_t );
	__DEFINE_integralPlaceHolderInterfaces( UInt32 , uint32_t );
	__DEFINE_integralPlaceHolderInterfaces( UInt64 , uint64_t );

#undef __DEFINE_integralPlaceHolderInterfaces  
#pragma endregion


#pragma region Floating Point Number Placeholder Interfaces
#define __DEFINE_floatingPointPlaceHolderInterfaces(name, underlyingType, default_)\
UCInterface( name , UC_WhereTypenameIs( "UC::" __ToString(name) ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );\
	UC_OnlyHasEmptyCtor;\
	UC_HasNoMethods;\
public:\
	underlyingType value;\
	forceinline name(underlyingType value=default_):value{value }{}\
	forceinline static GCP<name> make( underlyingType value ) { return GCP<name>( new name(value) ); }\
UCEndInterface

	__DEFINE_floatingPointPlaceHolderInterfaces( Single , float , 0.0f );
	__DEFINE_floatingPointPlaceHolderInterfaces( Double , double , 0.0 );

#undef __DEFINE_floatingPointPlaceHolderInterfaces
#pragma endregion


#pragma region byte and sbyte Placeholder Interfaces
#define __DEFINE_byteTypePlaceHolderInterfaces(name, underlyingType)\
UCInterface( name , UC_WhereTypenameIs( "UC::" __ToString(name) ) , UC_InheritsUCClasses( Object ) , UC_InheritsNoNativeClasses );\
	UC_OnlyHasEmptyCtor;\
	UC_HasNoMethods;\
public:\
	underlyingType value;\
	forceinline name():value{}{}\
	forceinline name(underlyingType value):value{value }{}\
	forceinline static GCP<name> make( underlyingType value ) { return GCP<name>( new name(value) ); }\
UCEndInterface

	__DEFINE_byteTypePlaceHolderInterfaces( Byte , byte );
	__DEFINE_byteTypePlaceHolderInterfaces( SByte , sbyte );

#undef __DEFINE_byteTypePlaceHolderInterfaces  
#pragma endregion
}

namespace UC
{
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
}
