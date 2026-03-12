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
#include "parsers/bstk_conf_parser.hpp"

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

std::string fs_utils::find_root_vhd(const std::string& path, const std::string& instance)
{
    auto vhd_path = fs::path(path) / "Engine" / instance / "Root.vhd";

    if (fs::exists(vhd_path))
    {
        return vhd_path.string();
    }

    if (path.find("Engine") == std::string::npos)
    {
        vhd_path = fs::path(path) / "Engine" / instance / "Root.vhd";
        if (fs::exists(vhd_path))
            return vhd_path.string();
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
