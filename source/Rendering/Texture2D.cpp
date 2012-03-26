
#include <GTEngine/Rendering/Texture2D.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    // Make sure the members are initialised in the correct order here.
    Texture2D::Texture2D()
        : GTImage::Image(), filter(TextureFilter_Linear), wrapMode(TextureWrapMode_Repeat), framebuffers(), rendererData(nullptr), refCount(1), syncinfo()
    {
        if (Renderer::HasFlippedTextures())     // <-- Will return true with OpenGL, but not necessarily everything else - thus why we need it.
        {
            this->FlipVertically();
        }
    }

    Texture2D::Texture2D(unsigned int width, unsigned int height, GTImage::ImageFormat format, const void *data)
        : GTImage::Image(), filter(TextureFilter_Linear), wrapMode(TextureWrapMode_Repeat), framebuffers(), rendererData(nullptr), refCount(1), syncinfo()
    {
        if (Renderer::HasFlippedTextures())
        {
            this->FlipVertically();
        }

        this->Load(width, height, format, data);
    }

    Texture2D::Texture2D(const char *filename)
        : GTImage::Image(filename), filter(TextureFilter_Linear), wrapMode(TextureWrapMode_Repeat), framebuffers(), rendererData(nullptr), refCount(1), syncinfo()
    {
        if (Renderer::HasFlippedTextures())
        {
            this->FlipVertically();
        }

        this->LoadMipmapFromFile(0);
    }

    Texture2D::~Texture2D()
    {
        // The texture needs to be detached from all framebuffers.
        while (this->framebuffers.root != nullptr)
        {
            this->framebuffers.root->value->DetachBuffer(this);
        }

        Renderer::MarkForCollection(this);
    }

    void Texture2D::SetFilter(TextureFilter filter)
    {
        this->filter = filter;

        // The texture is out of sync...
        this->syncinfo.filterChanged = true;
    }

    void Texture2D::SetWrapMode(TextureWrapMode wrapMode)
    {
        this->wrapMode = wrapMode;

        // We're now out of sync...
        this->syncinfo.wrapModeChanged = true;
    }

    void Texture2D::OnMipmapCreated(unsigned int)
    {
        this->syncinfo.dataChanged = true;
    }

    void Texture2D::OnMipmapDeleted(unsigned int)
    {
        this->syncinfo.dataChanged = true;
    }

    void Texture2D::OnMipmapChanged(unsigned int)
    {
        this->syncinfo.dataChanged = true;
    }


    void Texture2D::OnAttachToFramebuffer(Framebuffer *framebuffer)
    {
        if (this->framebuffers.Find(framebuffer) == nullptr)
        {
            this->framebuffers.Append(framebuffer);
        }
    }

    void Texture2D::OnDetachFromFramebuffer(Framebuffer *framebuffer)
    {
        this->framebuffers.Remove(this->framebuffers.Find(framebuffer));
    }
}