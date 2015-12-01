// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/GUIServerXMLParser.hpp>
#include <GTEngine/Core/Strings/Size.hpp>
#include <GTEngine/Core/Strings/Equal.hpp>
#include <cstring>

// A hate the using command, but it's such a pain doing Strings all the time...
using namespace GT;


#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wswitch-default"
    #pragma GCC diagnostic ignored "-Winline"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include <GTEngine/Core/rapidxml.hpp>
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

using namespace rapidxml;

namespace GTGUI
{
    GUIServerXMLParser::GUIServerXMLParser()
        : buffer(), lastError(),
          firstElement(nullptr), lastElement(nullptr),
          scripts(), externalScripts(),
          styles(), externalStyles(),
          includes()
    {
    }
    
    GUIServerXMLParser::~GUIServerXMLParser()
    {
        this->Clean();
    }
    
    bool GUIServerXMLParser::Parse(const char *xml, size_t xmlSizeInBytes)
    {
        // Everything needs to be cleaned.
        this->Clean();
        
        // If the size is -1, it means the string is null terminated.
        if (xmlSizeInBytes == static_cast<size_t>(-1))
        {
            xmlSizeInBytes = static_cast<size_t>(Strings::SizeInTs(xml));
        }
        
        
        // rapidxml is destructive, so we need to put the XML string into a buffer. Also, we don't want
        // the buffer getting too large so we'll tighten it if it looks like it's getting over-sized.
        size_t xmlBufferSize = xmlSizeInBytes + 1;
        bool   tightenBuffer = false;
        
        if (xmlBufferSize > this->buffer.GetDataSizeInBytes())
        {
            if (xmlBufferSize - this->buffer.GetDataSizeInBytes() > 4096)
            {
                tightenBuffer = true;
            }
        }
        
        this->buffer.Allocate(xmlBufferSize, tightenBuffer);
        std::memcpy(this->buffer.GetDataPointer(), xml, xmlSizeInBytes);
        reinterpret_cast<char*>(this->buffer.GetDataPointer())[xmlSizeInBytes] = '\0';
        
        xml_document<> document;
        try
        {
            document.parse<0>((char *)buffer.GetDataPointer());
        }
        catch (rapidxml::parse_error &e)
        {
            this->lastError = e.what();
            return false;
        }
        
        
        // If we get to this point, the document should have been successfully parsed.
        this->ParseXMLNode(&document, nullptr);
        
        return true;
    }
    
    const char* GUIServerXMLParser::GetLastErrorString() const
    {
        return this->lastError.c_str();
    }
    
    
    
    void GUIServerXMLParser::ParseXMLNode(void *_node, GUIServerXMLParser::Element *parent)
    {
        if (_node)
        {
            xml_node<> *node = (xml_node<> *)_node;
            
            // I'm sure there's a good reason for it, but rapidxml has a strange feature where if CDATA is specified, it will
            // actually be in a seperate child tag. Seems unintuitive to me, but whatever.
            if (Strings::Equal(node->name(), "script"))
            {
                xml_attribute<> *url = node->first_attribute("url");
                if (url != nullptr)
                {
                    this->externalScripts.Append(url->value(), static_cast<ptrdiff_t>(url->value_size()));
                }

                xml_node<> *cdata = node->first_node();
                if (cdata && cdata->type() == node_cdata)
                {
                    this->scripts.Append(cdata->value(), static_cast<ptrdiff_t>(cdata->value_size()));
                }
                else
                {
                    if (node->value() != nullptr)
                    {
                        this->scripts.Append(node->value(), static_cast<ptrdiff_t>(node->value_size()));
                    }
                }
            }
            else if (Strings::Equal(node->name(), "style"))
            {
                xml_attribute<> *url = node->first_attribute("url");
                if (url != nullptr)
                {
                    this->externalStyles.Append(url->value(), static_cast<ptrdiff_t>(url->value_size()));
                }

                xml_node<> *cdata = node->first_node();
                if (cdata && cdata->type() == node_cdata)
                {
                    this->styles.Append(cdata->value(), static_cast<ptrdiff_t>(cdata->value_size()));
                }
                else
                {
                    this->styles.Append(node->value(), static_cast<ptrdiff_t>(node->value_size()));
                }
            }
            else if (Strings::Equal(node->name(), "include"))
            {
                // All we care about here is the "url" attribute.
                xml_attribute<> *url = node->first_attribute("url");
                if (url != nullptr)
                {
                    this->includes.Append(url->value(), static_cast<ptrdiff_t>(url->value_size()));
                }
            }
            else
            {
                // We are probably looking at a normal element like <div> or similar. It may be the document XML tag, though, so
                // we need to check for that, too.
                if (node->type() == node_element)
                {
                    // The new element we'll be adding. We don't add it to the list yet just in case we get an error.
                    GUIServerXMLParser::Element element;
                    
                    element.tag    = node->name();
                    element.parent = parent;
                    
                    // Now some attributes.
                    xml_attribute<> *attr = node->first_attribute();
                    while (attr != nullptr)
                    {
                        if (Strings::Equal(attr->name(), "id"))
                        {
                            element.id = attr->value();
                        }
                        else if (Strings::Equal(attr->name(), "styleclass"))
                        {
                            element.styleclass = attr->value();
                        }
                        else if (Strings::Equal(attr->name(), "style"))
                        {
                            element.style = attr->value();
                        }
                        else if (Strings::Equal(attr->name(), "parentid"))
                        {
                            element.parentid = attr->value();
                        }
                        
                        attr = attr->next_attribute();
                    }
                    
                    // Now we need to add the new element to the end of the list.
                    GUIServerXMLParser::Element *newElement = new GUIServerXMLParser::Element(element);
                    this->AppendElement(newElement);
                    
                    // Now we need to check the children.
                    xml_node<> *child = node->first_node();
                    if (child)
                    {
                        while (child)
                        {
                            // If the child is a data node, that will be the value for 'node'. It's because of the way rapidxml handles CDATA.
                            if (child->type() == node_data)
                            {
                                newElement->text = node->value();
                            }
                            else
                            {
                                this->ParseXMLNode(child, newElement);
                            }
                            
                            child = child->next_sibling();
                        }
                    }
                    else
                    {
                        newElement->text = node->value();
                    }
                }
                else
                {
                    // It was the document node. We just iterate over it's children.
                    xml_node<> *child = node->first_node();
                    while (child)
                    {
                        this->ParseXMLNode(child, nullptr);
                        child = child->next_sibling();
                    }
                }
            }
        }
    }
    
    void GUIServerXMLParser::AppendElement(GUIServerXMLParser::Element *element)
    {
        if (element)
        {
            if (!this->firstElement)
            {
                this->firstElement = element;
            }
            
            if (this->lastElement)
            {
                this->lastElement->next = element;
            }
            
            this->lastElement = element;
        }
    }
    
    void GUIServerXMLParser::Clean()
    {
        this->lastError = "";
        
        // First the elements.
        while (this->firstElement)
        {
            auto next = this->firstElement->next;
            delete this->firstElement;
            this->firstElement = next;
        }
        
        this->firstElement = nullptr;
        this->lastElement  = nullptr;
        
        // Now the string lists.
        this->scripts.Clear();
        this->externalScripts.Clear();
        
        this->styles.Clear();
        this->externalStyles.Clear();
        
        this->includes.Clear();
    }
}
