#include <jpfsv.h>
#include "test.h"

static void LaunchNotepad(
	__out PPROCESS_INFORMATION ppi
	)
{
	STARTUPINFO si;
	WCHAR Cmd[] = L"\"notepad.exe\"";

	ZeroMemory( &si, sizeof( STARTUPINFO ) );
	ZeroMemory( ppi, sizeof( PROCESS_INFORMATION ) );
	si.cb = sizeof( STARTUPINFO );

	TEST( CreateProcess(
		NULL,
		Cmd,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		ppi ) );
}

//static void LoadAllModulesOfProcess(
//	__in DWORD ProcId,
//	__in JPFSV_HANDLE Resolver
//	)
//{
//	JPFSV_ENUM_HANDLE Enum;
//	HRESULT Hr = E_UNEXPECTED;
//
//	TEST_OK( JpfsvEnumModules( 0, ProcId, &Enum ) );
//
//	do
//	{
//		JPFSV_MODULE_INFO Mod;
//		Mod.Size = sizeof( JPFSV_MODULE_INFO );
//		Hr = JpfsvGetNextItem( Enum, &Mod );
//		
//		TEST( SUCCEEDED( Hr ) );
//
//		TEST_OK( JpfsvLoadModule( 
//			Resolver, 
//			Mod.ModulePath,  
//			Mod.LoadAddress,
//			Mod.ModuleSize ) );
//	}
//	while ( S_OK == Hr );
//}

static void TestLoadModules()
{
	JPFSV_HANDLE ResolverOwn;
	JPFSV_HANDLE ResolverNp;

	PROCESS_INFORMATION pi;
	LaunchNotepad( &pi );

	//
	// Give notepad some time to start...
	//
	Sleep( 1000 );

	TEST_OK( JpfsvLoadContext( pi.dwProcessId, NULL, &ResolverNp ) );
	TEST_OK( JpfsvLoadContext( pi.dwProcessId, NULL, &ResolverNp ) );
	TEST_OK( JpfsvLoadContext( GetCurrentProcessId(), NULL, &ResolverOwn ) );

	//TEST_OK( JpfsvLoadModule( 
	//	ResolverOwn, 
	//	L"jpfsv.dll",  
	//	( DWORD_PTR ) GetModuleHandle( L"jpfsv.dll" ),
	//	0 ) );

	//LoadAllModulesOfProcess( pi.dwProcessId, ResolverNp );

	TEST_OK( JpfsvUnloadContext( ResolverOwn ) );
	TEST_OK( JpfsvUnloadContext( ResolverNp ) );
	TEST_OK( JpfsvUnloadContext( ResolverNp ) );

	TEST( TerminateProcess( pi.hProcess, 0 ) );
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	//
	// Wait i.o. not to confuse further tests with dying process.
	//
	Sleep( 1000 );
}

static void TestLoadKernelModules()
{
	JPFSV_HANDLE Kctx;
	JPFSV_ENUM_HANDLE Enum;
	JPFSV_MODULE_INFO Mod;
	BOOL Wow64;
	HRESULT Hr;

	TEST( IsWow64Process( GetCurrentProcess(), &Wow64 ) );
	if ( Wow64 )
	{
		CFIX_INCONCLUSIVE( L"Kernel context loading cannot be used on "
			L"WOW64 - need to run natievely." );
	}

	//
	// Get a module.
	//
	TEST_OK( JpfsvEnumModules( 0, JPFSV_KERNEL, &Enum ) );
	
	Mod.Size = sizeof( JPFSV_MODULE_INFO );
	TEST_OK( JpfsvGetNextItem( Enum, &Mod ) );

	//
	// Load Ctx.
	//
	TEST_OK( JpfsvLoadContext( JPFSV_KERNEL, NULL, &Kctx ) );

	Hr = JpfsvLoadModuleContext(
		Kctx,
		Mod.ModulePath,
		Mod.LoadAddress,
		Mod.ModuleSize );
	TEST( S_OK == Hr || S_FALSE == Hr );	// depends on testcase order.

	TEST( S_FALSE == JpfsvLoadModuleContext(
		Kctx,
		Mod.ModulePath,
		Mod.LoadAddress,
		Mod.ModuleSize ) );

	TEST_OK( JpfsvUnloadContext( Kctx ) );
}

static void TestKernelContext()
{
	JPFSV_HANDLE Kctx;
	BOOL Wow64;

	TEST( IsWow64Process( GetCurrentProcess(), &Wow64 ) );

	if ( Wow64 )
	{
		TEST( JPFSV_E_UNSUP_ON_WOW64 == JpfsvLoadContext( JPFSV_KERNEL, NULL, &Kctx ) );
	}
	else
	{
		TEST_OK( JpfsvLoadContext( JPFSV_KERNEL, NULL, &Kctx ) );
		TEST_OK( JpfsvUnloadContext( Kctx ) );
	}
}

BEGIN_FIXTURE( SymResolver )
	FIXTURE_ENTRY( TestLoadModules )
	FIXTURE_ENTRY( TestLoadKernelModules )
	FIXTURE_ENTRY( TestKernelContext )
END_FIXTURE()