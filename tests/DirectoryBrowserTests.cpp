/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2026 Bernardo Giordano, Admiral Fish, piepie62
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

#include "DirectoryBrowser.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace
{
    int failures = 0;

    void check(bool condition, const char* what)
    {
        if (!condition)
        {
            failures++;
            std::printf("FAIL: %s\n", what);
        }
    }

    // A real tree under /tmp: the browser's whole job is talking to a filesystem, so the
    // tests give it one.
    class TemporaryTree
    {
    public:
        TemporaryTree()
        {
            char pattern[]   = "/tmp/pksm-browser-XXXXXX";
            const char* made = mkdtemp(pattern);
            root             = made ? made : "";
        }

        ~TemporaryTree()
        {
            for (auto path = made.rbegin(); path != made.rend(); ++path)
            {
                remove(path->c_str());
            }
            rmdir(root.c_str());
        }

        TemporaryTree(const TemporaryTree&)            = delete;
        TemporaryTree& operator=(const TemporaryTree&) = delete;

        const std::string& path() const { return root; }

        std::string directory(const std::string& name)
        {
            const std::string full = root + '/' + name;
            mkdir(full.c_str(), 0777);
            made.push_back(full);
            return full;
        }

        void file(const std::string& name)
        {
            const std::string full = root + '/' + name;
            if (FILE* out = fopen(full.c_str(), "w"))
            {
                fclose(out);
            }
            made.push_back(full);
        }

    private:
        std::string root;
        std::vector<std::string> made;
    };

    std::vector<std::string> names(const DirectoryBrowser& browser)
    {
        std::vector<std::string> ret;
        for (const auto& entry : browser.entries())
        {
            ret.push_back(entry.name);
        }
        return ret;
    }

    void listingIsOrderedAndFreeOfDots()
    {
        TemporaryTree tree;
        tree.directory("zeta");
        tree.directory("Alpha");
        tree.file("beta.txt");
        tree.file("alpha.txt");

        DirectoryBrowser browser(tree.path());

        check(browser.good(), "a readable root is good");
        check(names(browser) == std::vector<std::string>{"Alpha", "zeta", "alpha.txt", "beta.txt"},
            "folders come first, each group alphabetical");
        check(browser.entries()[0].directory && !browser.entries()[3].directory,
            "entries report whether they are folders");
        for (const auto& name : names(browser))
        {
            check(name != "." && name != "..", "the implicit dot entries never appear");
        }
    }

    void navigationStaysInsideTheRoot()
    {
        TemporaryTree tree;
        tree.directory("scripts");
        tree.file("scripts/inner.c");
        tree.file("outer.c");

        DirectoryBrowser browser(tree.path());

        check(browser.atRoot(), "a fresh browser sits at its root");
        check(!browser.leave(), "leaving the root fails");
        check(browser.path() == tree.path(), "a failed leave does not move");

        check(!browser.enter(1), "entering a file fails");
        check(browser.path() == tree.path(), "a failed enter does not move");
        check(!browser.enter(99), "entering past the end fails");

        check(browser.enter(0), "entering a folder succeeds");
        check(browser.path() == tree.path() + "/scripts", "entering joins one path component");
        check(!browser.atRoot(), "a descended browser is not at its root");
        check(names(browser) == std::vector<std::string>{"inner.c"}, "the new listing is read");
        check(
            browser.pathOf(0) == tree.path() + "/scripts/inner.c", "pathOf spells out a selection");
        check(browser.pathOf(1).empty(), "pathOf is empty past the end");

        check(browser.leave(), "leaving a subfolder succeeds");
        check(browser.path() == tree.path(), "leaving strips one path component");
        check(browser.atRoot(), "the browser is back at its root");
    }

    void trailingSlashesNameTheSamePlace()
    {
        TemporaryTree tree;
        tree.directory("scripts");

        DirectoryBrowser browser(tree.path() + "/");

        check(browser.path() == tree.path(), "a trailing slash is dropped from the root");
        check(browser.atRoot(), "the trailing slash still names the root");
        check(browser.pathOf(0) == tree.path() + "/scripts", "joins never double the slash");

        DirectoryBrowser device("romfs:");
        check(device.path() == "romfs:/", "a bare device keeps the slash its children hang off");

        DirectoryBrowser filesystemRoot("/");
        check(filesystemRoot.path() == "/", "the filesystem root keeps its only slash");
        if (!filesystemRoot.entries().empty())
        {
            check(filesystemRoot.pathOf(0).rfind("//", 0) != 0,
                "joining onto the filesystem root does not double the slash");
        }
    }

    void unreadableDirectoriesAreReported()
    {
        TemporaryTree tree;
        const std::string missing = tree.path() + "/nowhere";

        DirectoryBrowser browser(missing);

        check(!browser.good(), "an unreadable root is not good");
        check(browser.entries().empty(), "an unreadable root lists nothing");
        check(browser.path() == missing, "an unreadable root is still the reported path");
        check(!browser.leave(), "an unreadable root cannot be left");
    }

    void rerootMovesTheFloorOrKeepsTheOldOne()
    {
        TemporaryTree tree;
        tree.directory("scripts");
        tree.file("scripts/inner.c");

        DirectoryBrowser browser(tree.path());

        check(browser.reroot(tree.path() + "/scripts"), "rerooting to a readable path succeeds");
        check(browser.path() == tree.path() + "/scripts", "rerooting moves the listing");
        check(browser.atRoot(), "the new root is the root");
        check(!browser.leave(), "the browser cannot climb above the new root");

        check(!browser.reroot(tree.path() + "/nowhere"), "rerooting to an unreadable path fails");
        check(browser.path() == tree.path() + "/scripts", "a failed reroot keeps the location");
        check(browser.good(), "a failed reroot keeps the readable listing");
        check(names(browser) == std::vector<std::string>{"inner.c"}, "the kept listing is intact");
    }
}

int main()
{
    listingIsOrderedAndFreeOfDots();
    navigationStaysInsideTheRoot();
    trailingSlashesNameTheSamePlace();
    unreadableDirectoriesAreReported();
    rerootMovesTheFloorOrKeepsTheOldOne();

    if (failures == 0)
    {
        std::printf("DirectoryBrowser: all checks passed\n");
        return 0;
    }
    std::printf("DirectoryBrowser: %d check(s) failed\n", failures);
    return 1;
}
