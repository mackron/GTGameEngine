# GTGameEngine

GTGameEngine is a small game engine written in C++. It's intended to be small, simple
and easy to use. Some features of this project that sets it apart from others:

* Small builds
  * No large and bloated dependencies
  * A lot of functionality can be excluded using compile-time options
  * The goal is to make it possible for small games to have a binary size of <1MB
* Simple build system
  * External dependencies are included directly into the source tree where possible
  * Minimal external dependencies, and no hard-to-build dependencies
  * Where possible, dependencies are linked at run-time instead of compile time

This is very early in development - expect a lot of bugs and backwards compatibility
issues.


GTGameEngine depends on the following libraries:

* stb (https://github.com/nothings/stb)
* Lua (http://http://www.lua.org/)



## Licencing

GTGameEngine is licenced under the MIT licence:

Copyright (C) 2013 David Reid

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
