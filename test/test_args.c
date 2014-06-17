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
 * This is the file that contains the tests for the args module.
 *
 *   Sharath George (t_sharathg) Jan 2012
 */


/* ---------------- Header includes --------------------------------------- */
#include "vhdsyncxt_test.h"
#include <vhdsyncxt_args.h>

/* ---------------- Pre processor defines ----------------------------------*/
# define TEST_ARGS_URL                  "test_url"
# define TEST_ARGS_UUID                 "uuid.vhd"
/* ---------------- Struct defines and globals------------------------------*/


bool
test_args_init(
    );

bool
test_args_parse(
    );

vhd_sync_xt_test g_arg_tests[] =
{
        {"Arguments init",                  test_args_init,             0},
        {"Arguments parse",                 test_args_parse,            0}
};

pvhd_sync_xt_parameters g_test_parameters;

/* ---------------- Function Definitions -----------------------------------*/

bool
test_args_init(
    )
/*
 * This function tests init of args module.
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

    status = vhd_sync_xt_create_parameters(&g_test_parameters);
    if (status == false)
    {
        goto End;
    }

    if ( (g_test_parameters->progress_fd != 0)
         || (g_test_parameters->connection_socket != 0)
         || strncmp(g_test_parameters->localpath, ".", VHD_SYNC_XT_ARG_STRING_LENGTH)
        )
    {
        status = false;
        goto End;
    }

    vhd_sync_xt_destroy_parameters(g_test_parameters);

    status = true;

End:

    return status;
}

bool
test_args_parse(
    )
/*
 * This function tests parse of arguments.
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

    //
    // One block for each parameter subtest.
    //
    {
        int argc = 6;
        char *argv[]=
            {
                    "test",
                    "--download",
                    "--url",
                    TEST_ARGS_URL,
                    "--uuid",
                    TEST_ARGS_UUID,
                    ""
            };

        status = vhd_sync_xt_create_parameters(&g_test_parameters);
        if (status == false)
        {
            goto End;
        }

        status = vhd_sync_xt_parse_parameters(g_test_parameters,
                                              argc,
                                              argv
                                              );
        if (status == false
            || g_test_parameters->action != ACTION_DOWNLOAD
            || strncmp(g_test_parameters->url,
                       TEST_ARGS_URL,
                       VHD_SYNC_XT_ARG_STRING_LENGTH)
            || strncmp(g_test_parameters->imageuuid,
                       TEST_ARGS_UUID,
                       VHD_SYNC_XT_ARG_STRING_LENGTH)
            )
        {
            status = false;
            goto End;
        }
        vhd_sync_xt_destroy_parameters(g_test_parameters);
    }

    //
    // Cannot test multiple getopt calls from the same process.
    // It maintains internal state?. FIXME!
    //

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

    status = run_tests(g_arg_tests, sizeof(g_arg_tests)/sizeof(vhd_sync_xt_test));
    print_test_results(g_arg_tests, sizeof(g_arg_tests)/sizeof(vhd_sync_xt_test));

End:
    return (status == true)?0:1;
}
