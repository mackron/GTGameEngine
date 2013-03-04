
<shader id="Engine_GUI_VS">
    #version 330

    in vec2 VertexInput_Position;
    in vec2 VertexInput_TexCoord;
    in vec4 VertexInput_Colour;
    
    out vec2 VertexOutput_TexCoord;
    out vec4 VertexOutput_Colour;
    
    uniform mat4 Projection;
    uniform vec2 Offset;
    
    void main()
    {
        gl_Position = Projection * vec4(VertexInput_Position + Offset, 0.0, 1.0);
        VertexOutput_TexCoord = VertexInput_TexCoord;
        VertexOutput_Colour   = VertexInput_Colour;
    }
</shader>

<shader id="Engine_GUI_FS">
    #version 330

    in vec2 VertexOutput_TexCoord;
    in vec4 VertexOutput_Colour;
    
    layout(location = 0) out vec4 ColourOut0;
    
    uniform sampler2D Texture;
    
    void main()
    {
        //ColourOut0 = vec4(1.0, 1.0, 1.0, 1.0);
        ColourOut0 = texture2D(Texture, VertexOutput_TexCoord) * VertexOutput_Colour;
    }
</shader>
