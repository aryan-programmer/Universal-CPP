// UC++ Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Empty.hpp"
#define BOOST_TEST_MODULE UC++ Test
#include <boost/test/included/unit_test.hpp>


using namespace std;
using namespace UC;

struct MyConfig
{
	MyConfig( ) { boost::unit_test::unit_test_log.set_stream( std::cout ); }
};

BOOST_TEST_GLOBAL_CONFIGURATION( MyConfig );

BOOST_AUTO_TEST_SUITE( reflection_and_dynamic_typing )

const char* func = "Do";

BOOST_AUTO_TEST_CASE( function_reflection )
{
	cout << endl << endl << "Function Reflection" << endl;

	// Make object
	auto emp = Empty::Make( P_Any( ) );
	// Classic conventional Call
	BOOST_TEST( UCAsInt32( emp->Do( ) ) == 0 );
	BOOST_TEST( UCAsInt32( emp->Do( nullptr ) ) == 1 );
	BOOST_TEST( UCAsInt32( emp->Do( nullptr , nullptr ) ) == 2 );
	// Dynamic Call using "Call"
	BOOST_TEST( UCAsInt32( UCC( emp , func , { } ) ) == 0 );
	BOOST_TEST( UCAsInt32( UCC( emp , func , { nullptr } ) ) == 1 );
	BOOST_TEST( UCAsInt32( UCC( emp , func , { nullptr,nullptr } ) ) == 2 );

	BOOST_CHECK_THROW( UCC( emp , func , { nullptr,nullptr,nullptr } ) , UC::NoSuchFunction_Exception );
	BOOST_CHECK_THROW( UCC( emp , "NoDo" , { nullptr,nullptr,nullptr } ) , UC::NoSuchFunction_Exception );
}

BOOST_AUTO_TEST_CASE( class_and_function_reflection )
{
	cout << endl << endl << "Class And Function Reflection" << endl;

	BOOST_CHECK_THROW( Object::CreateInstance( "Empty" , { nullptr,nullptr } ) , UC::NoSuchConstructor_Exception );
	BOOST_CHECK_THROW( Object::CreateInstance( "TypeDoesntExist" , {} ) , UC::NoSuchRegisteredType_Exception );

	BOOST_TEST( Object::CreateInstance( "UC::Int64" , { } ) != nullptr );

	P_Any oemp = Object::CreateInstance( "Empty" , { nullptr } );

	// Create instance of Empty using reflection
	// Call conventionally, but,
	//!+ Conventional style Call not possible for Object
	/*
	oemp->Do( );
	oemp->Do( nullptr );
	oemp->Do( nullptr , nullptr );
	*/
	// Call using "Call"
	//!+ This will work however:
	BOOST_TEST( UCAsInt32( UCC( oemp , func , { } ) ) == 0 );
	BOOST_TEST( UCAsInt32( UCC( oemp , func , { nullptr } ) ) == 1 );
	BOOST_TEST( UCAsInt32( UCC( oemp , func , { nullptr,nullptr } ) ) == 2 );

	// Can dynamically cast as well
	auto newEmp = ObjCast< Empty >( oemp );

	BOOST_TEST( newEmp != nullptr );

	// Type-names
	cout
		<< oemp->GetTypeName( ) << endl
		<< newEmp->GetTypeName( ) << endl;

	BOOST_TEST( oemp->GetTypeName( ) == newEmp->GetTypeName( ) );

	// Native base classes
	BOOST_TEST( newEmp->nbase0::NativeBaseFunction( ) == 0 );

	// Native functions
	BOOST_TEST( newEmp->NativeFunction( ) == 0 );
}

BOOST_AUTO_TEST_SUITE_END( )

BOOST_AUTO_TEST_SUITE( containers_and_utility_functions )

BOOST_AUTO_TEST_CASE( NatStringConcat )
{
	cout << endl << endl << "NatString UC::ConcatNatStrings test" << endl;
	auto
		s1 = NatString( "ABCD" ) ,
		s2 = NatString( " EFG" ) ,
		s3 = NatString( " HIJK" ) ,
		s4 = NatString( " LMNOP" ) ,
		s5 = NatString( " QRS" ) ,
		s6 = NatString( " TUV" );
	NatString s7 = " WX";
	auto s8 = " Y & Z";
	auto& s9 = " DDB6C4E735604F4943D98C240DE36D4235200D52A89EC59C5F4B447AFA1AD305A89F5B141FEA67BD080F7B9BF11103A41CAA0CCD9DC211407448892FC23F91A6";

	auto p = std::chrono::steady_clock::now( );
	auto funcAlphabets = ConcatNatStrings(
		s1 , s2 , s3 , s4 , s5 , s6 , s7 , s8 , s9 ,
		NatString( " Null" ) );

	auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
	cout << "Func " << std::setw( 7 ) << diff << endl;

	p = std::chrono::steady_clock::now( );
	auto oprAlphabets =
		s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8 + s9 +
		NatString( " Null" );

	diff = ( std::chrono::steady_clock::now( ) - p ).count( );
	cout << "Opr+ " << std::setw( 7 ) << diff << endl << endl;

	BOOST_TEST( funcAlphabets == oprAlphabets );
	BOOST_TEST( funcAlphabets == "ABCD EFG HIJK LMNOP QRS TUV WX Y & Z DDB6C4E735604F4943D98C240DE36D4235200D52A89EC59C5F4B447AFA1AD305A89F5B141FEA67BD080F7B9BF11103A41CAA0CCD9DC211407448892FC23F91A6 Null" );
	BOOST_TEST( Hash( funcAlphabets ) == Hash( oprAlphabets ) );
}

BOOST_AUTO_TEST_CASE( UC_StringConcat )
{
	cout << endl << endl << "UC::String::Concat test" << endl;
	P_String
		s1 = String::Make( "ABCD" ) ,
		s2 = String::Make( " EFG" ) ,
		s3 = String::Make( " HIJK" );
	NatString s4 = NatString( " LMNOP" ) ,
		s5 = NatString( " QRS" ) ,
		s6 = NatString( " TUV" ) , s7 = " WX";
	auto s8 = " Y & Z";
	auto& s9 = " DDB6C4E735604F4943D98C240DE36D4235200D52A89EC59C5F4B447AFA1AD305A89F5B141FEA67BD080F7B9BF11103A41CAA0CCD9DC211407448892FC23F91A6";

	auto p = std::chrono::steady_clock::now( );
	auto funcAlphabets = String::Concat(
		s1 , s2 , s3 , s4 , s5 , s6 , s7 , s8 , s9 ,
		NatString( " Null" ) );

	auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
	cout << "Func " << std::setw( 7 ) << diff << endl;

	p = std::chrono::steady_clock::now( );
	auto oprAlphabets =
		*( *( *( *( *( *( *( *( *s1 + *s2 ) + *s3 ) + *String::Make( s4 ) ) + *String::Make( s5 ) ) + *String::Make( s6 ) ) + *String::Make( s7 ) ) + *String::Make( s8 ) ) + *String::Make( s9 ) ) + *String::Make( " Null" );

	diff = ( std::chrono::steady_clock::now( ) - p ).count( );
	cout << "Opr+ " << std::setw( 7 ) << diff << endl << endl;

	auto rEq = funcAlphabets == oprAlphabets;
	BOOST_TEST( rEq , "The results of UC::String::Concat & UC::String::operator + are different!" );
	BOOST_TEST( Hash( funcAlphabets ) == Hash( oprAlphabets ) );
}

BOOST_AUTO_TEST_CASE( DequeVsVectorVsBstDequeVsBstVector )
{
	cout << endl << endl << "UC::Deque vs UC::Vector vs UC::BstDeque vs UC::BstVector" << endl;

	auto dq = Deque<P_Int64>::Make( );
	auto vt = Vector<P_Int64>::Make( );
	auto bq = BstDeque<P_Int64>::Make( );
	auto bt = BstVector<P_Int64>::Make( );
	auto rng = std::mt19937( std::chrono::steady_clock::now( ).time_since_epoch( ).count( ) );
	cout << "Heavy insertion" << endl;
	{
		auto p = std::chrono::steady_clock::now( );
		using namespace UC::IntLiterals;
		dq->Add( 10_i64 , 20_i64 , 30_i64 , 40_i64 , 50_i64 , 60_i64 , 70_i64 , 80_i64 , 90_i64 );
		dq->Add( 100_i64 , 200_i64 , 300_i64 , 400_i64 , 500_i64 , 600_i64 , 700_i64 , 800_i64 , 9000_i64 );
		dq->AddBefore( 17 , 900_i64 , 1000_i64 , 2000_i64 , 3000_i64 , 4000_i64 , 5000_i64 , 6000_i64 , 7000_i64 , 8000_i64 );
		dq->AddFront( 1_i64 , 2_i64 , 3_i64 , 4_i64 , 5_i64 , 6_i64 , 7_i64 , 8_i64 , 9_i64 );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		using namespace UC::IntLiterals;
		vt->Add( 10_i64 , 20_i64 , 30_i64 , 40_i64 , 50_i64 , 60_i64 , 70_i64 , 80_i64 , 90_i64 );
		vt->Add( 100_i64 , 200_i64 , 300_i64 , 400_i64 , 500_i64 , 600_i64 , 700_i64 , 800_i64 , 9000_i64 );
		vt->AddBefore( 17 , 900_i64 , 1000_i64 , 2000_i64 , 3000_i64 , 4000_i64 , 5000_i64 , 6000_i64 , 7000_i64 , 8000_i64 );
		vt->AddFront( 1_i64 , 2_i64 , 3_i64 , 4_i64 , 5_i64 , 6_i64 , 7_i64 , 8_i64 , 9_i64 );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdVector := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		using namespace UC::IntLiterals;
		bq->Add( 10_i64 , 20_i64 , 30_i64 , 40_i64 , 50_i64 , 60_i64 , 70_i64 , 80_i64 , 90_i64 );
		bq->Add( 100_i64 , 200_i64 , 300_i64 , 400_i64 , 500_i64 , 600_i64 , 700_i64 , 800_i64 , 9000_i64 );
		bq->AddBefore( 17 , 900_i64 , 1000_i64 , 2000_i64 , 3000_i64 , 4000_i64 , 5000_i64 , 6000_i64 , 7000_i64 , 8000_i64 );
		bq->AddFront( 1_i64 , 2_i64 , 3_i64 , 4_i64 , 5_i64 , 6_i64 , 7_i64 , 8_i64 , 9_i64 );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		using namespace UC::IntLiterals;
		bt->Add( 10_i64 , 20_i64 , 30_i64 , 40_i64 , 50_i64 , 60_i64 , 70_i64 , 80_i64 , 90_i64 );
		bt->Add( 100_i64 , 200_i64 , 300_i64 , 400_i64 , 500_i64 , 600_i64 , 700_i64 , 800_i64 , 9000_i64 );
		bt->AddBefore( 17 , 900_i64 , 1000_i64 , 2000_i64 , 3000_i64 , 4000_i64 , 5000_i64 , 6000_i64 , 7000_i64 , 8000_i64 );
		bt->AddFront( 1_i64 , 2_i64 , 3_i64 , 4_i64 , 5_i64 , 6_i64 , 7_i64 , 8_i64 , 9_i64 );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstVector := " << std::setw( 10 ) << diff << endl;
	}

	BOOST_TEST( dq->Size( ) == 36 );
	BOOST_TEST( vt->Size( ) == 36 );
	BOOST_TEST( bq->Size( ) == 36 );
	BOOST_TEST( bt->Size( ) == 36 );

	cout << endl << "ToString" << endl;
	NatString dqStrRes;
	NatString vtStrRes;
	NatString bqStrRes;
	NatString btStrRes;
	{
		auto p = std::chrono::steady_clock::now( );
		dqStrRes = dq->ToString( );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		vtStrRes = vt->ToString( );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdVector := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		bqStrRes = bq->ToString( );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		btStrRes = bt->ToString( );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstVector := " << std::setw( 10 ) << diff << endl;
	}
	cout
		<< "StdDeque  StrRepr := " << dqStrRes << endl
		<< "StdVector StrRepr := " << vtStrRes << endl
		<< "BstDeque  StrRepr := " << bqStrRes << endl
		<< "BstVector StrRepr := " << btStrRes << endl;

	for ( size_t i = 0; i < dq->Size( ); i++ )
	{
		BOOST_TEST( dq->Get( i )->value == vt->Get( i )->value );
		BOOST_TEST( bq->Get( i )->value == bt->Get( i )->value );
		BOOST_TEST( dq->Get( i )->value == bq->Get( i )->value );
	}

	cout << endl << "GetHashCode" << endl;
	size_t dqHC = 0;
	size_t vtHC = 0;
	size_t bqHC = 0;
	size_t btHC = 0;
	{
		auto p = std::chrono::steady_clock::now( );
		dqHC = dq->GetHashCode( );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		vtHC = vt->GetHashCode( );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdVector := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		bqHC = bq->GetHashCode( );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		btHC = bt->GetHashCode( );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstVector := " << std::setw( 10 ) << diff << endl;
	}
	cout
		<< "StdDeque  HashCode := " << dqHC << endl
		<< "StdVector HashCode := " << vtHC << endl
		<< "BstDeque  HashCode := " << bqHC << endl
		<< "BstVector HashCode := " << btHC << endl;
	BOOST_TEST( dqHC == vtHC );
	BOOST_TEST( bqHC == btHC );
	BOOST_TEST( dqHC == bqHC );

	cout << endl << "Shuffling" << endl;
	{
		auto p = std::chrono::steady_clock::now( );
		std::shuffle( dq->begin( ) , dq->end( ) , rng );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		std::shuffle( vt->begin( ) , vt->end( ) , rng );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdVector := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		std::shuffle( bq->begin( ) , bq->end( ) , rng );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		std::shuffle( bt->begin( ) , bt->end( ) , rng );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstVector := " << std::setw( 10 ) << diff << endl;
	}
	cout
		<< "StdDeque  Shuffled := " << dq->ToString( ) << endl
		<< "StdVector Shuffled := " << vt->ToString( ) << endl
		<< "BstDeque  Shuffled := " << bq->ToString( ) << endl
		<< "BstVector Shuffled := " << bt->ToString( ) << endl;

	cout << endl << "Sorting" << endl;
	{
		auto p = std::chrono::steady_clock::now( );
		dq->Sort( [ ] ( const P_Int64& a , const P_Int64& b ) { return a->value > b->value; } );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		vt->Sort( [ ] ( const P_Int64& a , const P_Int64& b ) { return a->value > b->value; } );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdVector := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		bq->Sort( [ ] ( const P_Int64& a , const P_Int64& b ) { return a->value > b->value; } );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		bt->Sort( [ ] ( const P_Int64& a , const P_Int64& b ) { return a->value > b->value; } );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstVector := " << std::setw( 10 ) << diff << endl;
	}
	cout
		<< "StdDeque  Sorted := " << dq->ToString( ) << endl
		<< "StdVector Sorted := " << vt->ToString( ) << endl
		<< "BstDeque  Sorted := " << bq->ToString( ) << endl
		<< "BstVector Sorted := " << bt->ToString( ) << endl;

	for ( size_t i = 0; i < dq->Size( ); i++ )
	{
		BOOST_TEST( dq->Get( i )->value == vt->Get( i )->value );
		BOOST_TEST( bq->Get( i )->value == bt->Get( i )->value );
		BOOST_TEST( dq->Get( i )->value == bq->Get( i )->value );
	}

	cout << endl << "Random Access" << endl;
	{
		NatVector<size_t> vect;
		vect.reserve( dq->Size( ) );
		for ( size_t i = 0; i < dq->Size( ); i++ ) vect.push_back( i );
		std::shuffle( vect.begin( ) , vect.end( ) , rng );
		auto p = std::chrono::steady_clock::now( );
		for ( auto& i : vect )cout << dq->Get( i ) << ",";
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		NatVector<size_t> vect;
		vect.reserve( dq->Size( ) );
		for ( size_t i = 0; i < dq->Size( ); i++ ) vect.push_back( i );
		std::shuffle( vect.begin( ) , vect.end( ) , rng );
		auto p = std::chrono::steady_clock::now( );
		for ( auto& i : vect )cout << vt->Get( i ) << ",";
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdVector := " << std::setw( 10 ) << diff << endl;
	}
	{
		NatVector<size_t> vect;
		vect.reserve( dq->Size( ) );
		for ( size_t i = 0; i < dq->Size( ); i++ ) vect.push_back( i );
		std::shuffle( vect.begin( ) , vect.end( ) , rng );
		auto p = std::chrono::steady_clock::now( );
		for ( auto& i : vect )cout << bq->Get( i ) << ",";
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		NatVector<size_t> vect;
		vect.reserve( dq->Size( ) );
		for ( size_t i = 0; i < dq->Size( ); i++ ) vect.push_back( i );
		std::shuffle( vect.begin( ) , vect.end( ) , rng );
		auto p = std::chrono::steady_clock::now( );
		for ( auto& i : vect )cout << bt->Get( i ) << ",";
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstVector := " << std::setw( 10 ) << diff << endl;
	}

	cout << endl << "Random Delete" << endl;
	auto delSz = dq->Size( ) / 2;
	using ui64d = std::uniform_int<uint64_t>;
	{
		auto p = std::chrono::steady_clock::now( );
		for ( size_t i = 0; i < delSz; i++ ) dq->RemoveAt( ui64d( 0 , dq->Size( ) - 1 )( rng ) );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		for ( size_t i = 0; i < delSz; i++ ) vt->RemoveAt( ui64d( 0 , vt->Size( ) - 1 )( rng ) );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdVector := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		for ( size_t i = 0; i < delSz; i++ ) bq->RemoveAt( ui64d( 0 , bq->Size( ) - 1 )( rng ) );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstDeque  := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		for ( size_t i = 0; i < delSz; i++ ) bt->RemoveAt( ui64d( 0 , bt->Size( ) - 1 )( rng ) );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstVector := " << std::setw( 10 ) << diff << endl;
	}
	cout
		<< "StdDeque  Random Elements Deleted := " << dq->ToString( ) << endl
		<< "StdVector Random Elements Deleted := " << vt->ToString( ) << endl
		<< "BstDeque  Random Elements Deleted := " << bq->ToString( ) << endl
		<< "BstVector Random Elements Deleted := " << bt->ToString( ) << endl;

	BOOST_TEST( dq->Size( ) == 18 );
	BOOST_TEST( vt->Size( ) == 18 );
	BOOST_TEST( bq->Size( ) == 18 );
	BOOST_TEST( bt->Size( ) == 18 );
}

BOOST_AUTO_TEST_CASE( GCEqTest )
{
	using namespace UC::IntLiterals;
	using namespace UC::StrLiterals;

	auto int1 = 10_i16;
	auto int2 = 10_i16;

	BOOST_TEST( int1 == int2 );
	BOOST_TEST( !( int1 != int2 ) );
	BOOST_TEST( !int1.RefEq( int2 ) );
	BOOST_TEST( !int2.RefEq( int1 ) );
	BOOST_TEST( int1.RefNotEq( int2 ) );
	BOOST_TEST( int2.RefNotEq( int1 ) );


	auto str1 = "Hello"_us;
	auto str2 = "Hello"_us;

	BOOST_TEST( str1 == str2 );
	BOOST_TEST( !( str1 != str2 ) );
	BOOST_TEST( !str1.RefEq( str2 ) );
	BOOST_TEST( !str2.RefEq( str1 ) );
	BOOST_TEST( str1.RefNotEq( str2 ) );
	BOOST_TEST( str2.RefNotEq( str1 ) );
}

BOOST_AUTO_TEST_CASE( MapVsBstMap )
{
	using namespace UC::StrLiterals;
	using namespace UC::IntLiterals;

	cout << endl << endl << "UC::UnorderedMap vs UC::BstUnorderedMap" << endl;

	auto sm = UnorderedMap<   P_Int32 , P_String>::Make( );
	auto bm = BstUnorderedMap<P_Int32 , P_String>::Make( );
	auto rng = std::mt19937( std::chrono::steady_clock::now( ).time_since_epoch( ).count( ) );

	cout << "Direct construction" << endl;
	{
		auto p = std::chrono::steady_clock::now( );
		auto smn = UnorderedMap<   P_Int32 , P_String>::MakeI(
			{
				{0_i32 , "zero"_us 	  },
				{1_i32 , "one"_us 	  },
				{2_i32 , "two"_us 	  },
				{3_i32 , "three"_us	  },
				{4_i32 , "four"_us 	  },
				{5_i32 , "five"_us 	  },
				{6_i32 , "six"_us 	  },
				{7_i32 , "seven"_us	  },
				{8_i32 , "eight"_us	  },
				{9_i32 , "nine"_us 	  },
			} );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdUnorderedMap := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		auto bmn = BstUnorderedMap<   P_Int32 , P_String>::MakeI(
			{
				{0_i32 , "zero"_us 	  },
				{1_i32 , "one"_us 	  },
				{2_i32 , "two"_us 	  },
				{3_i32 , "three"_us	  },
				{4_i32 , "four"_us 	  },
				{5_i32 , "five"_us 	  },
				{6_i32 , "six"_us 	  },
				{7_i32 , "seven"_us	  },
				{8_i32 , "eight"_us	  },
				{9_i32 , "nine"_us 	  },
			} );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstUnorderedMap := " << std::setw( 10 ) << diff << endl;
	}

	cout << "Heavy insertion" << endl;
	{
		auto p = std::chrono::steady_clock::now( );
		sm->Add( 0_i32 , "zero"_us );
		sm->Add( 1_i32 , "one"_us );
		sm->Add( 2_i32 , "two"_us );
		sm->Add( 3_i32 , "three"_us );
		sm->Add( 4_i32 , "four"_us );
		sm->Add( 5_i32 , "five"_us );
		sm->Add( 6_i32 , "six"_us );
		sm->Add( 7_i32 , "seven"_us );
		sm->Add( 8_i32 , "eight"_us );
		sm->Add( 9_i32 , "nine"_us );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdUnorderedMap := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		bm->Add( 0_i32 , "zero"_us );
		bm->Add( 1_i32 , "one"_us );
		bm->Add( 2_i32 , "two"_us );
		bm->Add( 3_i32 , "three"_us );
		bm->Add( 4_i32 , "four"_us );
		bm->Add( 5_i32 , "five"_us );
		bm->Add( 6_i32 , "six"_us );
		bm->Add( 7_i32 , "seven"_us );
		bm->Add( 8_i32 , "eight"_us );
		bm->Add( 9_i32 , "nine"_us );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstUnorderedMap := " << std::setw( 10 ) << diff << endl;
	}

	BOOST_TEST( sm->Size( ) == 10 );
	BOOST_TEST( bm->Size( ) == 10 );

	cout << endl << "ToString" << endl;
	NatString smStrRes;
	NatString bmStrRes;
	{
		auto p = std::chrono::steady_clock::now( );
		smStrRes = sm->ToString( );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdUnorderedMap := " << std::setw( 10 ) << diff << endl;
	}
	{
		auto p = std::chrono::steady_clock::now( );
		bmStrRes = bm->ToString( );
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstUnorderedMap := " << std::setw( 10 ) << diff << endl;
	}
	cout
		<< "StdUnorderedMap StrRepr := " << smStrRes << endl
		<< "BstUnorderedMap StrRepr := " << bmStrRes << endl;

	cout << endl << "Random Access" << endl;
	{
		NatVector<P_Int32> vec {
			0_i32 ,
			1_i32 ,
			2_i32 ,
			3_i32 ,
			4_i32 ,
			5_i32 ,
			6_i32 ,
			7_i32 ,
			8_i32 ,
			9_i32 ,
		};
		std::shuffle( vec.begin( ) , vec.end( ) , rng );
		auto p = std::chrono::steady_clock::now( );
		for ( auto& i : vec )cout << sm->Get( i )->value << ",";
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "StdUnorderedMap := " << std::setw( 10 ) << diff << endl;
	}
	{
		NatVector<P_Int32> vec {
			0_i32 ,
			1_i32 ,
			2_i32 ,
			3_i32 ,
			4_i32 ,
			5_i32 ,
			6_i32 ,
			7_i32 ,
			8_i32 ,
			9_i32 ,
		};
		std::shuffle( vec.begin( ) , vec.end( ) , rng );
		auto p = std::chrono::steady_clock::now( );
		for ( auto& i : vec )cout << bm->Get( i )->value << ",";
		auto diff = ( std::chrono::steady_clock::now( ) - p ).count( );
		cout << "BstUnorderedMap := " << std::setw( 10 ) << diff << endl;
	}

}

BOOST_AUTO_TEST_SUITE_END( )

BOOST_AUTO_TEST_SUITE( function_objects )


struct Foo
{
	Foo( int num ) : num_( num ) { }
	int print_add( int i ) const { cout << num_ + i << '\n'; return num_ + i; }
	int num_;
};

int print_num( int i )
{
	cout << i << '\n';
	return i;
}

struct PrintNum
{
	int operator()( int i ) const
	{
		cout << i << '\n';
		return i;
	}
};

BOOST_AUTO_TEST_CASE( FunctionTest )
{
	cout << endl << endl << "UC::Function test" << endl;

	// store a free function
	auto f_display1 = MakeFunc<decltype( print_num )>( print_num );
	BOOST_TEST( f_display1->Eval( -9 ) == -9 );

	// store a lambda
	auto f_display2 = MakeFunc<void( )>( [ ] ( ) { print_num( 42 ); } );
	f_display2->Eval( );

	// store the result of a call to std::bind
	f_display2 = MakeFunc<void( )>( std::bind( print_num , 31337 ) );
	f_display2->Eval( );

	// store a call to a member function
	auto f_add_display = MakeFunc<void( const Foo& , int )>( &Foo::print_add );
	const Foo foo( 100 );
	f_add_display->Eval( foo , 1 );
	f_add_display->Eval( 110 , 1 );

	//// store a call to a data member accessor
	auto f_num = MakeFunc<int( Foo const& )>( &Foo::num_ );
	BOOST_TEST( f_num->Eval( foo ) == foo.num_ );

	// store a call to a member function and object
	f_display1 = MakeFunc<int( int )>( std::bind( &Foo::print_add , foo , std::placeholders::_1 ) );
	BOOST_TEST( f_display1->Eval( 2 ) == ( foo.num_ + 2 ) );

	// store a call to a member function and object ptr
	f_display1 = MakeFunc<int( int )>( std::bind( &Foo::print_add , &foo , std::placeholders::_1 ) );
	BOOST_TEST( f_display1->Eval( 3 ) == ( foo.num_ + 3 ) );

	// store a call to a function object
	f_display1 = MakeFunc<int( int )>( PrintNum( ) );
	BOOST_TEST( f_display1->Eval( 18 ) == 18 );
}

class PrintHelper
{
public:
	//declare event of type delegate
	P_Event<> beforePrintEvent;

	PrintHelper( ) :beforePrintEvent( Event<>::Make( ) ) { }

	void PrintNumber( int num )
	{
		beforePrintEvent->Eval( );
		cout << "Number: " << num << endl;
	}

	void PrintDecimal( int dec )
	{
		beforePrintEvent->Eval( );
		cout << "Decimal: " << dec << endl;
	}

	void PrintTemperature( int num )
	{
		beforePrintEvent->Eval( );
		cout << "Temperature: " << num << " C" << endl;
	}
};

BOOST_AUTO_TEST_CASE( EventTest )
{
	cout << endl << endl << "UC::Event test" << endl;

	PrintHelper _printHelper {};

	int val = 10;
	_printHelper.beforePrintEvent->Add( [ ] ( ) { cout << "BeforPrintEventHandler: PrintHelper is going to print a value" << endl; } );

	auto PrintTemperature = [ & ] ( )
	{
		_printHelper.PrintTemperature( val );
	};

	auto PrintNumber = [ & ] ( )
	{
		_printHelper.PrintNumber( val );
	};

	PrintNumber( );
	PrintNumber( );
	PrintTemperature( );
}

BOOST_AUTO_TEST_SUITE_END( )
