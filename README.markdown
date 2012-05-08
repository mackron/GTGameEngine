# GTGameEngine

GTGameEngine is yet another game engine written in C++. Apart from every other GalickTech
middleware library, it uses Bullet for physics, Lua for scripting and Cg for shaders. It
currently uses OpenGL, but it has been designed to (hopefully) allow a D3D implementation
relatively easily.

GTGameEngine supports Windows and Linux and compiles with MSVC, GCC and Clang. Other
compilers have not been tested, but should mostly work. The Linux build usually lags
behind the Windows build, so expect some errors with the Linux build occasionally.

GTGameEngine is built as a static library. We do not directly support shared/dynamic
libraries (.so/.dll).


GTGameEngine depends on the following libraries:

* GTCore, pcre
* GTImage, libpng, zlib
* GTType, FreeType 2
* GTGUI
* assimp




## Licencing

GTGameEngine is licenced under the MIT licence:

Copyright (C) 2012 by GalickTech Pty. Ltd.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

