#pragma once
#include <memory>
#include <pu/Plutonium>
#include <string>

namespace pksm::saves {

class Save {
private:
    std::string name;
    std::string path;

public:
    Save(const std::string& name, const std::string& path) : name(name), path(path) {}
    PU_SMART_CTOR(Save)

    const std::string& getName() const { return name; }
    const std::string& getPath() const { return path; }
};

}  // namespace pksm::saves