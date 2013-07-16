
<shader id="DefaultSceneRenderer_DepthPassVS">
<![CDATA[
    attribute vec3 VertexInput_Position;
    uniform   mat4 PVMMatrix;
    
    void main()
    {
        gl_Position = PVMMatrix * vec4(VertexInput_Position, 1.0);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_DepthPassFS">
<![CDATA[
    void main()
    {
    }
]]>
</shader>




<shader id="DefaultSceneRenderer_HighlightVS">
<![CDATA[
    attribute vec3 VertexInput_Position;
    uniform   mat4 PVMMatrix;
    
    void main()
    {
        gl_Position = PVMMatrix * vec4(VertexInput_Position, 1.0);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_HighlightFS">
<![CDATA[
    void main()
    {
        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 0.0);
    }
]]>
</shader>





<shader id="DefaultSceneRenderer_BlurVS">
<![CDATA[
    attribute vec3 VertexInput_Position;
    attribute vec4 VertexInput_TexCoord;

    varying vec4 VertexOutput_TexCoord;
    
    void main()
    {
        VertexOutput_TexCoord = VertexInput_TexCoord;
        gl_Position           = vec4(VertexInput_Position, 1.0);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurXFS">
<![CDATA[
    varying vec4 VertexOutput_TexCoord;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord.xy;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        /*
        float coefficients[21] = float[]
        (
            0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116,
            0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
            0.14107424,
            0.132526984, 0.109868729, 0.080381679, 0.051898313, 0.029570767,
            0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337
        );
        */

        float coefficients[21];
        coefficients[0]  = 0.000272337; coefficients[1]  = 0.00089296;  coefficients[2]  = 0.002583865; coefficients[3]  = 0.00659813;  coefficients[4]  = 0.014869116;
        coefficients[5]  = 0.029570767; coefficients[6]  = 0.051898313; coefficients[7]  = 0.080381679; coefficients[8]  = 0.109868729; coefficients[9]  = 0.132526984;
        coefficients[10] = 0.14107424;
        coefficients[11] = 0.132526984; coefficients[12] = 0.109868729; coefficients[13] = 0.080381679; coefficients[14] = 0.051898313; coefficients[15] = 0.029570767;
        coefficients[16] = 0.014869116; coefficients[17] = 0.00659813;  coefficients[18] = 0.002583865; coefficients[19] = 0.00089296;  coefficients[20] = 0.000272337;

        for(int i = 0; i < 21; i++)
        {
            gl_FragData[0] += texture2D(Texture, vec2(uv.x + (i - 10) * uvOffset, uv.y)) * coefficients[i];
        }
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurYFS">
<![CDATA[
    varying vec4 VertexOutput_TexCoord;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord.xy;
        float uvOffset = 1.0 / textureSize(Texture, 0).x;
    
        float coefficients[21] = float[]
        (
            0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116,
            0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
            0.14107424,
            0.132526984, 0.109868729, 0.080381679, 0.051898313, 0.029570767,
            0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337
        );
        
        
        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 21; i++)
        {
            gl_FragData[0] += texture2D(Texture, vec2(uv.x, uv.y + (i - 10) * uvOffset)) * coefficients[i];
        }
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurX7x7FS">
<![CDATA[
    varying vec4 VertexOutput_TexCoord;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord.xy;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[7] = float[]
        (
            0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006
        );
        
        
        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 7; i++)
        {
            gl_FragData[0] += texture2D(Texture, vec2(uv.x + (i - 3) * uvOffset, uv.y)) * coefficients[i];
        }
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurY7x7FS">
<![CDATA[
    varying vec4 VertexOutput_TexCoord;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord.xy;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[7] = float[]
        (
            0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006
        );
        
        
        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 7; i++)
        {
            gl_FragData[0] += texture2D(Texture, vec2(uv.x, uv.y + (i - 3) * uvOffset)) * coefficients[i];
        }
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_BlurX11x11FS">
<![CDATA[
    varying vec4 VertexOutput_TexCoord;

    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord.xy;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[11] = float[]
        (
            0.0005446104822918076, 0.005167120389696288, 0.029734724821789165, 0.10378438737379546, 0.21971154979445848,
            0.28211521427593783,
            0.21971154979445848, 0.10378438737379546, 0.029734724821789165, 0.005167120389696288, 0.0005446104822918076
        );

        
        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 11; i++)
        {
            gl_FragData[0] += texture2D(Texture, vec2(uv.x + (i - 5) * uvOffset, uv.y)) * coefficients[i];
        }
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurY11x11FS">
<![CDATA[
    varying vec4 VertexOutput_TexCoord;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord.xy;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[11] = float[]
        (
            0.0005446104822918076, 0.005167120389696288, 0.029734724821789165, 0.10378438737379546, 0.21971154979445848,
            0.28211521427593783,
            0.21971154979445848, 0.10378438737379546, 0.029734724821789165, 0.005167120389696288, 0.0005446104822918076
        );
        

        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 11; i++)
        {
            gl_FragData[0] += texture2D(Texture, vec2(uv.x, uv.y + (i - 5) * uvOffset)) * coefficients[i];
        }
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_BlurX15x15FS">
<![CDATA[
    varying vec4 VertexOutput_TexCoord;

    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord.xy;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[15] = float[]
        (
            0.0004364074260382143, 0.002216259780359165, 0.008765477469243402, 0.026999571389574404, 0.06476860475415001, 0.1210036840004649, 0.17605932135784913,
            0.19950134764464159,
            0.17605932135784913, 0.1210036840004649, 0.06476860475415001, 0.026999571389574404, 0.008765477469243402, 0.002216259780359165, 0.0004364074260382143
        );
        
        
        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 15; i++)
        {
            gl_FragData[0] += texture2D(Texture, vec2(uv.x + (i - 7) * uvOffset, uv.y)) * coefficients[i];
        }
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurY15x15FS">
<![CDATA[
    varying vec4 VertexOutput_TexCoord;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord.xy;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[15] = float[]
        (
            0.0004364074260382143, 0.002216259780359165, 0.008765477469243402, 0.026999571389574404, 0.06476860475415001, 0.1210036840004649, 0.17605932135784913,
            0.19950134764464159,
            0.17605932135784913, 0.1210036840004649, 0.06476860475415001, 0.026999571389574404, 0.008765477469243402, 0.002216259780359165, 0.0004364074260382143
        );
        
        
        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 15; i++)
        {
            gl_FragData[0] += texture2D(Texture, vec2(uv.x, uv.y + (i - 7) * uvOffset)) * coefficients[i];
        }
    }
]]>
</shader>



<shader id="DefaultSceneRenderer_ShadowMapVS">
<![CDATA[
    attribute vec3 VertexInput_Position;
    varying   vec4 ShadowCoord;
    uniform   mat4 PVMMatrix;
    
    void main()
    {
        gl_Position = PVMMatrix * vec4(VertexInput_Position, 1.0);
        ShadowCoord = gl_Position;
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_ShadowMapFS">
<![CDATA[
    varying vec4 VertexOutput_Position;
    varying vec4 ShadowCoord;
    
    void main()
    {
        float depth = ShadowCoord.z / ShadowCoord.w;
        gl_FragData[0] = vec4(depth, depth * depth, 0.0, 1.0);
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_PointShadowMapVS">
<![CDATA[
    attribute vec3 VertexInput_Position;
    varying   vec4 ShadowCoord;
    
    uniform mat4 PVMMatrix;
    uniform mat4 ViewModelMatrix;
    
    void main()
    {
        gl_Position = PVMMatrix       * vec4(VertexInput_Position, 1.0);
        ShadowCoord = ViewModelMatrix * vec4(VertexInput_Position, 1.0);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_PointShadowMapFS">
<![CDATA[
    varying vec4 ShadowCoord;

    void main()
    {
        float depth = length(ShadowCoord.xyz);
        gl_FragData[0] = vec4(depth, depth * depth, 0.0, 1.0);
    }
]]>
</shader>






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


















