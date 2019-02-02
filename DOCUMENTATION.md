![UC++ Logo in Iosevka SS03](.\Logo.png)

# UC++ Documentation

[TOC]

## Conventions

- `T` is a placeholder type for a UC++ Interface.
- `NT` is a placeholder type for a C++ class.
- `t` is a placeholder variable for instances of `UC::GCPtr`s of any type, generally `T`.
- `v` is a placeholder variable.
- `nt` is a placeholder variable for instances of any type, generally `NT`.
- All place holder types & variables can have a prefix number or alphanumeric string if used more than once.
- A **<u>_context-sensitive_</u>** place holder is a string of characters wrapped in question marks(??), e.g. ?name?. They can appear anywhere and context-sensitive. They may be color coded as <span style="color:green">green</span>, if possible.
- Macros are color coded in <span style="color:purple">purple</span>, if possible.

## <span style="color:purple">forceinline</span>

This macro is defined as being equal to <span style="color:purple">BOOST_FORCEINLINE</span>, which is defined as

```C++
// Macro to use in place of 'inline' to force a function to be inline
#if !defined(BOOST_FORCEINLINE)
#  if defined(_MSC_VER)
#    define BOOST_FORCEINLINE __forceinline
#  elif defined(__GNUC__) && __GNUC__ > 3
#    define BOOST_FORCEINLINE inline __attribute__ ((__always_inline__))
#  else
#    define BOOST_FORCEINLINE inline
#  endif
#endif
```

 It is a `Macro to use in place of 'inline' to force a function to be inline`.

## <span style="color:purple">UCInterface</span>(Name, StrName, Inheritance, NativeInheritance, ...)

<span style="color:purple">UCInterface</span> is a convenience macro that simplifies much of the complexity and boiler-plate code of declaring a proper UC++ Interface.

1. The first parameter is the name of the <span style="color:purple">UCInterface</span>.

2. The second parameter is the type-name of the <span style="color:purple">UCInterface</span>, there are 2 valid values for this <span style="color:purple">UC_WhereTypenameIsRealName</span> which assigns the same type-name as the name, or <span style="color:purple">UC_WhereTypenameIs</span>( "<span style="color:green">?name?</span>" ) where <span style="color:green">?name?</span> will become the **<u>_registered_</u>** name of the <span style="color:purple">UCInterface</span> used for reflection.

3. The third parameter is the group of UC++ Interfaces the <span style="color:purple">UCInterface</span> inherits, pass in the UC++ Interfaces as wrapped in <span style="color:purple">UC_InheritsUCClasses</span>(<span style="color:green">?classes?</span>). Each parameter will be expanded into a convenience *using* declaration, corresponding to the 0 based index of the inherited UC++ Interface, as base0, base1, base2, base3, ...

4. The fourth parameter is the group of native classes the <span style="color:purple">UCInterface</span> inherits, pass in the classes as wrapped in <span style="color:purple">UC_InheritsNativeClasses</span>(<span style="color:green">?classes?</span>). Each parameter will be expanded into a convenience *using* declaration, corresponding to the 0 based index of the inherited class, as nbase0, nbase1, nbase2, nbase3,...
   To inherit no native classes pass in <span style="color:purple">UC_InheritsNoNativeClasses</span> as this parameter.

5. The fifth parameter is "*optional*" and can include post modifiers like `final`, which prevent the UC++ Interface from being inherited.

   #### Be careful there is a ***vast*** difference in the name and the type-name of a <span style="color:purple">UCInterface</span>. The name represents the `struct` identifier of the <span style="color:purple">UCInterface</span>. The type-name is a string that is the return value of `T::SGetTypeName()` & `t.GetTypeName()`, also it is the **<u>_registered_</u>** name of the <span style="color:purple">UCInterface</span>, used for mainly for reflection.

### Remember, always make constructors `protected` or `private` **<u>_never_</u>** `public`.

To use certain things like <span style="color:purple">ME</span> or <span style="color:purple">WME</span> the object has to be constructed as a `UC::GCPtr` but if the object is called with the `public` constructor then there will be undefined behavior. So, to avoid that always make constructors `protected` or `private` **<u>_never_</u>** `public`.

### <span style="color:purple">UC_IsSingleton</span>

This macro defines Make() as being a function that is implemented as

```C++
static pself Make(){
	static pself v(new self());
	return v;
}
```

Where `self` refers to the class in which <span style="color:purple">UC_IsSingleton</span> is being expanded, and `pself` is equal to `UC::GCPtr<self>`. The macro also defines `GetInstance()`, `GetInst()`, `GetI()`, `Instance()` and `Inst()` which all call Make(). 

NOTE: The functions `GetInstance()`, `GetInst()`, `GetI()`, `Instance()` and `Inst()` are all <span style="color:purple">forceinline</span> so calling either `GetInstance()`, `GetInst()`, `GetI()`, `Instance()` or `Inst()` is equivalent to calling `Make()`.

#### OR

### <span style="color:purple">UC_IsAbstract</span>

Tag macro that defines no constructors and Make functions.

#### OR

### <span style="color:purple">UC_IsAbstractAndHasCtors</span>(name, hasEmptyCtor, ...)

Macro that defines constructors but no Make function. This macro has the same calling syntax as <span style="color:purple">UC_HasExplicitCtors</span>.

1. The name of the <span style="color:purple">UCInterface</span>.
2. The 2 valid values are <span style="color:purple">UC_HasNoEmptyCtor</span> & <span style="color:purple">UC_AlsoHasEmptyCtor</span>. This parameter is quite self-explanatory.
3. Pass in the parameters delimited by commas surrounded by braces for the multiple constructors delimited by commas.

#### OR

### <span style="color:purple">UC_OnlyHasEmptyCtor</span>

Defines a Make function that uses the 0 parameterized version of the constructor that may or may not be explicitly defined.

#### OR

### <span style="color:purple">UC_HasNativeCtorsAndEmptyCtor</span>

As the name suggests, the <span style="color:purple">UCInterface</span> has native constructors, and an empty constructor. The empty constructor must be there as it is required for reflection.

#### OR

### <span style="color:purple">UC_HasExplicitCtors</span>(name, hasEmptyCtor, ...)

Macro that defines constructors and a Make function. This macro has the same calling syntax as <span style="color:purple">UC_IsAbstractAndHasCtors</span>.

1. The name of the <span style="color:purple">UCInterface</span>.
2. The 2 valid values are <span style="color:purple">UC_HasNoEmptyCtor</span> & <span style="color:purple">UC_AlsoHasEmptyCtor</span>. This parameter is quite self-explanatory.
3. Pass in the parameters delimited by commas surrounded by braces for the multiple constructors delimited by commas.

#### THEN

### <span style="color:purple">UC_HasMethods</span>(...)

This macro defines functions and preps up the Call reflection function.

Call this macro as

```C++
UC_HasMethods((?name?, (...?args?...)), ...)
```

NOTE: If there are no arguments then instead of (<span style="color:green">?name?</span>, (<span style="color:green">...?args?...</span>)) write (<span style="color:green">?name?</span>).

NOTE: This function only adds declarations for the methods, to add implementations for them use 

```C++
UCMethod(?interface-name?::?name?, (...?args?...))
```

or

```C++
UCMethod(?interface-name?::?name?) 
```

The defines function has all the <span style="color:green">...?args?...</span> as being of the type `UC::P_Any`, the defined function also has a return type of `UC::P_Any`.

#### OR

### <span style="color:purple">UC_HasNoMethods</span>

This macro states that the <span style="color:purple">UCInterface</span> only has native functions and all reflection calls should be passed up to the base(s).

------

## <span style="color:purple">UCEndInterface</span>(name)

Terminates the <span style="color:purple">UCInterface</span> with the name specified.

## <span style="color:purple">UCRegister</span>(name)

Registers the <span style="color:purple">UCInterface</span> with the name specified as a real <span style="color:purple">UCInterface</span>. ***Remember that when you define a <span style="color:purple">UCInterface</span>, you must <span style="color:purple">UCRegister</span> it in the implementation `.cpp` file.***

## Remember that when you define a <span style="color:purple">UCInterface</span>, you must <span style="color:purple">UCRegister</span> it ***in the implementation `.cpp` file***, and ***only <span style="color:purple">UCRegister</span> it in the implementation `.cpp` file***.

## <span style="color:purple">UCException</span>(name)

Defines a well-formed UC++ exception class with the name specified.

### Remember <span style="color:purple">UCException</span>s aren't <span style="color:purple">UCInterface</span>s!

Dynamically allocating and throwing a small <span style="color:purple">UCException</span> that will either soon be caught and be destroyed or end program execution and be destroyed is a **<u>_terrible, terrible_</u>** idea.

### Remember use `throw ?exeception-type?(?message?)` **not** `throw ?exeception-type?::Make(?message?)`.

`?exeception-type?::Make(?message?)` is an expression that would work for a <span style="color:purple">UCInterface</span> <span style="color:purple">UCException</span> but Remember <span style="color:purple">UCException</span>s aren't <span style="color:purple">UCInterface</span>s.

### Remember: always and only use `try{...}catch(const ?exeception-type?& v){...}//more-catch-clauses` for catching <span style="color:purple">UCException</span>s.

If you use

```C++
try
{
    ...
}
catch(?exeception-type? ?name?)
{
    ...
}//?more-catch-clauses?
```

instead of

```C++
try
{
    ...
}
catch(const ?exeception-type?& ?name?)
{
    ...
}//?more-catch-clauses?
```

there will be a slow down in the application due to copying of the <span style="color:purple">UCException</span>.

## <span style="color:purple">UCBasedException</span>(name, base)

Defines a well-formed UC++ exception class with the name specified that inherits the base specified.

### Know and remember that a <span style="color:purple">UCBasedException</span> is no different from a <span style="color:purple">UCException</span>.

## <span style="color:purple">ME</span>

Gets the `UC::GCPtr` corresponding to the current instance of the <span style="color:purple">UCInterface</span>.

## <span style="color:purple">WME</span>

Gets the `UC::WeakPtr` corresponding to the current instance of the <span style="color:purple">UCInterface</span>.

## Don't use <span style="color:purple">ME</span> or <span style="color:purple">WME</span>, use `this`, unless you absolutely need to.

<span style="color:purple">ME</span> and <span style="color:purple">WME</span> aren't simple keywords not are they easy to small expressions, they involve real function calls, but **<u>_don't do this_</u>**:

```C++
UC::GCPtr<self>(this)
```

that is **<u>_the worst thing you could ever do_</u>**.

Bottom line: Use <span style="color:purple">ME</span> or <span style="color:purple">WME</span> only when absolutely need to, otherwise use `this`.

## <span style="color:purple">UCMethod</span>(name, args)

Defines a method with name specified and the arguments specified by args.

NOTE: If there are no arguments then instead of <span style="color:purple">UCMethod</span>(<span style="color:green">?name?</span>, (<span style="color:green">...?args?...</span>)) write <span style="color:purple">UCMethod</span>(<span style="color:green">?name?</span>).

To define implementations for <span style="color:purple">UCInterface</span> methods use this as:

```C++
UCMethod(?interface-name?::?name?, (...?args?...))
```

or

```C++
UCMethod(?interface-name?::?name?) 
```

The defines function has all the <span style="color:green">...?args?...</span> as being of the type `UC::P_Any`, the defined function also has a return type of `UC::P_Any`.

## <span style="color:purple">UCCtor</span>(name, args)

NOTE: If there are no arguments then instead of <span style="color:purple">UCCtor</span>(<span style="color:green">?name?</span>, (<span style="color:green">...?args?...</span>)) write <span style="color:purple">UCCtor</span>(<span style="color:green">?name?</span>).

To define implementations of a constructor for <span style="color:purple">UCInterface</span>s use this as:

```C++
UCCtor(?interface-name?::?name?, (...?args?...))
```

or

```C++
UCCtor(?interface-name?::?name?) 
```

The defines function has all the <span style="color:green">...?args?...</span> as being of the type `UC::P_Any`, the defined function also has a return type of `UC::P_Any`.

## <span style="color:purple">UCCast</span>(type, value)

Casts `value` to the specified type. If `value` can't be converted to the type specified, an exception of type will be thrown with the type `UC::InvalidCastException` with the message:
```C++
"Variable: \"?value?\" is not of the expected type: ?type?"
```

Where <span style="color:green">?value?</span> & <span style="color:green">?type?</span> refer to the value specified and the type specified respectively.

## <span style="color:purple">UCAsInt16</span>(v)

Gets the `value` from v, converts it to a `int16_t`. `v` can be of type `UC::Int16`, `UC::Byte` or `UC::SByte` and the macro will work properly, but if `v` is of any other type then, an exception of type will be thrown with the type `UC::InvalidCastException` with the message:

```C++
"Variable: \"?value?\" doesn't hold a value that can be converted to a int16_t"
```

Where <span style="color:green">?value?</span> refers to the value specified.

## <span style="color:purple">UCAsInt32</span>(v)

Gets the `value` from v, converts it to a `int32_t`. `v` can be of type `UC::Int32`, `UC::UInt16`, `UC::Int16`, `UC::Byte` or `UC::SByte` and the macro will work properly, but if `v` is of any other type then, an exception of type will be thrown with the type `UC::InvalidCastException` with the message:

```C++
"Variable: \"?value?\" doesn't hold a value that can be converted to a int32_t"
```

Where <span style="color:green">?value?</span> refers to the value specified.

## <span style="color:purple">UCAsInt64</span>(v)

Gets the `value` from v, converts it to a `int64_t`. `v` can be of type `UC::Int64`, `UC::UInt32`, `UC::Int32`, `UC::UInt16`, `UC::Int16`, `UC::Byte` or `UC::SByte` and the macro will work properly, but if `v` is of any other type then, an exception of type will be thrown with the type `UC::InvalidCastException` with the message:

```C++
"Variable: \"?value?\" doesn't hold a value that can be converted to a int64_t"
```

Where <span style="color:green">?value?</span> refers to the value specified.

## <span style="color:purple">UCAsUInt16</span>(v)

Gets the `value` from v, converts it to a `uint16_t`. `v` can be of type `UC::UInt16` or `UC::Byte` and the macro will work properly, but if `v` is of any other type then, an exception of type will be thrown with the type `UC::InvalidCastException` with the message:

```C++
"Variable: \"?value?\" doesn't hold a value that can be converted to a uint16_t"
```

Where <span style="color:green">?value?</span> refers to the value specified.

## <span style="color:purple">UCAsUInt32</span>(v)

Gets the `value` from v, converts it to a `uint32_t`. `v` can be of type `UC::UInt32`, `UC::UInt16` or `UC::Byte` and the macro will work properly, but if `v` is of any other type then, an exception of type will be thrown with the type `UC::InvalidCastException` with the message:

```C++
"Variable: \"?value?\" doesn't hold a value that can be converted to a uint32_t"
```

Where <span style="color:green">?value?</span> refers to the value specified.

## <span style="color:purple">UCAsUInt64</span>(v)

Gets the `value` from v, converts it to a `uint64_t`. `v` can be of type `UC::UInt64`, `UC::UInt32`, `UC::UInt16` or `UC::Byte` and the macro will work properly, but if `v` is of any other type then, an exception of type will be thrown with the type `UC::InvalidCastException` with the message:

```C++
"Variable: \"?value?\" doesn't hold a value that can be converted to a uint64_t"
```

Where <span style="color:green">?value?</span> refers to the value specified.

## <span style="color:purple">UCAsByte</span>(v)

Gets the `value` from v, converts it to a `UC::byte`. `v` must be of type `UC::Byte` for the macro will work properly, but if `v` is of any other type then, an exception of type will be thrown with the type `UC::InvalidCastException` with the message:

```C++
"Variable: \"?value?\" doesn't hold a value that can be converted to a UC::byte"
```

Where <span style="color:green">?value?</span> refers to the value specified.

## <span style="color:purple">UCAsSByte</span>(v)

Gets the `value` from v, converts it to a `UC::sbyte`. `v` must be of type `UC::SByte` for the macro will work properly, but if `v` is of any other type then, an exception of type will be thrown with the type `UC::InvalidCastException` with the message:

```C++
"Variable: \"?value?\" doesn't hold a value that can be converted to a UC::sbyte"
```

Where <span style="color:green">?value?</span> refers to the value specified.

## <span style="color:purple">UCAsFloat</span>(v)

Gets the `value` from v, converts it to a `float`. `v` can be of type `UC::Float`, `UC::UInt64`, `UC::Int64`, `UC::UInt32`, `UC::Int32`, `UC::UInt16`, `UC::Int16`, `UC::Byte` or `UC::SByte` and the macro will work properly, but if `v` is of any other type then, an exception of type will be thrown with the type `UC::InvalidCastException` with the message:

```C++
"Variable: \"?value?\" doesn't hold a value that can be converted to a float"
```

Where <span style="color:green">?value?</span> refers to the value specified.

## <span style="color:purple">UCAsDouble</span>(v)

Gets the `value` from v, converts it to a `double`. `v` can be of type `UC::Double`, `UC::Float`, `UC::UInt64`, `UC::Int64`, `UC::UInt32`, `UC::Int32`, `UC::UInt16`, `UC::Int16`, `UC::Byte` or `UC::SByte` and the macro will work properly, but if `v` is of any other type then, an exception of type will be thrown with the type `UC::InvalidCastException` with the message:

```C++
"Variable: \"?value?\" doesn't hold a value that can be converted to a double"
```

Where <span style="color:green">?value?</span> refers to the value specified.

## <span style="color:purple">UCC</span>(var, fname, args)

Calls the function with the name `fname` with the arguments `args` on the variable `var`. 

It expands to:

```C++
?variable?->Call(?function-name?, ?arguments?)
```

Where <span style="color:green">?variable?</span>, <span style="color:green">?function-name?</span> and <span style="color:green">?arguments?</span> refer to `var`, `fname` and `args` respectively.

## UC++ Delegates = `UC::Functor`

Delegates are from C#, but they aren't called delegates in UC++, they're called **<u>_Functors_</u>**.

Functor = **<u>_Func_</u>**tion + Opera**<u>_tor_</u>**

There are 3 core things to 'delegates' or functors

1. `UC::Function<TReturn, TParameters...>`

   This template interface is abstract, it isn't directly instantiated, `UC::MakeFunc` is used for instantiating instances of this interface.

   To call the stored function call `?functor?->Eval(?parameters?...)` with the designated <span style="color:green">?parameters?...</span>

2. Function type aliases and how to use them

   They are defined as aliases for **<u>_function types_</u>**, not **<u>_functor types_</u>**. Define one as

   ```C++
   using ?name? = ?return-type?(*)(?parameter-types?...)
   ```

   Where <span style="color:green">?name?</span> & <span style="color:green">?return-type?</span> refer to the name of the alias & return type respectively, and <span style="color:green">?parameter-types...?</span> refers to 0 or more parameters.

   To get a `UC::Function` from the alias use:

   ```C++
   UC::FuncFrom<?name?>
   ```

   To get a `UC::P_Function` from the alias use:

   ```C++
   UC::P_FuncFrom<?name?>
   ```

   To get a `UC::Event` from the alias use:

   ```C++
   UC::EventFrom<?name?>
   ```

   To get a `UC::P_Event` from the alias use:

   ```C++
   UC::P_EventFrom<?name?>
   ```

3. `UC::MakeFunc<TFunction, TRealFunction>(TRealFunction&& func)`

   Use this function as

   ```C++
   UC::MakeFunc<?function-type?>(?function/function-object/lambda/closure to-make-from?)
   ```

   Where <span style="color:green">?function-type?</span> refers to the type of the function it can be `UC::Function<TReturn, TParameters...>` or a Function type alias. <span style="color:green">?function/function-object/lambda/closure to-make-from?</span> refers to exactly what the name says.

## UC++ Signals = Events = `UC::Event`

Events are technically from C#. However they are also implemented in Qt (& Boost), but there they are called as signals and the functions which register to them are called slots. In C#, signals and slots are referred to as events and delegates. In UC++, signals and slots are referred to as events and functors.

The template parameters for an event are the same as a functor `UC::Event<TReturn, TParameters...>`.

To create an event use `?event-type::Make()?` and assign it to a variable to class field.

To add a function, function-object, lambda or closure use `?event?->Add(?function/function-object/lambda/closure to-add?)`. To add a `UC::Function` use `?event?->AddF(?functor?)`. The return value of these functions is the id of the functor added, hold onto it if you want to erase the functor later.

To remove a functor using it's id use `?event?->Remove(?id?)`.

To invoke the event use `?event?->Eval(?parameters?...)` with the designated <span style="color:green">?parameters?...</span>. If the functor returns a value then `Eval` returns the return value of the last function, if there are no functors added then an error of type `UC::NoFunctorsAddedToEvent_Exception`, with the message

```C++
"UC::Event<TReturn, TParameters...> has no added functors that can return a value that can be returned."
```

If you want to get the return value of all the functions in a `UC::NatVector<TReturn>` use `?event?->EvalAll(?parameters?...)` with the designated <span style="color:green">?parameters?...</span>. Obviously, if the functors return `void` then `EvalAll` will not return a `UC::NatVector<void>`. In reality, `EvalAll` will call all the functions and return `void`. If there are no functors added then the returned vector will have size `0`.

### An event **<u>_is_</u>** a functor, i.e. **<u>_you can chain events_</u>**, i.e. **<u>_subscribe an event to an event_</u>**!

## UC++ Generators

> In computer science, a generator is a special routine that can be used to control the iteration behaviour of a loop. In fact, all generators are iterators. A generator is very similar to a function that returns an array, in that a generator has parameters, can be called, and generates a sequence of values. However, instead of building an array containing all the values and returning them all at once, a generator yields the values one at a time, which requires less memory and allows the caller to get started processing the first few values immediately. In short, a generator looks like a function but behaves like an iterator.

From [Wikipedia:Generator (computer programming)](https://en.wikipedia.org/wiki/Generator_(computer_programming))

The generators in UC++ do fulfil the above requirements but UC++ generators can do more, much much more. 

But first, an example of unidirectional generators:

```C++
#include <iostream>
#include <Generator.hpp>

UCGen( int , Fibbonacci , ( ( int ) a , ( int ) b ) , c = 0 )
{
	UCYield( a );
	UCYield( b );
	for ( ;; )
    {
		c = a + b;
		a = b;
		b = c;
		UCYield( c );
	}
}
UCGenEnd


int main()
{
	using namespace std;
	auto gen = Fibbonacci( 1 , 1 );
	for ( size_t i = 0; i < 29; ++i )
        cout << *gen( ) << ', ';
}
```

Lets go through the line by line:

Line 1: `#include <iostream>` : Include the `iostream` header for output

Line 2: `#include <Generator.hpp>` : Includes the `Generator.hpp` header from UC++ for the generator types and macros.

Line 4: `UCGen( int , Fibbonacci , ( ( int ) a , ( int ) b ) , c = 0 )` : Define the start of the generator, the 1<sup>st</sup> parameter is the return type, the 2<sup>nd</sup> is the name of the generator, the 3<sup>rd</sup> is tuple of the parameters of the generator function, the types of the parameters have to be enclosed in parenthesis, and the final variadic parameters are the local variables, the types of the variables don't have to be specified.

Line 6: `UCYield( a );` : This inserts the appropriate code for returning from the function when called & resuming back from that very same point. When execution reaches this point, the function exits with the return value being `a` and when the generator is called again, execution resumes this point of execution.

Line 7: `UCYield( b );` : Same as above, but this time the `b` will be the return value.

Lines 8 to 12 have normal C++ code.

Line 13: `UCYield( c );` : Same as Line 6 & 7, but this time the `c` will be the return value.

Lines 14 to 21 also have normal C++ code.

Line 22: `auto gen = Fibbonacci( 1 , 1 );` : The generator function returns a 1-pass, non-linear "container", though a generator in reality, to use the values from the generator we have to store the generator as a variable. Now, when the function `Fibbonacci( 1 , 1 )` is called, the generator doesn't execute even a bit, the generator executes when you increment the iterator, or move the generator forward. Note: `gen` will be of type `UC::Generator<int>`.

Line 23 is a for loop which loops 29 times.

Line 24: `cout << *gen( ) << ', ';` : Let's concentrate on the `*gen()` part. As you know, to get the values of the generator we have either to increment the iterator as `++gen.begin()`, which will only move the generator forward but to get the value we have to write `*(++gen.begin())`. Or you could directly move the generator forward with `gen()`, to get the value write `*gen()`, much cleaner. Note: `gen()` returns `gen`.



Q. Can you guess the output?

A. Here's this code's output:

```pseudocode
1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229, 
```



##### Q. In regular generators, are you able to pass parameters to generator each time the generator is **<u>_incremented, <span style="color:red">not instantiated/called</span>?_</u>**

##### A. No?

##### A. Correction: Yes! With UC++'s bidirectional generators, you can pass parameters to generator each time the generator is incremented. But each time the generator is incremented then you **<u>_need_</u>** to pass in these InovcParameters specified.



### Bidirectional generators

Here is an example of a bidirectional generator:

```C++
#include <iostream>
#include <Generator.hpp>

enum class Operator {NOP , Add , Sub , Mul , Div , Mod , Pow , Rst};

UCBDGen( int64_t , Accumulator , ( ( int64_t ) val ) , ( ( Operator ) ( op ) , ( int64_t ) ( newVal ) ) );
{
	while ( true )
	{
		/**/ if ( op == Operator::Add )val = val + newVal;
		else if ( op == Operator::Sub )val = val - newVal;
		else if ( op == Operator::Mul )val = val * newVal;
		else if ( op == Operator::Div )val = val / newVal;
		else if ( op == Operator::Mod )val = val % newVal;
		else if ( op == Operator::Pow )val = static_cast<int64_t>( std::pow( val , newVal ) );
		else if ( op == Operator::Rst )val = newVal;
        
		UCYield( val );
	}
}
UCBDGenEnd;


int main()
{
    using namespace std;
    auto acc = Accumulator( 0 );
	cout << *acc( Operator::Sub , 2 ) << endl;
	cout << *acc( Operator::Add , 4 ) << endl;
	cout << *acc( Operator::Div , 2 ) << endl;
	cout << *acc( Operator::Mul , 4 ) << endl;
	cout << *acc( Operator::Pow , 4 ) << endl;
	cout << *acc( Operator::Mod , 10 ) << endl;
	cout << *acc( Operator::Rst , 256 ) << endl;
}
```

Let's jump to line 6: 

```C++
UCBDGen( int64_t , Accumulator , ( ( int64_t ) val ) , ( ( Operator ) ( op ) , ( int64_t ) ( newVal ) ) );
```

See the new macro? This new macro <span style="color:purple">UCBDGen</span>, it's similar to the previous one <span style="color:purple">UCGen</span> but this one supports InovcParameters, parameters called each incrementation of of the generator. 

Now to line 27: `auto acc = Accumulator( 0 );` : The generator function again returns a generator in which to use the values from the generator we have to store the generator as a variable. Again, when the function `Accumulator( 0 )` is called, the generator doesn't execute even a bit, the generator executes when you move the generator forward. Note: `gen` will be of type `UC::Generator<int, Operator, int64_t>`.

Line 28: `cout << *acc( Operator::Sub , 2 ) << endl;` : Let's focus on the `*acc( Operator::Sub , 2 )` part. Now if this was a normal generator you had to write `acc()` to increment it, but `acc` is a bidirectional generator, i.e. transfer of data takes place from the caller to the function and function to the caller, not just function to the caller as with unidirectional generators. In this function call `Operator::Sub` will be the value of the parameter `op` in the body of `Accumulator` only for this incrementation and `2` will be the value of the parameter `newVal` in the body of `Accumulator` again only for this incrementation. The value <span style="color:purple">UCYield</span>ed by `Accumulator` can be retrieved as `*acc`, but `acc( Operator::Sub , 2 )` just returns `acc`, so we can write `*acc( Operator::Sub , 2 )`, to increment the iterator with the InovcParameters & get the value that was <span style="color:purple">UCYield</span>ed.

Same thing for Lines 29 to 34.



Q. Can you anticipate the output?

A. Here's this code's output:

```pseudocode
-2
2
1
4
256
6
256
```



### Remember that iterators aren't supported for bidirectional generators.

Iterators aren't supported for bidirectional generators because in bidirectional generators InovcParameters have to be passed for each incrementation.

### Don't mix up bidirectional **<u>_generators_</u>** and bidirectional **<u>_iterators_</u>**. 

They are in no way similar, but still the differences are listed below.

|           Bidirectional **<u>_iterators_</u>**            |            Bidirectional **<u>_generators_</u>**             |
| :-------------------------------------------------------: | :----------------------------------------------------------: |
| Don't support 2 way traversal <br/>of the data structure. | Don't support 2 way traversal <br/>of the generated values.  |
|          Don't Allow for 2 way transfer of data.          | Allow for 2 way transfer of data <br/>between the caller and the function. |

