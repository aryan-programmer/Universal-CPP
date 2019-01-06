// UC++ Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Empty.hpp"


using namespace std;
using namespace UC;


int main( )
{
	const char* func = "Do";
	// Make object
	auto emp = Empty::make( PAny( ) );
	// Classic conventional Call
	emp->Do( );
	emp->Do( nullptr );
	emp->Do( nullptr , nullptr );
	// Dynamic Call using "Call"
	emp->Call( func , { } );
	emp->Call( func , { nullptr } );
	emp->Call( func , { nullptr,nullptr } );

	// Create instance of Empty using reflection
	PAny oemp = Object::CreateInstance( "Empty" , { nullptr } );
	// Call conventionally, but,
	//!+ Conventional style Call not possible for Object
	/*
	oemp->Do( );
	oemp->Do( nullptr );
	oemp->Do( nullptr , nullptr );
	*/
	// Call using "Call"
	//!+ This will work however:
	oemp->Call( func , { } );
	oemp->Call( func , { nullptr } );
	oemp->Call( func , { nullptr,nullptr } );

	// Can dynamically cast as well
	auto newEmp = ObjCast< Empty >( oemp );

	// Type-names
	cout
		<< oemp->GetTypeName( ) << endl
		<< emp->GetTypeName( ) << endl
		<< newEmp->GetTypeName( ) << endl;

	// Native base classes
	emp->nbase0::NativeBaseFunction( );
	newEmp->nbase0::NativeBaseFunction( );

	// Native functions
	emp->NativeFunction( );
	newEmp->NativeFunction( );

	return 0;
}
