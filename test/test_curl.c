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
 * This is the file that contains the tests for the curl module.
 *
 * These tests require that the test binary first run with the commandline
 * parameter
 *
 * $ test_curl generate_files
 *
 * This generates a test_curl.bin file. This file should be copied over
 * to a webserver.
 *
 * After the test_curl.bin has been copied to some webserver that supports
 * range gets, the test should be passed the parameter of the URL
 *
 * $ test_curl http://localhost/test_curl.bin
 *
 *   Sharath George (t_sharathg) Jan 2012
 */


/* ---------------- Header includes --------------------------------------- */
#include "vhdsyncxt_test.h"
#include <vhdsyncxt_curl.h>

/* ---------------- Pre processor defines ----------------------------------*/
#define TEST_CURL_FILENAME             "test_curl.bin"
#define TEST_CURL_LOCAL_FILE           "test_curl_recieved.bin"

//
//  TEST_FILE_SIZE_INT has to be divisible by TEST_FILE_RANGE for this test.
//
#define TEST_FILE_SIZE_INT             5120000
#define TEST_FILE_RANGE                  80000

/* ---------------- Struct defines and globals------------------------------*/

bool
test_curl_initialize(
    );

bool
test_curl_init(
    );

bool
test_curl_get_header(
    );

bool
test_curl_get_data(
    );

bool
test_curl_get_data_range(
    );

vhd_sync_xt_test g_curl_tests[] =
{
        {"Curl Init",                  test_curl_init,             0},
        {"Curl Get Header",            test_curl_get_header,       0},
        {"Curl Get Data",              test_curl_get_data,         0},
        {"Curl Get Range Data",        test_curl_get_data_range,   0}
};

pvhd_sync_xt_curl_config g_curl_config;
char *g_server_url;
bool g_header_status;

/* ---------------- Function Definitions -----------------------------------*/

bool
test_curl_initialize(
    )
/*
 * This function generates the file used for further tests in this file.
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
    FILE *out;
    int i;

    status = false;

    out = fopen(TEST_CURL_FILENAME, "w+");

    for (i=0; i < TEST_FILE_SIZE_INT ; ++i)
    {
        fwrite(&i, sizeof(i), 1, out);
    }

    fclose(out);

    status = true;
End:
    return status;
}

bool
test_curl_init(
    )
/*
 * This function tests init of curl module.
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

    status = vhd_sync_xt_create_curl_config(&g_curl_config, 0);
    if (status == false)
    {
        goto End;
    }

   vhd_sync_xt_destroy_curl_config(g_curl_config);

    status = true;
End:
    return status;
}


static size_t
test_curl_header_callback(
        void *data_stream,
        size_t size,
        size_t nmemb,
        void *user_data
        )
/*
 * This function is the callback set to receive the header data. This
 * sets the header status to be true if we get the current size in the
 * header
 *
 * Parameters:
 *
 *      data_stream - Supplies a line of header data.
 *
 *      size - Supplies the size of the data unit in the stream.
 *
 *      nmemb - Supplies the number of members of the data.
 *
 *      user_data - Set to point to our download context.
 *
 * Return Value:
 *
 *      Returns the size of data recieved.
 */
{
    bool *header_status;
    unsigned long int file_size;

    header_status = (bool *) user_data;

    if(strstr(data_stream, "Content-Length"))
    {
        sscanf((char*)data_stream,
                "Content-Length: %lu",
                &file_size);

        //
        // If we have the correct file size, pass the test.
        //
        if (TEST_FILE_SIZE_INT * sizeof(int) == file_size)
        {
            *header_status = true;
        }
    }

    return size * nmemb;
}

static size_t
test_curl_header_callback_null(
        void *data_stream,
        size_t size,
        size_t nmemb,
        void *user_data
        )
/*
 * This function is the callback set to receive the header data. This
 * does nothing while recieving data.
 *
 * Parameters:
 *
 *      data_stream - Supplies a line of header data.
 *
 *      size - Supplies the size of the data unit in the stream.
 *
 *      nmemb - Supplies the number of members of the data.
 *
 *      user_data - Set to point to our download context.
 *
 * Return Value:
 *
 *      Returns the size of data recieved.
 */
{
    return size * nmemb;
}

bool
test_curl_get_header(
    )
/*
 * This function tests getting a header without the body from curl.
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

    status = vhd_sync_xt_create_curl_config(&g_curl_config, 0);
    if (status == false)
    {
        goto End;
    }

    status = vhd_sync_xt_set_url(g_curl_config, g_server_url, NULL, NULL, NULL);
    if (status == false)
    {
        goto End;
    }

    g_header_status = false;

    status = vhd_sync_xt_set_curl_process_header(g_curl_config,
                                                 test_curl_header_callback,
                                                 &g_header_status
                                                 );
    if (status == false)
    {
        goto End;
    }

    if (vhd_sync_xt_perform_curl(g_curl_config) != 0 )
    {
        status = false;
        goto End;
    }

    vhd_sync_xt_destroy_curl_config(g_curl_config);

    status = g_header_status;
End:
    return status;
}

bool
test_curl_get_data(
    )
/*
 * This function tests getting the body of the file.
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
    FILE *out = NULL;
    int test;
    int i;
    size_t bytes_read;

    status = false;

    status = vhd_sync_xt_create_curl_config(&g_curl_config, 0);
    if (status == false)
    {
        goto End;
    }

    status = vhd_sync_xt_set_url(g_curl_config, g_server_url, NULL, NULL, NULL);
    if (status == false)
    {
        goto End;
    }

    out = fopen(TEST_CURL_LOCAL_FILE, "w+");

    status = vhd_sync_xt_set_curl_get_data(g_curl_config,
                                           test_curl_header_callback_null,
                                           out
                                           );
    if (status == false)
    {
        goto End;
    }

    if (vhd_sync_xt_perform_curl(g_curl_config) != 0 )
    {
        status = false;
        goto End;
    }

    //
    // verify we have the right data in the file.
    //
    fseek(out, 0L, SEEK_SET);

    for (i = 0; i < TEST_FILE_SIZE_INT; ++i)
    {
        bytes_read = fread(&test, sizeof(int), 1, out);
        if (bytes_read < 1)
        {
            status = false;
            goto End;
        }
        if (test != i)
        {
            status = false;
            goto End;
        }
    }

    vhd_sync_xt_destroy_curl_config(g_curl_config);

    status = true;
End:
    if (out != NULL)
    {
        fclose(out);
        out = NULL;
    }
    return status;
}


bool
test_curl_get_data_range(
    )
/*
 * This function tests range gets using curl.
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
    FILE *out = NULL;
    int test;
    int i;
    size_t bytes_read;
    unsigned long int current_range_offset;

    status = false;

    status = vhd_sync_xt_create_curl_config(&g_curl_config, 0);
    if (status == false)
    {
        goto End;
    }

    status = vhd_sync_xt_set_url(g_curl_config, g_server_url, NULL, NULL, NULL);
    if (status == false)
    {
        goto End;
    }

    out = fopen(TEST_CURL_LOCAL_FILE, "w+");

    status = vhd_sync_xt_set_curl_get_data(g_curl_config,
                                           test_curl_header_callback_null,
                                           out
                                           );
    if (status == false)
    {
        goto End;
    }

    //
    // Get first range
    //
    current_range_offset = 0;
    i = 0;
    while (current_range_offset < TEST_FILE_SIZE_INT * sizeof(int))
    {
        status = vhd_sync_xt_set_curl_data_range(g_curl_config,
                                                 current_range_offset,
                                                 current_range_offset + TEST_FILE_RANGE * sizeof(int) -1
                                                 );
        if (status = false)
        {
            goto End;
        }

        if (vhd_sync_xt_perform_curl(g_curl_config) != 0 )
        {
            status = false;
            goto End;
        }

        //
        // verify we have the right data in the file.
        //
        fseek(out, current_range_offset, SEEK_SET);

        for (; i < TEST_FILE_RANGE; ++i)
        {
            bytes_read = fread(&test, sizeof(int), 1, out);
            if (bytes_read < 1)
            {
                status = false;
                goto End;
            }
            if (test != i)
            {
                status = false;
                goto End;
            }
        }

        fseek(out, 0, SEEK_END);
        current_range_offset += TEST_FILE_RANGE * sizeof(int);

        //
        // Verify we dont have more data than we asked for.
        //
        if (ftell(out) != current_range_offset)
        {
            status = false;
            goto End;
        }
    }

    vhd_sync_xt_destroy_curl_config(g_curl_config);

    status = true;
End:
    if (out != NULL)
    {
        fclose(out);
        out = NULL;
    }
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
 *      Either
 *          generate_files
 *
 *      OR
 *          <serverurl>/test_curl.bin
 *
 * Return Value:
 *
 *      0 if all tests succeed.
 */
{
    bool status;

    if (argc < 2)
    {
        status = false;
        printf("Needs one parameter! Either \"generate_files\" or <http://server/path/to/test_curl.bin>\n");
        goto End;
    }

    if (!strcmp(argv[1], "generate_files"))
    {
        status = test_curl_initialize();
        goto End;
    }

    g_server_url = argv[1];

    status = run_tests(g_curl_tests,
                       sizeof(g_curl_tests)/sizeof(vhd_sync_xt_test)
                       );
    print_test_results(g_curl_tests,
                       sizeof(g_curl_tests)/sizeof(vhd_sync_xt_test)
                       );


End:
    return (status == true)?0:1;
}
