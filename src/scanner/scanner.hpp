//
// Created by zxsrxt on 10.03.2026.
//

#ifndef BSTK_ROOT_PATCHER_SCANNER_HPP
#define BSTK_ROOT_PATCHER_SCANNER_HPP

#include <vector>
#include <string>
#include <optional>
#include <cstdint>
#include <windows.h>

struct signature
{
    std::vector<std::optional<uint8_t>> pattern;
    std::vector<uint8_t> patch_bytes;

    signature(const std::string& pattern_str, const std::vector<uint8_t>& patch);
};

class vhd_mmap_scanner
{
public:
    explicit vhd_mmap_scanner(const std::string& path);
    ~vhd_mmap_scanner();

    vhd_mmap_scanner(const vhd_mmap_scanner&) = delete;
    vhd_mmap_scanner& operator=(const vhd_mmap_scanner&) = delete;

    bool is_ready() const { return p_data != nullptr; }

    bool patch(const signature& sig);

private:
    HANDLE h_file = INVALID_HANDLE_VALUE;
    HANDLE h_map = INVALID_HANDLE_VALUE;
    std::string file_path;
    uint8_t* p_data = nullptr;
    uint64_t file_size = 0;
};


#endif //BSTK_ROOT_PATCHER_SCANNER_HPP
