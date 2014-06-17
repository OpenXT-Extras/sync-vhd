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
 * This is the file that contains the tests for the errror log module.
 *
 *   Sharath George (t_sharathg) Jan 2012
 */


/* ---------------- Header includes --------------------------------------- */
#include "vhdsyncxt_test.h"
#include <vhdsyncxt_errorlog.h>

/* ---------------- Struct defines and globals------------------------------*/


bool
test_errorlog_init(
    );
bool
test_errorlog_max_messages(
    );

vhd_sync_xt_test g_errorlog_tests[] =
{
        {"Errorlog init",               test_errorlog_init,             0},
        {"Errorlog log max messages",   test_errorlog_max_messages,     0}
};


/* ---------------- Function Definitions -----------------------------------*/


bool
test_errorlog_init(
    )
/*
 * This function tests init of errorlog mdule
 *
 * Parameters:
 *
 *      None.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */
{
    bool status;

    status = false;

    vhd_sync_xt_error_log_initialize();

    //
    // Check if the erorr count is 0 & over flow is false
    //
    if ((g_error_log_count != 0) || (g_error_log_overflow != false))
    {
        status = false;
        goto End;
    }

    status = true;
End:
    return status;
}


bool
test_errorlog_max_messages(
    )
/*
 * This function tests init of errorlog mdule
 *
 * Parameters:
 *
 *      None.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */
{
    bool status;
    int i;

    status = false;

    vhd_sync_xt_error_log_initialize();

    //
    // Write > MAX_ERROR_COUNT_MESSAGES
    //
    for (i=0;i<=MAX_ERROR_COUNT;++i)
    {
        VHD_SYNC_XT_ERRORLOG("%d\n",i);
    }

    //
    // Check overflow bit set.
    //
    if (g_error_log_overflow == false)
    {
        status = false;
        goto End;
    }

    status = true;
End:
    return status;
}

int
main(
    int argc,
    char *argv[]
    )
/*
 * This function is the main function that runs all the tests.
 *
 * Parameters:
 *
 *      None.
 *
 * Return Value:
 *
 *      0 if all tests succeed.
 */
{
    bool status;

    status = run_tests(g_errorlog_tests, sizeof(g_errorlog_tests)/sizeof(vhd_sync_xt_test));
    print_test_results(g_errorlog_tests, sizeof(g_errorlog_tests)/sizeof(vhd_sync_xt_test));

End:
    return (status == true)?0:1;
}
