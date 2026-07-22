#pragma once
#include <fstream>
#include <string>
#include <string_view>

std::string load_file(std::string_view p_path) 
{
    std::ifstream file(std::string(p_path), std::ios::binary | std::ios::ate);
    if (!file) return {};
    const size_t size = static_cast<size_t>(file.tellg());
    std::string buffer(size, '\0');
    file.seekg(0);
    file.read(buffer.data(), size);
    return buffer;
}
