
<!--
This files contains all of the shaders used by the default viewport renderer (DefaultViewportRenderer). This renderer uses
multiple passes to get the appropriate effects. There are two main passes: the lighting pass and the material pass. The
lighting pass is sometimes split into sub-passes in order to get the lighting fully correct.

The lighting pass retrieves all of the lighting information. The material pass retrieves the colours of the material
which will be combined with the lighting pass.

The lighting pass is done independantly of the material pass. The two passes will be combined as the last stage of the
rendering pipeline.

In this file, we use simple codes to identify the lights used in a particular shader. We do this to avoid overly long IDs.

    A - Ambient light.
    D - Directional light.
    P - Point light.
    S - Spot light.
    
Each code is followed by a number to indicate how many of those lights are used in the shader. For example, if the shader
uses 1 or each light, it will use the following: A1D1P1.
-->

<shader id="Engine_FragmentInput">
    varying vec2 VertexOutput_TexCoord;
    varying vec3 VertexOutput_Normal;
    varying vec3 VertexOutput_Tangent;
    varying vec3 VertexOutput_Bitangent;
    varying vec4 VertexOutput_Position;
</shader>

<shader id="Engine_FragmentLightingOutput">
    void DoFinalLightingOutput(vec3 diffuse, vec3 specular)
    {
        gl_FragData[0].rgb = diffuse;
        gl_FragData[0].a   = 0.0;
        
        gl_FragData[1].rgb = specular;
        gl_FragData[1].a   = 0.0;
    }
</shader>

<shader id="Engine_FragmentLightingUniforms">
    uniform sampler2D Lighting_Normals;
    uniform vec2      ScreenSize;
</shader>

<shader id="Engine_LightingUtils">
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
</shader>

<shader id="Engine_AmbientLight">
    struct AmbientLight
    {
        vec3 Colour;
    };
    
    void CalculateAmbientLighting(AmbientLight light, inout vec3 diffuseOut, inout vec3 specularOut)
    {
        diffuseOut  += light.Colour;
        specularOut += vec3(0.0, 0.0, 0.0);
    }
</shader>

<shader id="Engine_DirectionalLight">
    struct DirectionalLight
    {
        vec3 Colour;
        vec3 Direction;
    };
    
    void CalculateDirectionalLighting(DirectionalLight light, inout vec3 diffuseOut, inout vec3 specularOut)
    {
        vec2 fragCoord = gl_FragCoord.xy / ScreenSize;
    
        vec3 N = texture2D(Lighting_Normals, fragCoord).rgb;
        vec3 L = -light.Direction;
        vec3 H = normalize(L - normalize(VertexOutput_Position.xyz));
        
        float diffuse  = DiffuseFactor(N, L);
        float specular = SpecularFactor(N, H, 64.0);
            
        diffuseOut  += light.Colour * diffuse;
        specularOut += light.Colour * specular;
    }
</shader>

<shader id="Engine_PointLight">
    struct PointLight
    {
        vec3 Colour;
        vec3 Position;
        
        float ConstantAttenuation;
        float LinearAttenuation;
        float QuadraticAttenuation;
    };

    void CalculatePointLighting(PointLight light, inout vec3 diffuseOut, inout vec3 specularOut)
    {
        // N - Input normal
        // L - Light vector from the light to the vertex
        
        vec2 fragCoord = gl_FragCoord.xy / ScreenSize;
        
        vec3 N = texture2D(Lighting_Normals, fragCoord).rgb;
        vec3 L = light.Position - VertexOutput_Position.xyz;
        vec3 H = normalize(normalize(L) - normalize(VertexOutput_Position.xyz));

        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, 64.0);
        float attenuation = AttenuationFactor(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, length(L));

        diffuseOut  += light.Colour * diffuse  * attenuation;
        specularOut += light.Colour * specular * attenuation;
    }
</shader>

<shader id="Engine_SpotLight">
    struct SpotLight
    {
        vec3  Colour;
        vec3  Position;
        vec3  Direction;
        float CosAngleInner;               // cos(radians(degrees))
        float CosAngleOuter;               // cos(radians(degrees))
        
        float ConstantAttenuation;
        float LinearAttenuation;
        float QuadraticAttenuation;
    };
    
    void CalculateSpotLighting(SpotLight light, inout vec3 diffuseOut, inout vec3 specularOut)
    {
        // N - Input normal
        // L - Non-normalized light vector from the light to the vertex
        
        vec2 fragCoord = gl_FragCoord.xy / ScreenSize;
        
        vec3 N     = texture2D(Lighting_Normals, fragCoord).rgb;
        vec3 L     = light.Position - VertexOutput_Position.xyz;
        vec3 H     = normalize(normalize(L) - normalize(VertexOutput_Position.xyz));
        
        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, 64.0);
        float attenuation = AttenuationFactor(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, length(L));
        float spot        = SpotFactor(L, light.Direction, light.CosAngleInner, light.CosAngleOuter);
            
        diffuseOut  += light.Colour * diffuse  * attenuation * spot;
        specularOut += light.Colour * specular * attenuation * spot;
    }
</shader>







<!-- *** Lighting Pass Shaders *** -->
<shader id="Engine_LightingPass_A1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_LightingUtils" />
    <include url="#Engine_AmbientLight" />
    
    <include>
        uniform AmbientLight ALights0;
        
	    void main()
	    {
            vec3 diffuse  = vec3(0.0, 0.0, 0.0);
            vec3 specular = vec3(0.0, 0.0, 0.0);
            CalculateAmbientLighting(ALights0, diffuse, specular);
            
            DoFinalLightingOutput(diffuse, specular);
	    }
    </include>
</shader>

<shader id="Engine_LightingPass_D1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_LightingUtils" />
    <include url="#Engine_DirectionalLight" />
    
    <include>
        uniform DirectionalLight DLights0;
        
	    void main()
	    {
            vec3 diffuse  = vec3(0.0, 0.0, 0.0);
            vec3 specular = vec3(0.0, 0.0, 0.0);
            CalculateDirectionalLighting(DLights0, diffuse, specular);

		    DoFinalLightingOutput(diffuse, specular);
	    }
    </include>
</shader>

<shader id="Engine_LightingPass_P1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_LightingUtils" />
    <include url="#Engine_PointLight" />
    
    <include>
        uniform PointLight PLights0;
        
	    void main()
	    {
            vec3 diffuse  = vec3(0.0, 0.0, 0.0);
            vec3 specular = vec3(0.0, 0.0, 0.0);
            CalculatePointLighting(PLights0, diffuse, specular);
            
		    DoFinalLightingOutput(diffuse, specular);
	    }
    </include>
</shader>

<shader id="Engine_LightingPass_S1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_LightingUtils" />
    <include url="#Engine_SpotLight" />
    
    <include>
        uniform SpotLight SLights0;
        
	    void main()
	    {
            vec3 diffuse  = vec3(0.0, 0.0, 0.0);
            vec3 specular = vec3(0.0, 0.0, 0.0);
            CalculateSpotLighting(SLights0, diffuse, specular);
            
		    DoFinalLightingOutput(diffuse, specular);
	    }
    </include>
</shader>


<shader id="Engine_LightingPass_A1D1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_LightingUtils" />
    <include url="#Engine_AmbientLight" />
    <include url="#Engine_DirectionalLight" />
    
    <include>
        uniform AmbientLight     ALights0;
        uniform DirectionalLight DLights0;
        
	    void main()
	    {
            vec3 diffuse  = ALights0.Colour;
            vec3 specular = vec3(0.0, 0.0, 0.0);
            CalculateDirectionalLighting(DLights0, diffuse, specular);
            
            DoFinalLightingOutput(diffuse, specular);
	    }
    </include>
</shader>

<shader id="Engine_LightingPass_A1P1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_LightingUtils" />
    <include url="#Engine_AmbientLight" />
    <include url="#Engine_PointLight" />
    
    <include>
        uniform AmbientLight ALights0;
        uniform PointLight   PLights0;
        
	    void main()
	    {
            vec3 diffuse  = ALights0.Colour;
            vec3 specular = vec3(0.0, 0.0, 0.0);
            CalculatePointLighting(PLights0, diffuse, specular);

		    DoFinalLightingOutput(diffuse, specular);
	    }
    </include>
</shader>


<!-- *** Material Pass Shaders *** -->
<shader id="Engine_MaterialPass_VS">
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
    
    uniform mat4 MVPMatrix;
    uniform mat3 NormalMatrix;
    
    void main()
    {
        VertexOutput_TexCoord  = VertexInput_TexCoord;
        VertexOutput_Normal    = normalize(NormalMatrix * VertexInput_Normal);
        VertexOutput_Tangent   = normalize(NormalMatrix * VertexInput_Tangent);
        VertexOutput_Bitangent = normalize(NormalMatrix * VertexInput_Bitangent);
        VertexOutput_Position  = MVPMatrix * vec4(VertexInput_Position, 1.0);
        
        gl_Position = VertexOutput_Position;
    }
</shader>

<shader id="Engine_MaterialPass_FS">
    <include url="#Engine_FragmentInput" />
    
    <include>
        vec3  Diffuse();
        vec3  Emissive();
        float Shininess();
        vec3  Normal();
        
        void main()
        {
            vec3  materialDiffuse   = Diffuse();
            vec3  materialEmissive  = Emissive();
            float materialShininess = Shininess();
            vec3  materialNormal    = Normal();
            
            gl_FragData[0].rgb = materialDiffuse;
            gl_FragData[0].a   = 1.0;
            
            gl_FragData[1].rgb = materialEmissive;
            gl_FragData[1].a   = materialShininess;
            
            gl_FragData[2].rgb = normalize(mat3(VertexOutput_Tangent, VertexOutput_Bitangent, VertexOutput_Normal) * materialNormal);
            gl_FragData[2].a   = 1.0;
        }
    </include>
</shader>


<!-- *** Combiner Shader *** -->
<shader id="Engine_LightingMaterialCombiner">
    varying vec2 VertexOutput_TexCoord;
    
    uniform sampler2D LightingBuffer0;      // rgb = diffuse;  a = nothing
    uniform sampler2D LightingBuffer1;      // rgb = specular; a = nothing
    uniform sampler2D MaterialBuffer0;      // rgb = diffuse;  a = transparancy
    uniform sampler2D MaterialBuffer1;      // rgb = emissive; a = shininess
    
    void main()
    {
        vec4 lightingTexel0 = texture2D(LightingBuffer0, VertexOutput_TexCoord);
        vec4 lightingTexel1 = texture2D(LightingBuffer1, VertexOutput_TexCoord);
        vec4 materialTexel0 = texture2D(MaterialBuffer0, VertexOutput_TexCoord);
        vec4 materialTexel1 = texture2D(MaterialBuffer1, VertexOutput_TexCoord);
        
        vec3 lightDiffuse  = lightingTexel0.rgb;
        vec3 lightSpecular = lightingTexel1.rgb;
        
        vec3  materialDiffuse      = materialTexel0.rgb;
        float materialTransparency = materialTexel0.a;
        vec3  materialEmissive     = materialTexel1.rgb;
        float materialShininess    = materialTexel1.a;
        
        
        gl_FragData[0].rgb = (materialDiffuse * lightDiffuse) + (materialShininess * lightSpecular) + materialEmissive;
        gl_FragData[0].a   = 1.0;
        
        gl_FragData[0].rgb = pow(gl_FragData[0].rgb, vec3(1.0 / 2.2));   // sRGB (approx.)
        
        
        //gl_FragData[0] = vec4(materialDiffuse, 1.0);
    }
</shader>

