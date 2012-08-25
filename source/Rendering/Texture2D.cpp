
#include <GTEngine/Rendering/Texture2D.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    // Make sure the members are initialised in the correct order here.
    Texture2D::Texture2D()
        : GTImage::Image(),
          minFilter(TextureFilter_LinearLinear), magFilter(TextureFilter_Linear),
          anisotropy(1), wrapMode(TextureWrapMode_Repeat),
          framebuffers(), shaders(),
          rendererData(nullptr), refCount(1), keepClientSideData(false),
          syncinfo()
    {
        if (Renderer::HasFlippedTextures())     // <-- Will return true with OpenGL, but not necessarily everything else - thus why we need it.
        {
            this->FlipVertically();
        }
    }

    Texture2D::Texture2D(unsigned int width, unsigned int height, GTImage::ImageFormat format, const void *data)
        : GTImage::Image(),
          minFilter(TextureFilter_LinearLinear), magFilter(TextureFilter_Linear),
          anisotropy(1), wrapMode(TextureWrapMode_Repeat),
          framebuffers(), shaders(),
          rendererData(nullptr), refCount(1), keepClientSideData(false),
          syncinfo()
    {
        if (Renderer::HasFlippedTextures())
        {
            this->FlipVertically();
        }

        this->Load(width, height, format, data);
    }

    Texture2D::Texture2D(const char* filename)
        : GTImage::Image(filename),
          minFilter(TextureFilter_LinearLinear), magFilter(TextureFilter_Linear),
          anisotropy(1), wrapMode(TextureWrapMode_Repeat),
          framebuffers(), shaders(),
          rendererData(nullptr), refCount(1), keepClientSideData(false),
          syncinfo()
    {
        if (Renderer::HasFlippedTextures())
        {
            this->FlipVertically();
        }

        // This pulls every mipmap from the image file.
        this->PullAllMipmaps();
        this->GenerateMipmaps();

        //this->LoadMipmapFromFile(0);
    }

    Texture2D::~Texture2D()
    {
        // The texture needs to be detached from all framebuffers.
        while (this->framebuffers.root != nullptr)
        {
            this->framebuffers.root->value->DetachBuffer(this);
        }

        // We also need to detach from all shaders.
        while (this->shaders.root != nullptr)
        {
            this->shaders.root->value->OnTextureDeleted(this);
            this->shaders.RemoveRoot();
        }

        Renderer::MarkForCollection(this);
    }


    void Texture2D::SetMinificationFilter(TextureFilter newFilter)
    {
        if (this->minFilter != newFilter)
        {
            this->minFilter = newFilter;
            this->syncinfo.minFilterChanged = true;
        }
    }

    void Texture2D::SetMagnificationFilter(TextureFilter newFilter)
    {
        if (this->magFilter != newFilter)
        {
            this->magFilter = newFilter;
            this->syncinfo.magFilterChanged = true;
        }
    }

    TextureFilter Texture2D::GetMinificationFilter() const
    {
        return this->minFilter;
    }

    TextureFilter Texture2D::GetMagnificationFilter() const
    {
        return this->magFilter;
    }


    void Texture2D::SetAnisotropy(unsigned int newAnisotropy)
    {
        if (this->anisotropy != newAnisotropy)
        {
            this->anisotropy = newAnisotropy;
            this->syncinfo.anisotropyChanged = true;
        }
    }

    unsigned int Texture2D::GetAnisotropy() const
    {
        return this->anisotropy;
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

    void Texture2D::OnMipmapChanged(unsigned int mipmapIndex)
    {
        this->syncinfo.dataChanged = true;

        if (!this->syncinfo.changedMipmaps.Exists(mipmapIndex))
        {
            this->syncinfo.changedMipmaps.PushBack(mipmapIndex);
        }
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


    void Texture2D::OnAttachToShader(Shader* shader)
    {
        if (this->shaders.Find(shader) == nullptr)
        {
            this->shaders.Append(shader);
        }
    }

    void Texture2D::OnDetachFromShader(Shader* shader)
    {
        this->shaders.Remove(this->shaders.Find(shader));
    }
}
