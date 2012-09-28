
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
    varying vec4 VertexOutput_PositionWS;
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

<shader id="Engine_DirectionalLight_NoShadow">
    struct DirectionalLight
    {
        vec3 Colour;
        vec3 Direction;
    };
    
    void CalculateDirectionalLighting(DirectionalLight light, inout vec3 diffuseOut, inout vec3 specularOut)
    {
        vec2 fragCoord = gl_FragCoord.xy / ScreenSize;
        
        vec4  texel         = texture2D(Lighting_Normals, fragCoord);
        vec3  normal        = texel.rgb;
        float specularPower = texel.a;
    
        vec3 N = normal;
        vec3 L = -light.Direction;
        vec3 H = normalize(L - normalize(VertexOutput_Position.xyz));
        
        float diffuse  = DiffuseFactor(N, L);
        float specular = SpecularFactor(N, H, specularPower);

        diffuseOut  += light.Colour * diffuse;
        specularOut += light.Colour * specular;
    }
</shader>

<shader id="Engine_DirectionalLight">
<![CDATA[
    struct DirectionalLight
    {
        vec3 Colour;
        vec3 Direction;
    };
    
    varying vec4 VertexOutput_ShadowCoord;
    uniform sampler2D ShadowMap;
    
    void CalculateDirectionalLighting(DirectionalLight light, inout vec3 diffuseOut, inout vec3 specularOut)
    {
        vec2 fragCoord = gl_FragCoord.xy / ScreenSize;
    
        vec4  texel         = texture2D(Lighting_Normals, fragCoord);
        vec3  normal        = texel.rgb;
        float specularPower = texel.a;
    
        vec3 N = normal;
        vec3 L = -light.Direction;
        vec3 H = normalize(L - normalize(VertexOutput_Position.xyz));
        
        float diffuse  = DiffuseFactor(N, L);
        float specular = SpecularFactor(N, H, specularPower);
        
        
        vec4  shadowCoord = VertexOutput_ShadowCoord / VertexOutput_ShadowCoord.w;
        float shadowDepth = shadowCoord.z - 0.00001;
        //float pixelDepth  = texture2D(ShadowMap, shadowCoord.xy * 0.5 + 0.5).r;
        
        //float shadow = (pixelDepth < shadowDepth) ? 0.0 : 1.0;
        
        
        float shadow = 0.0;
        
        vec2 filter[8];
        filter[0] = vec2( 0.0005,  0.0005);
        filter[1] = vec2( 0.0005, -0.0005);
        filter[2] = vec2(-0.0005,  0.0005);
        filter[3] = vec2(-0.0005, -0.0005);
        filter[4] = vec2( 0.00025,  0.00025);
        filter[5] = vec2( 0.00025, -0.00025);
        filter[6] = vec2(-0.00025,  0.00025);
        filter[7] = vec2(-0.00025, -0.00025);
        
        for (int i = 0; i < 8; ++i)
        {
            if (texture2D(ShadowMap, (shadowCoord.xy * 0.5 + 0.5) + filter[i]).r > shadowDepth)
            {
                shadow += 0.125;
            }
        }
        
        
        diffuseOut  += light.Colour * diffuse  * shadow;
        specularOut += light.Colour * specular * shadow;
    }
]]>
</shader>

<shader id="Engine_PointLight_NoShadow">
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
        
        vec4  texel         = texture2D(Lighting_Normals, fragCoord);
        vec3  normal        = texel.rgb;
        float specularPower = texel.a;
        
        vec3 N = normal;
        vec3 L = light.Position - VertexOutput_Position.xyz;
        vec3 H = normalize(normalize(L) - normalize(VertexOutput_Position.xyz));

        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, specularPower);
        float attenuation = AttenuationFactor(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, length(L));

        diffuseOut  += light.Colour * diffuse  * attenuation;
        specularOut += light.Colour * specular * attenuation;
    }
</shader>

<shader id="Engine_PointLight">
<![CDATA[
    struct PointLight
    {
        vec3 Colour;
        vec3 Position;
        
        float ConstantAttenuation;
        float LinearAttenuation;
        float QuadraticAttenuation;
    };
    
    uniform samplerCube ShadowMap;
    uniform vec3        PLight0_PositionWS;

    void CalculatePointLighting(PointLight light, inout vec3 diffuseOut, inout vec3 specularOut)
    {
        // N - Input normal
        // L - Light vector from the light to the vertex
        
        vec2 fragCoord = gl_FragCoord.xy / ScreenSize;
        
        vec4  texel         = texture2D(Lighting_Normals, fragCoord);
        vec3  normal        = texel.rgb;
        float specularPower = texel.a;
        
        vec3 N = normal;
        vec3 L = light.Position - VertexOutput_Position.xyz;
        vec3 H = normalize(normalize(L) - normalize(VertexOutput_Position.xyz));

        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, specularPower);
        float attenuation = AttenuationFactor(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, length(L));
        
        
        // This is the shadow calculation. This is messed up at the moment because we have a strange shadow map texture coordinate vector. This
        // is probably due to incorrect cube map generation which I'll look into later.
        vec3 lightToPixel = VertexOutput_PositionWS.xyz - PLight0_PositionWS;


        float pixelDepth = length(lightToPixel);
        pixelDepth -= (0.02 + pixelDepth * 0.005);
         
        vec3 filter[8];
        filter[0] = vec3( 0.0006,  0.0006,  0.0006) * pixelDepth;
        filter[1] = vec3( 0.0006, -0.0006,  0.0006) * pixelDepth;
        filter[2] = vec3(-0.0006,  0.0006, -0.0006) * pixelDepth;
        filter[3] = vec3(-0.0006, -0.0006, -0.0006) * pixelDepth;
        
        filter[4] = vec3( 0.0003,  0.0003,  0.0003) * pixelDepth;
        filter[5] = vec3( 0.0003, -0.0003,  0.0003) * pixelDepth;
        filter[6] = vec3(-0.0003,  0.0003, -0.0003) * pixelDepth;
        filter[7] = vec3(-0.0003, -0.0003, -0.0003) * pixelDepth;


        float shadow = 0.0;
        for (int i = 0; i < 8; ++i)
        {
            float shadowDepth  = textureCube(ShadowMap, vec3(lightToPixel.x, -lightToPixel.y, -lightToPixel.z) + filter[i]).r;
            shadow            += (pixelDepth < shadowDepth) ? 0.125 : 0.0;
        }
        
        diffuseOut  += light.Colour * diffuse  * attenuation * shadow;
        specularOut += light.Colour * specular * attenuation * shadow;
    }
]]>
</shader>

<shader id="Engine_SpotLight_NoShadow">
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
        
        vec4  texel         = texture2D(Lighting_Normals, fragCoord);
        vec3  normal        = texel.rgb;
        float specularPower = texel.a;
        
        vec3 N = normal;
        vec3 L = light.Position - VertexOutput_Position.xyz;
        vec3 H = normalize(normalize(L) - normalize(VertexOutput_Position.xyz));
        
        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, specularPower);
        float attenuation = AttenuationFactor(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, length(L));
        float spot        = SpotFactor(L, light.Direction, light.CosAngleInner, light.CosAngleOuter);
            
        diffuseOut  += light.Colour * diffuse  * attenuation * spot;
        specularOut += light.Colour * specular * attenuation * spot;
    }
</shader>

<shader id="Engine_SpotLight">
<![CDATA[
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
    
    varying vec4 VertexOutput_ShadowCoord;
    uniform sampler2D ShadowMap;
    
    void CalculateSpotLighting(SpotLight light, inout vec3 diffuseOut, inout vec3 specularOut)
    {
        // N - Input normal
        // L - Non-normalized light vector from the light to the vertex
        
        vec2 fragCoord = gl_FragCoord.xy / ScreenSize;
        
        vec4  texel         = texture2D(Lighting_Normals, fragCoord);
        vec3  normal        = texel.rgb;
        float specularPower = texel.a;
        
        vec3 N = normal;
        vec3 L = light.Position - VertexOutput_Position.xyz;
        vec3 H = normalize(normalize(L) - normalize(VertexOutput_Position.xyz));
        
        float diffuse     = DiffuseFactor(N, normalize(L));
        float specular    = SpecularFactor(N, H, specularPower);
        float attenuation = AttenuationFactor(light.ConstantAttenuation, light.LinearAttenuation, light.QuadraticAttenuation, length(L));
        float spot        = SpotFactor(L, light.Direction, light.CosAngleInner, light.CosAngleOuter);
        
        
        vec4  shadowCoord = VertexOutput_ShadowCoord / VertexOutput_ShadowCoord.w;
        float shadowDepth = shadowCoord.z - 0.0002;
        //float pixelDepth  = texture2D(ShadowMap, shadowCoord.xy * 0.5 + 0.5).r;
        
        
        
        float shadow = 1.0;
        if (VertexOutput_ShadowCoord.w > 0.0)
        {
            shadow = 0.0;
            
            vec2 filter[8];
            filter[0] = vec2( 0.0005,  0.0005);
            filter[1] = vec2( 0.0005, -0.0005);
            filter[2] = vec2(-0.0005,  0.0005);
            filter[3] = vec2(-0.0005, -0.0005);
            filter[4] = vec2( 0.00025,  0.00025);
            filter[5] = vec2( 0.00025, -0.00025);
            filter[6] = vec2(-0.00025,  0.00025);
            filter[7] = vec2(-0.00025, -0.00025);
            
            for (int i = 0; i < 8; ++i)
            {
                if (texture2D(ShadowMap, (shadowCoord.xy * 0.5 + 0.5) + filter[i]).r > shadowDepth)
                {
                    shadow += 0.125;
                }
            }
        
            //shadow = (pixelDepth < shadowDepth) ? 0.0 : 1.0;
        }
        

        diffuseOut  += light.Colour * diffuse  * attenuation * spot * shadow;
        specularOut += light.Colour * specular * attenuation * spot * shadow;
    }
]]>
</shader>







<!-- *** Lighting Pass Shaders *** -->
<shader id="Engine_LightingPass_NoShadow_A1">
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

<shader id="Engine_LightingPass_NoShadow_D1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_LightingUtils" />
    <include url="#Engine_DirectionalLight_NoShadow" />
    
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


<shader id="Engine_LightingPass_NoShadow_P1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_LightingUtils" />
    <include url="#Engine_PointLight_NoShadow" />
    
    <include>
        uniform PointLight PLight0;
        
	    void main()
	    {
            vec3 diffuse  = vec3(0.0, 0.0, 0.0);
            vec3 specular = vec3(0.0, 0.0, 0.0);
            CalculatePointLighting(PLight0, diffuse, specular);
            
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
        uniform PointLight PLight0;
        
	    void main()
	    {
            vec3 diffuse  = vec3(0.0, 0.0, 0.0);
            vec3 specular = vec3(0.0, 0.0, 0.0);
            CalculatePointLighting(PLight0, diffuse, specular);
            
		    DoFinalLightingOutput(diffuse, specular);
	    }
    </include>
</shader>


<shader id="Engine_LightingPass_NoShadow_S1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_LightingUtils" />
    <include url="#Engine_SpotLight_NoShadow" />
    
    <include>
        uniform SpotLight SLight0;
        
	    void main()
	    {
            vec3 diffuse  = vec3(0.0, 0.0, 0.0);
            vec3 specular = vec3(0.0, 0.0, 0.0);
            CalculateSpotLighting(SLight0, diffuse, specular);
            
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
        uniform SpotLight SLight0;
        
	    void main()
	    {
            vec3 diffuse  = vec3(0.0, 0.0, 0.0);
            vec3 specular = vec3(0.0, 0.0, 0.0);
            CalculateSpotLighting(SLight0, diffuse, specular);
            
		    DoFinalLightingOutput(diffuse, specular);
	    }
    </include>
</shader>


<shader id="Engine_LightingPass_NoShadow_A1D1">
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

<shader id="Engine_LightingPass_NoShadow_A1P1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_LightingUtils" />
    <include url="#Engine_AmbientLight" />
    <include url="#Engine_PointLight_NoShadow" />
    
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


<shader id="Engine_LightingPass_ColourClear">
    void main()
    {
        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);
    }
</shader>



<!-- *** Shadow Map Shaders *** -->
<shader id="Engine_LightingPass_ShadowMap">
    <include url="#Engine_FragmentInput" />
    
    <include>
        varying vec4 ShadowCoord;
    
        void main()
        {
            gl_FragData[0].r = ShadowCoord.z / ShadowCoord.w;
        }
    </include>
</shader>

<shader id="Engine_LightingPass_PointLightShadowMap">
    <include url="#Engine_FragmentInput" />
    
    <include>
        void main()
        {
            gl_FragData[0].r = length(VertexOutput_Position.xyz);
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
        vec4  Diffuse();
        vec3  Emissive();
        float Shininess();
        vec3  Normal();
        float Specular();
        
        void main()
        {
            vec4  materialDiffuse   = Diffuse();
            vec3  materialEmissive  = Emissive();
            float materialShininess = Shininess();
            vec3  materialNormal    = Normal();
            float materialSpecular  = Specular();
            

            gl_FragData[0].rgba = materialDiffuse;
            
            gl_FragData[1].rgb = materialEmissive;
            gl_FragData[1].a   = materialShininess;
            
            gl_FragData[2].rgb = normalize(mat3(VertexOutput_Tangent, VertexOutput_Bitangent, VertexOutput_Normal) * materialNormal);
            gl_FragData[2].a   = materialSpecular;
        }
    </include>
</shader>

<shader id="Engine_MaterialPass_Refraction_FS">
    <include url="#Engine_FragmentInput" />
    
    <include>
        vec4  Diffuse();
        vec3  Emissive();
        float Shininess();
        vec3  Normal();
        float Specular();
        vec3  Refraction();
        
        uniform sampler2D BackgroundTexture;
        
        void main()
        {
            vec4  materialDiffuse    = Diffuse();
            vec3  materialEmissive   = Emissive();
            float materialShininess  = Shininess();
            vec3  materialNormal     = Normal();
            float materialSpecular   = Specular();
            vec3  materialRefraction = Refraction();
            
            vec2 backgroundUV = ((VertexOutput_Position.xy / VertexOutput_Position.w) * 0.5 + 0.5) + (normalize(materialRefraction).xy * 0.01);
            
            gl_FragData[0].rgb = (materialDiffuse.rgb * materialDiffuse.a) + (texture2D(BackgroundTexture, backgroundUV).rgb * (1.0 - materialDiffuse.a));
            gl_FragData[0].a   = materialDiffuse.a;

            gl_FragData[1].rgb = materialEmissive + (texture2D(BackgroundTexture, backgroundUV).rgb * (1.0 - materialDiffuse.a));
            gl_FragData[1].a   = materialShininess;
            
            gl_FragData[2].rgb = normalize(mat3(VertexOutput_Tangent, VertexOutput_Bitangent, VertexOutput_Normal) * materialNormal);
            gl_FragData[2].a   = materialSpecular;
            
            
            // Lighting needs to be cleared to black.
            gl_FragData[3].rgba = vec4(0.0, 0.0, 0.0, 1.0);
            gl_FragData[4].rgba = vec4(0.0, 0.0, 0.0, 1.0);
        }
    </include>
</shader>


<shader id="Engine_MaterialPass_ClearBackground">
    uniform vec3 ClearColour;
    
    void main()
    {
        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        gl_FragData[1] = vec4(ClearColour,   1.0);
        gl_FragData[2] = vec4(0.0, 0.0, 1.0, 0.0);
    }
</shader>


<!-- *** Compositing Shaders *** -->
<shader id="Engine_Compositor_DiffuseOnly">
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
        
        gl_FragData[0] = vec4(materialDiffuse, materialTransparency);
    }
</shader>

<shader id="Engine_Compositor_DiffuseLightingOnly">
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
        
        gl_FragData[0] = vec4(lightDiffuse, 1.0);
    }
</shader>

<shader id="Engine_Compositor_NormalsOnly">
    varying vec2 VertexOutput_TexCoord;
    
    uniform sampler2D MaterialBuffer2;      // rgb = normal; a = nothing
    
    void main()
    {
        vec4 materialTexel2 = texture2D(MaterialBuffer2, VertexOutput_TexCoord);
        
        gl_FragData[0] = vec4(materialTexel2.rgb, 1.0);
    }
</shader>

<shader id="Engine_Compositor_OpaqueFinalOutput">
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
        gl_FragData[0].a   = materialTransparency;
    }
</shader>

<shader id="Engine_Compositor_FinalOutput">
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
        
        
        float lightFactor = materialTransparency;       // Just an alias to make things a little bit more readable.

        gl_FragData[0].rgb = (materialDiffuse * lightDiffuse * lightFactor) + (materialShininess * lightSpecular * lightFactor) + materialEmissive;
        gl_FragData[0].a   = materialTransparency;
        
        //gl_FragData[0].rgb = pow(gl_FragData[0].rgb, vec3(1.0 / 2.2));   // sRGB (approx.)
    }
</shader>


