#pragma once

#include <optional>
#include <string>

#include "data/saves/SaveData.hpp"

// Renames a save's display boxes. Whether a box is renamable and how long
// its name may be is per-generation policy answered here in the domain; the
// UI only asks. The party box is never renamable - its name is synthetic.
class IBoxNameEditor {
public:
    PU_SMART_CTOR(IBoxNameEditor)
    virtual ~IBoxNameEditor() = default;

    virtual bool CanRenameBox(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const = 0;

    // Longest name the save can store, in characters; 0 when not renamable
    virtual size_t GetBoxNameMaxLength(const pksm::saves::SaveData::Ref& saveData) const = 0;

    // First character of `name` the save's character set cannot store, as a
    // UTF-8 string; nullopt when the whole name stores
    virtual std::optional<std::string>
    FirstUnstorableBoxNameChar(const pksm::saves::SaveData::Ref& saveData, const std::string& name) const = 0;

    // Store the name through the save's own setter, which owns the encoding.
    // Blank names, and names the save's character set cannot represent at
    // all, are refused and keep the old name.
    virtual bool RenameBox(const pksm::saves::SaveData::Ref& saveData, int boxIndex, const std::string& name) = 0;
};
