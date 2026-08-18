import love;
// [shader("fragment")]
// float4 fragmentMain(float4 vtxcolor, Sampler2D texture, float2 uv) : SV_Target
// {
//     float4 color = texture.Sample(uv);
//     color.g += greenness;
//     return color;
// }

uniform float freakyness;

[Shader("pixel")]
void mainPixel() {
    float4 color = love::pixel::getMainTexture2D().Sample(love::pixel::readVertexTexCoord().xy);
    float4 freakycolor = color.zxyw;
    color = lerp(color, freakycolor, freakyness);
    love::pixel::writePixelColor(color, 0);
}

RWStructuredBuffer<uint> result;

[Shader("compute")]
[numthreads(1,1,1)]
void mainCompute(uint3 threadId : SV_DispatchThreadID) {
    result[threadId.x] = (uint16_t)threadId.x;
}
