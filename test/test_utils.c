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
 *
 * This contains generic functions used by all tests.
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

/* ---------------- Header includes --------------------------------------- */
#include "vhdsyncxt_test.h"

/* ---------------- Function Definitions -----------------------------------*/
void
print_test_results(
    pvhd_sync_xt_test test_array,
    int test_count
    )
/*
 * This function prints the results of the tests.
 *
 * Parameters:
 *
 *      test_array - Supplies the array of tests.
 *
 *      test_count - Supplies the number of elements in the array.
 *
 * Return Value:
 *
 *      None.
 */
{
    int i;
    int j;

    printf("****************************** Test Results**************************\n");
    for(i=0; i < test_count; ++i)
    {
        printf("%d.   %s",
                i+1,
                test_array[i].test_name);
        for (j=0; j< (60 - strlen(test_array[i].test_name));++j)
        {
            printf(" ");
        }

        printf("%s\n",
               (test_array[i].result == true)?"PASS":"FAIL"
               );
    }
    printf("*********************************************************************\n");
}

bool
run_tests(
    pvhd_sync_xt_test test_array,
    int test_count
    )
/*
 * This function runs all the tests in the array.
 *
 * Parameters:
 *
 *      test_array - Supplies the array of tests.
 *
 *      test_count - Supplies the number of elements in the array.
 *
 * Return Value:
 *
 *      TRUE if all tests pass, FALSE otherwise.
 */
{
    int i;
    bool status;

    status = true;
    for (i=0; i<test_count; ++i)
    {
        test_array[i].result = test_array[i].test_function();
        status = status && test_array[i].result;

    }
}

