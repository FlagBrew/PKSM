/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62, Allen Lydiard
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

#ifndef FETCH_HPP
#define FETCH_HPP

#include "types.h"
#include "utils/coretypes.h"
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>

// HTTP as PKSM needs it: three verbs, a file download, and a value that says what happened.
// Everything curl knows about - option names, the two error enums, curl_slist lifetime, mime
// parts, the stall guard, retries - is on the far side of this interface. A caller names a URL,
// a body and how long it is willing to wait; it never names a CURLOPT.
//
// This is not thread-safe on the 3DS, because SOC is not: every transfer runs on the one fetch
// thread, and the synchronous verbs are a wait on that thread rather than a transfer of their
// own.
namespace Fetch
{
    // Header lines exactly as they go on the wire ("generation: 6"). Building the curl_slist and
    // freeing it is this module's job, which is what removes the leak-on-early-return that every
    // caller used to have to get right.
    using Headers = std::vector<std::string>;

    // One part of a multipart/form-data body. The bytes are copied while the request is built,
    // so they only have to outlive the call.
    struct Part
    {
        std::string name;
        std::span<const u8> data;
    };

    // Why a request produced no HTTP answer.
    enum class Error : u8
    {
        None,      // there is a status and a body
        Init,      // no handle: the module is not up, or curl ran out of memory
        Submit,    // the request never went out
        Transport, // curl gave up: no route, TLS failure, timeout, connection dropped
        Cancelled, // cancel() reached it before it finished
    };

    struct Response
    {
        Error error = Error::None;
        // The number PKSM has always shown for a network failure, 0 when there is none: curl's
        // multi code for Submit, its easy code plus 100 for Transport. A plain int, so reporting
        // a failure does not mean knowing which of curl's two enums it came from.
        int code = 0;
        // The HTTP status. Only meaningful when ok().
        long status = 0;
        std::string body;

        bool ok() const { return error == Error::None; }

        // A failure, which by definition carries no status and no body.
        static Response failed(Error why, int code = 0) { return {why, code, 0, {}}; }
    };

    // What a request gets when the caller does not say otherwise. Long enough for the API to
    // answer, short enough that a wrong apiUrl fails while the user is still watching.
    inline constexpr long DEFAULT_TIMEOUT = 30;
    // Downloads run to whatever size the file is; the stall guard is what ends a dead one.
    inline constexpr long NO_TIMEOUT = 0;

    // The stall guard aborts a transfer that has been trickling below 300 bytes/s for ten
    // seconds, so a connection that dies mid-body does not sit on the whole timeout. It is armed
    // unless the caller asked for a timeout longer than this: a request given that much room is
    // waiting on server-side work - a legality check, a legalization, a bundle upload - which is
    // silent by design, and the guard would tear it down mid-processing. An unlimited timeout
    // keeps the guard, since a download streams the whole way through.
    //
    // That rule used to be a comment at one call site (GroupCloudAccess) and a latent bug at
    // another (LegalInfoScreen, 120 s with the guard left armed).
    inline constexpr long STALL_GUARD_LIMIT = 30;

    // Brings curl and the fetch thread up. Nothing below works before it returns 0.
    Result init();
    void exit();

    // The three verbs. Each blocks the calling thread until there is a Response.
    //
    // get() is retried on a connection-level failure; the posts are not, because a retried
    // upload is a second Pokémon on GPSS.
    Response get(
        const std::string& url, const Headers& headers = {}, long timeout = DEFAULT_TIMEOUT);
    Response postJson(const std::string& url, const std::string& body, const Headers& headers = {},
        long timeout = DEFAULT_TIMEOUT);
    Response postMultipart(const std::string& url, std::span<const Part> parts,
        const Headers& headers = {}, long timeout = DEFAULT_TIMEOUT);

    // A request in flight, and the only thing cancel() takes.
    class Request;
    using Handle = std::shared_ptr<Request>;

    // Runs exactly once per request, whatever happens: on the fetch thread when the request came
    // back or was cancelled, inline on the calling thread when it could never go out. So a
    // caller that waits on the callback for its answer always gets woken - the failure mode the
    // discarded CURLMcode used to leave behind.
    using Callback = std::function<void(Response)>;

    // Returns the handle to cancel with, or nullptr when the callback has already run.
    Handle getAsync(const std::string& url, Callback onDone = nullptr, const Headers& headers = {},
        long timeout = DEFAULT_TIMEOUT);
    Handle postJsonAsync(const std::string& url, const std::string& body, Callback onDone = nullptr,
        const Headers& headers = {}, long timeout = DEFAULT_TIMEOUT);

    // Drops a request the caller no longer wants; its callback runs with Error::Cancelled. Does
    // nothing to one that has already finished.
    void cancel(const Handle& request);

    // How far a download has got, in bytes. Called on the fetch thread, not the caller's, for
    // as long as the transfer runs.
    using Progress = std::function<void(u64 now, u64 total)>;

    // Streams the body straight to path instead of into memory, and retries like get(). Returns
    // 0, -errno if the file could not be opened, the negated failure code, or the negated HTTP
    // status when the server answered with something that is not the file. The file is removed
    // unless the download succeeded, so a failed download never leaves an error page behind
    // under the name of an asset.
    Result download(const std::string& url, const std::string& path,
        const Progress& onProgress = nullptr, long timeout = NO_TIMEOUT);
}

#endif
