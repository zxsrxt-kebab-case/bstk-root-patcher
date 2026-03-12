//
// Created by zxsrxt on 12.03.2026.
//

#include "bstk_conf_parser.hpp"

#include <fstream>

bstk_conf_parser::bstk_conf_parser(std::string const &path) : m_path(path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return;

    m_data.clear();
    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        auto sep = line.find('=');
        if (sep == std::string::npos)
            continue;

        auto key = line.substr(0, sep);
        auto value = line.substr(sep + 1);

        std::erase(value, '\"');
        std::erase(value, '\r');

        m_data[key] = value;
    }
}
