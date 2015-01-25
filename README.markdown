# GTGameEngine

GTGameEngine is yet another game engine written in C++. It currently uses OpenGL,
but it has been designed to (hopefully) allow a D3D implementation relatively easily.

GTGameEngine supports Windows and Linux and compiles with MSVC, GCC and Clang. Other
compilers have not been tested, but should mostly work. The Linux build usually lags
behind the Windows build, so expect some errors with the Linux build occasionally.

GTGameEngine is built as a static library. There is no direct support for
shared/dynamic libraries (.so/.dll).

This is very early in development - expect a lot of bugs and backwards compatibility
issues.


GTGameEngine depends on the following libraries:

* GTLib (https://github.com/mackron/GTLib)
* GLM (http://glm.g-truc.net)
* Bullet (http://bulletphysics.org/wordpress)
* assimp (http://assimp.sourceforge.net)
* TinyXML-2 (https://github.com/leethomason/tinyxml2)
* Recast+Detour (http://code.google.com/p/recastnavigation)
* HACD (http://sourceforge.net/projects/hacd)
* OpenAL (see note below)


TinyXML-2, Recast, Detour and HACD are included in the engine's source tree, so there
is no need to build these manually. Licences are below.

For audio we use OpenAL. We use OpenAL Soft (http://kcat.strangesoft.net/openal.html)
as our OpenAL implementation, but you should be able to use the implementation from
Creative just fine.



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


-------------------------------------------------------------------------------


Recast + Detour


Copyright (c) 2009 Mikko Mononen memon@inside.org

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
   
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
   
3. This notice may not be removed or altered from any source distribution.


-------------------------------------------------------------------------------


TinyXML-2


TinyXML-2 is released under the zlib license:

This software is provided 'as-is', without any express or implied warranty. In
no event will the authors be held liable for any damages arising from the use
of this software.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject to
the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim
   that you wrote the original software. If you use this software in a product,
   an acknowledgment in the product documentation would be appreciated but is
   not required.
   
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
	
3. This notice may not be removed or altered from any source distribution.
