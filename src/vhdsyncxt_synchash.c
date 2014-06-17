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
 * This file contains the functions to generate and operate on the hashes
 * used to sync a vhd upgrade using a seed vhd on local disk.
 *
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

/* ---------------- Header includes ---------------------------------------- */

#include <vhdsyncxt_synchash.h>

/* ---------------- Function Definitions ----------------------------------- */

r_checksum
vhd_sync_xt_calculate_r_cksum(
    char *data,
    size_t length
    )
/*
 * This function returns the rolling checksum of a block of data.
 *
 * Parameters:
 *
 *      data - Supplies a pointer to the start of the data block.
 *
 *      length - Supplies the length of the data block.
 *
 * Return Value:
 *
 *      Returns a rolling checksum struct
 */
{
    register unsigned short a = 0;
    register unsigned short b = 0;
    r_checksum r_sum;
    unsigned char current;

    while (length)
    {
        current = *data++;
        a += current;
        b += length * current;
        length--;
    }

    r_sum.a = a;
    r_sum.b = b;

    return r_sum;
}

bool
vhd_sync_xt_calculate_md5_checksum(
    char *data,
    size_t length,
    char *md5sum
    )
/*
 * This function returns the md5 checksum of a block of data.
 *
 * Parameters:
 *
 *      data - Supplies a pointer to the start of the data block.
 *
 *      length - Supplies the length of the data block.
 *
 *      md5sum - Supplies a placeholder to return the md5 checksum.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */
{
    bool status;
    int return_code;
    EVP_MD_CTX md5_context;
    unsigned int md5_length;

    status = false;

    return_code = EVP_DigestInit(&md5_context, EVP_md5());
    if (!return_code)
    {
        status = false;
        goto End;
    }

    return_code = EVP_DigestUpdate(&md5_context, data, length);
    if (!return_code)
    {
        status = false;
        goto End;
    }

    return_code = EVP_DigestFinal_ex(&md5_context, md5sum, &md5_length);
    if (!return_code)
    {
        status = false;
        goto End;
    }

    return_code = EVP_MD_CTX_cleanup(&md5_context);
    if (!return_code)
    {
        status = false;
        goto End;
    }

    status = true;

End:
    return status;
}


bool
vhd_sync_xt_create_synchash(
    char* input_file_path,
    char* destination_directory
    )
{
    bool status;
    FILE *out;
    FILE *in;
    pvhd_sync_xt_synchash_header synchash_header;
    char *basec, *base_name;
    struct timeval time_value;
    struct timezone time_zone;
    char output_file_path[VHD_SYNC_XT_PATH_LENGTH];

    status = false;

    synchash_header = calloc(sizeof(vhd_sync_xt_synchash_header), 1);
    if (synchash_header == NULL)
    {

        status = false;
        goto End;
    }

    //
    // Open our input file.
    //
    in = fopen(input_file_path, "r");
    if (in == NULL)
    {
        status = false;
        goto End;
    }

    //
    // Generate a name for our outputfilename and open the file.
    //
    basec = strdup(input_file_path);
    base_name = basename(basec);
    snprintf(output_file_path,
             VHD_SYNC_XT_PATH_LENGTH,
             "%s/%s%s",
             destination_directory,
             base_name,
             VHD_SYNC_XT_SYNCHASH_EXTENSION
             );

    out = fopen(output_file_path, "w+");
    if  (out == NULL)
    {
        status = false;
        goto End;
    }

    ///
    // Set the fields of our syncash_header.
    //
    strncpy(synchash_header->filename, base_name, VHD_SYNC_XT_PATH_LENGTH);

    //
    // Find the size of the input file.
    //
    fseek(in, 0, SEEK_END);
    synchash_header->file_length = ftell(in);

    if (!gettimeofday(&time_value, &time_zone))
    {
        status = false;
        goto End;
    }



    status = true;

End:
    return status;

}

