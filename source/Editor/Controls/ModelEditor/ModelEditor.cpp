// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/ModelEditor/ModelEditor.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/Scene/SceneNode.hpp>
#include <GTGameEngine/Scene/GraphicsComponent.hpp>

namespace GT
{
    ModelEditor::ModelEditor(Editor &editor, SubEditorAllocator &allocator, const char* absolutePath, ModelAsset* pAsset)
        : EditorSubEditor(editor, allocator, absolutePath),
          m_pAsset(pAsset), m_viewport(editor), m_hPanel(0),
          m_pSceneNode(nullptr)
    {
        GUIContext &gui = this->GetGUI();
        //const EditorTheme &theme = this->GetEditor().GetTheme();

        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != 0)
        {
            gui.SetElementSizeRatio(hRootElement, 1, 1);
            gui.SetElementChildAxis(hRootElement, ChildAxis_Horizontal);
            gui.EnableElementChildWidthFlexing(hRootElement);


            // Viewport
            gui.SetElementParent(m_viewport.GetRootGUIElement(), hRootElement);
            gui.SetElementSizeRatio(m_viewport.GetRootGUIElement(), 1, 1);
            gui.SetElementBorder(m_viewport.GetRootGUIElement(), 1, GTLib::Colour(0.2f, 0.2f, 0.2f));
                

            // Panel
            m_hPanel = gui.CreateElement();
            gui.SetElementParent(m_hPanel, hRootElement);
            gui.SetElementWidth(m_hPanel, 256U);
            gui.SetElementHeightRatio(m_hPanel, 1);



            // We need to create a scene node with a graphics component attached to it.
            m_pSceneNode = m_viewport.GetScene().CreateSceneNode();
            auto pGraphicsComponent = m_pSceneNode->AttachComponent<GraphicsComponent>();
            if (pGraphicsComponent != nullptr)
            {
                pGraphicsComponent->SetModel(absolutePath, &m_viewport.GetGraphicsAssetResourceManager());
            }

            m_viewport.GetScene().InsertSceneNode(m_pSceneNode);


            m_viewport.SetCameraPosition(vec3(0, 0, 5));
        }
    }

    ModelEditor::~ModelEditor()
    {
        m_viewport.GetScene().DeleteSceneNode(m_pSceneNode);
    }

    bool ModelEditor::Save()
    {
        return false;
    }

    void ModelEditor::OnActivate()
    {
    }

    void ModelEditor::OnDeactivate()
    {
    }

    void ModelEditor::Cut()
    {
    }

    void ModelEditor::Copy()
    {
    }

    void ModelEditor::Paste()
    {
    }

    void ModelEditor::Undo()
    {
    }

    void ModelEditor::Redo()
    {
    }

    void ModelEditor::SelectAll()
    {
    }
}
