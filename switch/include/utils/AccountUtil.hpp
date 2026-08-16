#pragma once

#include <functional>
#include <switch.h>

// Custom equality operator for AccountUid
inline bool operator==(const AccountUid& lhs, const AccountUid& rhs) {
    return lhs.uid[0] == rhs.uid[0] && lhs.uid[1] == rhs.uid[1];
}

// Custom hash function for AccountUid
struct AccountUidHash {
    size_t operator()(const AccountUid& uid) const {
        // Combine both u64 parts of the AccountUid using XOR
        return std::hash<u64>{}(uid.uid[0]) ^ (std::hash<u64>{}(uid.uid[1]) << 1);
    }
};