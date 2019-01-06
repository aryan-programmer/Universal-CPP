#include "pch.h"
#include "Empty.hpp"

using namespace std;

UCRegisterClass( Empty );

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
	return nullptr;
}

UCMethod( Empty::Do , ( _1 ) )
{
	cout << "1 parameter for " __FUNCTION__ << endl;
	return nullptr;
}

UCMethod( Empty::Do , ( _1 , _2 ) )
{
	cout << "2 parameter for " __FUNCTION__ << endl;
	return nullptr;
}

void Empty::NativeFunction( ) { std::cout << "From native function:" __FUNCTION__ << std::endl; }

void NEmpty::NativeBaseFunction( ) { std::cout << "From native function in base:" __FUNCTION__ << std::endl; }
