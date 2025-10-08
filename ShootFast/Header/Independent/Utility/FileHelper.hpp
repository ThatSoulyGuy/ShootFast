#ifndef SHOOTFAST_FILE_HELPER_HPP
#define SHOOTFAST_FILE_HELPER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include "Independent/Utility/AssetPath.hpp"

namespace ShootFast::Independent::Utility
{
    class FileHelper final
    {

    public:

        FileHelper(const FileHelper&) = delete;
        FileHelper(FileHelper&&) = delete;
        FileHelper& operator=(const FileHelper&) = delete;
        FileHelper& operator=(FileHelper&&) = delete;

        static std::string ReadFile(const AssetPath& path)
        {
            std::ifstream file(path.GetFullPath(), std::ios::binary | std::ios::ate);

            if (!file.is_open())
            {
                std::cerr << "Failed to read file '" << path.GetFullPath() << "'!\n";
                return {};
            }

            const auto fileSize = file.tellg();
            file.seekg(0);

            std::string result;

            result.resize(fileSize);

            if (!file.read(result.data(), fileSize))
            {
                std::cerr << "Failed to read contents of '" << path.GetFullPath() << "'!\n";
                return {};
            }

            return result;
        }

    private:

        FileHelper() = default;

    };
}

#endif