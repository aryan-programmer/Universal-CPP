![UC++ Logo in Iosevka SS03](E:\C++ Projects\UC++\Logo.png)

# UC++(Universal C++)


[TOC]

## <span style="color:red">× To implement</span> || <span style="color:green">√ Implemented</span> Features ← From Languages

### <span style="color:green">√ Garbage Collection  ← Objective-C</span>
Like Objective-C, UC++ supports GC, but only ARC (Automatic Reference Counting) style GC is supported, like in Objective-C where ARC is the recommended method.

------

### <span style="color:green">√ Reflection ← ~~Most~~ <u>***All***</u> languages with automatic garbage collection.</span>

Use `UC::P_Any` variables and `UC::Object::Call` as:

```C++
?variable?->Call(?function-name?, {?args?})
```

or use <span style="color:purple">UCC</span> as:

```C++
UCC(?variable?, ?function-name?, {?args?})
```

to call methods using reflection. And use

```C++
static UC::Object::CreateInstance
```
to create instances of <span style="color:purple">UCInterfaces</span> as

```C++
UC::Object::CreateInstance(?class-name?, {?args?})
```

------

### <span style="color:green">√ Dynamic Typing ← Python</span>

Use `UC::P_Any` variables and `UC::Object::Call` as:

```C++
?variable?->Call(?function-name?, {?args?})
```

or use <span style="color:purple">UCC</span> as:

```C++
UCC(?variable?, ?function-name?, {?args?})
```

to call methods using ***Dynamic Typing***. Yes, the methods to use for reflection and dynamic typing are the same. 

For casting to higher types use 
```C++
UC::ObjCast<T>
```
as 

```C++
UC::ObjCast<?TypeToCastTo?>(?variable?)
```

------


### <span style="color:blue">√ Multiple Class Inheritance ← C++</span>
From C++!!

------

### <span style="color:blue">√ Block Structured Programming ← Swift</span>
Already in C++.



# [Documentation](.\DOCUMENTATION.md)




# Conversion, Macro substitution

The pre-macro substitution state:

```C++
//Empty.hpp
struct NEmpty
{
	void NativeBaseFunction( );
};

UCInterface( Empty ,
			 UC_WhereTypenameIsRealName ,
			 UC_InheritsUCClasses( UC::Object ) ,
			 UC_InheritsNativeClasses( NEmpty )
)
UC_HasExplicitCtors( Empty , UC_AlsoHasEmptyMaker , ( _1 ) );
UC_HasMethods(
( Do ) ,
( Do , ( _1 ) ) ,
( Do , ( _1 , _2 ) )
);

void NativeFunction( );

UCEndInterface( Empty );
```


```C++
//Empty.cpp
#include <iostream>
#include "Empty.hpp"

using namespace std;

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
```

The post-macro substitution state:

```C++
//Empty.hpp
struct NEmpty
{
	void NativeBaseFunction( );
};

struct Empty : UC::Object , NEmpty , ::UC::EnableGCPtrFromMe<Empty>
{
	using base0 = UC::Object;
	using nbase0 = NEmpty;
	using self = Empty;
	using pself = ::UC::GCP<self>;
	using wself = ::UC::WeakPtr<self>;
	using EGCPFM = ::UC::EnableGCPtrFromMe<Empty>;
protected:
	auto callImplUpChain( const ::UC::NatString& fname , const ::UC::NatODeque& args ) -> ::UC::P_Any
	{
		try
		{
			auto res0 = base0::callImpl( fname , args );
			if ( res0 ) return res0;
		}
		catch ( const ::UC::NoSuchFunction_Exception& ) { };
		throw ::UC::NoSuchConstructor_Exception(::UC::ConcatNatStrings(
				::UC::NatString( "No function for type \"" "Empty" "\" with name \"" ) ,
				fname , "\" that takes in " , std::to_string( args.size( ) ) ,
				" parameters." ) );
	}
private:
	struct __classRegisterer
	{
		__classRegisterer( )
		{::UC::Object::addConstructor( "Empty" , &self::make_reflective );}
	};
	static __classRegisterer __classRegistererInstance;
public:
	static const ::UC::NatString& SGetTypeName( )
	{
		static auto nm = ::UC::NatString( "Empty" );
		return nm;
	}
	virtual const ::UC::NatString& GetTypeName( ) const override{return SGetTypeName( );}
	template<typename... Args>static ::UC::GCP<self> Make( Args&&... args )
	{return ::UC::GCP<self>( new self( std::forward<Args>( args )... ) );}
protected:
protected:
	Empty( );
	Empty( ::UC::P_Any _1 );
public:
	static ::UC::P_Any make_reflective( const ::UC::NatODeque& args )
	{
		switch ( args.size( ) )
		{
		case 0:return ::UC::GCP<self>( new self( ) );
		case 1: return ::UC::GCP<self>( new self( args[ 0 ] ) );
		default: throw ::UC::NoSuchConstructor_Exception(
			::UC::ConcatNatStrings(::UC::NatString( "No constructor for type \"" ) ,
				SGetTypeName( ) , "\" that takes in " , std::to_string( args.size( ) ) ,
				" parameters." ) );
		}
	};
public:
	virtual auto Do( ) -> ::UC::P_Any;
	virtual auto Do( ::UC::P_Any _1 ) -> ::UC::P_Any;
	virtual auto Do( ::UC::P_Any _1 , ::UC::P_Any _2 ) -> ::UC::P_Any;
protected:
	auto callImpl( const ::UC::NatString& fname , const ::UC::NatODeque& args ) -> ::UC::P_Any
	{
		const auto& argsLen = args.size( );
		if ( fname == "Do" && argsLen == 0 ) { return this->Do( ); }
		if ( fname == "Do" && argsLen == 1 ) { return this->Do( args[ 0 ] ); }
		if ( fname == "Do" && argsLen == 2 ) { return this->Do( args[ 0 ] , args[ 1 ] ); }
		return callImplUpChain( fname , args );
	}
public:
	virtual ::UC::P_Any Call( const ::UC::NatString& fname , const ::UC::NatODeque& args ) override {return callImpl( fname , args );};
	void NativeFunction( );
};
using P_Empty = ::UC::GCP<Empty>;
using W_Empty = ::UC::WeakPtr<Empty>;
```


```C++
//Empty.cpp
#include <iostream>
#include "Empty.hpp"

using namespace std;

Empty::__classRegisterer Empty::__classRegistererInstance {};

Empty::Empty( )
{
	cout << "0 parameters for " __FUNCTION__ << endl;
}

Empty::Empty( ::UC::P_Any _1 )
{
	cout << "1 parameter for "  __FUNCTION__ << endl;
}


auto Empty::Do( ) -> ::UC::P_Any
{
	cout << "0 parameters for "  __FUNCTION__ << endl;
	return nullptr;
}

auto Empty::Do( ::UC::P_Any _1 ) -> ::UC::P_Any
{
	cout << "1 parameter for "  __FUNCTION__ << endl;
	return nullptr;
}

auto Empty::Do( ::UC::P_Any _1 , ::UC::P_Any _2 ) -> ::UC::P_Any
{
	cout << "2 parameter for "  __FUNCTION__ << endl;
	return nullptr;
}

void Empty::NativeFunction( ) { std::cout << "From native function:" __FUNCTION__ << std::endl; }

void NEmpty::NativeBaseFunction( ) { std::cout << "From native function in base:" __FUNCTION__ << std::endl; }
```

See no surprises, no magic code, after macro substitution it's just plain C++. 
