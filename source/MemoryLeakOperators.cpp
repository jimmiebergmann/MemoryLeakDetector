// ///////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013 Jimmie Bergmann - jimmiebergmann@gmail.com
//
// This software is provided 'as-is', without any express or
// implied warranty. In no event will the authors be held
// liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute
// it freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but
//    is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any
//    source distribution.
// ///////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

#include "MemoryLeakOperators.hpp"
#include <string>
#include <iostream>
#include <unordered_map>
#include <cstdio>
#include <Windows.h>


// Memory info struct
struct MemoryInfo
{
	std::string File;
	int Line;
	size_t Size;
};

// Globals
typedef std::unordered_map< void *, MemoryInfo > MemoryInfoHashMap;
static MemoryInfoHashMap g_AllocatedMemory;
static bool g_UseDeleteOperator = true;

struct MemoryTracking
{
	// Overload the default constructor and destructor
	MemoryTracking( )
	{
	}

	~MemoryTracking( )
	{
		if( g_AllocatedMemory.size( ) )
		{
			OutputDebugStringA( "Detected memory leaks!\n" );
			OutputDebugStringA( "Dumping objects ->\n" );
			//std::cout << "Detected memory leaks!" << std::endl;
			//std::cout << "Dumping objects ->" << std::endl;

			const size_t CompleteMessageSize = 1024;
			char CompleteMessage[ CompleteMessageSize ];

			for( MemoryInfoHashMap::iterator it = g_AllocatedMemory.begin( ); it != g_AllocatedMemory.end( ); it++ )
			{
				//std::cout << it->second.File << "(" << it->second.Line << ") : " << it->second.Size << std::endl; 
				//OutputDebugStringA( "Memory leak detected" );

				sprintf_s( CompleteMessage, CompleteMessageSize, "%s(%i) : at 0x%p, %i bytes long.\n",
					it->second.File.c_str( ), it->second.Line, it->first, it->second.Size );
				OutputDebugStringA( CompleteMessage );

			}
			g_AllocatedMemory.clear( );

			//std::cout << "Object dump complete." << std::endl;
			OutputDebugStringA( "Object dump complete.\n" );
		}

		// Do not use the custom delete operator anymore
		g_UseDeleteOperator = false;
	}

	
} MemoryTrackingInstance;

// Static help functions
static void * NewMemory( size_t p_Size, int p_Block, const char * p_pFile, int p_Line )
{
	// Allocate the memory
	void * p = malloc( p_Size ); 

	// Add the pointer to the memory tracker
	MemoryInfo Info;
	Info.File = p_pFile;
	Info.Line = p_Line;
	Info.Size = p_Size;
	g_AllocatedMemory[ p ] = Info;
	
	// Return the pointer
	return p;
}

static void DeleteMemory( void * p_Ptr )
{
	if( g_UseDeleteOperator )
	{
		MemoryInfoHashMap::iterator it = g_AllocatedMemory.find( p_Ptr );
		if( it !=  g_AllocatedMemory.end( ) )
		{
			g_AllocatedMemory.erase( it );
		}
	}
	
	// Free the memory
	free( p_Ptr );
}


void * operator new( size_t p_Size, int p_Block, const char * p_pFile, int p_Line )
{
	return NewMemory( p_Size, p_Block, p_pFile, p_Line );
}

void * operator new[ ]( size_t p_Size, int p_Block, const char * p_pFile, int p_Line )
{
	return NewMemory( p_Size, p_Block, p_pFile, p_Line );
}

void operator delete( void * p_Ptr )
{
	DeleteMemory( p_Ptr );
}

void operator delete[ ]( void * p_Ptr )
{
	DeleteMemory( p_Ptr );
}


#endif