
<shader id="Engine_FullscreenQuad_VS">
    struct VertexInput
	{
		float2 Position : ATTR0;
	};
		
	struct VertexOutput
	{
		float4 Position : POSITION;
		float2 TexCoord : TEXCOORD0;
	};
        
	VertexOutput main(in VertexInput IN)
	{
		VertexOutput OUT;
		
		OUT.Position = float4(IN.Position, 0.0, 1.0);
        OUT.TexCoord = (IN.Position + 1.0) * 0.5;
			
		return OUT;
	}
</shader>

<shader id="GTEngine_FullscreenQuad_FS">
    struct FragmentInput
	{
		float2 TexCoord : TEXCOORD0;
	};
		
	struct FragmentOutput
	{
		float4 Color : COLOR;
	};
		
	uniform sampler2D Texture;
		
	FragmentOutput main(in FragmentInput IN)
	{
		FragmentOutput OUT;
			
		OUT.Color = tex2D(Texture, IN.TexCoord);
			
		return OUT;
	}
</shader>



<shader id="Engine_DefaultVS">
    struct VertexInput
	{
		float3 Position : ATTR0;
		float2 TexCoord : ATTR1;
		float3 Normal   : ATTR2;
	};
		
	struct VertexOutput
	{
		float4 ClipPosition : POSITION;
			
		float2 TexCoord    : TEXCOORD0;
		float3 Normal      : TEXCOORD1;
        float4 Position    : TEXCOORD2;
	};
		
    uniform float4x4 ModelViewMatrix;
	uniform float4x4 MVPMatrix;
    uniform float3x3 NormalMatrix;
		
	void main(VertexInput IN, out VertexOutput OUT)
	{
		OUT.ClipPosition = mul(MVPMatrix, float4(IN.Position, 1.0));
        
		OUT.TexCoord = IN.TexCoord;
		OUT.Normal   = normalize(mul(NormalMatrix, IN.Normal));
        OUT.Position = mul(ModelViewMatrix, float4(IN.Position, 1.0));
	}
</shader>

<shader id="Engine_DefaultFS">
    struct FragmentInput
	{
		float2 TexCoord : TEXCOORD0;
		float3 Normal   : TEXCOORD1;
	};
		
	struct FragmentOutput
	{
		float4 Color0 : COLOR0;
	};
    
    FragmentInput  IN;
    FragmentOutput OUT;
    

	uniform float3    LightDirection;
	uniform float3    LightColour;
	uniform float3    AmbientColour;
    
    
	void main()
	{
        float4 materialDiffuse  = Diffuse();
        float4 materialEmissive = Emissive();
    
		float NdotL = max(0.0, dot(IN.Normal, -LightDirection));
			
		float4 diffuse = float4(LightColour, 1.0) * NdotL;
		float4 ambient = float4(AmbientColour, 1.0);
		
		OUT.Color0 = (materialDiffuse * (diffuse + ambient)) + materialEmissive;
	}
</shader>
