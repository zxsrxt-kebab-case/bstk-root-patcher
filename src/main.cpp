/*
 * Copyright 2026 zxsrxt
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 */


#include <iostream>
#include <windows.h>

#include "fs_utils/fs_utils.hpp"
#include "log_utils/log_utils.hpp"
#include "log_utils/сonsole_ui.hpp"
#include "scanner/scanner.hpp"


void patch_all(vhd_mmap_scanner& scanner, vhd_mmap_scanner& hd_player_scanner)
{
    logger::log_info("Patching all...");

    signature patch_dev("53 48 8D 3D ?? ?? ?? ?? 31 C0 E8 ?? ?? ?? ?? 48 8D 3D ?? ?? ?? ?? BE 02 00 08 00", {0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3});
    signature patch_uid("48 8B 05 ?? ?? ?? ?? 48 85 C0 74 28 48 8D 0D ?? ?? ?? ?? 31 D2 66 66 2E 0F 1F 84 00 00 00 00 00", {0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3});

    scanner.patch(patch_dev);
    scanner.patch(patch_uid);

    signature integrity_bypass(
        "48 89 5C 24 ? 48 89 7C 24 ? 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? E8",
        { 0xB0, 0x01, 0xC3 } // return true;
    );

    hd_player_scanner.patch(integrity_bypass);
}

void unpatch_all(vhd_mmap_scanner& scanner, vhd_mmap_scanner& hd_player_scanner)
{
    signature unpatch_dev("B8 01 00 00 00 C3 0E 00 31 C0", {0x53, 0x48, 0x8D, 0x3D, 0xA8, 0x66});
    signature unpatch_uid("B8 01 00 00 00 C3 00 48 85 C0", {0x48, 0x8B, 0x05, 0x69, 0x7A, 0x12});

    scanner.patch(unpatch_dev);
    scanner.patch(unpatch_uid);

    signature integrity_unpatch(
        "B0 01 C3 24 ? 48 89 7C 24 ? 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? E8",
        { 0x48, 0x89, 0x5C } // original bytes;
    );

    hd_player_scanner.patch(integrity_unpatch);
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    console_ui ui;

    ui.set_color(Color::Gray);
    std::cout << "---- bluestacks root patcher by zxsrxt ----" << std::endl;
    ui.reset_color();

    const auto& root_vhd_path = fs_utils::find_root_vhd();
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

        if (choice == 0)
        {
            patch_all(scanner, hd_player_scanner);
        } else if (choice == 1)
        {
            unpatch_all(scanner, hd_player_scanner);
        }
    }

    logger::log_info("Done. Press Enter to exit...");
    std::cin.ignore();
    std::cin.get();
    return 0;
}
