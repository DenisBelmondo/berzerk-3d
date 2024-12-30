#ifndef EVENT_H
#define EVENT_H

#include <functional>
#include <vector>

namespace bm {

template <typename... Ts> struct Event {
    using Delegate = std::function<void(Ts...)>;
    // private:
    std::vector<Delegate> _eventQueue;
    // public:
    void addEventListener(Delegate &&delegate) {
        _eventQueue.push_back(delegate);
    }

    void invoke(Ts... args) {
        for (auto fn : _eventQueue) {
            std::invoke(fn, args...);
        }
    }
};

} // namespace bm

#endif // EVENT_H
