// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_GUI_ServerXMLParser_hpp_
#define __GTLib_GUI_ServerXMLParser_hpp_

#include "ServerEventHandler.hpp"
#include <GTEngine/Core/BasicBuffer.hpp>
#include <GTEngine/Core/Strings/Create.hpp>
#include <GTEngine/Core/Strings/List.hpp>

namespace GTGUI
{
    /**
    *   \brief  Class for parsing GTGUI Server XML scripts.
    *
    *   When constructing an XML parser, you must specify a ServerEventHandler. This will be used when
    *   an error needs to be posted.
    *
    *   To parse an XML script, call the Parse() method. This method takes a const char* containing the
    *   XML script to parse. It will return true if it was passed successfully; false otherwise.
    *
    *   After the script has been parsed, you can look at the content through publicly visible attributes.
    *   These attributes are reset during each call to Parse(), so make sure you have finished with them
    *   before calling Parse() again on the same ServerXMLParser object.
    *
    *   There are three main parts to a server XML script: <script>, <style> and elements. Scripts and 
    *   styles can be accessed via the \c scripts and \c styles string lists directly. The elements are
    *   arranged like a singly linked list with the first element being \c firstElement and the last
    *   element being \c lastElement. The next element in the list can be accessed with the \c next
    *   attribute on the element.
    *
    *   The strings in the \c scripts and \c styles lists use GTLib's start/end notation. The internal
    *   XML parser (rapidxml) will null terminate the string, though, so you shouldn't need to care about
    *   the end pointer.
    *
    *   When a script fails to parse, the GetLastErrorString() method can be used to retrieve the error
    *   message. The parser will stop when the first error message is retrieved.
    */
    class ServerXMLParser
    {
    public:
    
        struct Element
        {
            Element()
                : tag(nullptr), id(nullptr), parentid(nullptr), styleclass(nullptr), style(nullptr), text(nullptr),
                  next(nullptr), parent(nullptr), deleteID(false)
            {
            }
            
            // Simple copy constructor.
            Element(const Element &e)
                : tag(e.tag), id(e.id), parentid(e.parentid), styleclass(e.styleclass), style(e.style), text(e.text),
                  next(e.next), parent(e.parent), deleteID(e.deleteID)
            {
            }
            
            ~Element()
            {
                if (this->deleteID)
                {
                    GT::Strings::Delete((char *)this->id);
                }
            }
            
            /**
            *   \brief             Sets the ID of the element.
            *   \param  newID [in] The new ID of the XML element.
            *
            *   \remarks
            *       This method will make a copy of the ID.
            */
            void SetID(const char *newID)
            {
                if (newID)
                {
                    this->id       = GT::Strings::Create(newID);
                    this->deleteID = true;
                }
            }
            
            /// The tag name of the element.
            const char *tag;
            
            /// The ID of the element.
            const char *id;
            
            /// The ID of the parent element.
            const char *parentid;
            
            /// The style classes as defined in the 'styleclass' attribute.
            const char *styleclass;
            
            /// The style as defined in the 'style' attribute.
            const char *style;
            
            /// The text of the element. This is only used when the element has no children.
            const char *text;
            
            /// A pointer to the next element in the list.
            Element *next;
            
            /// A pointer to the parent element. This is \c NOT a pointer to the previous element in the list and
            /// it is \b NOT neccessarilly based on the XML hierarchy.
            Element* parent;
            
            /// Keeps track of whether or not to delete the ID.
            bool deleteID;
            
            
        private:    // No assignment.
            ServerXMLParser::Element & operator=(const ServerXMLParser::Element &);
        };
        
        
    public:
    
        /**
        *   \brief  Constructor.
        */
        ServerXMLParser();
        
        /**
        *   \brief  Destructor.
        */
        ~ServerXMLParser();
    
        /**
        *   \brief           Parses an XML script.
        *   \param  xml [in] The XML script to parse.
        *   \return          True if there were no errors; false otherwise.
        */
        bool Parse(const char* xml, size_t xmlSizeInBytes = static_cast<size_t>(-1));
        
        
        /// Retrieves the last error message.
        const char* GetLastErrorString() const;
        

    private:
    
        /**
        *   \brief                     Recursively parses an XML node.
        *   \param  node          [in] A pointer to the node being parsed. See remarks.
        *   \param  parentElement [in] A pointer to the parent element. Can be null; see remarks.
        *
        *   \remarks
        *       The \c node argument is a void* so that the internal XML parser implementation is hidden from the
        *       public interface. It can just be cast to the appropriate type internally.
        *       \par
        *       The \c parentElement argument is only valid for element tags (<div>, <button>, etc). Set to null if
        *       it's not needed.
        */
        void ParseXMLNode(void* node, ServerXMLParser::Element* parentElement);
        
        /**
        *   \brief               A helper function for appending a new element to the end of the list.
        *   \param  element [in] A pointer to the element being appended.
        */
        void AppendElement(ServerXMLParser::Element* element);
        
        /**
        *   \brief  Helper for clearning the internal data from the last parse.
        */
        void Clean();
        
    
    private:
    
        /// The buffer containing the input string. We need this because the XML parser (rapidxml) is destructive.
        GT::BasicBuffer buffer;
        
        /// The last error string.
        GT::String lastError;
        
    
    public:
        
        /// A pointer to the first element.
        Element* firstElement;
        
        /// A pointer to the last elemenet.
        Element* lastElement;
        
        /// The list of scripts.
        GT::Strings::List<char> scripts;
        GT::Strings::List<char> externalScripts;
        
        /// The list of style scripts.
        GT::Strings::List<char> styles;
        GT::Strings::List<char> externalStyles;

        /// The list of includes.
        GT::Strings::List<char> includes;
        
        
    private:    // No copying.
        ServerXMLParser(const ServerXMLParser &);
        ServerXMLParser & operator=(const ServerXMLParser &);
    };
}


#endif
