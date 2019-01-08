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
	auto emp = Empty::Make( P_Any( ) );
	// Classic conventional Call
	emp->Do( );
	emp->Do( nullptr );
	emp->Do( nullptr , nullptr );
	// Dynamic Call using "Call"
	emp->Call( func , { } );
	emp->Call( func , { nullptr } );
	emp->Call( func , { nullptr,nullptr } );

	// Create instance of Empty using reflection
	P_Any oemp = Object::CreateInstance( "Empty" , { nullptr } );
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

	P_String
		s1 = String::Make( " ABCD" ) ,
		s2 = String::Make( " EFG" ) ,
		s3 = String::Make( " HIJK" ) ,
		s4 = String::Make( " LMNOP" ) ,
		s5 = String::Make( " QRS" ) ,
		s6 = String::Make( " TUV" );
	NatString s7 = " WX";
	auto s8 = " Y & Z";
	auto& s9 = "DDB6C4E735604F4943D98C240DE36D4235200D52A89EC59C5F4B447AFA1AD305A89F5B141FEA67BD080F7B9BF11103A41CAA0CCD9DC211407448892FC23F91A6";

	for ( size_t i = 0; i < 10; i++ )
	{
		{
			auto p = std::chrono::steady_clock::now( );
			auto alphabets = String::Concat(
				s1 , s2 , s3 , s4 , s5 , s6 , s7 , s8 , s9 ,
				s1 , s2 , s3 , s4 , s5 , s6 , s7 , s8 , s9 ,
				s1 , s2 , s3 , s4 , s5 , s6 , s7 , s8 , s9 ,
				s1 , s2 , s3 , s4 , s5 , s6 , s7 , s8 , s9 ,
				s1 , s2 , s3 , s4 , s5 , s6 , s7 , s8 , s9 ,
				String::Make( "Null" ) );

			auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
			cout << std::setw( 7 ) << diff << endl;
		}

		//!+ Don't do this,
		{
			auto p = std::chrono::steady_clock::now( );
			auto alphabets =
				*( *( *( *( *( *( *( *( *( *( *( *s1 + *s2 ) + *s3 ) + *s4 ) + *s5 ) + *s6 ) + *String::Make( s7 ) ) + *String::Make( s8 ) ) + *String::Make( s9 ) ) +
						 *( *( *( *( *( *( *( *( *s1 + *s2 ) + *s3 ) + *s4 ) + *s5 ) + *s6 ) + *String::Make( s7 ) ) + *String::Make( s8 ) ) + *String::Make( s9 ) ) ) +
					  *( *( *( *( *( *( *( *( *s1 + *s2 ) + *s3 ) + *s4 ) + *s5 ) + *s6 ) + *String::Make( s7 ) ) + *String::Make( s8 ) ) + *String::Make( s9 ) ) ) +
				   *( *( *( *( *( *( *( *( *s1 + *s2 ) + *s3 ) + *s4 ) + *s5 ) + *s6 ) + *String::Make( s7 ) ) + *String::Make( s8 ) ) + *String::Make( s9 ) ) ) +
				*( *( *( *( *( *( *( *( *s1 + *s2 ) + *s3 ) + *s4 ) + *s5 ) + *s6 ) + *String::Make( s7 ) ) + *String::Make( s8 ) ) + *String::Make( s9 ) );

			auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
			cout << std::setw( 7 ) << diff << endl << endl;
		}
	}

	return 0;
}
