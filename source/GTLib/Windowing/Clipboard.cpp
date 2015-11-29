// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Windowing/Clipboard.hpp>
#include <GTLib/WindowManagement.hpp>
#include <GTLib/Strings.hpp>
#include <GTLib/Vector.hpp>

#if defined(_WIN32)
#include <GTLib/Windowing/Win32/Win32.hpp>
#include <GTLib/Strings/LineIterator.hpp>

namespace GTLib
{
    void Clipboard::SetText(const char* text, ptrdiff_t sizeInTs)
    {
        if (OpenClipboard(GTLib::Win32::GetFirstHWND()))
        {
            // Windows loves \n\r, so we need to make sure that is how the lines are formatted. If we don't do this, it will stuff
            // up pasting in things like Notepad.
            GTLib::String formattedText;

            GTLib::Strings::LineIterator line(text, sizeInTs);
            while (line)
            {
                if (line.start != text)
                {
                    formattedText.Append("\r\n", 2);
                }

                formattedText.Append(line.start, line.end - line.start);
                ++line;
            }



            HGLOBAL textHandle = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, (formattedText.GetLengthInTs() + 1) * sizeof(char));
            if (textHandle != nullptr)
            {
                if (EmptyClipboard())
                {
                    char* textCopy = reinterpret_cast<char*>(GlobalLock(textHandle));

                    memcpy(textCopy, formattedText.c_str(), formattedText.GetLengthInTs() * sizeof(char));
                    textCopy[formattedText.GetLengthInTs()] = '\0';

                    GlobalUnlock(textHandle);

                    if (SetClipboardData(CF_TEXT, textHandle) == NULL)
                    {
                        GlobalFree(textHandle);
                    }
                }
                else
                {
                    GlobalFree(textHandle);
                }
            }


            CloseClipboard();
        }
    }

    GTLib::String Clipboard::GetText()
    {
        GTLib::String result;

        if (IsClipboardFormatAvailable(CF_TEXT))
        {
            if (OpenClipboard(GTLib::Win32::GetFirstHWND()))
            {
                HGLOBAL textHandle = GetClipboardData(CF_TEXT);
                if (textHandle != 0)
                {
                    // Win32 loves \n\r line endings, but prefer \n. We need to convert.
                    auto text = reinterpret_cast<const char*>(GlobalLock(textHandle));

                    GTLib::Strings::LineIterator iLine(text);
                    while (iLine)
                    {
                        if (iLine.start != text)
                        {
                            result.Append("\n");
                        }

                        result.Append(iLine.start, iLine.end - iLine.start);

                        ++iLine;
                    }


                    GlobalUnlock(textHandle);
                }


                CloseClipboard();
            }
        }


        return result;
    }
}
#elif defined(__linux__)
#include <GTLib/Windowing/X11/X11.hpp>
#include <unistd.h>

namespace GTLib
{
    bool WaitForEvent(int type, XEvent &e)
    {
        const int retryCount = 500;
        const int retryDelay = 1000;

        for (int iRetry = 0; iRetry < retryCount; ++iRetry)
        {
            if (XCheckTypedWindowEvent(GetX11Display(), X11::GetFirstWindow(), type, &e))
            {
                return true;
            }

            usleep(retryDelay);
        }

        return false;
    }

    bool WaitForClipboardSelectionNotify(XEvent &e)
    {
        return WaitForEvent(SelectionNotify, e);
    }

    bool WaitForClipboardSelectionRequest(XEvent &e)
    {
        return WaitForEvent(SelectionRequest, e);
    }

    void Clipboard::SetText(const char* text, ptrdiff_t sizeInTs)
    {
        X11::SetClipboardText(text, sizeInTs, X11::GetAtomCLIPBOARD());

        XSetSelectionOwner(GetX11Display(), X11::GetAtomCLIPBOARD(), X11::GetFirstWindow(), CurrentTime);
        XFlush(GetX11Display());
    }

    GTLib::String Clipboard::GetText()
    {
        if (XGetSelectionOwner(GetX11Display(), X11::GetAtomCLIPBOARD()) == X11::GetFirstWindow())
        {
            return X11::GetClipboardText(X11::GetAtomCLIPBOARD());
        }
        else
        {
            XConvertSelection(GetX11Display(), X11::GetAtomCLIPBOARD(), X11::GetAtomTARGETS(), X11::GetAtomCLIPBOARD(), X11::GetFirstWindow(), CurrentTime);

            // We need to wait for the SelectionNotify event.
            XEvent e;
            if (WaitForClipboardSelectionNotify(e))
            {
                Atom selectedTarget = None;

                if (e.xselection.property != None && e.xselection.target == X11::GetAtomTARGETS())
                {
                    X11::Property property;
                    X11::GetWindowProperty(e.xany.window, e.xselection.property, property);

                    auto atoms = reinterpret_cast<uint32_t*>(property.data.buffer);
                    if (atoms != nullptr)
                    {
                        // We'll just pick the first string format we recognize.
                        for (unsigned long iAtom = 0; iAtom < property.itemCount; ++iAtom)
                        {
                            if (atoms[iAtom] != None)
                            {
                                auto name = const_cast<const char*>(XGetAtomName(GetX11Display(), static_cast<Atom>(atoms[iAtom])));
                                if (name)
                                {
                                    if (GTLib::Strings::Equal<false>(name, "MULTIPLE"))
                                    {
                                    #if 0
                                        X11::GetWindowProperty(e.xany.window, e.xselection.property, property);

                                        for (size_t i = 0; i < property.itemCount; i += 2)
                                        {
                                            Atom propertyAtom = static_cast<Atom>(reinterpret_cast<uint32_t*>(property.data.buffer)[i]);
                                            Atom targetAtom   = static_cast<Atom>(reinterpret_cast<uint32_t*>(property.data.buffer)[i + 1]);

                                            if (propertyAtom != None)
                                            {
                                                printf("PROPERTY: %s\n", XGetAtomName(GetX11Display(), propertyAtom));
                                            }
                                            if (targetAtom != None)
                                            {
                                                printf("TARGET: %s\n", XGetAtomName(GetX11Display(), targetAtom));
                                            }
                                        }
                                    #endif
                                    }
                                    else
                                    {
                                        if (GTLib::Strings::Equal<false>(name, "STRING")        ||
                                            GTLib::Strings::Equal<false>(name, "UTF8_STRING")   ||
                                            GTLib::Strings::Equal<false>(name, "UTF16_STRING")  ||
                                            GTLib::Strings::Equal<false>(name, "UTF32_STRING")  ||
                                            GTLib::Strings::Equal<false>(name, "TEXT")          ||
                                            GTLib::Strings::Equal<false>(name, "COMPOUND_TEXT") ||
                                            GTLib::Strings::Equal<false>(name, "application/x-gtk-text-buffer-rich-text") ||
                                            GTLib::Strings::FindFirst(name, "text/") != nullptr)
                                        {
                                            selectedTarget = atoms[iAtom];
                                            XConvertSelection(e.xselection.display, e.xselection.property, selectedTarget, e.xselection.property, e.xany.window, CurrentTime);

                                            if (WaitForClipboardSelectionNotify(e))
                                            {
                                                X11::GetWindowProperty(e.xany.window, e.xselection.property, property);

                                                // We need to determine the format and then convert over to UTF-8.
                                                GTLib::String valueUTF8;

                                                if (GTLib::Strings::Equal<false>(name, "UTF16_STRING") ||
                                                    (property.data.count >= 2 && property.data.buffer[0] == 0xFF && property.data.buffer[1] == 0xFE))
                                                {
                                                    // It's UTF16.

                                                    // Remove null-terminators.
                                                    size_t charCount = property.data.count / 2;
                                                    for (size_t i = 0; i < charCount; )
                                                    {
                                                        char16_t c = reinterpret_cast<char16_t*>(property.data.buffer)[i];
                                                        if (c == 0)
                                                        {
                                                            property.data.Remove(i * 2 + 0);
                                                            property.data.Remove(i * 2 + 1);
                                                        }
                                                        else
                                                        {
                                                            ++i;
                                                        }
                                                    }

                                                    valueUTF8.Assign(reinterpret_cast<char16_t*>(property.data.buffer + 2), property.data.count/2 - 2);
                                                }
                                                else if (GTLib::Strings::Equal<false>(name, "UTF32_STRING"))
                                                {
                                                    // It's UTF-32.

                                                    // Remove null-terminators.
                                                    size_t charCount = property.data.count / 4;
                                                    for (size_t i = 0; i < charCount; )
                                                    {
                                                        char32_t c = reinterpret_cast<char32_t*>(property.data.buffer)[i];
                                                        if (c == 0)
                                                        {
                                                            property.data.Remove(i * 4 + 0);
                                                            property.data.Remove(i * 4 + 1);
                                                            property.data.Remove(i * 4 + 2);
                                                            property.data.Remove(i * 4 + 3);
                                                        }
                                                        else
                                                        {
                                                            ++i;
                                                        }
                                                    }

                                                    valueUTF8.Assign(reinterpret_cast<char32_t*>(property.data.buffer), property.data.count/4);
                                                }
                                                else
                                                {
                                                    // Assume UTF-8.

                                                    // Remove null-terminators.
                                                    size_t charCount = property.data.count;
                                                    for (size_t i = 0; i < charCount; )
                                                    {
                                                        char c = reinterpret_cast<char*>(property.data.buffer)[i];
                                                        if (c == 0)
                                                        {
                                                            property.data.Remove(i);
                                                        }
                                                        else
                                                        {
                                                            ++i;
                                                        }
                                                    }

                                                    valueUTF8.Assign(reinterpret_cast<char*>(property.data.buffer), property.data.count);
                                                }

                                                // We have the format in UTF-8 format and there are no null terminators.
                                                return valueUTF8;
                                            }
                                        }

                                        //printf("ATOM NAME: %s\n", name);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    // We didn't receive TARGETS.
                    return "";
                }
            }
            else
            {
                // Clipboard timed-out.
                return "";
            }
        }

        return "";
    }
}
#endif
