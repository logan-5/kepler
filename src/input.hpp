#ifndef INPUT_HPP
#define INPUT_HPP

#include <functional>
#include <map>
#include <utility>
#include <vector>

class Input {
    struct Impl;

   public:
    enum class Key {
        W,
        A,
        S,
        D,
        Q,
        E,
    };

    Input(std::unique_ptr<Impl> impl);
    ~Input();

    using KeyCallback = std::function<void()>;

    void setKeyCallback(Key k, KeyCallback cb) {
        keyCallbacks[k].push_back(std::move(cb));
    }

    void update();

   private:
    std::map<Key, std::vector<KeyCallback>> keyCallbacks;

    friend class Window;
    std::unique_ptr<Impl> impl;
};

#endif
