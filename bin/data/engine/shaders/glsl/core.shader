
<shader id="Engine_FullscreenQuad_VS">
    attribute vec2 VertexInput_Position;

    varying vec2 VertexOutput_TexCoord;

	void main()
	{
        VertexOutput_TexCoord = (VertexInput_Position + 1.0) * 0.5;
        gl_Position = vec4(VertexInput_Position, 0.0, 1.0);
	}
</shader>

<shader id="Engine_FullscreenQuad_FS">
    varying vec2 VertexOutput_TexCoord;

	uniform sampler2D Texture;

	void main()
	{
		gl_FragData[0] = texture2D(Texture, VertexOutput_TexCoord);
	}
</shader>



<shader id="Engine_Textured2DQuad_VS">
    #version 330

    in vec2 VertexInput_Position;
    in vec2 VertexInput_TexCoord;
    
    out vec2 VertexOutput_TexCoord;
    
    uniform mat4 Projection;

	void main()
	{
        gl_Position = Projection * vec4(VertexInput_Position, 0.0, 1.0);
        VertexOutput_TexCoord = VertexInput_TexCoord;
	}
</shader>

<shader id="Engine_Textured2DQuad_FS">
    #version 330
    
    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut0;
    
    uniform sampler2D Texture;

	void main()
	{
		ColourOut0 = texture2D(Texture, VertexOutput_TexCoord);
	}
</shader>
