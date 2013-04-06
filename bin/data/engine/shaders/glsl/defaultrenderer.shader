
<shader id="DefaultSceneRenderer_DepthPassVS">
<![CDATA[
    #version 330
    
    in      vec3 VertexInput_Position;
    uniform mat4 PVMMatrix;
    
    void main()
    {
        gl_Position = PVMMatrix * vec4(VertexInput_Position, 1.0);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_DepthPassFS">
<![CDATA[
    #version 330
    
    void main()
    {
    }
]]>
</shader>




<shader id="DefaultSceneRenderer_HighlightVS">
<![CDATA[
    #version 330
    
    in      vec3 VertexInput_Position;
    uniform mat4 PVMMatrix;
    
    void main()
    {
        gl_Position = PVMMatrix * vec4(VertexInput_Position, 1.0);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_HighlightFS">
<![CDATA[
    #version 330
    
    layout(location = 0) out vec4 ColourOut;
    
    void main()
    {
        ColourOut = vec4(0.0, 0.0, 0.0, 0.0);
    }
]]>
</shader>





<shader id="DefaultSceneRenderer_BlurVS">
<![CDATA[
    #version 330
    
    in vec3 VertexInput_Position;
    in vec2 VertexInput_TexCoord;

    out vec2 VertexOutput_TexCoord;
    
    void main()
    {
        VertexOutput_TexCoord = VertexInput_TexCoord;
        gl_Position           = vec4(VertexInput_Position, 1.0);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurXFS">
<![CDATA[
    #version 330
    
    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[21] = float[]
        (
            0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116,
            0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
            0.14107424,
            0.132526984, 0.109868729, 0.080381679, 0.051898313, 0.029570767,
            0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337
        );
        
        
        ColourOut = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 21; i++)
        {
            ColourOut += texture2D(Texture, vec2(uv.x + (i - 10) * uvOffset, uv.y)) * coefficients[i];
        }
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurYFS">
<![CDATA[
    #version 330
    
    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord;
        float uvOffset = 1.0 / textureSize(Texture, 0).x;
    
        float coefficients[21] = float[]
        (
            0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116,
            0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
            0.14107424,
            0.132526984, 0.109868729, 0.080381679, 0.051898313, 0.029570767,
            0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337
        );
        
        
        ColourOut = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 21; i++)
        {
            ColourOut += texture2D(Texture, vec2(uv.x, uv.y + (i - 10) * uvOffset)) * coefficients[i];
        }
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurX7x7FS">
<![CDATA[
    #version 330
    
    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[7] = float[]
        (
            0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006
        );
        
        
        ColourOut = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 7; i++)
        {
            ColourOut += texture2D(Texture, vec2(uv.x + (i - 3) * uvOffset, uv.y)) * coefficients[i];
        }
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurY7x7FS">
<![CDATA[
    #version 330
    
    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[7] = float[]
        (
            0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006
        );
        
        
        ColourOut = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 7; i++)
        {
            ColourOut += texture2D(Texture, vec2(uv.x, uv.y + (i - 3) * uvOffset)) * coefficients[i];
        }
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_BlurX11x11FS">
<![CDATA[
    #version 330
    
    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[11] = float[]
        (
            0.0005446104822918076, 0.005167120389696288, 0.029734724821789165, 0.10378438737379546, 0.21971154979445848,
            0.28211521427593783,
            0.21971154979445848, 0.10378438737379546, 0.029734724821789165, 0.005167120389696288, 0.0005446104822918076
        );

        
        ColourOut = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 11; i++)
        {
            ColourOut += texture2D(Texture, vec2(uv.x + (i - 5) * uvOffset, uv.y)) * coefficients[i];
        }
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurY11x11FS">
<![CDATA[
    #version 330
    
    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[11] = float[]
        (
            0.0005446104822918076, 0.005167120389696288, 0.029734724821789165, 0.10378438737379546, 0.21971154979445848,
            0.28211521427593783,
            0.21971154979445848, 0.10378438737379546, 0.029734724821789165, 0.005167120389696288, 0.0005446104822918076
        );
        

        ColourOut = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 11; i++)
        {
            ColourOut += texture2D(Texture, vec2(uv.x, uv.y + (i - 5) * uvOffset)) * coefficients[i];
        }
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_BlurX15x15FS">
<![CDATA[
    #version 330
    
    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[15] = float[]
        (
            0.0004364074260382143, 0.002216259780359165, 0.008765477469243402, 0.026999571389574404, 0.06476860475415001, 0.1210036840004649, 0.17605932135784913,
            0.19950134764464159,
            0.17605932135784913, 0.1210036840004649, 0.06476860475415001, 0.026999571389574404, 0.008765477469243402, 0.002216259780359165, 0.0004364074260382143
        );
        
        
        ColourOut = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 15; i++)
        {
            ColourOut += texture2D(Texture, vec2(uv.x + (i - 7) * uvOffset, uv.y)) * coefficients[i];
        }
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BlurY15x15FS">
<![CDATA[
    #version 330
    
    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D Texture;
    
    void main()
    {
        vec2  uv       = VertexOutput_TexCoord;
        float uvOffset = 1.0 / float(textureSize(Texture, 0));
    
        float coefficients[15] = float[]
        (
            0.0004364074260382143, 0.002216259780359165, 0.008765477469243402, 0.026999571389574404, 0.06476860475415001, 0.1210036840004649, 0.17605932135784913,
            0.19950134764464159,
            0.17605932135784913, 0.1210036840004649, 0.06476860475415001, 0.026999571389574404, 0.008765477469243402, 0.002216259780359165, 0.0004364074260382143
        );
        
        
        ColourOut = vec4(0.0, 0.0, 0.0, 1.0);
        
        for(int i = 0; i < 15; i++)
        {
            ColourOut += texture2D(Texture, vec2(uv.x, uv.y + (i - 7) * uvOffset)) * coefficients[i];
        }
    }
]]>
</shader>



<shader id="DefaultSceneRenderer_ShadowMapVS">
<![CDATA[
    #version 330
    
    in      vec3 VertexInput_Position;
    out     vec4 ShadowCoord;
    uniform mat4 PVMMatrix;
    
    void main()
    {
        gl_Position = PVMMatrix * vec4(VertexInput_Position, 1.0);
        ShadowCoord = gl_Position;
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_ShadowMapFS">
<![CDATA[
    #version 330
    
    in vec4 VertexOutput_Position;
    in vec4 ShadowCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    void main()
    {
        float depth = ShadowCoord.z / ShadowCoord.w;
        ColourOut = vec4(depth, depth * depth, 0.0, 1.0);
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_PointShadowMapVS">
<![CDATA[
    #version 330
    
    in      vec3 VertexInput_Position;
    out     vec4 ShadowCoord;
    
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
    #version 330
    
    in vec4 ShadowCoord;
    layout(location = 0) out vec4 ColourOut;
    
    void main()
    {
        float depth = length(ShadowCoord.xyz);
        
        ColourOut = vec4(depth, depth * depth, 0.0, 1.0);
    }
]]>
</shader>



<shader id="DefaultSceneRenderer_LightingVS">
<![CDATA[
    #version 330
    
    in  vec3 VertexInput_Position;
    in  vec3 VertexInput_PositionVS;
    in  vec2 VertexInput_TexCoord;
    in  vec3 VertexInput_Normal;
    in  vec3 VertexInput_Tangent;
    in  vec3 VertexInput_Bitangent;
    
    out vec4 VertexOutput_Position;
    out vec4 VertexOutput_PositionLS;
    out vec4 VertexOutput_PositionVS;
    out vec4 VertexOutput_PositionWS;
    out vec2 VertexOutput_TexCoord;
    out vec3 VertexOutput_Normal;
    out vec3 VertexOutput_Tangent;
    out vec3 VertexOutput_Bitangent;
    
    uniform mat4 PVMMatrix;
    uniform mat4 ViewModelMatrix;
    uniform mat4 ModelMatrix;
    uniform mat3 NormalMatrix;
    uniform mat4 LightPVMMatrix;
    
    void main()
    {
        VertexOutput_Position   = PVMMatrix       * vec4(VertexInput_Position, 1.0);
        VertexOutput_PositionLS = LightPVMMatrix  * vec4(VertexInput_Position, 1.0);
        VertexOutput_PositionVS = ViewModelMatrix * vec4(VertexInput_Position, 1.0);
        VertexOutput_PositionWS = ModelMatrix     * vec4(VertexInput_Position, 1.0);
        VertexOutput_TexCoord   = VertexInput_TexCoord;
        VertexOutput_Normal     = normalize(NormalMatrix * VertexInput_Normal);
        VertexOutput_Tangent    = normalize(NormalMatrix * VertexInput_Tangent);
        VertexOutput_Bitangent  = normalize(NormalMatrix * VertexInput_Bitangent);
        
        gl_Position = VertexOutput_Position;
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_LightingFS">
<![CDATA[
    #version 330
    
    vec3 Normal();
    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut);
    
    in vec4 VertexOutput_Position;
    in vec4 VertexOutput_PositionLS;
    in vec4 VertexOutput_PositionVS;
    in vec4 VertexOutput_PositionWS;
    in vec2 VertexOutput_TexCoord;
    in vec3 VertexOutput_Normal;
    in vec3 VertexOutput_Tangent;
    in vec3 VertexOutput_Bitangent;
    
    layout(location = 0) out vec4 DiffuseOut;
    layout(location = 1) out vec4 SpecularOut;
    
    
    float DiffuseFactor(vec3 N, vec3 L)
    {
        return max(0.0, dot(N, L));
    }
    
    float SpecularFactor(vec3 N, vec3 H, float power)
    {
        return max(0.0, pow(max(dot(N, H), 0.0), power));       // The 'power' is the size of the highlight. Larger values means smaller, more focused specular highlights.
    }
    
    float SpotFactor(vec3 L, vec3 lightDirection, float cosAngleInner, float cosAngleOuter)
    {
        return clamp((dot(-L, lightDirection) - cosAngleOuter) / (cosAngleInner - cosAngleOuter), 0.0, 1.0);
    }
    
    float AttenuationFactor(float r, float f, float d)
    {
        return pow(max(0.0, 1.0 - (d / r)), f + 1.0);
    }
    
    
    void main()
    {
        vec3 normal = normalize(mat3(VertexOutput_Tangent, VertexOutput_Bitangent, VertexOutput_Normal) * Normal());
        
        vec3 diffuse;
        vec3 specular;
        DoLighting(normal, diffuse, specular);
        
        DiffuseOut  = vec4(diffuse,  1.0);
        SpecularOut = vec4(specular, 1.0);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_AmbientLight">
<![CDATA[
    uniform vec3 Colour;
    
    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut)
    {
        diffuseOut  = Colour;
        specularOut = vec3(0.0, 0.0, 0.0);
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_DirectionalLight">
<![CDATA[
    uniform vec3 Colour;
    uniform vec3 Position;
    uniform vec3 Direction;
    
    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut)
    {
        vec3 N = normal;
        vec3 L = -Direction;
        vec3 H = normalize(L - normalize(VertexOutput_PositionVS.xyz));
    
        float diffuse  = DiffuseFactor(N, L);
        float specular = SpecularFactor(N, H, 64.0f);

        diffuseOut  = Colour * diffuse;
        specularOut = Colour * specular;
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_ShadowDirectionalLight">
<![CDATA[
    uniform vec3 Colour;
    uniform vec3 Position;
    uniform vec3 Direction;
    
    
    uniform sampler2D ShadowMap;
    
    float linstep(float low, float high, float v)
    {
        return clamp((v - low) / (high - low), 0.0, 1.0);
    }
    
    
    float CalculateShadowVSM(vec2 shadowUV, float fragmentDepth)
    {
        float bias     = 0.00075;
        vec2  moments  = texture2D(ShadowMap, shadowUV).xy;
        float variance = moments.y - (moments.x * moments.x);
        float d        = fragmentDepth - moments.x;
        float p        = smoothstep(fragmentDepth - bias, fragmentDepth, moments.x);
        float pMax     = linstep(0.2, 1.0, variance / (variance + (d * d)));
        
        return clamp(max(p, pMax), 0.0, 1.0);
    }
    
    float CalculateShadow()
    {
        vec3  shadowCoord   = VertexOutput_PositionLS.xyz / VertexOutput_PositionLS.w;
        vec2  shadowUV      = shadowCoord.xy * 0.5 + 0.5;
        float fragmentDepth = shadowCoord.z;
    
        return CalculateShadowVSM(shadowUV, fragmentDepth);
    }
    
    
    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut)
    {
        vec3 N = normal;
        vec3 L = -Direction;
        vec3 H = normalize(L - normalize(VertexOutput_PositionVS.xyz));
    
        float diffuse  = DiffuseFactor(N, L);
        float specular = SpecularFactor(N, H, 64.0f);
        float shadow   = CalculateShadow();

        diffuseOut  = Colour * (diffuse  * shadow);
        specularOut = Colour * (specular * shadow);
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_PointLightVS">
<![CDATA[
    #version 330
    
    in  vec3 VertexInput_Position;
    in  vec3 VertexInput_PositionVS;
    in  vec2 VertexInput_TexCoord;
    in  vec3 VertexInput_Normal;
    in  vec3 VertexInput_Tangent;
    in  vec3 VertexInput_Bitangent;
    
    out vec4 VertexOutput_Position;
    out vec4 VertexOutput_PositionVS;
    out vec4 VertexOutput_PositionWS;
    out vec2 VertexOutput_TexCoord;
    out vec3 VertexOutput_Normal;
    out vec3 VertexOutput_Tangent;
    out vec3 VertexOutput_Bitangent;
    
    out vec3 VertexOutput_L;
    
    
    uniform mat4 PVMMatrix;
    uniform mat4 ViewModelMatrix;
    uniform mat4 ModelMatrix;
    uniform mat3 NormalMatrix;
    
    uniform vec3 Position;
    
    void main()
    {
        VertexOutput_Position   = PVMMatrix       * vec4(VertexInput_Position, 1.0);
        VertexOutput_PositionVS = ViewModelMatrix * vec4(VertexInput_Position, 1.0);
        VertexOutput_PositionWS = ModelMatrix     * vec4(VertexInput_Position, 1.0);
        VertexOutput_TexCoord   = VertexInput_TexCoord;
        VertexOutput_Normal     = normalize(NormalMatrix * VertexInput_Normal);
        VertexOutput_Tangent    = normalize(NormalMatrix * VertexInput_Tangent);
        VertexOutput_Bitangent  = normalize(NormalMatrix * VertexInput_Bitangent);
        
        VertexOutput_L          = Position - VertexOutput_PositionVS.xyz;
        
        gl_Position = VertexOutput_Position;
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_PointLight">
<![CDATA[
    in vec3 VertexOutput_L;

    uniform vec3  Colour;
    uniform float Radius;
    uniform float Falloff;
    
    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut)
    {
        // N - Input normal
        // L - Light vector from the light to the vertex
        
        vec3 N = normal;
        vec3 L = normalize(VertexOutput_L);
        vec3 H = normalize(L - normalize(VertexOutput_PositionVS.xyz));

        float diffuse     = DiffuseFactor(N, L);
        float specular    = SpecularFactor(N, H, 64.0f);
        float attenuation = AttenuationFactor(Radius, Falloff, length(VertexOutput_L));

        diffuseOut  = Colour * (diffuse  * attenuation);
        specularOut = Colour * (specular * attenuation);
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_ShadowPointLightVS">
<![CDATA[
    #version 330
    
    in  vec3 VertexInput_Position;
    in  vec3 VertexInput_PositionVS;
    in  vec2 VertexInput_TexCoord;
    in  vec3 VertexInput_Normal;
    in  vec3 VertexInput_Tangent;
    in  vec3 VertexInput_Bitangent;
    
    out vec4 VertexOutput_Position;
    out vec4 VertexOutput_PositionVS;
    out vec4 VertexOutput_PositionWS;
    out vec2 VertexOutput_TexCoord;
    out vec3 VertexOutput_Normal;
    out vec3 VertexOutput_Tangent;
    out vec3 VertexOutput_Bitangent;
    
    out vec3 VertexOutput_ShadowCoord;
    out vec3 VertexOutput_L;
    
    
    uniform mat4 PVMMatrix;
    uniform mat4 ViewModelMatrix;
    uniform mat4 ModelMatrix;
    uniform mat3 NormalMatrix;
    
    uniform vec3 LightPositionWS;
    uniform vec3 Position;
    
    void main()
    {
        VertexOutput_Position   = PVMMatrix       * vec4(VertexInput_Position, 1.0);
        VertexOutput_PositionVS = ViewModelMatrix * vec4(VertexInput_Position, 1.0);
        VertexOutput_PositionWS = ModelMatrix     * vec4(VertexInput_Position, 1.0);
        VertexOutput_TexCoord   = VertexInput_TexCoord;
        VertexOutput_Normal     = normalize(NormalMatrix * VertexInput_Normal);
        VertexOutput_Tangent    = normalize(NormalMatrix * VertexInput_Tangent);
        VertexOutput_Bitangent  = normalize(NormalMatrix * VertexInput_Bitangent);
        
        VertexOutput_ShadowCoord = VertexOutput_PositionWS.xyz - LightPositionWS;
        VertexOutput_L           = Position - VertexOutput_PositionVS.xyz;
        
        gl_Position = VertexOutput_Position;
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_ShadowPointLight">
<![CDATA[
    in vec3 VertexOutput_ShadowCoord;
    in vec3 VertexOutput_L;
    
    uniform vec3  Colour;
    uniform float Radius;
    uniform float Falloff;
    
    uniform samplerCube ShadowMap;
    
    
    float linstep(float low, float high, float v)
    {
        return clamp((v - low) / (high - low), 0.0, 1.0);
    }
    
    float CalculateShadowVSM(vec3 shadowCoord, float fragmentDepth)
    {
        float bias     = 0.04 * fragmentDepth;       // This can affect seams. Lower value = more seams.
        vec2  moments  = texture(ShadowMap, shadowCoord).xy;
        float variance = moments.y - (moments.x * moments.x);
        float d        = fragmentDepth - moments.x;
        float p        = smoothstep(fragmentDepth - bias, fragmentDepth, moments.x);
        float pMax     = linstep(0.4, 1.0, variance / ((d * d) + variance));
        
        return clamp(max(p, pMax), 0.0, 1.0);
    }
    
    
    
    float CalculateShadow()
    {
        return CalculateShadowVSM(VertexOutput_ShadowCoord, length(VertexOutput_ShadowCoord));
    }
    
    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut)
    {
        // N - Input normal
        // L - Light vector from the light to the vertex
        
        vec3 N = normal;
        vec3 L = normalize(VertexOutput_L);
        vec3 H = normalize(L - normalize(VertexOutput_PositionVS.xyz));

        float diffuse     = DiffuseFactor(N, L);
        float specular    = SpecularFactor(N, H, 64.0f);
        float attenuation = AttenuationFactor(Radius, Falloff, length(VertexOutput_L));
        float shadow      = CalculateShadow();

        diffuseOut  = Colour * (diffuse  * attenuation * shadow);
        specularOut = Colour * (specular * attenuation * shadow);
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_SpotLight">
<![CDATA[
    uniform vec3  Colour;
    uniform vec3  Position;
    uniform vec3  Direction;
    uniform float Length;
    uniform float Falloff;
    uniform float CosAngleInner;
    uniform float CosAngleOuter;
    

    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut)
    {
        vec3 N = normal;
        vec3 L = Position - VertexOutput_PositionVS.xyz;
        vec3 H = normalize(normalize(L) - normalize(VertexOutput_PositionVS.xyz));
        
        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, 64.0f);
        float attenuation = AttenuationFactor(Length, Falloff, length(L));
        float spot        = SpotFactor(normalize(L), Direction, CosAngleInner, CosAngleOuter);
            
        diffuseOut  = Colour * (diffuse  * attenuation * spot);
        specularOut = Colour * (specular * attenuation * spot);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_ShadowSpotLight">
<![CDATA[
    uniform vec3  Colour;
    uniform vec3  Position;
    uniform vec3  Direction;
    uniform float Length;
    uniform float Falloff;
    uniform float CosAngleInner;
    uniform float CosAngleOuter;

    
    
    uniform sampler2D ShadowMap;
    
    float linstep(float low, float high, float v)
    {
        return clamp((v - low) / (high - low), 0.0, 1.0);
    }
    
    float CalculateShadowVSM(vec2 shadowUV, float fragmentDepth)
    {
        float bias     = 0.001 * fragmentDepth;
        vec2  moments  = texture2D(ShadowMap, shadowUV).xy;
        float variance = moments.y - (moments.x * moments.x);
        float d        = fragmentDepth - moments.x;
        float p        = smoothstep(fragmentDepth - bias, fragmentDepth, moments.x);
        float pMax     = linstep(0.2, 1.0, variance / ((d * d) + variance));
        
        return clamp(max(p, pMax), 0.0, 1.0);
    }
    
    float CalculateShadow()
    {
        vec3  shadowCoord   = VertexOutput_PositionLS.xyz / VertexOutput_PositionLS.w;
        vec2  shadowUV      = shadowCoord.xy * 0.5 + 0.5;
        float fragmentDepth = shadowCoord.z;
    
        return CalculateShadowVSM(shadowUV, fragmentDepth);
    }
    
    
    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut)
    {
        vec3 N = normal;
        vec3 L = Position - VertexOutput_PositionVS.xyz;
        vec3 H = normalize(normalize(L) - normalize(VertexOutput_PositionVS.xyz));
        
        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, 64.0f);
        float attenuation = AttenuationFactor(Length, Falloff, length(L));
        float spot        = SpotFactor(normalize(L), Direction, CosAngleInner, CosAngleOuter);
        float shadow      = CalculateShadow();
        
            
        diffuseOut  = Colour * (diffuse  * attenuation * spot * shadow);
        specularOut = Colour * (specular * attenuation * spot * shadow);
    }
]]>
</shader>







<shader id="DefaultSceneRenderer_MaterialVS">
<![CDATA[
    #version 330
    
    in  vec3 VertexInput_Position;
    in  vec2 VertexInput_TexCoord;
    in  vec3 VertexInput_Normal;
    in  vec3 VertexInput_Tangent;
    in  vec3 VertexInput_Bitangent;
    
    out vec4 VertexOutput_Position;
    out vec4 VertexOutput_PositionVS;
    out vec2 VertexOutput_TexCoord;
    out vec3 VertexOutput_Normal;
    out vec3 VertexOutput_Tangent;
    out vec3 VertexOutput_Bitangent;
    out vec2 lightUV;
    
    uniform mat4 PVMMatrix;
    uniform mat4 ViewModelMatrix;
    uniform mat3 NormalMatrix;
    
    void main()
    {
        VertexOutput_Position   = PVMMatrix       * vec4(VertexInput_Position, 1.0);
        VertexOutput_PositionVS = ViewModelMatrix * vec4(VertexInput_Position, 1.0);
        VertexOutput_TexCoord   = VertexInput_TexCoord;
        VertexOutput_Normal     = normalize(NormalMatrix * VertexInput_Normal);
        VertexOutput_Tangent    = normalize(NormalMatrix * VertexInput_Tangent);
        VertexOutput_Bitangent  = normalize(NormalMatrix * VertexInput_Bitangent);
        
        gl_Position = VertexOutput_Position;
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_MaterialFS">
<![CDATA[
    #version 330
    
    vec4  Diffuse();
    vec3  Emissive();
    float Shininess();
    
    
    in vec4 VertexOutput_Position;
    in vec4 VertexOutput_PositionVS;
    in vec2 VertexOutput_TexCoord;
    in vec3 VertexOutput_Normal;
    in vec3 VertexOutput_Tangent;
    in vec3 VertexOutput_Bitangent;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D DiffuseLighting;
    uniform sampler2D SpecularLighting;
    
    void main()
    {
        vec4  materialDiffuse   = Diffuse();
        vec3  materialEmissive  = Emissive();
        float materialShininess = Shininess();
        
        vec2  lightUV           = (VertexOutput_Position.xy / VertexOutput_Position.w) * 0.5 + 0.5;
        vec3  lightDiffuse      = texture2D(DiffuseLighting,  lightUV).rgb;
        vec3  lightSpecular     = texture2D(SpecularLighting, lightUV).rgb;
        
        ColourOut.rgb = (materialDiffuse.rgb * lightDiffuse) + (materialShininess * lightSpecular) + materialEmissive;
        ColourOut.a   = materialDiffuse.a;
        
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_MaterialRefractiveFS">
<![CDATA[
    #version 330
    
    vec4  Diffuse();
    vec3  Emissive();
    float Shininess();
    vec3  Refraction();
    
    
    in vec4 VertexOutput_Position;
    in vec4 VertexOutput_PositionVS;
    in vec2 VertexOutput_TexCoord;
    in vec3 VertexOutput_Normal;
    in vec3 VertexOutput_Tangent;
    in vec3 VertexOutput_Bitangent;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D DiffuseLighting;
    uniform sampler2D SpecularLighting;
    uniform sampler2D BackgroundTexture;
    
    void main()
    {
        vec4  materialDiffuse    = Diffuse();
        vec3  materialEmissive   = Emissive();
        float materialShininess  = Shininess();
        vec3  materialRefraction = Refraction();
        
        float lightFactor        = materialDiffuse.a;                           // Just an alias to make things a little bit more readable.
        vec2  lightUV            = (VertexOutput_Position.xy / VertexOutput_Position.w) * 0.5 + 0.5;
        vec3  lightDiffuse       = texture2D(DiffuseLighting,  lightUV).rgb * lightFactor;
        vec3  lightSpecular      = texture2D(SpecularLighting, lightUV).rgb * lightFactor;
        
        
        
        // The diffuse needs to be modified to account for refraction.
        vec2 backgroundUV   = lightUV + (normalize(materialRefraction).xy * 0.01);
        
        ColourOut.rgb  = texture2D(BackgroundTexture, backgroundUV, 0).rgb * (1.0 - materialDiffuse.a);
        ColourOut.rgb += (materialDiffuse.rgb * lightDiffuse) + (materialShininess * lightSpecular) + materialEmissive;
    }
]]>
</shader>




<shader id="DefaultSceneRenderer_FinalCompositionVS">
<![CDATA[
    #version 330
    
    in vec3 VertexInput_Position;
    in vec2 VertexInput_TexCoord;

    out vec2 VertexOutput_TexCoord;
    
    void main()
    {
        VertexOutput_TexCoord = VertexInput_TexCoord;
        gl_Position           = vec4(VertexInput_Position, 1.0);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_FinalCompositionHDRFS">
<![CDATA[
    #version 330

    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D ColourBuffer;
    uniform sampler2D BloomBuffer;
    uniform float     Exposure;
    uniform float     BloomFactor;
    
    void main()
    {
        vec4  bloom     = texture2D(BloomBuffer, VertexOutput_TexCoord, 0);
        float luminance = dot(vec4(0.30, 0.59, 0.11, 0.0), texture2D(ColourBuffer, VertexOutput_TexCoord, 1000.0));
        
        
        // Bloom.
        ColourOut = (bloom * BloomFactor) + texture2D(ColourBuffer, VertexOutput_TexCoord);
        
        // Tone Mapping.
        ColourOut = ColourOut * min(5.0, Exposure * (Exposure / luminance + 1.0) / (Exposure + 1.0));
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_FinalCompositionHDRNoBloomFS">
<![CDATA[
    #version 330

    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D ColourBuffer;
    uniform float     Exposure;
    
    void main()
    {
        float luminance = dot(vec4(0.30, 0.59, 0.11, 0.0), texture2D(ColourBuffer, VertexOutput_TexCoord, 1000.0));

        // Tone Mapping.
        ColourOut = texture2D(ColourBuffer, VertexOutput_TexCoord) * min(5.0, Exposure * (Exposure / luminance + 1.0) / (Exposure + 1.0));
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_FinalCompositionLDRFS">
<![CDATA[
    #version 330

    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D ColourBuffer;
    
    void main()
    {
        ColourOut = texture2D(ColourBuffer, VertexOutput_TexCoord);
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_BloomFS">  <!-- Vertex Shader is DefaultSceneRenderer_FinalCompositionVS -->
<![CDATA[
    #version 330

    in vec2 VertexOutput_TexCoord;
    
    layout(location = 0) out vec4 ColourOut;
    
    uniform sampler2D ColourBuffer;
    
    void main()
    {
        ColourOut = max(vec4(0.0), texture2D(ColourBuffer, VertexOutput_TexCoord, 0) - vec4(1.0, 1.0, 1.0, 0.0));
    }
]]>
</shader>


















