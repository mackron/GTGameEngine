
<shader id="Engine_GUIQuad_VS">
    struct VertexInput
	{
		float2 Position : ATTR0;
		float2 TexCoord : ATTR1;
	};
		
	struct VertexOutput
	{
		float4 Position : POSITION;
		float2 TexCoord : TEXCOORD0;
	};
        
    uniform float4x4 Projection;
		
	VertexOutput main(in VertexInput IN)
	{
		VertexOutput OUT;
		
		OUT.Position = mul(Projection, float4(IN.Position, 0.0, 1.0));
		OUT.TexCoord = IN.TexCoord;
			
		return OUT;
	}
</shader>

<shader id="Engine_GUIQuad_FS">
    struct FragmentInput
	{
		float2 TexCoord : TEXCOORD0;
	};
		
	struct FragmentOutput
	{
		float4 Color : COLOR;
	};
		
	uniform sampler2D Texture;
    uniform float3    Color;
		
	FragmentOutput main(in FragmentInput IN)
	{
		FragmentOutput OUT;
			
		OUT.Color = tex2D(Texture, IN.TexCoord) * float4(Color, 1.0);
			
		return OUT;
	}
</shader>
    
<shader id="Engine_GUIQuad_NoTexture_FS">
    struct FragmentInput
	{
		float2 TexCoord : TEXCOORD0;
	};
		
	struct FragmentOutput
	{
		float4 Color : COLOR;
	};
		
    uniform float3 Color;
		
	FragmentOutput main(in FragmentInput IN)
	{
		FragmentOutput OUT;
			
		OUT.Color = float4(Color, 1.0);
			
		return OUT;
	}
</shader>

<!-- Text Shader -->
<shader id="Engine_GUIText_VS">
    struct VertexInput
    {
        float3 Position : ATTR0;
        float2 TexCoord : ATTR1;
    };

    struct VertexOutput
    {
        float4 Position : POSITION;
        float2 TexCoord : TEXCOORD0;
    };

    uniform float4x4 Projection;
    uniform float2   Offset;

    VertexOutput main(in VertexInput IN)
    {
        VertexOutput OUT;

        OUT.Position = mul(Projection, float4(IN.Position.xy + Offset, IN.Position.z, 1.0));
        OUT.TexCoord = IN.TexCoord;

        return OUT;
    }
</shader>

<shader id="Engine_GUIText_FS">
    struct FragmentInput
    {
        float2 TexCoord : TEXCOORD0;
    };

    struct FragmentOutput
    {
        float4 Color : COLOR;
    };

    uniform sampler2D Texture;
    uniform float3    Color;

    FragmentOutput main(in FragmentInput IN)
    {
        FragmentOutput OUT;

        //OUT.Color = tex2D(Texture, IN.TexCoord) * float4(Color, 1.0);
        OUT.Color = float4(Color, tex2D(Texture, IN.TexCoord).a);

        return OUT;
    }
</shader>