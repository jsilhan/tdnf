/*
 * Copyright (C) 2015 VMware, Inc. All Rights Reserved.
 *
 * Licensed under the GNU Lesser General Public License v2.1 (the "License");
 * you may not use this file except in compliance with the License. The terms
 * of the License are located in the COPYING file of this distribution.
 */

/*
 * Header   : prototypes.h
 *
 * Abstract :
 *
 *            commonlib 
 *
 *            common library
 *
 * Authors  : Priyesh Padmavilasom (ppadmavilasom@vmware.com)
 */

//memory.c
uint32_t
TDNFAllocateMemory(
    size_t nNumElements,
    size_t nSize,
    void** ppMemory
    );

uint32_t
TDNFAllocateString(
    const char* pszSrc,
    char** ppszDst
    );

void
TDNFFreeMemory(
    void* pMemory
    );

//strings.c
uint32_t
TDNFAllocateString(
    const char* pszSrc,
    char** ppszDst
    );

uint32_t
TDNFSafeAllocateString(
    const char* pszSrc,
    char** ppszDst
    );

uint32_t
TDNFAllocateStringPrintf(
    char** ppszDst,
    const char* pszFmt,
    ...
    );

uint32_t
TDNFAllocateStringN(
    const char* pszSrc,
    uint32_t dwNumElements,
    char** ppszDst
    );

uint32_t
TDNFReplaceString(
    const char* pszSource,
    const char* pszSearch,
    const char* pszReplace,
    char** ppszDst
    );
