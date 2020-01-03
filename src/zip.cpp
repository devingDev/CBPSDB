#include "zip.hpp"

#include <psp2/io/stat.h>

#define dir_delimter '/'
#define MAX_FILENAME 512
#define READ_SIZE 8192

static void mkdir_rec(const char* dir)
{
    char tmp[256];
    char* p = nullptr;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
        if (*p == '/')
        {
            *p = 0;
            sceIoMkdir(tmp, 0777);
            *p = '/';
        }
    sceIoMkdir(tmp, 0777);
}

std::string dirnameOf(const std::string& fname)
{
    size_t pos = fname.find_last_of("\\/");
    return (std::string::npos == pos) ? "" : fname.substr(0, pos);
}

Zipfile::Zipfile(const std::string zip_path)
{
    zipfile_ = unzOpen(zip_path.c_str());
    if (!zipfile_)
    {
        //"zip_path not found"
    }

    if (unzGetGlobalInfo(zipfile_, &global_info_) != UNZ_OK)
    {
        //"Could not read file global info"
    }
}

Zipfile::~Zipfile()
{
    if (zipfile_)
        unzClose(zipfile_);
}

int Zipfile::Unzip(const std::string outpath)
{
    if (uncompressed_size_ == 0)
    {
        UncompressedSize();
    }

    char read_buffer[READ_SIZE];

    uLong i;
    uint64_t s_progress = 0;
    if (unzGoToFirstFile(zipfile_) != UNZ_OK)
    {
        //"Cannot go to first file"
    }

    for (i = 0; i < global_info_.number_entry; ++i)
    {
        unz_file_info file_info;
        char filename[MAX_FILENAME];
        char fullfilepath[MAX_FILENAME];
        if (unzGetCurrentFileInfo(zipfile_, &file_info, filename, MAX_FILENAME, nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            //"Could not read file info"
        }

        sprintf(fullfilepath, "%s%s", outpath.c_str(), filename);

        // Check if this entry is a directory or file.
        const size_t filename_length = strlen(fullfilepath);
        if (fullfilepath[filename_length - 1] == dir_delimter)
        {
            mkdir_rec(fullfilepath);
        }
        else
        {
            // Create the dir where the file will be placed
            std::string destdir = dirnameOf(std::string(fullfilepath));
            mkdir_rec(destdir.c_str());

            // Entry is a file, so extract it.
            if (unzOpenCurrentFile(zipfile_) != UNZ_OK)
            {
                //"Could not open file"
            }

            // Open a file to write out the data.
            FILE* out = fopen(fullfilepath, "wb");
            if (out == nullptr)
            {
                //"Could not open destination file"
                unzCloseCurrentFile(zipfile_);
            }

            int error;
            do
            {
                error = unzReadCurrentFile(zipfile_, read_buffer, READ_SIZE);
                if (error < 0)
                {
                    // "error : ", error;
                    unzCloseCurrentFile(zipfile_);
                }

                if (error > 0)
                {
                    fwrite(read_buffer, error, 1, out); 
                    s_progress += error;
                }
            } while (error > 0);

            fclose(out);
        }

        unzCloseCurrentFile(zipfile_);

        // Go the the next entry listed in the zip file.
        if ((i + 1) < global_info_.number_entry)
        {
            if (unzGoToNextFile(zipfile_) != UNZ_OK)
            {
                //"Could not read next file"
            }
        }
    }

    return 0;
}

int Zipfile::UncompressedSize()
{
    uncompressed_size_ = 0;

    if (unzGoToFirstFile(zipfile_) != UNZ_OK)
    {
        //"Cannot go to first file"
    }

    uLong i;
    for (i = 0; i < global_info_.number_entry; ++i)
    {
        unz_file_info file_info;
        char filename[MAX_FILENAME];
        if (unzGetCurrentFileInfo(zipfile_, &file_info, filename, MAX_FILENAME, nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            //"Could not read file info"
        }

        uncompressed_size_ += file_info.uncompressed_size;

        // Go the the next entry listed in the zip file.
        if ((i + 1) < global_info_.number_entry)
        {
            if (unzGoToNextFile(zipfile_) != UNZ_OK)
            {
                //"Could not read next file"
            }
        }
    }
    return 0;
}
