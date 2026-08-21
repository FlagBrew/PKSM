#include "Subsystems.hpp"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

namespace
{
    std::vector<std::string> log;

    void record(const std::string& entry) { log.push_back(entry); }
}

int main()
{
    // Whatever comes up goes back down, in reverse, exactly once.
    {
        log.clear();
        pksm::Subsystems subsystems;
        assert(subsystems.acquire("a", [] { record("a up"); }, [] { record("a down"); }));
        assert(subsystems.acquire("b", [] { record("b up"); }, [] { record("b down"); }));
        assert(subsystems.acquire("c", [] { record("c up"); }, [] { record("c down"); }));
        assert(subsystems.size() == 3);
        assert(subsystems.holds("b"));
        assert(!subsystems.holds("d"));
        assert(!subsystems.failure());

        subsystems.releaseAll();
        assert(subsystems.size() == 0);
        assert(!subsystems.holds("b"));
        assert((log == std::vector<std::string>{
                          "a up", "b up", "c up", "c down", "b down", "a down"}));

        // Idempotent: nothing is released twice, and neither will the destructor.
        subsystems.releaseAll();
        assert(log.size() == 6);
    }

    // A failure partway up releases the prefix that came up, and nothing else. The step
    // that failed has no teardown recorded, because it never came up.
    {
        log.clear();
        {
            pksm::Subsystems subsystems;
            assert(subsystems.acquire("a", [] { record("a up"); }, [] { record("a down"); }));
            assert(!subsystems.acquire(
                "b", [] { record("b up"); return false; }, [] { record("b down"); }));
            assert(subsystems.failure());
            assert(subsystems.failure()->name == "b");
            assert(subsystems.failure()->status < 0);
            assert(subsystems.size() == 1);
        }
        assert((log == std::vector<std::string>{"a up", "b up", "a down"}));
    }

    // Once a step has failed, nothing later runs at all - not even its `up`.
    {
        log.clear();
        pksm::Subsystems subsystems;
        assert(!subsystems.acquire("a", [] { return -7; }, [] { record("a down"); }));
        assert(!subsystems.acquire("b", [] { record("b up"); return 0; }));
        assert(subsystems.failure()->name == "a");
        assert(subsystems.failure()->status == -7);
        assert(log.empty());
        assert(subsystems.size() == 0);
    }

    // The three shapes an init comes in. A Result is taken at its word: only negative is
    // failure, so libctru's convention survives the trip.
    {
        pksm::Subsystems subsystems;
        assert(subsystems.acquire("void", [] {}));
        assert(subsystems.acquire("bool", [] { return true; }));
        assert(subsystems.acquire("result", [] { return 0; }));
        assert(subsystems.acquire("positive result", [] { return 1; }));
        assert(subsystems.size() == 4);
    }

    // A step with nothing to release still takes its place in the sequence.
    {
        log.clear();
        pksm::Subsystems subsystems;
        assert(subsystems.acquire("a", [] { record("a up"); }, [] { record("a down"); }));
        assert(subsystems.acquire("check", [] { record("check"); }));
        assert(subsystems.holds("check"));
        subsystems.releaseAll();
        assert((log == std::vector<std::string>{"a up", "check", "a down"}));
    }

    // Every acquire and release is reported, in order, so the startup log follows the
    // sequence without a line per step written by hand.
    {
        log.clear();
        pksm::Subsystems subsystems;
        subsystems.observe(
            [](std::string_view name, pksm::Subsystems::Event event)
            {
                record(std::string(name) +
                       (event == pksm::Subsystems::Event::Acquired ? " acquired" : " released"));
            });
        assert(subsystems.acquire("a", [] {}, [] {}));
        assert(subsystems.acquire("b", [] {}, [] {}));
        assert(!subsystems.acquire("c", [] { return false; }, [] {}));
        subsystems.releaseAll();
        assert((log == std::vector<std::string>{
                          "a acquired", "b acquired", "b released", "a released"}));
    }

    std::printf("Subsystems: all checks passed\n");
    return 0;
}
