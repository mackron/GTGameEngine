
<shader id="DefaultSceneRenderer_FinalCompositionVS">
<![CDATA[
    attribute vec3 VertexInput_Position;
    attribute vec2 VertexInput_TexCoord;

    varying vec2 VertexOutput_TexCoord;
    
    void main()
    {
        VertexOutput_TexCoord = VertexInput_TexCoord;
        gl_Position           = vec4(VertexInput_Position, 1.0);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_FinalCompositionHDRFS">
<![CDATA[
    varying vec2 VertexOutput_TexCoord;
    
    uniform sampler2D ColourBuffer;
    uniform sampler2D BloomBuffer;
    uniform float     Exposure;
    uniform float     BloomFactor;
    
    void main()
    {
        vec4  bloom     = texture2D(BloomBuffer, VertexOutput_TexCoord, 0);
        float luminance = dot(vec4(0.30, 0.59, 0.11, 0.0), texture2D(ColourBuffer, VertexOutput_TexCoord, 1000.0));
        
        
        // Bloom.
        gl_FragData[0] = (bloom * BloomFactor) + texture2D(ColourBuffer, VertexOutput_TexCoord);
        
        // Tone Mapping.
        gl_FragData[0] = gl_FragData[0] * min(5.0, Exposure * (Exposure / luminance + 1.0) / (Exposure + 1.0));
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_FinalCompositionHDRNoBloomFS">
<![CDATA[
    varying vec2 VertexOutput_TexCoord;
    
    uniform sampler2D ColourBuffer;
    uniform float     Exposure;
    
    void main()
    {
        float luminance = dot(vec4(0.30, 0.59, 0.11, 0.0), texture2D(ColourBuffer, VertexOutput_TexCoord, 1000.0));

        // Tone Mapping.
        gl_FragData[0] = texture2D(ColourBuffer, VertexOutput_TexCoord) * min(5.0, Exposure * (Exposure / luminance + 1.0) / (Exposure + 1.0));
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_FinalCompositionLDRFS">
<![CDATA[
    varying vec2 VertexOutput_TexCoord;
    
    uniform sampler2D ColourBuffer;
    
    void main()
    {
        gl_FragData[0] = texture2D(ColourBuffer, VertexOutput_TexCoord);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BloomFS">  <!-- Vertex Shader is DefaultSceneRenderer_FinalCompositionVS -->
<![CDATA[
    varying vec2 VertexOutput_TexCoord;
    
    uniform sampler2D ColourBuffer;
    
    void main()
    {
        gl_FragData[0] = max(vec4(0.0), texture2D(ColourBuffer, VertexOutput_TexCoord, 0) - vec4(1.0, 1.0, 1.0, 0.0));
    }
]]>
</shader>


















