
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



<shader id="Engine_ColouredBGQuad_VS">
    attribute vec2 VertexInput_Position;

	void main()
	{
        gl_Position = vec4(VertexInput_Position, 0.0, 1.0);
	}
</shader>

<shader id="Engine_ColouredBGQuad_FS">
    uniform vec3 Colour;

	void main()
	{
		gl_FragData[0] = vec4(Colour, 1.0);
	}
</shader>



<shader id="Engine_DefaultVS">
    attribute vec3 VertexInput_Position;
    attribute vec2 VertexInput_TexCoord;
    attribute vec3 VertexInput_Normal;
    attribute vec3 VertexInput_Tangent;
    attribute vec3 VertexInput_Bitangent;
    
    varying vec2 VertexOutput_TexCoord;
    varying vec3 VertexOutput_Normal;
    varying vec3 VertexOutput_Tangent;
    varying vec3 VertexOutput_Bitangent;
    varying vec4 VertexOutput_Position;
    varying vec4 VertexOutput_PositionWS;
		
    uniform mat4 ModelMatrix;
    uniform mat4 ModelViewMatrix;
	uniform mat4 MVPMatrix;
    uniform mat3 NormalMatrix;
		
	void main()
	{
        gl_Position = MVPMatrix * vec4(VertexInput_Position, 1.0);
        
		VertexOutput_TexCoord    = VertexInput_TexCoord;
		VertexOutput_Normal      = normalize(NormalMatrix * VertexInput_Normal);
        VertexOutput_Tangent     = normalize(NormalMatrix * VertexInput_Tangent);
        VertexOutput_Bitangent   = normalize(NormalMatrix * VertexInput_Bitangent);
        VertexOutput_Position    = ModelViewMatrix * vec4(VertexInput_Position, 1.0);
        VertexOutput_PositionWS  = ModelMatrix     * vec4(VertexInput_Position, 1.0);
	}
</shader>

<shader id="Engine_DefaultShadowVS">
    attribute vec3 VertexInput_Position;
    attribute vec2 VertexInput_TexCoord;
    attribute vec3 VertexInput_Normal;
    attribute vec3 VertexInput_Tangent;
    attribute vec3 VertexInput_Bitangent;
    
    varying vec2 VertexOutput_TexCoord;
    varying vec3 VertexOutput_Normal;
    varying vec3 VertexOutput_Tangent;
    varying vec3 VertexOutput_Bitangent;
    varying vec4 VertexOutput_Position;
    varying vec4 VertexOutput_PositionWS;
    varying vec4 VertexOutput_ShadowCoord;
		
    uniform mat4 ModelMatrix;
    uniform mat4 ModelViewMatrix;
	uniform mat4 MVPMatrix;
    uniform mat3 NormalMatrix;

    uniform mat4 ShadowProjectionMatrix;
    uniform mat4 ShadowViewMatrix;
    
		
	void main()
	{
        gl_Position = MVPMatrix * vec4(VertexInput_Position, 1.0);
        
		VertexOutput_TexCoord    = VertexInput_TexCoord;
		VertexOutput_Normal      = normalize(NormalMatrix * VertexInput_Normal);
        VertexOutput_Tangent     = normalize(NormalMatrix * VertexInput_Tangent);
        VertexOutput_Bitangent   = normalize(NormalMatrix * VertexInput_Bitangent);
        VertexOutput_Position    = ModelViewMatrix * vec4(VertexInput_Position, 1.0);
        VertexOutput_PositionWS  = ModelMatrix     * vec4(VertexInput_Position, 1.0);
        VertexOutput_ShadowCoord = (ShadowProjectionMatrix * ShadowViewMatrix * ModelMatrix) * vec4(VertexInput_Position, 1.0);
	}
</shader>


<shader id="Engine_ShadowVS">
    attribute vec3 VertexInput_Position;

	uniform mat4 MVPMatrix;
    
    varying vec4 ShadowCoord;

	void main()
	{
        ShadowCoord = MVPMatrix * vec4(VertexInput_Position, 1.0);
        gl_Position = ShadowCoord;
	}
</shader>

<shader id="Engine_PointLightShadowVS">
    attribute vec3 VertexInput_Position;
    varying   vec4 VertexOutput_Position;
    

    uniform mat4 ModelViewMatrix;
	uniform mat4 MVPMatrix;

	void main()
	{
        gl_Position           = MVPMatrix       * vec4(VertexInput_Position, 1.0);
        VertexOutput_Position = ModelViewMatrix * vec4(VertexInput_Position, 1.0);
	}
</shader>


<shader id="Engine_ClearDepth_VS">
    attribute vec3 VertexInput_Position;

	void main()
	{
        gl_Position = vec4(VertexInput_Position, 1.0);
	}
</shader>

<shader id="Engine_ClearDepth_FS">
    void main()
    {
    }
</shader>
