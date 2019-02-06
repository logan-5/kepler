#ifndef INPUT_HPP
#define INPUT_HPP

#include "kepler_config.hpp"

#include <functional>
#include <map>
#include <utility>
#include <vector>

NS_KEPLER_BEGIN

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
        B,
        Esc,
        LeftArrow,
        RightArrow,
        UpArrow,
        DownArrow,
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

NS_KEPLER_END

#endif
