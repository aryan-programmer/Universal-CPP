#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include <boost\config.hpp>
#include <boost\algorithm\string.hpp>

#define forceinline BOOST_FORCEINLINE

namespace UC
{
	class Object;

	// PAny = Pointer to Any (U++) value
	using PAny = std::shared_ptr<Object>;
	// WAny = Weakly pointer to Any (emc2) value
	using WAny = std::weak_ptr<Object>;

	using HashVal = std::size_t;
	//TODO: Replace GCOVect & String with typed deriving from Object.

	// GCOVect = Garbage Collectible Object Vector
	using GCOVect = std::vector<PAny>;
	using String = std::string;

	template<typename T>
	using GCP = std::shared_ptr<T>;

	class Object : public std::enable_shared_from_this<Object>
	{
	public:
		virtual ~Object( );

		virtual const char* GetTypeName( ) = 0;
		virtual PAny Call( String fname , GCOVect args ) = 0;

		Object( Object&& ) = delete;
		Object& operator=( Object&& ) = delete;

		static PAny CreateInstance( String className , GCOVect args );
	protected:
		Object( );
		PAny callImpl( String fname , GCOVect args );
		static void addConstructor( String className , PAny( *ctor )( GCOVect args ) );
	private:
		static std::unordered_map<String , PAny( *)( GCOVect args )>& getClassCtors( );
	};

	class RepeatingClassNameException : public std::exception
	{
		String str;
	public:
		RepeatingClassNameException( String&& str ) noexcept : std::exception( ) , str( str ) { }
		RepeatingClassNameException( const String& str ) noexcept : std::exception( ) , str( str ) { }
		virtual char const* what( ) const override { return str.c_str( ); }
	};

	template<typename T>forceinline GCP<T> ObjCast( PAny v ) { return std::dynamic_pointer_cast< T >( v ); }
}

#include "Interface.hpp"
