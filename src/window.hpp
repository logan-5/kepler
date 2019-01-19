#ifndef WINDOW_H
#define WINDOW_H

#include "types.hpp"

#include <functional>
#include <memory>
#include <string>

class Window {
   public:
    using ErrorCallback = void (*)(int, const char*);
    Window(Resolution resolution,
           const std::string& title,
           ErrorCallback errorCallback);
    Window(Window&&);
    Window& operator=(Window&&);
    ~Window();

    bool shouldClose() const;
    void update();

   private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif
