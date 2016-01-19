#include "yaml.h"

#include <fstream>

namespace
{
unsigned int leadingSpaces(const std::string& string)
{
    auto number = 0u;
    for (; string[number] == ' '; ++number)
        ;
    return number;
}
}

namespace util
{
std::unique_ptr<YAMLNode> parseYAML(std::string filename)
{
    return YAMLNode::parseYAML(filename);
}

std::unique_ptr<YAMLNode> YAMLNode::parseYAML(std::string filename)
{
    auto root = std::make_unique<YAMLNode>();
    auto current = root.get();

    std::ifstream sourceFile(filename);
    std::string line;
    while (std::getline(sourceFile, line))
    {
        auto level = leadingSpaces(line);
        if (line[level] == '#' || line.size() == level)
            continue;
        for (; current->m_level >= static_cast<int>(level);
             current = current->parent())
            ;
        if (line[level] == '-')
        {
            ++level;
            for (; line[level] == ' '; ++level)
                ;
            current->m_values.push_back(line.substr(level));
        }
        else
        {
            const auto pos = line.find(":");
            const auto key = line.substr(level, pos - level);
            current->m_children.push_back(
                {key, std::make_unique<YAMLNode>(current, level)});
            current = (*current)[key];

            const auto rest = line.substr(pos + 1);
            const auto spaces = leadingSpaces(rest);
            const auto value = rest.substr(spaces);
            if (value.size() > 0)
            {
                current->m_values.push_back(value);
            }
        }
    }
    return root;
}

YAMLNode* YAMLNode::operator[](const std::string& key)
{
    for (auto& child : m_children)
    {
        if (child.first == key)
        {
            return child.second.get();
        }
    }
    m_children.push_back({key, std::make_unique<YAMLNode>(this, m_level + 1)});
    return m_children.back().second.get();
}
}
