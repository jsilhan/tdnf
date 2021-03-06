/*
 * Copyright (C) 2015 VMware, Inc. All Rights Reserved.
 *
 * Licensed under the GNU Lesser General Public License v2.1 (the "License");
 * you may not use this file except in compliance with the License. The terms
 * of the License are located in the COPYING file of this distribution.
 */

/*
 * Module   : client.c
 *
 * Abstract :
 *
 *            tdnfclientlib
 *
 *            client library
 *
 * Authors  : Priyesh Padmavilasom (ppadmavilasom@vmware.com)
 */

#include "includes.h"

uint32_t
TDNFApplyScopeFilter(
    HyQuery hQuery,
    TDNF_SCOPE nScope
    )
{
    uint32_t dwError = 0;

    if(!hQuery || nScope == SCOPE_NONE)
    {
        dwError = ERROR_TDNF_INVALID_PARAMETER;
        BAIL_ON_TDNF_ERROR(dwError);
    }

    switch(nScope)
    {
        case SCOPE_INSTALLED:
            dwError = hy_query_filter(
                          hQuery,
                          HY_PKG_REPONAME,
                          HY_EQ,
                          HY_SYSTEM_REPO_NAME);
            BAIL_ON_TDNF_HAWKEY_ERROR(dwError);
            break;

        case SCOPE_AVAILABLE:
            dwError = hy_query_filter(
                          hQuery,
                          HY_PKG_REPONAME,
                          HY_NEQ,
                          HY_SYSTEM_REPO_NAME);
            BAIL_ON_TDNF_HAWKEY_ERROR(dwError);
            break;
           
        case SCOPE_UPGRADES:
            hy_query_filter_upgrades(hQuery, 1);
            break;

        case SCOPE_RECENT:
            hy_query_filter_latest_per_arch(hQuery, 1);
            break;
        case SCOPE_DOWNGRADES:
            hy_query_filter_downgrades(hQuery, 1);
            break;

        default:
            break;
    }

cleanup:
    return dwError;

error:
    goto cleanup;
}

uint32_t
TDNFApplyPackageFilter(
    HyQuery hQuery,
    char** ppszPackageNameSpecs
    )
{
    uint32_t dwError = 0;
    int nCmpType = HY_EQ;
    char** ppszPackagesTemp = NULL;

    if(!hQuery || !ppszPackageNameSpecs)
    {
        dwError = ERROR_TDNF_INVALID_PARAMETER;
        BAIL_ON_TDNF_ERROR(dwError);
    }

    if(!*ppszPackageNameSpecs)
    {
        goto cleanup;
    }

    ppszPackagesTemp = ppszPackageNameSpecs;
    while(ppszPackagesTemp && *ppszPackagesTemp)
    {
        if(TDNFIsGlob(*ppszPackagesTemp))
        {
            nCmpType = HY_GLOB;
            break;
        }
        ++ppszPackagesTemp;
    }

    dwError = hy_query_filter_in(
                  hQuery,
                  HY_PKG_NAME,
                  nCmpType,
                  (const char**)ppszPackageNameSpecs);
    BAIL_ON_TDNF_HAWKEY_ERROR(dwError);

cleanup:
    return dwError;

error:
    goto cleanup;
}

void
TDNFFreePackageInfo(
    PTDNF_PKG_INFO pPkgInfo
    )
{
    while(pPkgInfo)
    {
        PTDNF_PKG_INFO pPkgInfoTemp = pPkgInfo;
        pPkgInfo = pPkgInfo->pNext;

        TDNFFreePackageInfoContents(pPkgInfoTemp);
        TDNFFreeMemory(pPkgInfoTemp);
    }
}

void
TDNFFreePackageInfoArray(
    PTDNF_PKG_INFO pPkgInfoArray,
    uint32_t unLength
    )
{
    uint32_t unIndex = 0;
    if(pPkgInfoArray && unLength > 0)
    {
      for(unIndex = 0; unIndex < unLength; ++unIndex)
      {
        TDNFFreePackageInfoContents(&pPkgInfoArray[unIndex]);
      }
    }
    TDNF_SAFE_FREE_MEMORY(pPkgInfoArray);
}

void
TDNFFreePackageInfoContents(
    PTDNF_PKG_INFO pPkgInfo
    )
{
    if(pPkgInfo)
    {
      TDNF_SAFE_FREE_MEMORY(pPkgInfo->pszName);
      TDNF_SAFE_FREE_MEMORY(pPkgInfo->pszRepoName);
      TDNF_SAFE_FREE_MEMORY(pPkgInfo->pszVersion);
      TDNF_SAFE_FREE_MEMORY(pPkgInfo->pszArch);
      TDNF_SAFE_FREE_MEMORY(pPkgInfo->pszSummary);
      TDNF_SAFE_FREE_MEMORY(pPkgInfo->pszURL);
      TDNF_SAFE_FREE_MEMORY(pPkgInfo->pszLicense);
      TDNF_SAFE_FREE_MEMORY(pPkgInfo->pszDescription);
      TDNF_SAFE_FREE_MEMORY(pPkgInfo->pszFormattedSize);
      TDNF_SAFE_FREE_MEMORY(pPkgInfo->pszRelease);
    }
}

void
TDNFFreeSolvedPackageInfo(
    PTDNF_SOLVED_PKG_INFO pSolvedPkgInfo
    )
{
    int i = 0;
    if(pSolvedPkgInfo)
    {
       TDNF_SAFE_FREE_PKGINFO(pSolvedPkgInfo->pPkgsNotAvailable);
       TDNF_SAFE_FREE_PKGINFO(pSolvedPkgInfo->pPkgsExisting);
       TDNF_SAFE_FREE_PKGINFO(pSolvedPkgInfo->pPkgsToInstall);
       TDNF_SAFE_FREE_PKGINFO(pSolvedPkgInfo->pPkgsToUpgrade);
       TDNF_SAFE_FREE_PKGINFO(pSolvedPkgInfo->pPkgsToDowngrade);
       TDNF_SAFE_FREE_PKGINFO(pSolvedPkgInfo->pPkgsToRemove);
       TDNF_SAFE_FREE_PKGINFO(pSolvedPkgInfo->pPkgsUnNeeded);
       TDNF_SAFE_FREE_PKGINFO(pSolvedPkgInfo->pPkgsToReinstall);
       TDNF_SAFE_FREE_PKGINFO(pSolvedPkgInfo->pPkgsObsoleted);

       if(pSolvedPkgInfo->ppszPkgsNotResolved)
       {
           while(pSolvedPkgInfo->ppszPkgsNotResolved[i])
           {
               TDNF_SAFE_FREE_MEMORY(pSolvedPkgInfo->ppszPkgsNotResolved[i++]);
           }
       }
       TDNF_SAFE_FREE_MEMORY(pSolvedPkgInfo->ppszPkgsNotResolved);
    }
    TDNF_SAFE_FREE_MEMORY(pSolvedPkgInfo);
}
