/* filename          /Kernel32/Source/Page.c
 * date              2018.11.07
 * last edit date    2018.11.07
 * author            NO.00[UNKNOWN]
 * brief             source file for paging function
*/

#include "Page.h"


/**
 *  function name : kInitializePageTables
 *  brief         : generate paging table for IA-32e Kernel
 */
void kInitializePageTables( void )
{
	PML4TENTRY* pstPML4TEntry;
	PDPTENTRY* pstPDPTEntry;
	PDENTRY* pstPDEntry;
	DWORD dwMappingAddress;
	int i;

	// generate PML4 table
	// init by 0 except first entry
	pstPML4TEntry = ( PML4TENTRY* ) 0x100000;
	kSetPageEntryData( &( pstPML4TEntry[ 0 ] ), 0x00, 0x101000, PAGE_FLAGS_DEFAULT,
			0 );
	for( i = 1 ; i < PAGE_MAXENTRYCOUNT ; i++ )
		kSetPageEntryData( &( pstPML4TEntry[ i ] ), 0, 0, 0, 0 );
	
	
	// page directory pointer table
	// 512GByte can be mapped by on PDPT
	// 64 entry for 64GByte mapping
	pstPDPTEntry = ( PDPTENTRY* ) 0x101000;
	for ( i = 0 ; i < 64 ; i++ )
		kSetPageEntryData( &( pstPDPTEntry[ i ] ), 0, 0x102000 + ( i * PAGE_TABLESIZE ), PAGE_FLAGS_DEFAULT, 0 );
	
	for ( i = 64 ; i < PAGE_MAXENTRYCOUNT ; i++ )
		kSetPageEntryData( &( pstPDPTEntry[ i ] ), 0, 0, 0, 0 );
	
	
	// page directory table
	// one page directory can map 1GByte
	// 64 page directory for 64GByte
	pstPDEntry = ( PDENTRY* ) 0x102000;
	dwMappingAddress = 0;
	for( i = 0 ; i < PAGE_MAXENTRYCOUNT * 64 ; i++ )
	{
		kSetPageEntryData( &( pstPDEntry[ i ] ), ( i * ( PAGE_DEFAULTSIZE >> 20 ) ) >> 12, dwMappingAddress, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0 );
		dwMappingAddress += PAGE_DEFAULTSIZE;
	}	
}

/**
 *  function name : kSetPageEntryData
 *  brief         : set base address and flag to page entry
 */
void kSetPageEntryData( PTENTRY* pstEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags )
{
	pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
	pstEntry->dwUpperBaseAddressAndEXB = ( dwUpperBaseAddress & 0xFF ) | 
		dwUpperFlags;
}