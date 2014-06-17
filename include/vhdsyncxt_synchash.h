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
 * This is the header file that contains the main declarations for 
 * generating and comparing hashes for syncing images from the server. 
 *
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

#ifndef _VHD_SYNC_XT_SYNC_HASH_H_
#define _VHD_SYNC_XT_SYNC_HASH_H_

/* ---------------- External Header includes ------------------------------- */
#include <stdio.h>
#include <openssl/evp.h>

/* ---------------- Internal Header includes ------------------------------- */
#include <vhdsyncxt_errorlog.h>

/* ---------------- PreProcessor Defines ----------------------------------- */

#define VHD_SYNC_XT_SYNCHASH_EXTENSION              ".synchash"

#define VHD_SYNC_XT_SHA1_HASH_SIZE                  20


/* ---------------- Structure Defines -------------------------------------- */
typedef enum 
_vhd_sync_xt_synchash_hash_type
{
    HASH_TYPE_SHA1=0,
    HASH_TYPE_MAXIMUM
}vhd_sync_xt_synchash_hash_type, *pvhd_sync_xt_synchash_hash_type;

typedef struct _vhd_sync_xt_synchash_version
{
    short int   major_version;
    short int   minor_version;
} vhd_sync_xt_synchash_version, *pvdh_sync_xt_synchash_version;


//
// This is the header of the synchhash that goes at the begining of the 
// synchash file. The header is 1KB.
// 
// A lot of padding has been added round the size of the header as well as
// to add features that will in the future allow us to store features per
// vhd block, that might include some vhd parameters as well.
//
typedef struct _vhd_sync_xt_synchash_header
{
    vhd_sync_xt_synchash_version version;                           // Offset 0
    char                         filename[VHD_SYNC_XT_PATH_LENGTH]; // Offset 8
    unsigned long int            file_length;                     // Offset 264
    unsigned long int            timestamp;                       // Offset 272
    unsigned int                 block_size;                      // Offset 280
    unsigned int                 hash_type;                       // Offset 284
    char                         padding1[224];                   // Offset 288
    union                                                         // Offset 512
    {
        char                     sha1_hash[VHD_SYNC_XT_SHA1_HASH_SIZE];
        char                     padding2[512];                           
    };
                                                           // Total Size : 1024
} vhd_sync_xt_synchash_header, *pvhd_sync_xt_synchash_header;

//
// Struct for the rolling checksum to be calculated as described in the
// rsync paper.
//
typedef struct _r_checksum
{
    unsigned short int a;
    unsigned short int b;
} r_checksum, *pr_checksum;

//
// The header of the synchash file is followed by one of these structs for each
// block of the input file. 
//
typedef struct _vhd_sync_xt_synchash_block_hash
{
    r_checksum                 rolling_checksum;
    unsigned char              md5_digest[16];
} vhd_sync_xt_synchash_block_hash, *pvhd_sync_xt_synchash_blockhash;

/* ---------------- Function Declarations -----------------------------------*/
r_checksum
vhd_sync_xt_calculate_r_cksum(
    char *data,
    size_t length
    );

bool
vhd_sync_xt_create_synchash(
    char* input_file_path,
    char* destination_directory
    );

bool
vhd_sync_xt_calculate_md5_checksum(
    char *data,
    size_t length,
    char *md5sum
    );
#endif  // ifndef _VHD_SYNC_XT_SYNCHASH_H

