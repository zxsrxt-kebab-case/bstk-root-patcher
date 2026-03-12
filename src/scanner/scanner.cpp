//
// Created by zxsrxt on 10.03.2026.
//

#include "scanner.hpp"

#include "log_utils/log_utils.hpp"
#include "log_utils/сonsole_ui.hpp"

#include <tlhelp32.h>

signature::signature(const std::string& pattern_str, const std::vector<uint8_t>& patch)
    : patch_bytes(patch)
{
    std::istringstream iss(pattern_str);
    std::string word;

    while (iss >> word)
    {
        if (word == "?" || word == "??")
        {
            pattern.push_back(std::nullopt);
        } else
        {
            pattern.push_back(static_cast<uint8_t>(std::stoul(word, nullptr, 16)));
        }
    }
}

vhd_mmap_scanner::vhd_mmap_scanner(const std::string& path) : file_path(path)
{
    h_file = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (h_file == INVALID_HANDLE_VALUE)
        return;

    LARGE_INTEGER size;
    GetFileSizeEx(h_file, &size);
    file_size = size.QuadPart;

    h_map = CreateFileMappingA(h_file, NULL, PAGE_READWRITE, 0, 0, NULL);
    if (!h_map) return;

    p_data = static_cast<uint8_t*>(MapViewOfFile(h_map, FILE_MAP_ALL_ACCESS, 0, 0, 0));
}

vhd_mmap_scanner::~vhd_mmap_scanner() {
    if (p_data) UnmapViewOfFile(p_data);
    if (h_map) CloseHandle(h_map);
    if (h_file != INVALID_HANDLE_VALUE) CloseHandle(h_file);
}



bool vhd_mmap_scanner::patch(const signature& sig)
{
    if (!is_ready() || sig.pattern.empty()) return false;

    console_ui ui;
    bool found_any = false;
    COORD base_pos = ui.get_cursor_pos();

    std::optional<uint8_t> first_byte = sig.pattern[0];

    for (size_t i = 0; i <= file_size - sig.pattern.size(); ++i)
    {
        if (first_byte.has_value() && p_data[i] != first_byte.value())
        {
            continue;
        }

        bool match = true;
        for (size_t j = 1; j < sig.pattern.size(); ++j)
        {
            if (sig.pattern[j].has_value() && p_data[i + j] != sig.pattern[j].value())
            {
                match = false;
                break;
            }
        }

        if (match)
        {
            ui.clear_line(base_pos.Y);

            ui.set_color(Color::Green);
            std::cout << "[SUCCESS] ";
            ui.reset_color();

            std::cout << "Found at " << std::hex;
            ui.set_color(Color::Yellow);
            std::cout << "0x" << i << std::dec;
            ui.reset_color();
            std::cout << std::endl;

            base_pos = ui.get_cursor_pos();
            std::copy(sig.patch_bytes.begin(), sig.patch_bytes.end(), p_data + i);
            found_any = true;
        }

        if (i % (file_size / 200) == 0)
        {
            ui.set_cursor(0, base_pos.Y);
            ui.clear_line(base_pos.Y);
            ui.draw_progress(static_cast<float>(i) / file_size);
        }
    }

    ui.set_cursor(0, base_pos.Y);
    ui.clear_line(base_pos.Y);
    ui.draw_progress(1.0f);
    std::cout << std::endl;

    if (!found_any)
    {
        logger::log_error("Signature not found in the {}", file_path);
    } else
    {
        logger::log_info("Successfully patched {}", file_path);
        FlushViewOfFile(p_data, 0);
    }

    return found_any;
}


