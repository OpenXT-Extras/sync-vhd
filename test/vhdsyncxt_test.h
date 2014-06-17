/*
 * Copyright (c) 2012 Citrix Systems, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * This contains the types and declarations for functions used by all tests.
 *
 *  Created on: 2012-02-09
 *      Author: sharathg
 */

#ifndef _VHD_SYNC_XT_TEST_H_
#define _VHD_SYNC_XT_TEST_H_
/* ---------------- External Header includes ------------------------------- */
#include <stdio.h>
#include <curl/curl.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>


/* ---------------- Structure Defines -------------------------------------- */

typedef struct _vhd_sync_xt_test
{
    char test_name[100];
    bool (*test_function)(void);
    bool result;
} vhd_sync_xt_test, *pvhd_sync_xt_test;


/* ---------------- Function Declarations ---------------------------------- */
void
print_test_results(
    pvhd_sync_xt_test test_array,
    int test_count
    );

bool
run_tests(
    pvhd_sync_xt_test test_array,
    int test_count
    );

#endif /* _VHD_SYNC_XT_TEST_H_*/
