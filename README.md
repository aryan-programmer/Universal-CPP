![UC++ Logo in Iosevka SS03](Logo.png)

# UC++(Universal C++)


[TOC]

## <span style="color:red">× To implement</span> || <span style="color:green">√ Implemented</span> Features ← From Languages

### <span style="color:green">√ Garbage Collection  ← Objective-C</span>
Like Objective-C, UC++ support s GC, but only ARC (Automatic Reference Counting) style GC is supported, like in Objective-C where ARC is the recommended method.

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

### <span style="color:green">√ Delegates ← C#</span>

Delegates are from C#, but they aren't called delegates, they're called **<u>_Functors_</u>**.

Functor = **<u>_Func_</u>**tion + Opera**<u>_tor_</u>**

There are 3 core things to 'delegates' or functors

1. `UC::Function<TReturn, TParameters...>`

   This template interface is abstract, it isn't directly instantiated, `UC::MakeFunc` is used for instantiating instances of this interface.

   To call the stored function call `?functor?->Eval(?parameters?...)` with the designated <span style="color:green">?parameters?...</span>

2. Function type aliases and how to use them

   They are defined as aliases for **<u>_function types_</u>**, not **<u>_functor types_</u>**. Define one as

   ```C++
   typedef ?return-type? ?name?(?parameter-types?...)
   ```

   Where <span style="color:green">?name?</span> & <span style="color:green">?return-type?</span> refer to the name of the alias & return type respectively, and <span style="color:green">?parameter-types...?</span> refers to 0 or more parameters.

   To get a `UC::Function` from the alias use:

   ```C++
   UC::FuncFrom<?name?>
   ```

   To get a `UC::Event` from the alias use:

   ```C++
   UC::EventFrom<?name?>
   ```

3. `UC::MakeFunc<TFunction, TRealFunction>(TRealFunction&& func)`

   Use this function as

   ```C++
   UC::MakeFunc<?function-type?>(?function/function-object/lambda/closure to-make-from?)
   ```

   Where <span style="color:green">?function-type?</span> refers to the type of the function it can be `UC::Function<TReturn, TParameters...>` or a Function type alias. <span style="color:green">?function/function-object/lambda/closure to-make-from?</span> refers to exactly what the name says.

### <span style="color:green">√ Events ← Qt</span>

Events are technically from C#. However they are also implemented in Qt (& Boost), but there they are called as signals and the functions which register to them are called slots. In C#, signals and slots are referred to as events and delegates. In UC++, signals and slots are referred to as events and functors (would you rather prefer "Group of UC++ GC pointer to function object" and "UC++ GC pointer to function object").

The template parameters for an event are the same as a functor `UC::Event<TReturn, TParameters...>`.

To create an event use `?event-type::Make()?` and assign it to a variable to class field.

To add a function, function-object, lambda or closure use `?event?->Add(?function/function-object/lambda/closure to-add?)`. To add a `UC::Function` use `?event?->AddF(?functor?)`. The return value of these functions is the id of the functor added, hold onto it if you want to erase the functor later.

To remove a functor using it's id use `?event?->Remove(?id?)`.

To invoke the event use `?event?->Eval(?parameters?...)` with the designated <span style="color:green">?parameters?...</span>. If the functor returns a value then `Eval` returns the return value of the last function, if there are no functors added then an error of type `UC::NoFunctorsAddedToEvent_Exception`, with the message

```C++
"UC::Event<TReturn, TParameters...> has no added functors that can return a value that can be returned."
```

If you want to get the return value of all the functions in a `UC::NatVector<TReturn>` use `?event?->EvalAll(?parameters?...)` with the designated <span style="color:green">?parameters?...</span>. Obviously, if the functors return `void` then `EvalAll` will not return a `UC::NatVector<void>`. In reality, `EvalAll` will call all the functions and return `void`. If there are no functors added then the returned vector will have size `0`.

An event is a functor, i.e. you can chain events, i.e. subscribe an event to an event.

### <span style="color:gray">√ Singleton ← Kotlin</span>

The singleton pattern comes inbuilt with Kotlin, UC++ also provides an inbuilt singleton pattern. 

For more information look to the documentation on the macro <span style="color:purple">UC_IsSingleton</span>.

### <span style="color:green">√ Generators ← C#</span>

From [Wikipedia:Generator (computer programming)](https://en.wikipedia.org/wiki/Generator_(computer_programming))

> In computer science, a generator is a special routine that can be used to control the iteration behaviour of a loop. In fact, all generators are iterators. A generator is very similar to a function that returns an array, in that a generator has parameters, can be called, and generates a sequence of values. However, instead of building an array containing all the values and returning them all at once, a generator yields the values one at a time, which requires less memory and allows the caller to get started processing the first few values immediately. In short, a generator looks like a function but behaves like an iterator.

The generators in UC++ do fulfil the above requirements but UC++ generators can do more, much. Look to the documentation for more info.

### <span style="color:darkgreen">√ Coroutines ← Unity</span>

A similar system to the one provided by Unity by UC++, though there are some differences.

Look to the documentation for more info.

### <span style="color:blue">√ Multiple Class Inheritance ← C++</span>

From C++!!

### <span style="color:blue">√ Block Structured Programming ← Swift</span>
Already in C++.



# [Documentation](./DOCUMENTATION.md)




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

UCEndInterface;
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
