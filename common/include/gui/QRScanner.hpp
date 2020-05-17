/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
 *       * Requiring preservation of specified reasonable legal notices or
 *         author attributions in that material or in the Appropriate Legal
 *         Notices displayed by works containing it.
 *       * Prohibiting misrepresentation of the origin of that material,
 *         or requiring that modified versions of such material be marked in
 *         reasonable ways as different from the original version.
 */

#ifndef QRSCANNER_HPP
#define QRSCANNER_HPP

#include "Configuration.hpp"
#include "PGF.hpp"
#include "PGT.hpp"
#include "PK4.hpp"
#include "PK5.hpp"
#include "PK6.hpp"
#include "PK7.hpp"
#include "PK8.hpp"
#include "Sav.hpp"
#include "WC4.hpp"
#include "WC6.hpp"
#include "WC7.hpp"
#include "WC8.hpp"
#include "base64.hpp"
#include "gui.hpp"
#include "types.h"
#include <memory>
#include <string>
#include <vector>

template <typename T>
struct QRModeTraits;

template <>
struct QRModeTraits<PKX>
{
    using ReturnType = std::unique_ptr<PKX>;
};

template <>
struct QRModeTraits<PK4>
{
    using ReturnType = std::unique_ptr<PKX>;
};

template <>
struct QRModeTraits<PK5>
{
    using ReturnType = std::unique_ptr<PKX>;
};

template <>
struct QRModeTraits<PK6>
{
    using ReturnType = std::unique_ptr<PKX>;
};

template <>
struct QRModeTraits<PK7>
{
    using ReturnType = std::unique_ptr<PKX>;
};

template <>
struct QRModeTraits<PK8>
{
    using ReturnType = std::unique_ptr<PKX>;
};

// May actually return a PGT instead of a WC4, but that doesn't matter for usage
template <>
struct QRModeTraits<WC4>
{
    using ReturnType = std::unique_ptr<WCX>;
};
// May actually return a WC4 instead of a PGT, but that doesn't matter for usage
template <>
struct QRModeTraits<PGT>
{
    using ReturnType = std::unique_ptr<WCX>;
};

template <>
struct QRModeTraits<PGF>
{
    using ReturnType = std::unique_ptr<WCX>;
};

template <>
struct QRModeTraits<WC6>
{
    using ReturnType = std::unique_ptr<WCX>;
};

template <>
struct QRModeTraits<WC7>
{
    using ReturnType = std::unique_ptr<WCX>;
};

template <>
struct QRModeTraits<WC8>
{
    using ReturnType = std::unique_ptr<WCX>;
};

template <>
struct QRModeTraits<std::string>
{
    using ReturnType = std::string;
};

namespace QR_Internal
{
    // Empty == cancelled
    std::vector<u8> scan();
}

template <typename Mode>
class QRScanner
{
private:
    using Traits = QRModeTraits<Mode>;
    class QRException : public std::exception
    {
    public:
        QRException(const std::string& message) : mMessage("QRException: " + message) {}

        const char* what() const noexcept override { return mMessage.c_str(); }

    private:
        std::string mMessage;
    };

public:
    static typename Traits::ReturnType scan()
    {
        std::vector<u8> data = QR_Internal::scan();
        size_t b64Begin      = 0;
        if (data.empty())
        {
            return (typename Traits::ReturnType){};
        }

        if constexpr (std::is_same_v<Mode, PKX>)
        {
            std::string strData{data.begin(), data.end()};
            auto firstColon = strData.find_first_of(':');
            auto lastColon  = strData.find_last_of(':');
            if (firstColon == lastColon)
            {
                Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                return nullptr;
            }
            Generation g = Generation::fromString(((std::string_view)strData).substr(0, firstColon));
            if (g == Generation::UNUSED)
            {
                Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                return nullptr;
            }
            auto pkmData = base64_decode(((std::string_view)strData).substr(lastColon + 1));
            if (pkmData.empty())
            {
                Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                return nullptr;
            }
            return PKX::getPKM(g, pkmData.data(), pkmData.size(), false);
        }

        if constexpr (std::is_same_v<Mode, PK4> || std::is_same_v<Mode, PK5> || std::is_same_v<Mode, PK8> || std::is_same_v<Mode, WC4> ||
                      std::is_same_v<Mode, PGT> || std::is_same_v<Mode, PGF> || std::is_same_v<Mode, WC7> || std::is_same_v<Mode, WC8>)
        {
            if (data.size() <= 6 || !std::equal(data.begin(), data.begin() + 6, "null/#"))
            {
                Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                return nullptr;
            }
            b64Begin = 6;
        }
        if constexpr (std::is_same_v<Mode, PK6>)
        {
            if (data.size() <= 40 || !std::equal(data.begin(), data.begin() + 40, "http://lunarcookies.github.io/b1s1.html#"))
            {
                Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                return nullptr;
            }
            b64Begin = 40;
        }
        if constexpr (std::is_same_v<Mode, WC6>)
        {
            if (data.size() <= 38 || !std::equal(data.begin(), data.begin() + 38, "http://lunarcookies.github.io/wc.html#"))
            {
                Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                return nullptr;
            }
            b64Begin = 38;
        }
        if constexpr (std::is_same_v<Mode, PK7>)
        {
            if (data.size() != 0x1A2 || !std::equal(data.begin(), data.begin() + 4, "POKE"))
            {
                Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                return nullptr;
            }
        }

        // Uses base64
        if constexpr (std::is_same_v<Mode, PK4> || std::is_same_v<Mode, PK5> || std::is_same_v<Mode, PK6> || std::is_same_v<Mode, PK8> ||
                      std::is_same_v<Mode, WC4> || std::is_same_v<Mode, PGT> || std::is_same_v<Mode, PGF> || std::is_same_v<Mode, WC6> ||
                      std::is_same_v<Mode, WC7> || std::is_same_v<Mode, WC8>)
        {
            std::vector<u8> decoded = base64_decode((const char*)data.data() + b64Begin, data.size() - b64Begin);

            if constexpr (std::is_same_v<Mode, PK4> || std::is_same_v<Mode, PK5>)
            {
                if (decoded.size() != PK4::BOX_LENGTH) // PK4/PK5 length
                {
                    Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                    return nullptr;
                }

                if (std::is_same_v<Mode, PK4>)
                {
                    return PKX::getPKM<Generation::FOUR>(decoded.data());
                }
                else
                {
                    return PKX::getPKM<Generation::FIVE>(decoded.data());
                }
            }

            if constexpr (std::is_same_v<Mode, PK6>)
            {
                if (decoded.size() != PK6::BOX_LENGTH)
                {
                    Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                    return nullptr;
                }

                return PKX::getPKM<Generation::SIX>(decoded.data());
            }

            if constexpr (std::is_same_v<Mode, PK8>)
            {
                if (decoded.size() != PK8::BOX_LENGTH)
                {
                    Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                    return nullptr;
                }

                return PKX::getPKM<Generation::EIGHT>(decoded.data());
            }

            // Interchangeable, so has to be separate
            if constexpr (std::is_same_v<Mode, WC4> || std::is_same_v<Mode, PGT>)
            {
                if (decoded.size() == PGT::length)
                {
                    return std::make_unique<PGT>(decoded.data());
                }
                else if (decoded.size() == WC4::length)
                {
                    return std::make_unique<WC4>(decoded.data());
                }
                else
                {
                    Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                    return nullptr;
                }
            }

            if constexpr (std::is_same_v<Mode, PGF> || std::is_same_v<Mode, WC8>)
            {
                if (decoded.size() == Mode::length)
                {
                    return std::make_unique<Mode>(decoded.data());
                }
                else
                {
                    Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                    return nullptr;
                }
            }

            if constexpr (std::is_same_v<Mode, WC6> || std::is_same_v<Mode, WC7>)
            {
                if (decoded.size() == Mode::length)
                {
                    return std::make_unique<Mode>(decoded.data(), false);
                }
                else if (decoded.size() == Mode::lengthFull)
                {
                    return std::make_unique<Mode>(decoded.data(), true);
                }
                else
                {
                    Gui::warn(i18n::localize("QR_WRONG_FORMAT"));
                    return nullptr;
                }
            }
        }

        if constexpr (std::is_same_v<Mode, PK7>)
        {
            return PKX::getPKM<Generation::SEVEN>(data.data() + 0x30);
        }

        if constexpr (std::is_same_v<Mode, std::string>)
        {
            if (data.back() == '\0')
            {
                return std::string((char*)data.data(), data.size() - 1);
            }
            else
            {
                return std::string((char*)data.data(), data.size());
            }
        }

        // Should never happen
        throw QRException("Unknown QR mode called");
    }
};

#endif
