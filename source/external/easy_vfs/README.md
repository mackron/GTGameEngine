# About
easy_vfs is an implementation of a virtual file system which allows you to load files
from archives/packages (such as zip files) using a common API.

Some noteworthy features:
 - Supports verbose absolute paths to avoid ambiguity. For example you can specify a path
   such as "my/package.zip/file.txt"
 - Supports shortened, transparent paths by automatically scanning for supported archives. The
   path "my/package.zip/file.txt" can be shortened to "my/file.txt", for example.
 - Fully recursive. A path such as "pack1.zip/pack2.zip/file.txt" should work just fine.
 - Easily supports custom package formats - just implement the relevant callbacks. See
   the "extras" folder for examples.
 - No compulsory dependencies except for the C standard library.
    - Optionally depends on miniz.c which is included in easy_vfs' source tree and released
	  under the same licence. This is required for ZIP archive support.
	- Optionally depends on easy_path which is used to strip away some duplicate code if
	  your project happens to already use it (such as my projects).

Limitations:
 - When a file contained within a ZIP file is opened, the entire uncompressed data is loaded
   onto the heap. Keep this in mind when working with large files.
 - ZIP archives are read-only at the moment.
 - Any API's that involve changing a file need to be passed a verbose, absolute path. This is
   to ensure there is no ambiguity when choosing the relevant file. In the future there will
   be a special base directory for write operations.

 
# How to use it
There's just a few files. Just add these to your project's source tree and you should be
good to go. The "extras" folder contains implementations of supported archive formats -
just include the ones you want, and leave out the ones you don't.

Below is an example:
```c
// Create a context.
easyvfs_context* pVFS = easyvfs_createcontext();
if (pVFS == NULL)
{
	// There was an error creating the context.
}

// Register the archive callbacks. This enables support for a particular type of
// archive. If you do not specify any archives only the native file system will be
// supported.
easyvfs_registerarchivecallbacks_zip(pVFS);	// ZIP files.
easyvfs_registerarchivecallbacks_mtl(pVFS);	// Wavefront MTL files.

// Add your base directories for loading from relative paths. If you do not specify at
// least one base directory you will need to load from absolute paths.
easyvfs_addbasedirectory(pVFS, "C:/Users/Admin");
easyvfs_addbasedirectory(pVFS, "C:/My/Folder");

...

// Open a file. A relative path was specified which means it will first check it against
// "C:/Users/Admin". If it can't be found it will then check against "C:/My/Folder".
easyvfs_file* pFile = easyvfs_openfile(pVFS, "my/file.txt", easyvfs_readwrite);
if (pFile == NULL)
{
	// There was an error loading the file. It probably doesn't exist.
}

easyvfs_readfile(pFile, buffer, bufferSize);
easyvfs_closefile(pFile);

...

// Shutdown.
easyvfs_deletecontext(pVFS);
```


# Platforms
Currently, only Windows is supported. The platform-specific section is quite small so it
should be quite easy to add support for other platforms.

Support for Linux is coming soon.


# TODO
 - Add support for Linux
 - Add support for a base directory for write operations
 - Add support for Quake 2 PAK files, but extend it to allow longer file names.
 - Improve ZIP files.


# License
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>