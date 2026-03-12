//
// Created by zxsrxt on 10.03.2026.
//
#include "fs_utils.hpp"
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <vector>
#include <string>

#include "../log_utils/log_utils.hpp"

namespace fs = std::filesystem;

std::string fs_utils::get_bluestacks_data_dir(const std::string& key)
{
    HKEY hKey;
    LONG lRes = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\BlueStacks_nxt", 0, KEY_READ, &hKey);
    if (lRes != ERROR_SUCCESS)
    {
        return "";
    }

    char buffer[MAX_PATH];
    DWORD bufferSize = sizeof(buffer);

    lRes = RegQueryValueExA(hKey, key.c_str(), nullptr, nullptr, (LPBYTE)buffer, &bufferSize);
    RegCloseKey(hKey);

    if (lRes == ERROR_SUCCESS)
    {
        return {buffer};
    }
    return "";
}

std::string fs_utils::find_root_vhd()
{
    std::string data_dir = get_bluestacks_data_dir("DataDir");

    if (data_dir.empty())
    {
        logger::log_error("can`t find root vhd data directory");
        return "";
    }

    //try severAL instances
    std::vector<std::string> instances = {"Pie64", "Rvc64", "Nougat64", "Nougat32"};

    for (const auto& instance : instances)
    {
        fs::path vhd_path = fs::path(data_dir) / instance / "Root.vhd";

        logger::log_info("trying to find at {}", vhd_path.string());

        if (fs::exists(vhd_path))
        {
            return vhd_path.string();
        }
    }

    return "";
}
std::string fs_utils::find_hd_player()
{
    std::string install_dir = get_bluestacks_data_dir("InstallDir");

    if (install_dir.empty())
    {
        logger::log_error("Could not find BlueStacks/MSI installation directory in registry.");
        return "";
    }

    fs::path player_path = fs::path(install_dir) / "HD-Player.exe";

    logger::log_info("Checking for player at: {}", player_path.string());

    if (fs::exists(player_path))
    {
        return player_path.string();
    }

    fs::path alternate_path = fs::path(install_dir) / "Client" / "HD-Player.exe";
    if (fs::exists(alternate_path))
    {
        return alternate_path.string();
    }

    logger::log_error("HD-Player.exe not found in install directory.");
    return "";
}
