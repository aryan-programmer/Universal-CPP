#include "stdafx.h"
#include "Object.hpp"

namespace UC
{
	Object::~Object( ) = default;
	PAny Object::CreateInstance( NatString className , NatODeque args )
	{
		return getClassCtors( ).at( className )( args );
	}
	Object::Object( ) = default;
	void Object::addConstructor( NatString className , PAny( *ctor )( NatODeque args ) )
	{
		if ( getClassCtors( ).emplace( className , ctor ).second == false )
			throw RepeatingClassNameException( std::move( className ) );
	}

	std::unordered_map<NatString , PAny( *)( NatODeque args )>& Object::getClassCtors( )
	{
		static std::unordered_map<NatString , PAny( *)( NatODeque args )> classCtors {};
		return classCtors;
	}
}
