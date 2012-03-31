
#ifndef __GTEngine_Editor_hpp_
#define __GTEngine_Editor_hpp_

namespace GTGUI
{
    class Element;
    class Server;
}

namespace GTEngine
{
    /// Class representing the editor.
    ///
    /// An editor object with every Game object.
    class Editor
    {
    public:

        /// Constructor.
        Editor();

        /// Destructor.
        ~Editor();


        /// Initialises the editor.
        bool Startup(GTGUI::Server &guiServer);

        /// Whether or not the editor is initialised.
        bool IsStarted() const { return this->isStarted; }


        /// Opens the editor.
        void Open();

        /// Closes the editor.
        void Close();

        /// Determines whether or not the editor is open.
        bool IsOpen() const { return this->isOpen; }


    private:

        /// Structure containing the GUI elements.
        struct _GUI
        {
            _GUI()
                : main(nullptr)
            {
            }

            /// The main editor element. Every single element used by the editor is a child of this element.
            GTGUI::Element* main;

        }GUI;

        /// Whether or not the editor has be started up.
        bool isStarted;

        /// Whether or not the editor is open. Defaults to false.
        bool isOpen;

    };
}

#endif