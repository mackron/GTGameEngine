
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
        ColourOut = vec4(1.0, 0.5, 0.0, 0.1);
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
        //float dx    = dFdx(depth);
        //float dy    = dFdy(depth);
        
        //ColourOut = vec4(depth, (depth * depth) + (0.25 * ((dx * dx) + (dy * dy))), 0.0, 1.0);
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
        float angle = dot(normalize(-L), lightDirection);
        return clamp((angle - cosAngleOuter) / (cosAngleInner - cosAngleOuter), 0.0, 1.0);
    }
    
    float AttenuationFactor(float c, float l, float q, float d)     // d = distance
    {
        return 1.0 / (c + (l * d) + (q * d * d));
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

        diffuseOut  += Colour * diffuse;
        specularOut += Colour * specular;
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
    
    
    float CalculateShadowBasic(vec2 shadowUV, float fragmentDepth)
    {
        float bias        = 0.00001;
        float shadowDepth = texture2D(ShadowMap, shadowUV).r + bias;

        return step(fragmentDepth, shadowDepth);
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
    
    
        //return CalculateShadowBasic(shadowUV, fragmentDepth);
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

        diffuseOut  += Colour * diffuse  * shadow;
        specularOut += Colour * specular * shadow;
    }
]]>
</shader>




<shader id="DefaultSceneRenderer_PointLight">
<![CDATA[
    uniform vec3  Colour;
    uniform vec3  Position;
    uniform float ConstantAttenuation;
    uniform float LinearAttenuation;
    uniform float QuadraticAttenuation;
    
    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut)
    {
        // N - Input normal
        // L - Light vector from the light to the vertex
        
        vec3 N = normal;
        vec3 L = Position - VertexOutput_PositionVS.xyz;
        vec3 H = normalize(normalize(L) - normalize(VertexOutput_PositionVS.xyz));

        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, 64.0f);
        float attenuation = AttenuationFactor(ConstantAttenuation, LinearAttenuation, QuadraticAttenuation, length(L));

        diffuseOut  = Colour * diffuse  * attenuation;
        specularOut = Colour * specular * attenuation;
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_ShadowPointLight">
<![CDATA[
    uniform vec3  Colour;
    uniform vec3  Position;
    uniform float ConstantAttenuation;
    uniform float LinearAttenuation;
    uniform float QuadraticAttenuation;
    
    uniform samplerCube ShadowMap;
    uniform vec3        PositionWS;
    
    float linstep(float low, float high, float v)
    {
        return clamp((v - low) / (high - low), 0.0, 1.0);
    }
    
    
    float CalculateShadowBasic(vec3 shadowCoord, float fragmentDepth)
    {
        float bias        = 0.02;
        float shadowDepth = texture(ShadowMap, shadowCoord).r + bias;

        return step(fragmentDepth, shadowDepth);
    }

    float CalculateShadowVSM(vec3 shadowCoord, float fragmentDepth)
    {
        float bias     = 0.1;       // This can affect seams. Lower value = more seams.
        vec2  moments  = texture(ShadowMap, shadowCoord).xy;
        float variance = moments.y - (moments.x * moments.x);
        float d        = fragmentDepth - moments.x;
        float p        = smoothstep(fragmentDepth - bias, fragmentDepth, moments.x);
        float pMax     = linstep(0.2, 1.0, variance / (variance + (d * d)));
        
        return clamp(max(p, pMax), 0.0, 1.0);
    }
    
    
    
    float CalculateShadow()
    {
        vec3 shadowCoord = VertexOutput_PositionWS.xyz - PositionWS;
        shadowCoord.y = -shadowCoord.y;
        shadowCoord.z = -shadowCoord.z;
        
        float fragmentDepth = length(shadowCoord);
        
        //return CalculateShadowBasic(shadowCoord, fragmentDepth);
        return CalculateShadowVSM(shadowCoord, fragmentDepth);
    }
    
    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut)
    {
        // N - Input normal
        // L - Light vector from the light to the vertex
        
        vec3 N = normal;
        vec3 L = Position - VertexOutput_PositionVS.xyz;
        vec3 H = normalize(normalize(L) - normalize(VertexOutput_PositionVS.xyz));

        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, 64.0f);
        float attenuation = AttenuationFactor(ConstantAttenuation, LinearAttenuation, QuadraticAttenuation, length(L));
        float shadow      = CalculateShadow();

        diffuseOut  = Colour * diffuse  * attenuation * shadow;
        specularOut = Colour * specular * attenuation * shadow;
    }
]]>
</shader>


<shader id="DefaultSceneRenderer_SpotLight">
<![CDATA[
    uniform vec3  Colour;
    uniform vec3  Position;
    uniform vec3  Direction;
    uniform float ConstantAttenuation;
    uniform float LinearAttenuation;
    uniform float QuadraticAttenuation;
    uniform float CosAngleInner;
    uniform float CosAngleOuter;
    
    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut)
    {
        vec3 N = normal;
        vec3 L = Position - VertexOutput_PositionVS.xyz;
        vec3 H = normalize(normalize(L) - normalize(VertexOutput_PositionVS.xyz));
        
        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, 64.0f);
        float attenuation = AttenuationFactor(ConstantAttenuation, LinearAttenuation, QuadraticAttenuation, length(L));
        float spot        = SpotFactor(L, Direction, CosAngleInner, CosAngleOuter);
            
        diffuseOut  = Colour * diffuse  * attenuation * spot;
        specularOut = Colour * specular * attenuation * spot;
    }
]]>
</shader>

<shader id="DefaultSceneRenderer_ShadowSpotLight">
<![CDATA[
    uniform vec3      Colour;
    uniform vec3      Position;
    uniform vec3      Direction;
    uniform float     ConstantAttenuation;
    uniform float     LinearAttenuation;
    uniform float     QuadraticAttenuation;
    uniform float     CosAngleInner;
    uniform float     CosAngleOuter;
    
    
    uniform sampler2D ShadowMap;
    
    float linstep(float low, float high, float v)
    {
        return clamp((v - low) / (high - low), 0.0, 1.0);
    }
    
    
    float CalculateShadowBasic(vec2 shadowUV, float fragmentDepth)
    {
        float bias        = 0.001;
        float shadowDepth = texture2D(ShadowMap, shadowUV).r + bias;

        return step(fragmentDepth, shadowDepth);
    }
    
    float CalculateShadowVSM(vec2 shadowUV, float fragmentDepth)
    {
        float bias     = 0.001;
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
    
    
        //return CalculateShadowBasic(shadowUV, fragmentDepth);
        return CalculateShadowVSM(shadowUV, fragmentDepth);
    }
    
    
    void DoLighting(in vec3 normal, out vec3 diffuseOut, out vec3 specularOut)
    {
        vec3 N = normal;
        vec3 L = Position - VertexOutput_PositionVS.xyz;
        vec3 H = normalize(normalize(L) - normalize(VertexOutput_PositionVS.xyz));
        
        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, 64.0f);
        float attenuation = AttenuationFactor(ConstantAttenuation, LinearAttenuation, QuadraticAttenuation, length(L));
        float spot        = SpotFactor(L, Direction, CosAngleInner, CosAngleOuter);
        float shadow      = CalculateShadow();
        
            
        diffuseOut  = Colour * diffuse  * attenuation * spot * shadow;
        specularOut = Colour * specular * attenuation * spot * shadow;
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
        
        vec2  lightUV           = (VertexOutput_Position.xy / VertexOutput_Position.w) * 0.5 + 0.5;     // <-- Can this compuation be done in the vertex shader?
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
        vec2  lightUV            = (VertexOutput_Position.xy / VertexOutput_Position.w) * 0.5 + 0.5;     // <-- Can this compuation be done in the vertex shader?
        vec3  lightDiffuse       = texture2D(DiffuseLighting,  lightUV).rgb * lightFactor;
        vec3  lightSpecular      = texture2D(SpecularLighting, lightUV).rgb * lightFactor;
        
        
        
        // The diffuse needs to be modified to account for refraction.
        vec2 backgroundUV   = ((VertexOutput_Position.xy / VertexOutput_Position.w) * 0.5 + 0.5) + (normalize(materialRefraction).xy * 0.01);
        
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
        vec4  finalColour = texture2D(ColourBuffer, VertexOutput_TexCoord);
        vec4  bloom       = vec4(0.0);
        float luminance   = dot(vec4(0.30, 0.59, 0.11, 0.0), texture2D(ColourBuffer, VertexOutput_TexCoord, 1000.0));

        vec4 bloom0 = texture2D(BloomBuffer, VertexOutput_TexCoord, 1);
        vec4 bloom1 = texture2D(BloomBuffer, VertexOutput_TexCoord, 2);
        vec4 bloom2 = texture2D(BloomBuffer, VertexOutput_TexCoord, 3);
        bloom = bloom0 + bloom1 + bloom2;
        
        
        // Bloom.
        finalColour += bloom * BloomFactor;
        
        // Tone Mapping.
        finalColour *= Exposure * (Exposure / luminance + 1.0) / (Exposure + 1.0);
        
        ColourOut = finalColour;
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
        vec4  finalColour = texture2D(ColourBuffer, VertexOutput_TexCoord);
        float luminance   = dot(vec4(0.30, 0.59, 0.11, 0.0), texture2D(ColourBuffer, VertexOutput_TexCoord, 1000.0));

        // Tone Mapping.
        finalColour *= Exposure * (Exposure / luminance + 1.0) / (Exposure + 1.0);
        
        ColourOut = finalColour;
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
        ColourOut = max(vec4(0.0), texture2D(ColourBuffer, VertexOutput_TexCoord, 0) - vec4(1.0));
    }
]]>
</shader>

















