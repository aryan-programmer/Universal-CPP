![UC++ Logo in Iosevka SS03](E:\C++ Projects\UC++\Logo.png)

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

To use certain things like <span style="color:purple">ME</span> or <span style="color:purple">WME</span> the object has to be constructed as a `UC::GCPtr` but if the object is called with the `public` constructor then there will be undefined behaviour. So, to avoid that always make constructors `protected` or `private` **<u>_never_</u>** `public`.

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

### Remember always and only use `try{...}catch(const ?exeception-type?& v){...}//more-catch-clauses` for catching <span style="color:purple">UCException</span>s.

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

