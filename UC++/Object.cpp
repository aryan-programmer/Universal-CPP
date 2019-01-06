#include "stdafx.h"
#include "Object.hpp"

namespace UC
{
	Object::~Object( ) = default;
	PAny Object::CreateInstance( String className , GCOVect args )
	{
		auto& _ = getClassCtors( );
		return getClassCtors( ).at( className )( args );
	}
	Object::Object( ) = default;
	PAny Object::callImpl( String fname , GCOVect args ) { return nullptr; }


	void Object::addConstructor( String className , PAny( *ctor )( GCOVect args ) )
	{
		if ( getClassCtors( ).emplace( className , ctor ).second == false )
			throw RepeatingClassNameException( std::move( className ) );
	}

	std::unordered_map<String , PAny( *)( GCOVect args )>& Object::getClassCtors( )
	{
		static std::unordered_map<String , PAny( *)( GCOVect args )> classCtors {};
		return classCtors;
	}
}
