#include "stdafx.h"
#include "Object.hpp"

namespace UC
{
	Object::~Object( ) = default;
	P_Any Object::CreateInstance( const NatString& className , const NatODeque& args )
	{
		return getClassCtors( ).at( className )( args );
	}
	Object::Object( ) = default;
	void Object::addConstructor( const NatString& className , P_Any( *ctor )( const NatODeque& args ) )
	{
		if ( getClassCtors( ).emplace( className , ctor ).second == false )
			throw RepeatingClassNameException( std::move( className ) );
	}

	std::unordered_map<NatString , P_Any( *)( const NatODeque& args )>& Object::getClassCtors( )
	{
		static std::unordered_map<NatString , P_Any( *)( const NatODeque& args )> classCtors {};
		return classCtors;
	}

	UCMethod( String::ULength ) { return Int64::Make( NLength( ) ); }
	UCMethod( String::UContains , ( s ) ) { return Bool::Make( NContains( UCCast( String , s ) ) ); }
	UCMethod( String::UEndsWith , ( s ) ) { return Bool::Make( NEndsWith( UCCast( String , s ) ) ); }
	UCMethod( String::UStartsWith , ( s ) ) 
	{ return Bool::Make( NStartsWith( UCCast( String , s ) ) ); }
	UCMethod( String::UEquals , ( s ) ) { return Bool::Make( NEquals( UCCast( String , s ) ) ); }
	UCMethod( String::USubstring , ( startIdx ) ) { return NSubstring( UCAsUInt64( startIdx ) ); }
	UCMethod( String::USubstring , ( startIdx , count ) )
	{ return NSubstring( UCAsUInt64( startIdx ) , UCAsUInt64( count ) ); }
}
