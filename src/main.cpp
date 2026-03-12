/*
 * Copyright 2026 zxsrxt
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 */


#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <windows.h>

#include "fs_utils/fs_utils.hpp"
#include "log_utils/log_utils.hpp"
#include "log_utils/сonsole_ui.hpp"
#include "parsers/bstk_conf_parser.hpp"
#include "scanner/scanner.hpp"

std::unordered_map<std::string, std::vector<signature>> g_sigs_map
{
    {
        "Nougat32_patch",
        {
            signature("55 89 E5 53 56 83 E4 ? 83 EC ? E8 ? ? ? ? 5B 81 C3 ? ? ? ? 8D 83 ? ? ? ? 89 04 24 E8 ? ? ? ? 8D 83 ? ? ? ? 89 04 24 C7 44 24 ? ? ? ? ? E8 ? ? ? ? 85 C0 78 ? 89 04 24",
                {0xB0, 0x01, 0xC3}),
        }
    },
    {
        "Nougat32_unpatch",
        {
            signature("B0 01 C3 53 56 83 E4 ? 83 EC ? E8 ? ? ? ? 5B 81 C3 ? ? ? ? 8D 83 ? ? ? ? 89 04 24 E8 ? ? ? ? 8D 83 ? ? ? ? 89 04 24 C7 44 24 ? ? ? ? ? E8 ? ? ? ? 85 C0 78 ? 89 04 24",
                {0x55, 0x89, 0xE5}),
        }
    },
    {
        "Pie64_patch",
        {
            signature("48 8B 05 ?? ?? ?? ?? 48 85 C0 74 ?? 48 8D 0D ?? ?? ?? ?? 31 D2 66 66 2E 0F 1F 84 00 ? ? ? ? 39 39 74",
                {0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3}),
            signature("53 48 8D 3D ? ? ? ? 31 C0 E8 ? ? ? ? 48 8D 3D ? ? ? ? BE ? ? ? ? E8 ? ? ? ? 85 C0 78 ? 89 C7",
                {0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3}),
        }
    },
    {
        "Pie64_unpatch",
        {
            signature("B8 01 00 00 00 C3 00 48 85 C0 74 ?? 48 8D 0D ?? ?? ?? ?? 31 D2 66 66 2E 0F 1F 84 00 ? ? ? ? 39 39 74",
                {0x48, 0x8b, 0x05, 0x69, 0x7a, 0x12}),
            signature("B8 01 00 00 00 C3 ? ? 31 C0 E8 ? ? ? ? 48 8D 3D ? ? ? ? BE ? ? ? ? E8 ? ? ? ? 85 C0 78 ? 89 C7",
                {0x53, 0x48, 0x8D, 0x3D, 0xA8, 0x66}),
        }
    },
    {
        "Rvc64_patch",
        {
            signature("48 8B 05 ? ? ? ? 48 85 C0 74 ? 48 8D 0D ? ? ? ? 31 D2 66 2E 0F 1F 84 00 ? ? ? ? 90 39 39 74",
                {0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3}),
            signature("53 48 8D 3D ? ? ? ? 31 C0 E8 ? ? ? ? 48 8D 3D ? ? ? ? BE ? ? ? ? E8 ? ? ? ? 85 C0 78 ? 89 C7",
                {0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3}),
        }
    },
    {
        "Rvc64_unpatch",
        {
            signature("B8 01 00 00 00 C3 ? 48 85 C0 74 ? 48 8D 0D ? ? ? ? 31 D2 66 2E 0F 1F 84 00 ? ? ? ? 90 39 39 74 ?",
                {0x48, 0x8b, 0x05, 0x99, 0x48, 0x00}),
            signature("B8 01 00 00 00 C3 ? ? 31 C0 E8 ? ? ? ? 48 8D 3D ? ? ? ? BE ? ? ? ? E8 ? ? ? ? 85 C0 78 ? 89 C7",
                {0x53, 0x48, 0x8D, 0x3D, 0x3C, 0xD1}),
        }
    },
    {
        "Tiramisu64_patch",
        {
            signature("48 8B 0D ? ? ? ? 48 85 C9 74 ? B0 ? 39 3D ? ? ? ? 74 ? BE ? ? ? ? 48 8D 05 ? ? ? ? 66 2E 0F 1F 84",
                {0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3}),
            signature("53 48 8D 3D ? ? ? ? 31 C0 E8 ? ? ? ? 48 8D 3D ? ? ? ? BE ? ? ? ? E8 ? ? ? ? 85 C0 78 ? 89 C7",
                {0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3}),
        }
    },
    {
        "Tiramisu64_unpatch",
        {
            signature("B8 01 00 00 00 C3 ? 48 85 C9 74 ? B0 ? 39 3D ? ? ? ? 74 ? BE ? ? ? ? 48 8D 05 ? ? ? ? 66 2E 0F 1F 84",
                {0x48, 0x8b, 0x0D, 0x09, 0x48, 0x00}),
            signature("B8 01 00 00 00 C3 ? ? 31 C0 E8 ? ? ? ? 48 8D 3D ? ? ? ? BE ? ? ? ? E8 ? ? ? ? 85 C0 78 ? 89 C7",
                {0x53, 0x48, 0x8D, 0x3D, 0x0C, 0xD1}),
        }
    },

    {
        "HD-Player.exe_patch",
        {
            signature("48 89 5C 24 ? 48 89 7C 24 ? 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? E8",
                {0xB0, 0x01, 0xC3})
        }
    },
    {
        "HD-Player.exe_unpatch",
        {
            signature("B0 01 C3 24 ? 48 89 7C 24 ? 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? E8",
                {0x48, 0x89, 0x5C}),
        }
    },
};


void apply_signatures_from_map(vhd_mmap_scanner& scanner, const std::string& target_key)
{
    if (g_sigs_map.contains(target_key))
    {
        const auto& sigs = g_sigs_map[target_key];
        logger::log_info("Loaded {} signatures for [{}]", sigs.size(), target_key);

        for (const auto& sig : sigs)
        {
            scanner.patch(sig);
        }
    }
    else
    {
        logger::log_error("No signatures found in map for: {}", target_key);
    }
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    console_ui ui;

    ui.set_color(Color::Gray);
    std::cout << "---- bluestacks root patcher by zxsrxt ----" << std::endl;
    ui.reset_color();

    const auto& nxt_path = fs_utils::get_bluestacks_data_dir("UserDefinedDir");
    if (nxt_path.empty())
    {
        logger::log_error("can`t find data path, exiting...");
        std::cin.get();
        return 1;
    }
    auto config_path = std::filesystem::path(nxt_path) / "bluestacks.conf";

    bstk_conf_parser config(config_path.string());
    if (config.is_empty())
    {
        logger::log_error("Failed to parse bluestacks.conf at: {}", config_path.string());
        std::cin.get();
        return 1;
    }

    auto instance_ids = config["installed_images"].as<std::vector<std::string>>();

    if (instance_ids.empty())
    {
        logger::log_error("No installed instances found in config.");
        std::cin.get();
        return 1;
    }

    auto cfg_instance = config["instance"];
    ui.set_color(Color::Yellow);
    for (size_t i = 0; i < instance_ids.size(); ++i)
    {
        const auto& id = instance_ids[i];
        auto display_name = cfg_instance[id]["display_name"].as<std::string>();

        std::cout << std::format("[{}] {}: {}", i, id, display_name) << std::endl;
    }

    std::cout << std::endl << "Choose instance: " << std::endl;
    ui.reset_color();

    int instance_choice = -1;
    std::cin >> instance_choice;

    if (instance_choice < 0 || instance_choice >= instance_ids.size())
    {
        logger::log_error("Invalid index! Please enter a number between 0 and {}", instance_ids.size() - 1);
        std::cin.get();
        return 1;
    }

    const auto& selected_instance = instance_ids[instance_choice];
    const auto& root_vhd_path = fs_utils::find_root_vhd(nxt_path, selected_instance);
    if (root_vhd_path.empty())
    {
        logger::log_error("can`t find Root.vhd file, exiting...");
        std::cin.get();
        return 1;
    }
    const auto& hd_player_path = fs_utils::find_hd_player();
    if (hd_player_path.empty())
    {
        logger::log_error("can`t find HD-Player.exe file, exiting...");
        std::cin.get();
        return 1;
    }

    logger::log_info("Found Root.vhd: {}", root_vhd_path);

    ui.set_color(Color::Yellow);
    std::cout << "\n[0] Patch (Enable Root)\n[1] Unpatch (Restore Original)\n\nSelect option: ";
    ui.reset_color();

    int choice;
    std::cin >> choice;
    {
        std::string action_str;
        if (choice == 0) action_str = "patch";
        else if (choice == 1) action_str = "unpatch";
        else {
            logger::log_error("Invalid choice.");
            return 1;
        }

        const auto& vhd_sig_key = selected_instance + "_" + action_str;
        const auto& hd_player_sig_key = "HD-Player.exe_" + action_str;

        vhd_mmap_scanner scanner(root_vhd_path);
        if (!scanner.is_ready())
        {
            logger::log_error("Could not open Root.vhd. Make sure BlueStacks is closed!");
            return 1;
        }

        vhd_mmap_scanner hd_player_scanner(hd_player_path);
        if (!hd_player_scanner.is_ready())
        {
            logger::log_error("Could not open HD-Player.exe. Make sure BlueStacks is closed!");
            return 1;
        }

        logger::log_info("Starting {} process for {}...", action_str, selected_instance);

        apply_signatures_from_map(scanner, vhd_sig_key);

        apply_signatures_from_map(hd_player_scanner, hd_player_sig_key);
    }

    logger::log_info("Done. Press Enter to exit...");
    std::cin.ignore();
    std::cin.get();
    return 0;
}
