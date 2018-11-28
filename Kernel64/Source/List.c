/* filename          /Kernel64/Source/List.c
 * date              2018.11.27
 * last edit date    2018.11.27
 * author            NO.00[UNKNOWN]
 * brief             source code for list
*/

#include "List.h"

/**
 *  function name : kInitializeList
 *  parameters    : pstList(LIST*)
 *  return value  : void
 *  brief         : init list
 */
void kInitializeList( LIST* pstList )
{
    pstList->iItemCount = 0;
    pstList->pvHeader   = NULL;
    pstList->pvTail     = NULL;
}

/**
 *  function name : kGetListCount
 *  parameters    : pstList(const LIST*)
 *  return value  : int
 *  brief         : return contents number of list
 */
int kGetListCount( const LIST* pstList )
{
    return pstList->iItemCount;
}

/**
 *  function name : kGetListCount
 *  parameters    : pstList(LIST*)
 *                  pvItem(void*)
 *  return value  : void
 *  brief         : add data on list
 */
void kAddListToTail( LIST* pstList, void* pvItem )
{
    LISTLINK* pstLink;
    
    pstLink = ( LISTLINK* ) pvItem;
    pstLink->pvNext = NULL;
    
    if ( pstList->pvHeader == NULL )
    {
        pstList->pvHeader   = pvItem;
        pstList->pvTail     = pvItem;
        pstList->iItemCount = 1;

        return ;
    }
    
    pstLink         = ( LISTLINK* ) pstList->pvTail;
    pstLink->pvNext = pvItem;
    pstList->pvTail = pvItem;
    pstList->iItemCount++;
}

/**
 *  function name : kAddListToHeader
 *  parameters    : pstList(LIST*)
 *                  pvItem(void*)
 *  return value  : void
 *  brief         : add data on first place of list
 */
void kAddListToHeader( LIST* pstList, void* pvItem )
{
    LISTLINK* pstLink;
    
    pstLink = ( LISTLINK* ) pvItem;
    pstLink->pvNext = pstList->pvHeader;    
    
    if ( pstList->pvHeader == NULL )
    {
        pstList->pvHeader   = pvItem;
        pstList->pvTail     = pvItem;
        pstList->iItemCount = 1;
        
        return ;
    }
    
    pstList->pvHeader = pvItem;
    pstList->iItemCount++;
}

/**
 *  function name : kRemoveList
 *  parameters    : pstList(LIST*)
 *                  qwID(QWORD)
 *  return value  : void*
 *  brief         : remove data and return its address
 */
void* kRemoveList( LIST* pstList, QWORD qwID )
{
    LISTLINK* pstLink;
    LISTLINK* pstPreviousLink;
    
    pstPreviousLink = ( LISTLINK* ) pstList->pvHeader;
    for ( pstLink = pstPreviousLink ; pstLink != NULL ; pstLink = pstLink->pvNext )
    {
        if ( pstLink->qwID == qwID )
        {
            if ( ( pstLink == pstList->pvHeader ) && ( pstLink == pstList->pvTail ) )
            {
                pstList->pvHeader = NULL;
                pstList->pvTail   = NULL;
            }
            else if ( pstLink == pstList->pvHeader )
                pstList->pvHeader = pstLink->pvNext;
            else if ( pstLink == pstList->pvTail )
                pstList->pvTail = pstPreviousLink;
            else
                pstPreviousLink->pvNext = pstLink->pvNext;
            
            pstList->iItemCount--;
            return pstLink;
        }
        pstPreviousLink = pstLink;
    }
    return NULL;
}

/**
 *  function name : kRemoveListFromHeader
 *  parameters    : pstList(LIST*)
 *  return value  : void*
 *  brief         : remove first data and return its address
 */
void* kRemoveListFromHeader( LIST* pstList )
{
    LISTLINK* pstLink;
    
    if ( pstList->iItemCount == 0 )
        return NULL;

    pstLink = ( LISTLINK* ) pstList->pvHeader;
    return kRemoveList( pstList, pstLink->qwID );
}

/**
 *  function name : kRemoveListFromTail
 *  parameters    : pstList(LIST*)
 *  return value  : void*
 *  brief         : remove last data and return its address
 */
void* kRemoveListFromTail( LIST* pstList )
{
    LISTLINK* pstLink;
    
    if ( pstList->iItemCount == 0 )
        return NULL;
    
    pstLink = ( LISTLINK* ) pstList->pvTail;
    return kRemoveList( pstList, pstLink->qwID );
}


/**
 *  function name : kFindList
 *  parameters    : pstList(const LIST*)
 *                  qwID(QWORD)
 *  return value  : void*
 *  brief         : find data on list and return its address
 */
void* kFindList( const LIST* pstList, QWORD qwID )
{
    LISTLINK* pstLink;
    
    for ( pstLink = ( LISTLINK* ) pstList->pvHeader ; pstLink != NULL ; pstLink = pstLink->pvNext )
    {
        if ( pstLink->qwID == qwID )
            return pstLink;
    }
    return NULL;    
}

/**
 *  function name : kGetHeaderFromList
 *  parameters    : pstList(const LIST*)
 *  return value  : void*
 *  brief         : return header of list
 */
void* kGetHeaderFromList( const LIST* pstList )
{
    return pstList->pvHeader;
}

/**
 *  function name : kGetTailFromList
 *  parameters    : pstList(const LIST*)
 *  return value  : void*
 *  brief         : return tail of list
 */
void* kGetTailFromList( const LIST* pstList )
{
    return pstList->pvTail;
}

/**
 *  function name : kGetNextFromList
 *  parameters    : pstList(const LIST*)
 *                  pstCurrent(void*)
 *  return value  : void*
 *  brief         : return next data on list
 */
void* kGetNextFromList( const LIST* pstList, void* pstCurrent )
{
    LISTLINK* pstLink;
    
    pstLink = ( LISTLINK* ) pstCurrent;

    return pstLink->pvNext;
}