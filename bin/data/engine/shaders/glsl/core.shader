
<shader id="Engine_Textured2DQuad_VS">
    #version 120

    attribute vec2 VertexInput_Position;
    attribute vec2 VertexInput_TexCoord;
    
    varying vec2 VertexOutput_TexCoord;
    
    uniform mat4 Projection;

	void main()
	{
        gl_Position = Projection * vec4(VertexInput_Position, 0.0, 1.0);
        VertexOutput_TexCoord = VertexInput_TexCoord;
	}
</shader>

<shader id="Engine_Textured2DQuad_FS">
    #version 120
    
    varying vec2 VertexOutput_TexCoord;
    
    uniform sampler2D Texture;

	void main()
	{
		gl_FragData[0] = texture2D(Texture, VertexOutput_TexCoord);
	}
</shader>


<shader id="Engine_GUI_VS">
    #version 120
    
    attribute vec2 VertexInput_Position;
    attribute vec2 VertexInput_TexCoord;
    attribute vec4 VertexInput_Colour;
    
    varying vec2 VertexOutput_TexCoord;
    varying vec4 VertexOutput_Colour;
    
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
    #version 120
    
    varying vec2 VertexOutput_TexCoord;
    varying vec4 VertexOutput_Colour;
    
    uniform sampler2D Texture;
    
    void main()
    {
        gl_FragData[0] = texture2D(Texture, VertexOutput_TexCoord) * VertexOutput_Colour;
    }
</shader>

