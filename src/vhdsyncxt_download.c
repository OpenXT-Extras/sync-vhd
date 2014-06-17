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
 * This contains the functions that handle the download of a vhd from the 
 * server.  
 *
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

/* ---------------- Header includes ---------------------------------------- */

#include <vhdsyncxt_download.h>

/* ---------------- Function Definitions ----------------------------------- */
bool
vhd_sync_xt_open_partial_file(
    pvhd_sync_xt_download_context download_context
    )
/*
 * This function opens a partial download if it already exists. If it does
 * not exist yet, it creates a new partial download.
 *
 * Parameters:
 *
 *      download_context - Supplies the download context structure.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */
{
    bool status;
    int return_code;

    //
    // Generate our partial download filename.
    //
    snprintf(download_context->partial_file_path,
            VHD_SYNC_XT_PATH_LENGTH,
            "%s%s",
            download_context->local_file_path,
            VHD_SYNC_XT_PARTIAL_FILE_EXTENSION
            );

    download_context->out = fopen(download_context->partial_file_path,"a+");
    if (download_context->out == NULL)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_open_partial_file: Could not perform curl to get file size. \n");
        status = false;
        goto End;
    }

    //
    // Get the file size of our partial file.
    //
    return_code = fseek(download_context->out, 0, SEEK_END);
    if (return_code != 0)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_open_partial_file: Could not get file size of partial download. \n");
        status = false;
        goto End;
    }

    download_context->start_offset = ftell(download_context->out);

    status = true;

End:
    return status;
}

static size_t
vhd_sync_xt_header_callback_null(
        void *data_stream,
        size_t size,
        size_t nmemb,
        void *user_data
        )
/*
 * This function is the null callback that accepts the header.
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

static size_t
vhd_sync_xt_header_callback(
        void *data_stream,
        size_t size,
        size_t nmemb,
        void *user_data
        )
/*
 * This function is the callback set to receive the header data. This
 * updates the download context with useful data from the header.
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
    pvhd_sync_xt_download_context download_context;

    download_context = (pvhd_sync_xt_download_context) user_data;

    //
    // Why isnt strnstr in glibc? Fixme!!! curl makes no guarantees about null
    // termination.
    //
    if(strstr(data_stream, "Content-Length"))
    {
        sscanf((char*)data_stream,
                "Content-Length: %lu",
                &download_context->file_size);
    }

    return size * nmemb;
}


bool
vhd_sync_xt_get_download_file_size(
    pvhd_sync_xt_download_context download_context
    )
/*
 * This function sends a head request to the server to get the
 * file size of the file  we are downloading. The header data is handled
 * by the vhd_sync_xt_header_callback callback function.
 *
 * Parameters:
 *
 *      download_context - Supplies the download context structure.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */
{
    bool status;
    CURLcode res;

    status = false;

    status = vhd_sync_xt_set_curl_process_header(
                 download_context->curl_config,
                 (void*)vhd_sync_xt_header_callback,
                 (void*)download_context
                 );
    if (status == false)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_get_download_file_size: Could not set curl options for header download.\n");
        goto End;
    }

    res = vhd_sync_xt_perform_curl(download_context->curl_config);
    if (res != CURLE_OK)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_get_download_file_size: Could not perform curl to get file size. \n");
        status = false;
        goto End;
    }

    status = true;

End:
    return status;
}

void
vhd_sync_xt_update_progress(
    pvhd_sync_xt_download_context download_context
    )
{
    int progress;
    char progress_message[VHD_SYNC_XT_PROGRESS_MESSAGE_LENGTH];

    //
    // Update our progress on each chunk.
    //
    progress = download_context->current_offset * 100
               / download_context->file_size;

    snprintf(progress_message,
            VHD_SYNC_XT_PROGRESS_MESSAGE_LENGTH,
            "Progress : %d\n",
            progress
            );

    if (download_context->progress_fd != 0)
    {
        write(download_context->progress_fd,
              progress_message,
              strlen(progress_message)
              );
    }
}

int
vhd_sync_xt_start_download(
    pvhd_sync_xt_download_context download_context
    )
/*
 * This function starts the download of the file from the server. It tries to
 * download this file using multiple range GET reqeusts from the server.
 *
 *
 * Parameters:
 *
 *      download_context - Supplies the download context structure.
 *
 * Return Value:
 *
 *      Errno. TBD.
 */
{
    CURLcode res;
    bool status;
    FILE *test = NULL;
    long unsigned int end_offset;

    status = false;
    res = 1;

    //
    // Get the size of the file we are downloading.
    //
    status = vhd_sync_xt_get_download_file_size(download_context);
    if (status == false)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_start_download: Could not get the file size.\n");
        goto End;
    }

    //
    // Add check to see if we already have the file.
    //
    test = fopen(download_context->local_file_path,"rb");
    if (test != NULL)
    {
        //
        // Local file exists exit.
        //
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_start_download: Local file exists.\n");
        fclose(test);
        test = NULL;
        res = VHD_SYNC_XT_ERROR_FILE_EXISTS;
        goto End;
    }

    //
    // Open our partial file and set our start_offest if not 0.
    //
    status = vhd_sync_xt_open_partial_file(download_context);
    if (status == false)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_start_download: Could not open partial file.\n");
        goto End;
    }


    status = vhd_sync_xt_set_curl_get_data(download_context->curl_config,
                                           vhd_sync_xt_header_callback_null,
                                           download_context->out
                                           );
    if(status == false)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_start_download: Could not set curl options for download\n");
        goto End;
    }

    //
    //  Download in chunks
    //

    download_context->current_offset = download_context->start_offset;
    while (download_context->current_offset < download_context->file_size)
    {
        end_offset = download_context->current_offset
                     + download_context->chunk_size
                     -1;
        if (end_offset > download_context->file_size)
        {
            end_offset = download_context->file_size;
        }

        vhd_sync_xt_update_progress(download_context);

        //
        // Set our data range.
        //
        status = vhd_sync_xt_set_curl_data_range(download_context->curl_config,
                                                 download_context->current_offset,
                                                 end_offset
                                                 );
        if(status == false)
        {
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_start_download: Could not set curl range option for download.\n");
            goto End;
        }

        //
        // Get our range data.
        //
        res = vhd_sync_xt_perform_curl(download_context->curl_config);

        //
        // update our current offset.
        //
        download_context->current_offset = end_offset + 1;
    }

End:
    return res;

}


bool
vhd_sync_xt_finalize_download(
    pvhd_sync_xt_download_context download_context
    )
/*
 * This function finalizes the download.
 *
 *
 * Parameters:
 *
 *      download_context - Supplies the download context structure.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */
{
    bool status;
    int result;

    status = false;

    result = rename(download_context->partial_file_path,
                    download_context->local_file_path
                    );
    if (result !=0)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_finalize_download: Could not rename complete file.\n");
        status = false;
        goto End;
    }

    vhd_sync_xt_update_progress(download_context);

    status = true;

End:
    return status;
}

bool
vhd_sync_xt_create_download_context(
    pvhd_sync_xt_curl_config curl_config,
    char* local_path,
    char* local_filename,
    char* url,
    char* ca_cert,
    char* ca_path,
    char* credentials,
    int progress_fd,
    pvhd_sync_xt_download_context* download_context
    )
/*
 * This function creates a context struct for downloading a file from the 
 * server.
 *
 * Parameters:
 *
 *      curl_config - Supplies a curl_config struct that was created.
 *
 *      local_path - Supplies the local path where the file is to be downloaded
 *
 *      local_filename - Supplies the local filename.
 *
 *      url - Supplies the url to get the file from.
 *
 *      ca_cert - Supplies a certificate for the server.
 *
 *      ca_path - Supplies a certificate path to verify the server cert.
 *
 *      credentials - Supplies the login credentials for auth with the server.
 *
 *      progress_fd - Supplies a file descriptor to write progress status to.
 *
 *      download_context - Supplies a placeholder to return the download context
 *          that was created.
 *          
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */  
{
    bool status;
    pvhd_sync_xt_download_context download_context_local;

    status = false;

    download_context_local = calloc(1, sizeof(vhd_sync_xt_download_context));
    if (download_context_local == NULL)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_create_download_context: Could not allocate memory for download context.\n");
        status = false;
        goto End;
    }

    if (curl_config == NULL)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_create_download_context: did not get a curl context.\n");
        status = false;
        goto End;
    }
    download_context_local->curl_config = curl_config;

    download_context_local->local_path = local_path;
    download_context_local->local_filename = local_filename;
    download_context_local->url = url;
    download_context_local->ca_cert = ca_cert;
    download_context_local->ca_path = ca_path;
    download_context_local->credentials = credentials;

    download_context_local->progress_fd = progress_fd;

    //
    // Set default chunk size.
    //
    download_context_local->chunk_size = VHD_SYNC_XT_DEFAULT_CHUNK_SIZE;

    //
    // Generate our local filename including path.
    //
    snprintf(download_context_local->local_file_path,
            VHD_SYNC_XT_PATH_LENGTH,
            "%s/%s",
            download_context_local->local_path,
            download_context_local->local_filename
            );

    //
    // Set the curl url
    //
    status = vhd_sync_xt_set_url(download_context_local->curl_config,
                                 download_context_local->url,
                                 download_context_local->ca_cert,
                                 download_context_local->ca_path,
                                 download_context_local->credentials
                                 );
    if (status == false)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_create_download_context: could not set url.\n");
        goto End;
    }

    *download_context = download_context_local;
    download_context_local = NULL;

    status = true;
End:
    if (download_context_local != NULL)
    {
        vhd_sync_xt_destroy_download_context(download_context_local);
    }
    return status;
}



void
vhd_sync_xt_destroy_download_context(
    pvhd_sync_xt_download_context download_context
    )
/*
 * This function destroys a download_context struct that stores the download 
 * configuration.
 * 
 * Parameters:
 *
 *      download_context - Supplies a poitner to the download context.
 *
 * Return Value:
 *
 *      None.
 */  
{
    if (download_context == NULL)
    {
        return;
    }

    if (download_context->out != NULL)
    {
    	fclose(download_context->out);
    }

    free (download_context);
}



