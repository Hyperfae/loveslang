namespace love {
    public float4 getConstantColor() {
        return float4(1);
    }
}

namespace love::vertex {

    layout (location = 0) in float4 VertexPosition;
    layout (location = 1) in float4 VertexTexCoord;
    layout (location = 2) in float4 VertexColor;

    out float4 VaryingTexCoord;
    out float4 VaryingColor;
}

namespace love::pixel {

    internal out float4 love_PixelColor;
    internal uniform Sampler2D MainTex;

    [ForceInline]
    public void writePixelColor(float4 color) {
        __target_switch
        {
            case glsl:
                __intrinsic_asm R"CODESTRING(love_PixelColor = $0)CODESTRING";
            default:
                love_PixelColor = color;
        }
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

    internal in float4 VertexTexCoord;
    [ForceInline]
    public float4 readVertexTexCoord() {
        __target_switch
        {
            case glsl:
            {
                __requirePrelude(R"CODESTRING(uniform sampler2D MainTex;)CODESTRING");
                __intrinsic_asm R"CODESTRING( VaryingTexCoord )CODESTRING";
            }
            default:
                return VertexTexCoord;
        }
    }

    [ForceInline]
    public Sampler2D getMainTexture2D() {
        __target_switch
        {
            case glsl:
                __intrinsic_asm R"CODESTRING( MainTex )CODESTRING";
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

