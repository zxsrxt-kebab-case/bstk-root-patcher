//
// Created by zxsrxt on 10.03.2026.
//

#ifndef BSTK_ROOT_PATCHER_FS_UTILS_HPP
#define BSTK_ROOT_PATCHER_FS_UTILS_HPP
#include <string>

//simple class wrapper for functions related to fs(find bstk paths)
class fs_utils
{
public:
    static std::string get_bluestacks_data_dir(const std::string& key);
    static std::string find_root_vhd(const std::string& path, const std::string& instance);
    static std::string find_hd_player();
};


#endif //BSTK_ROOT_PATCHER_FS_UTILS_HPP