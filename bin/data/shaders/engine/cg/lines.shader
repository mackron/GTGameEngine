
<shader id="Engine_SimpleLine_VS">
	struct VertexInput
	{
		float3 Position : ATTR0;
	};
		
	struct VertexOutput
	{
		float4 Position : POSITION;
	};
        
    uniform float4x4 MVPMatrix;
		
	VertexOutput main(in VertexInput IN)
	{
		VertexOutput OUT;
		
		OUT.Position = mul(MVPMatrix, float4(IN.Position, 1.0));
			
		return OUT;
	}
</shader>
    
<shader id="Engine_SimpleLine_FS">
	struct FragmentOutput
	{
		float4 Color : COLOR;
	};
		
    uniform float3 Color;
		
	FragmentOutput main()
	{
		FragmentOutput OUT;
			
		OUT.Color = float4(Color, 1.0);
			
		return OUT;
	}
</shader>