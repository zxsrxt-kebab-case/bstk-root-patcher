//
// Created by zxsrxt on 12.03.2026.
//

#ifndef BSTK_ROOT_PATCHER_BSTK_CONF_PARSER_HPP
#define BSTK_ROOT_PATCHER_BSTK_CONF_PARSER_HPP
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <type_traits>

class bstk_conf_parser
{
    std::string m_path{};
    std::map<std::string, std::string> m_data;
public:

    struct conf_proxy
    {
        const std::map<std::string, std::string>& data;
        std::string path;

        conf_proxy operator[](const std::string& key) const
        {
            return { data, path.empty() ? key : path + "." + key };
        }

        template <typename T>
        T as() const
        {
            const auto& full_key = (path.rfind("bst.", 0) == 0) ? path : "bst." + path;

            auto it = data.find(full_key);
            const auto& val = (it != data.end()) ? it->second : "";

            if constexpr (std::is_same_v<T, std::string>)
            {
                return val;
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                try { return val.empty() ? 0 : std::stoi(val); }
                catch (...) { return 0; }
            }
            else if constexpr (std::is_same_v<T, std::vector<std::string>>)
            {
                std::vector<std::string> vec;
                if (val.empty())
                    return vec;
                std::stringstream ss(val);
                std::string item;
                while (std::getline(ss, item, ','))
                {
                    if (!item.empty())
                        vec.push_back(item);
                }
                return vec;
            }
            return T{};
        }

        operator std::string() const { return as<std::string>(); }
    };

    bstk_conf_parser() = default;

    explicit bstk_conf_parser(std::string const& path);

    conf_proxy operator[](const std::string& key) const
    {
        return { m_data, key };
    }

    bool is_empty() const { return m_data.empty(); }
};


#endif //BSTK_ROOT_PATCHER_BSTK_CONF_PARSER_HPP