
<shader id="Engine_GUIQuad_VS">
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

<shader id="Engine_GUIQuad_FS">
    varying vec2 VertexOutput_TexCoord;
    
    uniform sampler2D Texture;
    uniform vec4      Color;
		
	void main()
	{
		gl_FragData[0] = texture2D(Texture, VertexOutput_TexCoord) * Color;
	}
</shader>
    
<shader id="Engine_GUIQuad_NoTexture_FS">
    varying vec2 VertexOutput_TexCoord;
    
    uniform vec4 Color;
		
	void main()
	{
		gl_FragData[0] = Color;
	}
</shader>


<!-- Shader for RCDraw -->
<shader id="Engine_GUIDraw_VS">
    attribute float2 VertexInput_Position;
    attribute float2 VertexInput_TexCoord;
    attribute float4 VertexInput_Colour;
    
    varying float2 VertexOutput_TexCoord;
    varying float4 VertexOutput_Colour;
    
    uniform mat4 Projection;
    
    void main()
    {
        gl_Position = Projection * vec4(VertexInput_Position, 0.0, 1.0);
        VertexOutput_TexCoord = VertexInput_TexCoord;
        VertexOutput_Colour   = VertexInput_Colour;
    }
</shader>

<shader id="Engine_GUIDraw_FS">
    varying float2 VertexOutput_TexCoord;
    varying float4 VertexOutput_Colour;
    
    uniform sampler2D Texture;
    
    void main()
    {
        gl_FragData[0] = texture2D(Texture, VertexOutput_TexCoord) * VertexOutput_Colour;
    }
</shader>



<!-- Shadow Shader -->
<shader id="Engine_GUIShadow_VS">
    attribute vec2 VertexInput_Position;
    attribute vec4 VertexInput_Colour;
    
    varying vec4 VertexOutput_Colour;
    
    uniform mat4 Projection;
    
    void main()
    {
        gl_Position = Projection * vec4(VertexInput_Position, 0.0, 1.0);
        VertexOutput_Colour = VertexInput_Colour;
    }
</shader>

<shader id="Engine_GUIShadow_FS">
    varying vec4 VertexOutput_Colour;
    
    void main()
    {
        gl_FragData[0] = VertexOutput_Colour;
    }
</shader>

<!-- Text Shader -->
<shader id="Engine_GUIText_VS">
    attribute vec3 VertexInput_Position;
    attribute vec2 VertexInput_TexCoord;
    
    varying vec2 VertexOutput_TexCoord;
    
    uniform mat4 Projection;
    uniform vec2 Offset;
    
    void main()
    {
        gl_Position = Projection * vec4(VertexInput_Position.xy + Offset, VertexInput_Position.z, 1.0);
        VertexOutput_TexCoord = VertexInput_TexCoord;
    }
</shader>

<shader id="Engine_GUIText_FS">
    varying vec2 VertexOutput_TexCoord;
    
    uniform sampler2D Texture;
    uniform vec4      Color;
    
    void main()
    {
        gl_FragData[0] = vec4(Color.rgb, Color.a * texture2D(Texture, VertexOutput_TexCoord).a);
    }
</shader>