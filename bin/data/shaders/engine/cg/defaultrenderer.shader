
<!--
This files contains all of the shaders used by the default viewport renderer (DefaultViewportRenderer). This renderer uses
multiple passes to get the appropriate effects. There are two main passes, each of which are split into sub-passes: the
lighting pass and the material pass.

The lighting pass retrieves all of the lighting information. The material pass retrieves the colours of the material
which will be combined with the lighting pass.

The lighting pass is done independantly of the material pass. The two passes will be combined as the last stage of the
rendering pipeline.

In this file, we use simple codes to identify the lights used in a particular shader. We do this to avoid overly long IDs.

    A - Ambient light.
    D - Directional light.
    P - Point light.
    
Each code is followed by a number to indicate how many of those lights are used in the shader. For example, if the shader
uses 1 or each light, it will use the following: A1D1P1.
-->

<shader id="Engine_FragmentInput">
    struct FragmentInput
	{
		float2 TexCoord       : TEXCOORD0;
		float3 Normal         : TEXCOORD1;
        float4 Position       : TEXCOORD2;
        float4 WindowPosition : WPOS;
	};
    in FragmentInput IN;
</shader>

<shader id="Engine_FragmentLightingOutput">
    struct FragmentOutput
	{
		float4 Color0 : COLOR0;
        float4 Color1 : COLOR1;
	};
</shader>

<shader id="Engine_FragmentLightingUniforms">
    uniform sampler2D Lighting_Diffuse;
    uniform sampler2D Lighting_Specular;
    uniform float2    ScreenSize;
</shader>

<shader id="Engine_AmbientLight">
    struct AmbientLight
    {
        float3 Colour;
    };
</shader>

<shader id="Engine_DirectionalLight">
    struct DirectionalLight
    {
        float3 Colour;
        float3 Direction;
    };
    
    void CalculateDirectionalLighting(DirectionalLight light, vec3 cameraPos, out float3 diffuseOut, out float3 specularOut)
    {
        float3 N     = normalize(IN.Normal);
        float3 L     = -light.Direction;
        float3 V     = normalize(cameraPos - IN.Position.xyz);
        float3 H     = normalize(L + V);
        float  NdotL = max(0.0, dot(N, L));
        float  NdotH = max(0.0, pow(max(dot(N, H), 0), 64.0));
            
        diffuseOut  = light.Colour * NdotL;
        specularOut = light.Colour * NdotH;
    }
</shader>

<shader id="Engine_PointLight">
    struct PointLight
    {
        float3 Colour;
        float3 Position;
        float  ConstantAttenuation;
        float  LinearAttenuation;
        float  QuadraticAttenuation;
    };
    
    float CalculatePointLightAttenuation(PointLight light, float d)     // d = distance
    {
        float c = light.ConstantAttenuation;
        float l = light.LinearAttenuation;
        float q = light.QuadraticAttenuation;
            
        return 1.0 / (c + (l * d) + (q * d * d));
    }
    
    void CalculatePointLighting(PointLight light, vec3 cameraPos, out float4 diffuseOut, out float4 specularOut)
    {
        // N - Input normal
        // L - Light vector from the light to the vertex
        // D - Distance between the light and the vertex
        
        float3 N     = normalize(IN.Normal);
        float3 L     = light.Position - IN.Position.xyz;
        float  D     = length(L);
        float3 V     = normalize(cameraPos - IN.Position.xyz);
        float3 H     = normalize(L + V);
        float  NdotL = max(0.0, dot(N, normalize(L)));
        float  NdotH = max(0.0, pow(max(dot(N, H), 0), 64.0));         // Last argument is shininess. Larger values means smaller, more focused specular highlight.
            
        float attenuation = CalculatePointLightAttenuation(light, D);
            
        diffuseOut  = float4(light.Colour, 1.0) * NdotL * attenuation;
        specularOut = float4(light.Colour, 1.0) * NdotH * attenuation;
    }
</shader>

<shader id="Engine_Attenuation">
    
</shader>



<!-- *** Lighting Pass Shaders *** -->
<shader id="Engine_LightingPass_A1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_AmbientLight" />
    
    <include>
        uniform AmbientLight ALights[1];
        
	    void main(out FragmentOutput OUT)
	    {
            float2 fragCoord = IN.WindowPosition.xy / ScreenSize;

		    OUT.Color0   = tex2D(Lighting_Diffuse, fragCoord) + float4(ALights[0].Colour, 1.0);
            OUT.Color0.a = 1.0f;
            
            OUT.Color1   = tex2D(Lighting_Specular, fragCoord) + float4(0.0, 0.0, 0.0, 1.0);
            OUT.Color1.a = 1.0f;
	    }
    </include>
</shader>

<shader id="Engine_LightingPass_D1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_DirectionalLight" />
    
    <include>
        uniform DirectionalLight DLights[1];
        uniform float3 CameraPosition;
        
	    void main(out FragmentOutput OUT)
	    {
            float2 fragCoord = IN.WindowPosition.xy / ScreenSize;
            
            float3 diffuse  = float3(0.0, 0.0, 0.0);
            float3 specular = float3(0.0, 0.0, 0.0);
            CalculateDirectionalLighting(DLights[0], cameraPos, diffuse, specular);


		    OUT.Color0.rgb = tex2D(Lighting_Diffuse, fragCoord).rgb + diffuse;
            OUT.Color0.a   = 1.0f;
            
            OUT.Color1.rgb = tex2D(Lighting_Specular, fragCoord).rgb + specular;
            OUT.Color1.a   = 1.0f;
	    }
    </include>
</shader>

<shader id="Engine_LightingPass_P1">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_FragmentLightingOutput" />
    <include url="#Engine_FragmentLightingUniforms" />
    <include url="#Engine_PointLight" />
    
    <include>
        uniform PointLight PLights[1];
        uniform float3     CameraPosition;
        
	    void main(out FragmentOutput OUT)
	    {
            float2 fragCoord = IN.WindowPosition.xy / ScreenSize;
            
            float4 diffuse  = float4(0.0, 0.0, 0.0, 0.0);
            float4 specular = float4(0.0, 0.0, 0.0, 0.0);
            CalculatePointLighting(PLights[0], CameraPosition, diffuse, specular);
            
		    OUT.Color0   = tex2D(Lighting_Diffuse, fragCoord) + diffuse;
            OUT.Color0.a = 1.0f;
            
            OUT.Color1   = tex2D(Lighting_Specular, fragCoord) + specular;
            OUT.Color1.a = 1.0f;
	    }
    </include>
</shader>


<!-- *** Material Pass Shaders *** -->
<shader id="Engine_MaterialPass_VS">
    struct VertexInput
	{
		float3 Position : ATTR0;
		float2 TexCoord : ATTR1;
		float3 Normal   : ATTR2;
	};
		
	struct VertexOutput
	{
		float4 ClipPosition : POSITION;

		float2 TexCoord  : TEXCOORD0;
		float3 Normal    : TEXCOORD1;
	};

	uniform float4x4 MVPMatrix;
    uniform float3x3 NormalMatrix;          // transpose(inverse(float3x3(ModelViewMatrix)))
    
	void main(VertexInput IN, out VertexOutput OUT)
	{
		OUT.ClipPosition = mul(MVPMatrix, float4(IN.Position, 1.0));
        
		OUT.TexCoord = IN.TexCoord;
		OUT.Normal   = normalize(mul(NormalMatrix, IN.Normal));
	}
</shader>

<shader id="Engine_MaterialPass_FS">
    <include url="#Engine_FragmentInput" />
    <include url="#Engine_DirectionalLight" />
    
    <include>
        struct FragmentOutput
        {
            float4 Color0 : COLOR0;
            float4 Color1 : COLOR1;
        };
        
        float3 Diffuse();
        float3 Emissive();
        float  Shininess();
        
	    void main(out FragmentOutput OUT)
	    {
            float3 materialDiffuse   = Diffuse();
            float3 materialEmissive  = Emissive();
            float  materialShininess = Shininess();
        
		    OUT.Color0.rgb = materialDiffuse;
            OUT.Color0.a   = 1.0f;
            
            OUT.Color1.rgb = materialEmissive;
            OUT.Color1.a   = materialShininess;
	    }
    </include>
</shader>


<!-- *** Combiner Shader *** -->
<shader id="Engine_LightingMaterialCombiner">
    struct FragmentInput
    {
        float2 TexCoord : TEXCOORD0;
    };
    in FragmentInput IN;
    
    struct FragmentOutput
    {
        float4 Color0 : COLOR0;
    };
    
    uniform sampler2D Lighting_Diffuse;     // rgb = diffuse;  a = nothing
    uniform sampler2D Lighting_Specular;    // rgb = specular; a = noting
    uniform sampler2D MaterialBuffer0;      // rgb = diffuse;  a = transparancy
    uniform sampler2D MaterialBuffer1;      // rgb = emissive; a = shininess
    
    void main(out FragmentOutput OUT)
    {
        float4 lightingTexel0 = tex2D(Lighting_Diffuse,  IN.TexCoord);
        float4 lightingTexel1 = tex2D(Lighting_Specular, IN.TexCoord);
        float4 materialTexel0 = tex2D(MaterialBuffer0,   IN.TexCoord);
        float4 materialTexel1 = tex2D(MaterialBuffer1,   IN.TexCoord);
        
        float3 lightDiffuse  = lightingTexel0.rgb;
        float3 lightSpecular = lightingTexel1.rgb;
        
        float3 materialDiffuse      = materialTexel0.rgb;
        float  materialTransparency = materialTexel0.a;
        float3 materialEmissive     = materialTexel1.rgb;
        float  materialShininess    = materialTexel1.a;
        
        OUT.Color0.rgb = (materialDiffuse * lightDiffuse) + (materialShininess * lightSpecular) + materialEmissive;
        OUT.Color0.a   = 1.0f;
    }
</shader>
