/*----------------------------------------------------------------------
 * Purpose:
 *		WRK-specific routines.
 *
 * Copyright:
 *		Johannes Passing (johannes.passing@googlemail.com)
 */

#pragma warning(disable:4214)   // bit field types other than int
#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4115)   // named type definition in parentheses
#pragma warning(disable:4127)   // condition expression is constant
#pragma warning(disable:4242)   
#pragma warning(disable:4244)   
#pragma warning(disable:4311)   
#pragma warning(disable:4312)   
#pragma warning(disable:4273)	// inconsistent linkage

#pragma warning(disable:6320)   
#pragma warning(disable:6385)   

#include <ntos.h>

VOID JpfbtWrkSetFbtDataCurrentThread(
	__in PVOID Data 
	)
{
	PsGetCurrentThread()->ReservedForFbt = Data;
}

PVOID JpfbtWrkGetFbtDataCurrentThread()
{
	//
	// N.B. PspCreateThread zeroes the ETHREAD, so ReservedForFbt
	// is either NULL or a valid pointer.
	//
	return PsGetCurrentThread()->ReservedForFbt;
}

