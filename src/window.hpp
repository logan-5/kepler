#ifndef WINDOW_H
#define WINDOW_H

#include "types.hpp"

#include <exception>
#include <functional>
#include <memory>
#include <string>

class Input;

struct initialization_error : std::runtime_error {
    using runtime_error::runtime_error;
};

class Window {
   public:
    using WindowSizeCallback = std::function<void(Resolution)>;
    using ErrorCallback = void (*)(int, const char*);
    Window(Resolution resolution,
           const std::string& title,
           ErrorCallback errorCallback);
    Window(Window&&);
    Window& operator=(Window&&);
    ~Window();

    bool shouldClose() const;
    void update();

    float getTime() const;

    void setWindowSizeCallback(WindowSizeCallback cb);

    Input& getInput();
    const Input& getInput() const;

    Resolution getResolution() const;

    void requestClose();

   private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif
