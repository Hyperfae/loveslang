import love;

[Shader("pixel")]
void pixelMain() {
    love::pixel::lambda((float4 vtxcolor, Sampler2D tex, float2 uv, float2 fragCoord) => {
        return vtxcolor * tex.Sample(uv) * float4(uv, 1, 1);
    });
}