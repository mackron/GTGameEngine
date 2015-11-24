// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Path.hpp>
#include <GTLib/BasicBuffer.hpp>
#include <GTLib/Strings/Tokenizer.hpp>
#include <GTLib/Strings/Equal.hpp>
#include <GTLib/Strings/List.hpp>
#include <GTLib/Strings/Find.hpp>
#include <GTLib/Strings/Iterator.hpp>
#include <GTLib/IO.hpp>
#include <GTLib/stdlib.hpp>

#include <cstring>

namespace GTLib
{
    Path::Path(const char *path)
        : parts(), partCount(0), leadingSlash(false), str(nullptr)
    {
        *this = path;
    }

    Path::~Path()
    {
        // We need to iterate over each item and remove them. This will delete the strings.
        while (this->parts.root != nullptr)
        {
            this->RemoveFirst();
        }

		free(this->str);
    }

    size_t Path::Size() const
    {
        size_t size = this->parts.BuildStringSize() + this->partCount;

        // We need to adjust the size depending on whether or not we have parts and a leading slash.
        if (this->partCount > 0)
        {
            if (!this->leadingSlash)
            {
                --size;
            }
        }
        else
        {
            if (this->leadingSlash)
            {
                ++size;
            }
        }

        return size;
    }

    void Path::Copy(char *dest) const
    {
        // We need to loop over each part and copy it over.
        if (this->leadingSlash)
        {
            *dest++ = '/';
        }

        Strings::ListItem<char> *root = this->parts.root;
        while (root)
        {
            size_t length = static_cast<size_t>(root->end - root->start);
            std::memcpy(dest, root->start, length);
            dest += length;

            // Need a slash between directories.
            if (root != this->parts.last)
            {
                *dest++ = '/';
            }

            root = root->next;
        }

        // Now attach on a null terminator.
        *dest = '\0';
    }

    void Path::MakeAbsolute(const char *base)
    {
        // The way we will make the path absolute is by first appending the base path to the end of the list. Then, we will
        // loop over the new parts and move them to the front. The reason we don't use a seperate list for base and then
        // just append straight to the front is because we want to avoid allocating as many list items as possible.
        if (Path::IsAbsolute(base) && !this->IsAbsolute())
        {
            // Just need to check if we're leading with a slash.
            if (base && *base == '/')
            {
                this->leadingSlash = true;
            }

            // We will make a list containing the sections from the base. We will then move those parts over to the main path
            // at let that list memory manage it.
            Strings::List<char> baseParts;
            Path::SplitPath(base, baseParts);

            // We will first run through the main path parts and remove the end items in baseParts for each "..".
            while (this->parts.root)
            {
                if (Strings::Equal(this->parts.root->start, this->parts.root->end - this->parts.root->start, "..", 2))
                {
                    this->RemoveFirst();

                    if (baseParts.last)
                    {
                        baseParts.Remove(baseParts.last);
                    }
                }
                else
                {
                    break;
                }
            }

            // Now we just rip the items from the baseParts list and attach it to the beginning of our main list.
            while (baseParts.last != nullptr)
            {
                this->parts.Prepend(GTLib::Strings::Create(baseParts.last->start, baseParts.last->end - baseParts.last->start));
                this->partCount += 1;

                baseParts.Remove(baseParts.last);
            }
        }
    }

    //void Path::MakeAbsolute()
    //{
    //    this->MakeAbsolute(GTLib::IO::GetCurrentDirectory().c_str());
    //}


    void Path::MakeRelative(const char* relativeTo)
    {
        if (Path::IsAbsolute(relativeTo) && this->IsAbsolute())
        {
            // We need to split the input path.
            Strings::List<char> relativeToParts;
            Path::SplitPath(relativeTo, relativeToParts, true);

            // We iterate over every part in the 'relativeTo' path. We remove every part until we hit a part that is not equal. Then, we just add '..' parts.
            while (relativeToParts.root != nullptr)
            {
                if (this->parts.root != nullptr)
                {
                    if (relativeToParts.root->IsEqual(*this->parts.root))
                    {
                        this->RemoveFirst();
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    // Not relative to the given path.
                    return;
                }

                relativeToParts.Remove(relativeToParts.root);
            }

            // Now we do the '..' phase.
            while (relativeToParts.root != nullptr)
            {
                this->Prepend("..");
                relativeToParts.Remove(relativeToParts.root);
            }

            // A relative path will never have a leading slash.
            this->leadingSlash = false;
        }
    }

    //void Path::MakeRelative()
    //{
    //    this->MakeRelative(GTLib::IO::GetCurrentDirectory().c_str());
    //}


    Path & Path::Append(const char* other)
    {
        Strings::List<char> otherParts;
        Path::SplitPath(other, otherParts);

        // We need to handle the leading ".." directories.
        while (otherParts.root)
        {
            if (otherParts.root && Strings::Equal(otherParts.root->start, otherParts.root->end - otherParts.root->start, "..", 2))
            {
                // We don't want to remove the ".." directory if there is nothing left to remove. When we find a '..', we need to
                // subtract two parts: one for the part being removed, and another for the '..' which will have been included by
                if (this->parts.last)
                {
                    this->RemoveLast();
                    otherParts.Remove(otherParts.root);
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }

        while (otherParts.root != nullptr)
        {
            this->parts.Append(GTLib::Strings::Create(otherParts.root->start, otherParts.root->end - otherParts.root->start));
            this->partCount += 1;

            otherParts.Remove(otherParts.root);
        }

        return *this;
    }

    Path & Path::Prepend(const char* other)
    {
        Strings::List<char> otherParts;
        Path::SplitPath(other, otherParts);

        while (otherParts.last != nullptr)
        {
            this->parts.Prepend(GTLib::Strings::Create(otherParts.last->start, otherParts.last->end - otherParts.last->start));
            this->partCount += 1;

            otherParts.Remove(otherParts.last);
        }

        this->leadingSlash = (other != nullptr) && (*other == '/');

        return *this;
    }

    Path & Path::RemoveFirst()
    {
        if (this->parts.root != nullptr)
        {
            GTLib::Strings::Delete(const_cast<char*>(this->parts.root->start));

            this->parts.Remove(this->parts.root);
            --this->partCount;
        }
        else
        {
            if (this->leadingSlash)
            {
                this->leadingSlash = false;
            }
        }

        return *this;
    }

    Path & Path::RemoveLast()
    {
        if (this->parts.last != nullptr)
        {
            GTLib::Strings::Delete(const_cast<char*>(this->parts.last->start));

            this->parts.Remove(this->parts.last);
            --this->partCount;
        }
        else
        {
            // If we have a leading slash, that is now the next to go.
            if (this->leadingSlash)
            {
                this->leadingSlash = false;
            }
        }

        return *this;
    }

    void Path::Clean()
    {
        Strings::List<char> newParts;
        size_t newPartCount = Path::SplitPath(this->c_str(), newParts, true);

        while (this->parts.root != nullptr)
        {
            this->RemoveFirst();
        }


        while (newParts.root != nullptr)
        {
            this->parts.Append(GTLib::Strings::Create(newParts.root->start, newParts.root->end - newParts.root->start));
            newParts.Remove(newParts.root);
        }

        this->partCount = newPartCount;
    }

    bool Path::IsAbsolute() const
    {
        return this->leadingSlash || (this->parts.root && Path::IsAbsolute(this->parts.root->start));
    }

	const char* Path::c_str()
	{
		free(this->str);

        size_t size = this->Size();
		this->str = static_cast<char*>(malloc(size));
		this->Copy(this->str);

		return this->str;
	}

    Path & Path::operator= (const char* path)
    {
        // We need to delete all of the previous parts.
        while (this->parts.root)
        {
            this->RemoveFirst();
        }

        // We don't +1 for the null terminator here because the last character will be a slash, which we don't
        // use in the final string.
        Strings::List<char> newParts;
        this->partCount = Path::SplitPath(path, newParts);

        while (newParts.root != nullptr)
        {
            this->parts.Append(GTLib::Strings::Create(newParts.root->start, newParts.root->end - newParts.root->start));
            newParts.Remove(newParts.root);
        }

        // We need to check if we have a leading slash.
        if (path && *path == '/')
        {
            this->leadingSlash = true;
        }

        return *this;
    }


    size_t Path::SplitPath(const char *path, Strings::List<char> &list, bool resolveRelativeParts)
    {
        size_t result = 0;

        Strings::Tokenizer i(path, "/");
        while (i)
        {
            if (resolveRelativeParts && i.Equals("..") && list.last != nullptr && !list.last->IsEqual(".."))
            {
                list.Remove(list.last);

                if (result > 0)
                {
                    --result;
                }
            }
            else
            {
                list.Append(i.start, i.GetSizeInTs());
                ++result;
            }

            ++i;
        }

        return result;
    }

    bool Path::IsRelative(const char *path)
    {
        if (path != nullptr && *path != '\0')
        {
            char path0 = *path;
            if (path0 == '/')
            {
                return false;
            }

            ++path;
            if (*path != '\0')
            {
                char path1 = *path;
                if (((path0 >= 'a' && path0 <= 'z') || (path0 >= 'A' && path0 <= 'Z')) && path1 == ':')
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool Path::IsAbsolute(const char *path)
    {
        return !Path::IsRelative(path);
    }

    const char * Path::Extension(const char *path)
    {
        // We need to find the last occurance of a dot.
        const char *result = Strings::FindLast(path, '.');
        if (result != nullptr)
        {
            return result + 1;
        }
        else
        {
            return "";
        }
    }

    bool Path::ExtensionEqual(const char *path, const char *ext)
    {
        const char *temp = Strings::FindLast(path, '.');
        if (temp != nullptr)
        {
            return Strings::Equal(temp + 1, ext);
        }

        return false;
    }

    char * Path::FormatSlashes(char *path, char32_t newSlash)
    {
        Strings::Iterator<char> i(path);
        while (i)
        {
            if (i.character == '\\' || i.character == '/')
            {
                *((char *)i.str) = (char)newSlash;
            }

            ++i;
        }

        return path;
    }
}
