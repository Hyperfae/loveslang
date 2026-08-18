namespace love {

    public float4 getConstantColor() {
        return getUniformsPerDraw()[10];
    }

    public layout(location = 4) inout float4 VaryingTexCoord = float4(0);
    public layout(location = 5) inout float4 VaryingColor = float4(0);

    internal uniform float4[12] love_UniformsPerDraw;
    [ForceInline]
    internal float4[12] getUniformsPerDraw() {
        __target_switch
        {
            case glsl:
                __intrinsic_asm R"CODESTRING( love_UniformsPerDraw )CODESTRING";
            default:
                return love_UniformsPerDraw;
        }
    }

    public float4x4 getTransformMatrix() {
        return float4x4(getUniformsPerDraw()[0], getUniformsPerDraw()[1], getUniformsPerDraw()[2], getUniformsPerDraw()[3]);
    }

    public float4x4 getProjectionMatrix() {
        return float4x4(getUniformsPerDraw()[4], getUniformsPerDraw()[5], getUniformsPerDraw()[6], getUniformsPerDraw()[7]);
    }

    [ForceInline]
    public float4x4 getTransformProjectionMatrix() {
        return mul(getTransformMatrix(), getProjectionMatrix());
    }
}

namespace love::vertex {

    struct VertexData<T = void> {
        float4 VertexPosition;
        float4 VertexTexCoord;
        float4 VertexColor;
        T attributes;
    }

    struct VertexOutput<T = void> {
        float4 ClipSpacePosition;
        float4 VertexTexCoord;
        float4 VertexColor;
        T attributes;
    }

    public layout (location = 0) in float4 VertexPosition;
    public layout (location = 1) in float4 VertexTexCoord;
    public layout (location = 2) in float4 VertexColor;

    [ForceInline]
    public void writePosition(float4 position) {
        __target_switch
        {
            case glsl:
                __intrinsic_asm R"CODESTRING(love_Position = $0)CODESTRING";
            // default:
        }
    }
}

namespace love::pixel {
    internal layout (location = 0) out float4 love_PixelColor;
    internal uniform Sampler2D MainTex;

    [ForceInline]
    public void writePixelColor(float4 color) {
        love_PixelColor = color;
        // __target_switch
        // {
        //     case glsl:
        //         __intrinsic_asm R"CODESTRING(love_PixelColor = $0)CODESTRING";
        //     default:
        // }
    }

    [ForceInline]
    public float4 readPixelColor() {
        __target_switch
        {
            case glsl:
                __intrinsic_asm R"CODESTRING( love_PixelColor )CODESTRING";
            default:
                return love_PixelColor;
        }
    }

    [ForceInline]
    public float4 readVertexTexCoord() {
        return love::VaryingTexCoord;
    }

    [ForceInline]
    public Sampler2D getMainTexture2D() {
        __target_switch
        {
            case glsl:
            {
                __requirePrelude(R"CODESTRING(uniform sampler2D MainTex;)CODESTRING");
                __intrinsic_asm R"CODESTRING( MainTex )CODESTRING";
            }
            default:
                return MainTex;
        }
    }

    [ForceInline]
    public float4 VideoTexel(float2 coords) {
        __target_switch
        {
            case glsl:
                __intrinsic_asm R"CODESTRING( VideoTexel($0) )CODESTRING";
            default:
                // TODO: Implement VideoTexel in Slang
                return float4(0, 0, 0, 1);
        }
    }
}

