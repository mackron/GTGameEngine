// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/GUI/GUIMarkupLoader.hpp>
#include <GTGE/GUI/GUIServer.hpp>
#include <GTGE/GTEngine.hpp>
#include <dr_libs/dr_path.h>

namespace GT
{
    GUIMarkupLoader::GUIMarkupLoader(GUIServer &server)
        : m_server(server), m_loadedFiles()
    {
    }
    
    GUIMarkupLoader::~GUIMarkupLoader()
    {
    }
    
    
    
    bool GUIMarkupLoader::Load(const char* markup, const char* absoluteDirectory, GT::String &topElementID)
    {
        GT::Vector<GUIElement*> loadedElements(1);
        if (this->Load(markup, static_cast<size_t>(-1), absoluteDirectory, loadedElements))
        {
            if (loadedElements.count > 0)
            {
                assert(loadedElements[0] != nullptr);
                {
                    topElementID = loadedElements[0]->GetID();
                }
            }
            
            this->PostLoad(loadedElements);
            return true;
        }
        
        return false;
    }
    
    
    bool GUIMarkupLoader::LoadFile(const char* filePath)
    {
        GT::Vector<GUIElement*> loadedElements(32);
        if (this->LoadFile(filePath, loadedElements))
        {
            this->PostLoad(loadedElements);
            
            return true;
        }
        
        return false;
    }
    
    void GUIMarkupLoader::UnloadFile(const char* filePath)
    {
        char absolutePath[DRVFS_MAX_PATH];
        if (drvfs_find_absolute_path(GT::g_Context->GetVFS(), filePath, absolutePath, sizeof(absolutePath)))
        {
            m_loadedFiles.Remove(absolutePath);
        }
    }
    
    
    bool GUIMarkupLoader::IsFileLoaded(const char* absolutePath) const
    {
        return m_loadedFiles.Exists(absolutePath);
    }
    
    
    
    ////////////////////////////////////////////////
    // Private
    
    bool GUIMarkupLoader::Load(const char* markup, size_t markupSizeInBytes, const char* absoluteDirectory, GT::Vector<GUIElement*> &loadedElementsOut)
    {
        // We just create a server XML parser and look at it's results.
        GUIServerXMLParser parser;
        if (parser.Parse(markup, markupSizeInBytes))
        {
            // We'll load <include /> tags first. We need to load relative to the given directory as defined by 'directory'.
            // If this is null, we load relative to the current directory. If this was called from LoadFromFile(), 'directory'
            // will be that files absolute path.
            for (auto i = parser.includes.root; i != nullptr; i = i->next)
            {
                // Step 1: Translate the 'url' attribute to an absolute path based on 'directory'.
                char absURL[DRVFS_MAX_PATH];
                if (absoluteDirectory != nullptr)
                {
                    drpath_copy_and_append(absURL, sizeof(absURL), absoluteDirectory, i->start);
                }
                else
                {
                    drvfs_find_absolute_path(GT::g_Context->GetVFS(), i->start, absURL, sizeof(absURL));
                }

                // Step 2: Load the include file.
                this->LoadFile(absURL, loadedElementsOut);
            }



            // We want to load styles first. The reason for this is that when elements are created, they'll want to reference
            // style objects. Here is the order of style loading:
            //
            // 1) <style url='' />
            // 2) <style>...</style>
            // 3) <div ... style='' ...>
            //
            // Because all styles need to be loaded before any scripts, we need to do two passes of every element. The first
            // pass will retrieve the styling information. The second pass will do the actual creation of the elements.

            // 1) <style url='' />
            for (auto i = parser.externalStyles.root; i != nullptr; i = i->next)
            {
                // Step 1: Get the absolute path of this URL based on 'directory'.
                char absURL[DRVFS_MAX_PATH];
                if (absoluteDirectory != nullptr)
                {
                    drpath_copy_and_append(absURL, sizeof(absURL), absoluteDirectory, i->start);
                }
                else
                {
                    drvfs_find_absolute_path(GT::g_Context->GetVFS(), i->start, absURL, sizeof(absURL));
                }

                // Step 2: Load the file.
                m_server.GetStyleServer().LoadFromFile(absURL);
            }

            // 2) <style>...</style>
            for (auto i = parser.styles.root; i != nullptr; i = i->next)
            {
                m_server.GetStyleServer().Load(i->start, absoluteDirectory);
            }

            // 3) <div ... style='' ...>
            //
            // This pass will generate any automatic IDs.
            GUIServerXMLParser::Element *element = parser.firstElement;
            while (element)
            {
                // We're going to need an ID.
                if (element->id == nullptr)
                {
                    // We don't have an ID, so we need to create it.
                    char result[64];
                    m_server.NextAnonymousID(result, 64);

                    element->SetID(result);
                }

                // If we don't have a parentid, we need one.
                if (!element->parentid)
                {
                    if (element->parent)
                    {
                        element->parentid = element->parent->id;
                    }
                    else
                    {
                        // If the element is the root, it won't need a parent.
                        if (!GT::Strings::Equal(element->id, "_Root"))
                        {
                            element->parentid = "_Root";
                        }
                    }
                }



                // Now the actual style.
                GT::Strings::List<char> stylingScript;
                stylingScript.Append("#");
                stylingScript.Append(element->id);
                stylingScript.Append("\n{\n");
                stylingScript.Append(element->style);
                stylingScript.Append("\n}\n");

                // Now we need to ensure there is at least an empty style class instantiation for each style class in the
                // styleclass attribute.
                GT::Strings::WhitespaceTokenizerUTF8 token(element->styleclass);
                while (token)
                {
                    stylingScript.Append(token.start, token.GetSizeInTs());
                    stylingScript.Append("{}\n");

                    ++token;
                }

                m_server.GetStyleServer().Load(stylingScript.c_str(), absoluteDirectory);


                // Here is where we output any errors. Note how we're not using a conditional here because it's possible that non-critical errors will
                // be posted, which will still cause Load() to return true.
                GUIStyleScriptError styleError;
                while (m_server.GetStyleServer().GetLastError(styleError))
                {
                    m_server.PostError(styleError.GetFormatted(m_server.GetErrorMessageLevel()).c_str());
                }


                // Move to the next element...
                element = element->next;
            }



            // Create elements from the XML markup.
            element = parser.firstElement;
            while (element != nullptr)
            {
                auto newElement = m_server.CreateElement(element->id, m_server.GetElementByID(element->parentid));
                assert(newElement != nullptr);
                {
                    // Styles.
                    newElement->AttachStyleClass(m_server.GetStyleServer().GetDefaultStyleClass(), false);

                    GT::Strings::WhitespaceTokenizerUTF8 token(element->styleclass);
                    while (token)
                    {
                        newElement->AttachStyleClass(GT::String(token.start, token.GetSizeInTs()).c_str(), false);

                        ++token;
                    }

                    newElement->AttachStyleClass((GT::String("#") + element->id).c_str());


                    // Text.
                    //
                    // We're going to use HTML's whitespace system, where all whitespace is ignore except for a single space between words. To achieve
                    // this we simply tokenize against whitespace.
                    GT::Strings::List<char> elementText;

                    bool doneFirstToken = false;
                    token = GT::Strings::WhitespaceTokenizerUTF8(element->text);
                    while (token)
                    {
                        if (doneFirstToken)
                        {
                            elementText.Append(" ");
                        }

                        elementText.Append(token.start, token.GetSizeInTs());
                        ++token;

                        doneFirstToken = true;
                    }


                    // Only set text if we actually have something to set.
                    if (elementText.root != nullptr)
                    {
                        newElement->SetText(elementText.c_str());
                    }
                    else
                    {
                        // We may want to update the font for certain elements based on their style.
                        if (newElement->style.editableText->value || newElement->style.width->Automatic() || newElement->style.height->Automatic())
                        {
                            newElement->UpdateFontFromStyle(true);
                        }
                    }
                    


                    // We need to keep track of the new element.
                    loadedElementsOut.PushBack(newElement);
                }


                element = element->next;
            }


            // 2) <script url='' />
            for (auto i = parser.externalScripts.root; i != nullptr; i = i->next)
            {
                // Step 1: Get the absolute path of this URL based on 'directory'.
                char absURL[DRVFS_MAX_PATH];
                if (absoluteDirectory != nullptr)
                {
                    drpath_copy_and_append(absURL, sizeof(absURL), absoluteDirectory, i->start);
                }
                else
                {
                    drvfs_find_absolute_path(GT::g_Context->GetVFS(), i->start, absURL, sizeof(absURL));
                }

                // Step 2: Load the file.
                m_server.GetScriptServer().ExecuteFromFile(absURL, 0);
            }


            // 3) <script>...</script>
            for (auto i = parser.scripts.root; i != nullptr; i = i->next)
            {
                m_server.GetScriptServer().Execute(i->start, 0);
            }


            return true;
        }

        return false;
    }
    
    bool GUIMarkupLoader::LoadFile(const char* filePath, GT::Vector<GUIElement*> &loadedElementsOut)
    {
        char absolutePath[DRVFS_MAX_PATH];
        if (drvfs_find_absolute_path(GT::g_Context->GetVFS(), filePath, absolutePath, sizeof(absolutePath)))
        {
            if (this->IsFileLoaded(absolutePath))
            {
                return true;
            }
            else
            {
                size_t fileSize;
                char* pFileData = drvfs_open_and_read_text_file(GT::g_Context->GetVFS(), absolutePath, &fileSize);
                if (pFileData != nullptr)
                {
                    char absoluteDir[DRVFS_MAX_PATH];
                    drpath_copy_base_path(absolutePath, absoluteDir, sizeof(absoluteDir));

                    bool result = this->Load(pFileData, fileSize, absoluteDir, loadedElementsOut);
                    if (result)
                    {
                        // We need to keep track of the file so it's not unnecessarily reloaded.
                        m_loadedFiles.Add(absolutePath, true);
                    }

                    drvfs_free(pFileData);
                    return result;
                }
            }
        }
        else
        {
            char msg[256];
            GT::IO::snprintf(msg, 256, "Could not find file: %s", filePath);
            
            m_server.PostError(msg);
        }
        
        return false;
    }
    
    
    void GUIMarkupLoader::PostLoad(const GT::Vector<GUIElement*> &loadedElements)
    {
        //m_server.UpdateLayout();
        
        for (size_t iElement = 0; iElement < loadedElements.GetCount(); ++iElement)
        {
            auto element = loadedElements[iElement];
            assert(element != nullptr);
            {
                if (element->IsVisible())
                {
                    m_server.OnShow(element);
                }
                else
                {
                    m_server.OnHide(element);
                }
            }
        }
    }
}
