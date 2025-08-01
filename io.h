#pragma once

#include <ArduinoJson.h>
#include <vector>
#include <array>
#include "FS.h"
#include <LittleFS.h>
#include "SD_MMC.h"
#include <cstdint>
#include <string>

#define DEBUG 1

#ifdef DEBUG
    #define debug(...) Serial.print(__VA_ARGS__)
    #define debugln(...) Serial.println(__VA_ARGS__)
    #define debugf(...) Serial.printf(__VA_ARGS__)
#else
    #define debug(...)
    #define debugln(...)
    #define debugf(...)
#endif

#define SD_MMC_CMD  38
#define SD_MMC_CLK  39
#define SD_MMC_D0   40

#define ANIMATIONS "//animations"
#define RENDERCACHE "//render_state.json"



/**
 * Attempt to initialize the sdcard file system. 
 * @return True if the sdcard was successfully mounted, false otherwise.
 */
bool sdmmcInit(void);
    


/**
 * Determine the file system to use based on the SD card status.
 * @return The file system to use for file operations.
 */
fs::FS& determineFileSystem(void);



/**
 * @brief   Read the contents of a file from the specified file system.
 * @warning This dynamically allocates memory for the file contents - for our use case, this is fine though.
 * @param   fs The file system to read from.
 * @param   path The path to the file to read.
 * @return  The contents of the file as a string.
 */
std::string readFile(fs::FS& fs, const std::string& path);


/**
 * @brief A simple wrapper for file paths and directory metadata.
 * @details This struct is used to represent files and directories in a file system.
 * It includes information about whether the item is a directory, its name, parent directory, and path.
 * It also provides a method to check if the item is the root directory.
 * @note The path is constructed based on the parent directory and the name of the item.
 * @warning The path is expected to be in a format compatible with the file system.
 */
struct FileWrapper {
    private:

        bool isDir;
        const FileWrapper* parent = nullptr;
        fs::FS* fs;
        std::string name;
        std::string path;
        std::vector<FileWrapper> children;
    
        /**
         * @brief List the contents of this directory.
         * @param fs The file system to list.
         * @param path The path to the directory to list.
         * @return A vector of FileWrapper objects representing the files and directories in the specified path.
         */
        std::vector<FileWrapper> _listDir(void) const {
            std::vector<FileWrapper> files;
            File root = fs->open(path.c_str());
            if (!root || !root.isDirectory()) {
                debugf("Failed to open directory: %s\n", path.c_str());
                return files;
            }

            debugf("Successfully opened %s\n", path.c_str());

            File file = root.openNextFile();
            while (file) {
                FileWrapper wrapper(*fs, std::string(file.name()), file.isDirectory(), this, {});
                files.push_back(wrapper);
                file = root.openNextFile();
            }

            debugln("Done listing");
            root.close();
            
            return files;
        }

    public:
        
        FileWrapper(
            fs::FS& filesystem,
            const std::string& fname,
            bool isdir = true,
            const FileWrapper* dir = nullptr,
            std::vector<FileWrapper> subdirs = {}
        ) : name(fname), isDir(isdir), parent(dir) {
            this->fs = &filesystem;

            if (this->isRoot()) {
                path = fname;
                debugf("Creating our root node %s\n", path.c_str());
            } else {
                path = dir ? dir->path + "/" + fname : fname;
                if (path.back() == '/') path.pop_back();
                if (path.empty()) path = "/";
                if (path.front() != '/') path.insert(0, "/");
                debugf("Creating %s %s at %s\n",isDir ? "Dir" : "File",  fname.c_str(), path.c_str());
            }

            /**
             * We needed to construct the path before we could do anything with subdirs.
             */
            if (isDir) children = subdirs;
            if (children.empty() && isDir) children = _listDir();
        }   

        bool isDirectory(void) const {
            return this->isDir;
        }

        /**
         * @brief Check if this FileWrapper represents the root directory.
         * @return True if this is the root directory, false otherwise.
         */
        bool isRoot(void) const {
            return name == "/" || name.empty() || parent == nullptr;
        }

        /**
         * @brief List the files and directories.
         * @return A vector of FileWrapper objects representing the files and directories in this FileWrapper dir.
         */
        const std::vector<FileWrapper>& listDir() const {
            return this->children;
        }

        const FileWrapper& getDir(const std::string& name) const {
            for (const FileWrapper& file : this->children) {
                if (file.isDirectory() and name == file.getName()) return file;
            }

            return *this;
        }

        const FileWrapper& getFile(const std::string& name) const {
            for (const FileWrapper& file: this->children) {
                if (!file.isDirectory() and name == file.getName()) return file;
            }

            return *this;
        }

        const std::string& getName(void) const {
            return this->name;
        }

        const std::string& getPath(void) const {
            return this->path;
        }
};