#include "pch.h"
#include "Empty.hpp"

using namespace std;
using namespace UC::IntLiterals;

UCRegister( Empty );

UCCtor( Empty::Empty )
{
	cout << "0 parameters for " __FUNCTION__ << endl;
}

UCCtor( Empty::Empty , ( _1 ) )
{
	cout << "1 parameter for " __FUNCTION__ << endl;
}


UCMethod( Empty::Do )
{
	cout << "0 parameters for " __FUNCTION__ << endl;
	return 0_i16;
}

UCMethod( Empty::Do , ( _1 ) )
{
	cout << "1 parameter for " __FUNCTION__ << endl;
	return 1_i16;
}

UCMethod( Empty::Do , ( _1 , _2 ) )
{
	cout << "2 parameter for " __FUNCTION__ << endl;
	return 2_i16;
}

int Empty::NativeFunction( ) { std::cout << "From native function:" __FUNCTION__ << std::endl; return 0; }

int NEmpty::NativeBaseFunction( ) { std::cout << "From native function in base:" __FUNCTION__ << std::endl; return 0; }
