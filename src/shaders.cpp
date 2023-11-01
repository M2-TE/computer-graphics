#include <cmrc/cmrc.hpp>
CMRC_DECLARE(shaders);

const char* read_shader(const std::string& path) {
    auto fs = cmrc::shaders::get_filesystem();
    auto file = fs.open(path);
    return file.cbegin();
}