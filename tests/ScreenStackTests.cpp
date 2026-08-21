#include "Screen.hpp"
#include "ScreenStack.hpp"
#include <cassert>
#include <memory>
#include <utility>

namespace
{
    class TestScreen final : public Screen
    {
    public:
        TestScreen(int id, std::shared_ptr<int> destroyed) : id(id), destroyed(destroyed) {}

        ~TestScreen() override { (*destroyed)++; }

        int id;

    private:
        std::shared_ptr<int> destroyed;
    };

    TestScreen* top()
    {
        return static_cast<TestScreen*>(ScreenStack::top());
    }
}

int main()
{
    auto destroyed = std::make_shared<int>(0);

    // Popping an empty stack is harmless.
    ScreenStack::requestPop();
    assert(!ScreenStack::applyPending());
    assert(ScreenStack::top() == nullptr);

    auto root         = std::make_unique<TestScreen>(1, destroyed);
    auto* rootAddress = root.get();
    ScreenStack::push(std::move(root));
    assert(ScreenStack::top() == nullptr);
    assert(ScreenStack::applyPending());
    assert(top() == rootAddress);

    auto child         = std::make_unique<TestScreen>(2, destroyed);
    auto* childAddress = child.get();
    ScreenStack::push(std::move(child));
    assert(top() == rootAddress);
    assert(ScreenStack::applyPending());
    assert(top() == childAddress);

    // A pop does not destroy the current Screen until the frame edge.
    ScreenStack::requestPop();
    assert(top() == childAddress);
    assert(*destroyed == 0);
    assert(ScreenStack::applyPending());
    assert(top() == rootAddress);
    assert(*destroyed == 1);

    // A lone root pop is ignored, so the frame loop can never dereference an empty stack.
    ScreenStack::requestPop();
    assert(!ScreenStack::applyPending());
    assert(top() == rootAddress);
    assert(*destroyed == 1);

    // Pop-then-push is an atomic replacement, including when replacing the root Screen.
    auto replacement         = std::make_unique<TestScreen>(3, destroyed);
    auto* replacementAddress = replacement.get();
    ScreenStack::requestPop();
    ScreenStack::push(std::move(replacement));
    assert(top() == rootAddress);
    assert(ScreenStack::applyPending());
    assert(top() == replacementAddress);
    assert(*destroyed == 2);

    // Excess pops stop at the root.
    ScreenStack::push(std::make_unique<TestScreen>(4, destroyed));
    assert(ScreenStack::applyPending());
    ScreenStack::requestPop();
    ScreenStack::requestPop();
    assert(ScreenStack::applyPending());
    assert(top() == replacementAddress);
    assert(*destroyed == 3);
}
