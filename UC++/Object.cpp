#include "stdafx.h"
#include "Object.hpp"

namespace UC
{
	std::unordered_map<NatString , Object::CtorT>& getClassCtors( )
	{
		static std::unordered_map<NatString , Object::CtorT> classCtors {};
		return classCtors;
	}
	Object::~Object( ) = default;
	NatString Object::ToString( ) const { return GetTypeName( ); }
	int64_t Object::GetHashCode( ) const { return Hash( ME ); }
	P<Object> Object::CreateInstance( const NatString& className , const NatOVector& args )
	{
		decltype( auto ) map = getClassCtors( );
		auto findName = className.substr( 0 , className.find( "<" ) );
		auto itr = map.find( findName );
		if ( itr != map.end( ) ) return itr->second( args );
		else
			throw UC::NoSuchRegisteredType_Exception( UC::ConcatNatStrings(
				"There is no *registered* type with the name \"" ,
				findName ,
				"\", let alone a *registered* constructor for it that takes in " ,
				std::to_string( args.size( ) ) ,
				" parameters." ) );
	}
	Object::Object( ) = default;
	void Object::addConstructor( const NatString& className , CtorT ctor )
	{
		decltype( auto ) map = getClassCtors( );
		if ( map.emplace( className , ctor ).second == false )
			throw RepeatingClassNameException( std::move( className ) );
	}

	UCRegister( Int16 );
	UCRegister( Int32 );
	UCRegister( Int64 );
	UCRegister( UInt16 );
	UCRegister( UInt32 );
	UCRegister( UInt64 );

	UCRegister( Float );
	UCRegister( Double );

	UCRegister( Byte );
	UCRegister( SByte );
	UCRegister( Bool );

	UCRegister( String );

	UCMethod( String::ULength ) { return Int64::Make( NLength( ) ); }
	UCMethod( String::UContains , ( s ) ) { return Bool::Make( NContains( UCCast( String , s ) ) ); }
	UCMethod( String::UEndsWith , ( s ) ) { return Bool::Make( NEndsWith( UCCast( String , s ) ) ); }
	UCMethod( String::UStartsWith , ( s ) )
	{ return Bool::Make( NStartsWith( UCCast( String , s ) ) ); }
	UCMethod( String::UEquals , ( s ) ) { return Bool::Make( NEquals( UCCast( String , s ) ) ); }
	UCMethod( String::USubstring , ( startIdx ) ) { return NSubstring( UCAsUInt64( startIdx ) ); }
	UCMethod( String::USubstring , ( startIdx , count ) )
	{ return NSubstring( UCAsUInt64( startIdx ) , UCAsUInt64( count ) ); }

	UCMethod( String::Get , ( Idx ) ) { return Byte::Make( static_cast< byte >( value[ UCAsUInt64( Idx ) ] ) ); }
	UCMethod( String::OpAdd , ( _1 ) ) { return Concat( ME , UCCast( String , _1 ) ); }
	UCMethod( String::OpAdd , ( _1 , _2 ) ) { return Concat( ME , UCCast( String , _1 ) , UCCast( String , _2 ) ); }
	UCMethod( String::OpAdd , ( _1 , _2 , _3 ) ) { return Concat( ME , UCCast( String , _1 ) , UCCast( String , _2 ) , UCCast( String , _3 ) ); }
	UCMethod( String::OpAdd , ( _1 , _2 , _3 , _4 ) ) { return Concat( ME , UCCast( String , _1 ) , UCCast( String , _2 ) , UCCast( String , _3 ) , UCCast( String , _4 ) ); }
	UCMethod( String::OpAdd , ( _1 , _2 , _3 , _4 , _5 ) ) { return Concat( ME , UCCast( String , _1 ) , UCCast( String , _2 ) , UCCast( String , _3 ) , UCCast( String , _4 ) , UCCast( String , _5 ) ); }
	UCMethod( String::OpAdd , ( _1 , _2 , _3 , _4 , _5 , _6 ) ) { return Concat( ME , UCCast( String , _1 ) , UCCast( String , _2 ) , UCCast( String , _3 ) , UCCast( String , _4 ) , UCCast( String , _5 ) , UCCast( String , _6 ) ); }
	UCMethod( String::OpAdd , ( _1 , _2 , _3 , _4 , _5 , _6 , _7 ) ) { return Concat( ME , UCCast( String , _1 ) , UCCast( String , _2 ) , UCCast( String , _3 ) , UCCast( String , _4 ) , UCCast( String , _5 ) , UCCast( String , _6 ) , UCCast( String , _7 ) ); }
	UCMethod( String::OpAdd , ( _1 , _2 , _3 , _4 , _5 , _6 , _7 , _8 ) ) { return Concat( ME , UCCast( String , _1 ) , UCCast( String , _2 ) , UCCast( String , _3 ) , UCCast( String , _4 ) , UCCast( String , _5 ) , UCCast( String , _6 ) , UCCast( String , _7 ) , UCCast( String , _8 ) ); }
	UCMethod( String::OpAdd , ( _1 , _2 , _3 , _4 , _5 , _6 , _7 , _8 , _9 ) ) { return Concat( ME , UCCast( String , _1 ) , UCCast( String , _2 ) , UCCast( String , _3 ) , UCCast( String , _4 ) , UCCast( String , _5 ) , UCCast( String , _6 ) , UCCast( String , _7 ) , UCCast( String , _8 ) , UCCast( String , _9 ) ); }
	UCMethod( String::OpAdd , ( _1 , _2 , _3 , _4 , _5 , _6 , _7 , _8 , _9 , _10 ) ) { return Concat( ME , UCCast( String , _1 ) , UCCast( String , _2 ) , UCCast( String , _3 ) , UCCast( String , _4 ) , UCCast( String , _5 ) , UCCast( String , _6 ) , UCCast( String , _7 ) , UCCast( String , _8 ) , UCCast( String , _9 ) , UCCast( String , _10 ) ); }

	UCRegisterTemplate( Deque , <P<Object>> );
	UCRegisterTemplate( Vector , <P<Object>> );
	UCRegisterTemplate( BstDeque , <P<Object>> );
	UCRegisterTemplate( BstVector , <P<Object>> );

	UCRegisterTemplate( UnorderedMap , <P<Object> , P<Object>> );
	UCRegisterTemplate( BstUnorderedMap , <P<Object> , P<Object>> );
}
