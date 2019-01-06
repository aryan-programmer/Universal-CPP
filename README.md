# UC++(Universal C++)


[TOC]

## <span style="color:red">× To implement</span> || <span style="color:lightgreen">√ Implemented</span> Features ← From Languages

### <span style="color:lightgreen">√ Garbage Collection  ← Objective-C</span>
Like Objective-C, UC++ supports GC, but only ARC (Automatic Reference Counting) style GC is supported, like in Objective-C where ARC is the recommended method.

------

### <span style="color:lightgreen">√ Reflection ← ~~Most~~ <u>***All***</u> languages with automatic garbage collection.</span>

Use `UC::PAny` variables and `UC::Object::Call` as 

```C++
?variable?->Call(?function-name?, {?args?})
```

to call methods using reflection, and use 
```C++
static UC::Object::CreateInstance
```
to create instances of <span style="color:pink">UCInterfaces</span> as

`UC::Object::CreateInstance(?class-name?, {?args?})` .

------

### <span style="color:lightgreen">√ Dynamic Typing ← Python</span>

Use `UC::PAny` variables and `UC::Object::Call` as 

```C++
?variable?->Call(?function-name?, {?args?})
```

to call methods using achieve ***Dynamic Typing***. Yes, the methods to use for reflection and dynamic typing are the same. 

For casting to higher types use 
```C++
UC::ObjCast<T>
```
as 

```C++
UC::ObjCast<?TypeToCastTo?>(?variable?)
```

------


### <span style="color:skyblue">√ Multiple Class Inheritance ← C++</span>
From C++!!

------

### <span style="color:skyblue">√ Block Structured Programming ← Swift</span>
Already in C++.



## Conversion & Macro substitution

The pre-macro substitution state:

```C++
struct NEmpty
{
	void NativeBaseFunction( );
};

UCInterface( Empty ,
			 UC_WhereTypenameIsRealName ,
			 UC_InheritsUCClasses( UC::Object ) ,
			 UC_InheritsNativeClasses( NEmpty )
);
UC_HasExplicitCtors( Empty , UC_AlsoHasEmptyMaker , ( _1 ) );
UC_HasMethods(
( Do ) ,
( Do , ( _1 ) ) ,
( Do , ( _1 , _2 ) )
);

void NativeFunction( );

UCEndInterface;

using namespace std;

UCRegisterClass( Empty );

UCCTor( Empty::Empty )
{
	cout << "0 parameters for " __FUNCTION__ << endl;
}

UCCTor( Empty::Empty , ( _1 ) )
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
struct NEmpty
{
	void NativeBaseFunction( );
};

struct Empty : UC::Object , NEmpty
{
	using base0 = UC::Object;
	using nbase0 = NEmpty;
	using self = Empty;
protected:
	::UC::PAny callImplUpChain( ::UC::String fname , ::UC::GCOVect args )
	{
		auto res0 = base0::callImpl( fname , args );
		if ( res0 ) return res0;
	}
private:
	struct __classRegisterer
	{
		__classRegisterer( )
		{
			::UC::Object::addConstructor( "Empty" , &self::make_reflective );
		}
	};
	static __classRegisterer __classRegistererInstance;
public:
	virtual const char * GetTypeName( ) override
	{
		static auto nm = "Empty";
		return nm;
	};
	template<typename... Args>
	static ::UC::GCP<self> make( Args&&... args )
	{
		return ::UC::GCP<self>( new self( std::forward<Args>( args )... ) );
	}
protected:
	Empty( );
	Empty( ::UC::PAny _1 );
public:
	static ::UC::PAny make_reflective( ::UC::GCOVect args )
	{
		switch ( args.size( ) )
		{
		case 0:return ::UC::GCP<self>( new self( ) );
		case 1: return ::UC::GCP<self>( new self( args[ 0 ] ) );
		default: return nullptr;
		}
	}
public:
	auto Do( ) -> ::UC::PAny;
	auto Do( ::UC::PAny _1 ) -> ::UC::PAny;
	auto Do( ::UC::PAny _1 , ::UC::PAny _2 ) -> ::UC::PAny;
protected:
	auto callImpl( ::UC::String fname , ::UC::GCOVect args ) -> ::UC::PAny
	{
		if ( fname == "Do" && args.size( ) == 0 ) { return Do( ); }
		if ( fname == "Do" && args.size( ) == 1 ) { return Do( args[ 0 ] ); }
		if ( fname == "Do" && args.size( ) == 2 ) { return Do( args[ 0 ] , args[ 1 ] ); }
		return callImplUpChain( fname , args );
	}
public:
	virtual ::UC::PAny Call( ::UC::String fname , ::UC::GCOVect args ) override
	{
		return callImpl( fname , args );
	};
	void NativeFunction( );
};

using namespace std;
Empty::__classRegisterer Empty::__classRegistererInstance {};

Empty::Empty( )
{
	cout << "0 parameters for "  __FUNCTION__ << endl;
}

Empty::Empty( ::UC::PAny _1 )
{
	cout << "1 parameter for "  __FUNCTION__ << endl;
}

auto Empty::Do( ) -> ::UC::PAny
{
	cout << "0 parameters for "  __FUNCTION__ << endl; return nullptr;
}

auto Empty::Do( ::UC::PAny _1 ) -> ::UC::PAny
{
	cout << "1 parameter for "  __FUNCTION__ << endl;
	return nullptr;
}

auto Empty::Do( ::UC::PAny _1 , ::UC::PAny _2 ) -> ::UC::PAny
{
	cout << "2 parameter for "  __FUNCTION__ << endl;
	return nullptr;
}

void Empty::NativeFunction( )
{
	std::cout << "From native function:"  __FUNCTION__ << std::endl;
}

void NEmpty::NativeBaseFunction( )
{
	std::cout << "From native function in base:"  __FUNCTION__ << std::endl;
}
```



See no surprises, no magic code, after macro substitution it's just plain C++.