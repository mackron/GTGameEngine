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

* GTCore
  * pcre (http://www.pcre.org)
  * Lua (http://www.lua.org)
* GTImage
  * libpng (http://www.libpng.org)
  * zlib (http://zlib.net)
* GTType
  * FreeType 2 (http://www.freetype.org)
* GTGUI
* GLM (http://glm.g-truc.net)
* Bullet (http://bulletphysics.org/wordpress)
* assimp (http://assimp.sourceforge.net)
* Recast+Detour (http://code.google.com/p/recastnavigation)
* RapidXML (http://rapidxml.sourceforge.net)
* HACD (http://sourceforge.net/projects/hacd)




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


-------------------------------------------------------------------------------


HACD


Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
All rights reserved.
 

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. The names of the contributors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
