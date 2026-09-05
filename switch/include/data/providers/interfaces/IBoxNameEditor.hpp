#pragma once

#include <optional>
#include <string>

#include "data/saves/SaveData.hpp"

// Per-generation box rename policy; the party box is never renamable
class IBoxNameEditor {
public:
    PU_SMART_CTOR(IBoxNameEditor)
    virtual ~IBoxNameEditor() = default;

    virtual bool CanRenameBox(const pksm::saves::SaveData::Ref& saveData, int boxIndex) const = 0;

    // Longest name the save can store, in characters; 0 when not renamable
    virtual size_t GetBoxNameMaxLength(const pksm::saves::SaveData::Ref& saveData) const = 0;

    // First character the save's character set cannot store, as UTF-8; nullopt when all store
    virtual std::optional<std::string>
    FirstUnstorableBoxNameChar(const pksm::saves::SaveData::Ref& saveData, const std::string& name) const = 0;

    // Blank or wholly unstorable names are refused and keep the old name
    virtual bool RenameBox(const pksm::saves::SaveData::Ref& saveData, int boxIndex, const std::string& name) = 0;
};
