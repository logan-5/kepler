# kepler
A 3D game engine written for learning purposes. I hope to learn about new and interesting realtime 3D graphics techniques, technologies, and tricks by implementing them here. I don't have any formal training in graphics whatsoever, and so everything implemented here is self-taught, either through inventing solutions to problems myself, or finding learning resources wherever they exist in the wide world.

# goals
* Despite being a learning space, code should be well-architected, rather than slapdash or thrown together. 
* Abstractions should abound, and be powerful, easy to use, and hard to misuse.
* Strong typing, from graphics API constructs to general programming tools, is paramount. Silly mistakes, like passing a function a parameter in degrees when it expects radians, should be impossible.
* C++ code should be highly idiomatic and use the full depth of expressiveness the language provides. Advanced language features like template metaprogramming should certainly be used when they help the resulting code achieve the rest of the goals listed here.

# features
This list will grow as I continue to learn new stuff. Features as of now:
* Blinn-Phong shading
* Deferred shading
* Forward shading

# building/running
This project uses CMake. The typical process on Mac OS or Linux of setting up a build folder and then running might look like:
```git clone https://github.com/logan-5/kepler.git
cd kepler
git submodule update --init
mkdir build
cd build
cmake -G"Unix Makefiles" ..
make
./kepler
