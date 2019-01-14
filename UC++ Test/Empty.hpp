#pragma once


struct NEmpty
{
	int NativeBaseFunction( );
};

UCInterface( Empty ,
			 UC_WhereTypenameIsRealName ,
			 UC_InheritsUCClasses( UC::Object ) ,
			 UC_InheritsNativeClasses( NEmpty )
)
UC_HasExplicitCtors( Empty , UC_AlsoHasEmptyCtor , ( _1 ) );
UC_HasMethods(
( Do ) ,
( Do , ( _1 ) ) ,
( Do , ( _1 , _2 ) )
);

int NativeFunction( );

UCEndInterface( Empty );

