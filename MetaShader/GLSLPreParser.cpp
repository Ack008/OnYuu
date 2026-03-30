
// Generated from GLSLPreParser.g4 by ANTLR 4.10


#include "GLSLPreParserListener.h"
#include "GLSLPreParserVisitor.h"

#include "GLSLPreParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct GLSLPreParserStaticData final {
  GLSLPreParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  GLSLPreParserStaticData(const GLSLPreParserStaticData&) = delete;
  GLSLPreParserStaticData(GLSLPreParserStaticData&&) = delete;
  GLSLPreParserStaticData& operator=(const GLSLPreParserStaticData&) = delete;
  GLSLPreParserStaticData& operator=(GLSLPreParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

std::once_flag glslpreparserParserOnceFlag;
GLSLPreParserStaticData *glslpreparserParserStaticData = nullptr;

void glslpreparserParserInitialize() {
  assert(glslpreparserParserStaticData == nullptr);
  auto staticData = std::make_unique<GLSLPreParserStaticData>(
    std::vector<std::string>{
      "translation_unit", "compiler_directive", "behavior", "constant_expression", 
      "define_directive", "elif_directive", "else_directive", "endif_directive", 
      "error_directive", "error_message", "extension_directive", "extension_name", 
      "group_of_lines", "if_directive", "ifdef_directive", "ifndef_directive", 
      "line_directive", "line_expression", "macro_esc_newline", "macro_identifier", 
      "macro_name", "macro_text", "macro_text_", "number", "off", "on", 
      "pragma_debug", "pragma_directive", "pragma_optimize", "profile", 
      "program_text", "stdgl", "undef_directive", "version_directive"
    },
    std::vector<std::string>{
      "", "'atomic_uint'", "'attribute'", "'bool'", "'break'", "'buffer'", 
      "'bvec2'", "'bvec3'", "'bvec4'", "'case'", "'centroid'", "'coherent'", 
      "'const'", "'continue'", "'default'", "'discard'", "'dmat2'", "'dmat2x2'", 
      "'dmat2x3'", "'dmat2x4'", "'dmat3'", "'dmat3x2'", "'dmat3x3'", "'dmat3x4'", 
      "'dmat4'", "'dmat4x2'", "'dmat4x3'", "'dmat4x4'", "'do'", "'double'", 
      "'dvec2'", "'dvec3'", "'dvec4'", "'else'", "'false'", "'flat'", "'float'", 
      "'for'", "'highp'", "'if'", "'iimage1D'", "'iimage1DArray'", "'iimage2D'", 
      "'iimage2DArray'", "'iimage2DMS'", "'iimage2DMSArray'", "'iimage2DRect'", 
      "'iimage3D'", "'iimageBuffer'", "'iimageCube'", "'iimageCubeArray'", 
      "'image1D'", "'image1DArray'", "'image2D'", "'image2DArray'", "'image2DMS'", 
      "'image2DMSArray'", "'image2DRect'", "'image3D'", "'imageBuffer'", 
      "'imageCube'", "'imageCubeArray'", "'in'", "'inout'", "'int'", "'invariant'", 
      "'isampler1D'", "'isampler1DArray'", "'isampler2D'", "'isampler2DArray'", 
      "'isampler2DMS'", "'isampler2DMSArray'", "'isampler2DRect'", "'isampler3D'", 
      "'isamplerBuffer'", "'isamplerCube'", "'isamplerCubeArray'", "'isubpassInput'", 
      "'isubpassInputMS'", "'itexture1D'", "'itexture1DArray'", "'itexture2D'", 
      "'itexture2DArray'", "'itexture2DMS'", "'itexture2DMSArray'", "'itexture2DRect'", 
      "'itexture3D'", "'itextureBuffer'", "'itextureCube'", "'itextureCubeArray'", 
      "'ivec2'", "'ivec3'", "'ivec4'", "'layout'", "'lowp'", "'mat2'", "'mat2x2'", 
      "'mat2x3'", "'mat2x4'", "'mat3'", "'mat3x2'", "'mat3x3'", "'mat3x4'", 
      "'mat4'", "'mat4x2'", "'mat4x3'", "'mat4x4'", "'mediump'", "'noperspective'", 
      "'out'", "'patch'", "'precise'", "'precision'", "'readonly'", "'restrict'", 
      "'return'", "'sample'", "'sampler'", "'sampler1D'", "'sampler1DArray'", 
      "'sampler1DArrayShadow'", "'sampler1DShadow'", "'sampler2D'", "'sampler2DArray'", 
      "'sampler2DArrayShadow'", "'sampler2DMS'", "'sampler2DMSArray'", "'sampler2DRect'", 
      "'sampler2DRectShadow'", "'sampler2DShadow'", "'sampler3D'", "'samplerBuffer'", 
      "'samplerCube'", "'samplerCubeArray'", "'samplerCubeArrayShadow'", 
      "'samplerCubeShadow'", "'samplerShadow'", "'shared'", "'smooth'", 
      "'struct'", "'subpassInput'", "'subpassInputMS'", "'subroutine'", 
      "'switch'", "'texture1D'", "'texture1DArray'", "'texture2D'", "'texture2DArray'", 
      "'texture2DMS'", "'texture2DMSArray'", "'texture2DRect'", "'texture3D'", 
      "'textureBuffer'", "'textureCube'", "'textureCubeArray'", "'true'", 
      "'uimage1D'", "'uimage1DArray'", "'uimage2D'", "'uimage2DArray'", 
      "'uimage2DMS'", "'uimage2DMSArray'", "'uimage2DRect'", "'uimage3D'", 
      "'uimageBuffer'", "'uimageCube'", "'uimageCubeArray'", "'uint'", "'uniform'", 
      "'usampler1D'", "'usampler1DArray'", "'usampler2D'", "'usampler2DArray'", 
      "'usampler2DMS'", "'usampler2DMSArray'", "'usampler2DRect'", "'usampler3D'", 
      "'usamplerBuffer'", "'usamplerCube'", "'usamplerCubeArray'", "'usubpassInput'", 
      "'usubpassInputMS'", "'utexture1D'", "'utexture1DArray'", "'utexture2D'", 
      "'utexture2DArray'", "'utexture2DMS'", "'utexture2DMSArray'", "'utexture2DRect'", 
      "'utexture3D'", "'utextureBuffer'", "'utextureCube'", "'utextureCubeArray'", 
      "'uvec2'", "'uvec3'", "'uvec4'", "'varying'", "'vec2'", "'vec3'", 
      "'vec4'", "'void'", "'volatile'", "'while'", "'writeonly'", "'+='", 
      "'&'", "'&='", "'&&'", "'!'", "'^'", "':'", "','", "'-'", "'--'", 
      "'/='", "'.'", "'=='", "'='", "'>='", "'++'", "'<='", "'<'", "'<<='", 
      "'{'", "'['", "'<<'", "'('", "'%='", "'*='", "'!='", "", "'|='", "'||'", 
      "'%'", "'+'", "'\\u003F'", "'>'", "'>>='", "'}'", "']'", "'>>'", "')'", 
      "';'", "'/'", "'*'", "'-='", "'~'", "'|'", "'^='", "'^^'", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "'debug'", "", "'off'", "'on'", "'optimize'", 
      "", "'STDGL'"
    },
    std::vector<std::string>{
      "", "ATOMIC_UINT", "ATTRIBUTE", "BOOL", "BREAK", "BUFFER", "BVEC2", 
      "BVEC3", "BVEC4", "CASE", "CENTROID", "COHERENT", "CONST", "CONTINUE", 
      "DEFAULT", "DISCARD", "DMAT2", "DMAT2X2", "DMAT2X3", "DMAT2X4", "DMAT3", 
      "DMAT3X2", "DMAT3X3", "DMAT3X4", "DMAT4", "DMAT4X2", "DMAT4X3", "DMAT4X4", 
      "DO", "DOUBLE", "DVEC2", "DVEC3", "DVEC4", "ELSE", "FALSE", "FLAT", 
      "FLOAT", "FOR", "HIGHP", "IF", "IIMAGE1D", "IIMAGE1DARRAY", "IIMAGE2D", 
      "IIMAGE2DARRAY", "IIMAGE2DMS", "IIMAGE2DMSARRAY", "IIMAGE2DRECT", 
      "IIMAGE3D", "IIMAGEBUFFER", "IIMAGECUBE", "IIMAGECUBEARRAY", "IMAGE1D", 
      "IMAGE1DARRAY", "IMAGE2D", "IMAGE2DARRAY", "IMAGE2DMS", "IMAGE2DMSARRAY", 
      "IMAGE2DRECT", "IMAGE3D", "IMAGEBUFFER", "IMAGECUBE", "IMAGECUBEARRAY", 
      "IN", "INOUT", "INT", "INVARIANT", "ISAMPLER1D", "ISAMPLER1DARRAY", 
      "ISAMPLER2D", "ISAMPLER2DARRAY", "ISAMPLER2DMS", "ISAMPLER2DMSARRAY", 
      "ISAMPLER2DRECT", "ISAMPLER3D", "ISAMPLERBUFFER", "ISAMPLERCUBE", 
      "ISAMPLERCUBEARRAY", "ISUBPASSINPUT", "ISUBPASSINPUTMS", "ITEXTURE1D", 
      "ITEXTURE1DARRAY", "ITEXTURE2D", "ITEXTURE2DARRAY", "ITEXTURE2DMS", 
      "ITEXTURE2DMSARRAY", "ITEXTURE2DRECT", "ITEXTURE3D", "ITEXTUREBUFFER", 
      "ITEXTURECUBE", "ITEXTURECUBEARRAY", "IVEC2", "IVEC3", "IVEC4", "LAYOUT", 
      "LOWP", "MAT2", "MAT2X2", "MAT2X3", "MAT2X4", "MAT3", "MAT3X2", "MAT3X3", 
      "MAT3X4", "MAT4", "MAT4X2", "MAT4X3", "MAT4X4", "MEDIUMP", "NOPERSPECTIVE", 
      "OUT", "PATCH", "PRECISE", "PRECISION", "READONLY", "RESTRICT", "RETURN", 
      "SAMPLE", "SAMPLER", "SAMPLER1D", "SAMPLER1DARRAY", "SAMPLER1DARRAYSHADOW", 
      "SAMPLER1DSHADOW", "SAMPLER2D", "SAMPLER2DARRAY", "SAMPLER2DARRAYSHADOW", 
      "SAMPLER2DMS", "SAMPLER2DMSARRAY", "SAMPLER2DRECT", "SAMPLER2DRECTSHADOW", 
      "SAMPLER2DSHADOW", "SAMPLER3D", "SAMPLERBUFFER", "SAMPLERCUBE", "SAMPLERCUBEARRAY", 
      "SAMPLERCUBEARRAYSHADOW", "SAMPLERCUBESHADOW", "SAMPLERSHADOW", "SHARED", 
      "SMOOTH", "STRUCT", "SUBPASSINPUT", "SUBPASSINPUTMS", "SUBROUTINE", 
      "SWITCH", "TEXTURE1D", "TEXTURE1DARRAY", "TEXTURE2D", "TEXTURE2DARRAY", 
      "TEXTURE2DMS", "TEXTURE2DMSARRAY", "TEXTURE2DRECT", "TEXTURE3D", "TEXTUREBUFFER", 
      "TEXTURECUBE", "TEXTURECUBEARRAY", "TRUE", "UIMAGE1D", "UIMAGE1DARRAY", 
      "UIMAGE2D", "UIMAGE2DARRAY", "UIMAGE2DMS", "UIMAGE2DMSARRAY", "UIMAGE2DRECT", 
      "UIMAGE3D", "UIMAGEBUFFER", "UIMAGECUBE", "UIMAGECUBEARRAY", "UINT", 
      "UNIFORM", "USAMPLER1D", "USAMPLER1DARRAY", "USAMPLER2D", "USAMPLER2DARRAY", 
      "USAMPLER2DMS", "USAMPLER2DMSARRAY", "USAMPLER2DRECT", "USAMPLER3D", 
      "USAMPLERBUFFER", "USAMPLERCUBE", "USAMPLERCUBEARRAY", "USUBPASSINPUT", 
      "USUBPASSINPUTMS", "UTEXTURE1D", "UTEXTURE1DARRAY", "UTEXTURE2D", 
      "UTEXTURE2DARRAY", "UTEXTURE2DMS", "UTEXTURE2DMSARRAY", "UTEXTURE2DRECT", 
      "UTEXTURE3D", "UTEXTUREBUFFER", "UTEXTURECUBE", "UTEXTURECUBEARRAY", 
      "UVEC2", "UVEC3", "UVEC4", "VARYING", "VEC2", "VEC3", "VEC4", "VOID", 
      "VOLATILE", "WHILE", "WRITEONLY", "ADD_ASSIGN", "AMPERSAND", "AND_ASSIGN", 
      "AND_OP", "BANG", "CARET", "COLON", "COMMA", "DASH", "DEC_OP", "DIV_ASSIGN", 
      "DOT", "EQ_OP", "EQUAL", "GE_OP", "INC_OP", "LE_OP", "LEFT_ANGLE", 
      "LEFT_ASSIGN", "LEFT_BRACE", "LEFT_BRACKET", "LEFT_OP", "LEFT_PAREN", 
      "MOD_ASSIGN", "MUL_ASSIGN", "NE_OP", "NUMBER_SIGN", "OR_ASSIGN", "OR_OP", 
      "PERCENT", "PLUS", "QUESTION", "RIGHT_ANGLE", "RIGHT_ASSIGN", "RIGHT_BRACE", 
      "RIGHT_BRACKET", "RIGHT_OP", "RIGHT_PAREN", "SEMICOLON", "SLASH", 
      "STAR", "SUB_ASSIGN", "TILDE", "VERTICAL_BAR", "XOR_ASSIGN", "XOR_OP", 
      "DOUBLECONSTANT", "FLOATCONSTANT", "INTCONSTANT", "UINTCONSTANT", 
      "BLOCK_COMMENT", "LINE_COMMENT", "LINE_CONTINUATION", "IDENTIFIER", 
      "WHITE_SPACE", "DEFINE_DIRECTIVE", "ELIF_DIRECTIVE", "ELSE_DIRECTIVE", 
      "ENDIF_DIRECTIVE", "ERROR_DIRECTIVE", "EXTENSION_DIRECTIVE", "IF_DIRECTIVE", 
      "IFDEF_DIRECTIVE", "IFNDEF_DIRECTIVE", "LINE_DIRECTIVE", "PRAGMA_DIRECTIVE", 
      "UNDEF_DIRECTIVE", "VERSION_DIRECTIVE", "SPACE_TAB_0", "NEWLINE_0", 
      "MACRO_NAME", "NEWLINE_1", "SPACE_TAB_1", "CONSTANT_EXPRESSION", "NEWLINE_2", 
      "ERROR_MESSAGE", "NEWLINE_3", "BEHAVIOR", "EXTENSION_NAME", "NEWLINE_4", 
      "SPACE_TAB_2", "NEWLINE_5", "MACRO_IDENTIFIER", "NEWLINE_6", "SPACE_TAB_3", 
      "LINE_EXPRESSION", "NEWLINE_7", "MACRO_ESC_NEWLINE", "MACRO_TEXT", 
      "NEWLINE_8", "DEBUG", "NEWLINE_9", "OFF", "ON", "OPTIMIZE", "SPACE_TAB_5", 
      "STDGL", "PROGRAM_TEXT", "NEWLINE_10", "SPACE_TAB_6", "NEWLINE_11", 
      "NUMBER", "PROFILE", "SPACE_TAB_7"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,307,243,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,1,0,5,0,70,8,0,10,
  	0,12,0,73,9,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,
  	88,8,1,1,2,1,2,1,3,1,3,1,4,1,4,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,6,1,
  	6,1,6,1,6,1,7,1,7,1,7,1,8,1,8,1,8,1,8,1,9,1,9,1,10,1,10,1,10,1,10,1,10,
  	1,10,1,11,1,11,1,12,1,12,5,12,127,8,12,10,12,12,12,130,9,12,1,13,1,13,
  	1,13,1,13,1,13,5,13,137,8,13,10,13,12,13,140,9,13,1,13,3,13,143,8,13,
  	1,13,1,13,1,14,1,14,1,14,1,14,1,14,5,14,152,8,14,10,14,12,14,155,9,14,
  	1,14,3,14,158,8,14,1,14,1,14,1,15,1,15,1,15,1,15,1,15,5,15,167,8,15,10,
  	15,12,15,170,9,15,1,15,3,15,173,8,15,1,15,1,15,1,16,1,16,1,16,1,16,1,
  	17,1,17,1,18,1,18,1,19,1,19,1,20,1,20,1,21,1,21,5,21,191,8,21,10,21,12,
  	21,194,9,21,1,22,1,22,1,23,1,23,1,24,1,24,1,25,1,25,1,26,1,26,1,26,1,
  	26,3,26,208,8,26,1,26,1,26,1,27,1,27,1,27,1,27,1,27,3,27,217,8,27,1,28,
  	1,28,1,28,1,28,3,28,223,8,28,1,28,1,28,1,29,1,29,1,30,1,30,1,31,1,31,
  	1,32,1,32,1,32,1,32,1,33,1,33,1,33,1,33,3,33,241,8,33,1,33,0,0,34,0,2,
  	4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,
  	52,54,56,58,60,62,64,66,0,0,236,0,71,1,0,0,0,2,87,1,0,0,0,4,89,1,0,0,
  	0,6,91,1,0,0,0,8,93,1,0,0,0,10,98,1,0,0,0,12,103,1,0,0,0,14,107,1,0,0,
  	0,16,110,1,0,0,0,18,114,1,0,0,0,20,116,1,0,0,0,22,122,1,0,0,0,24,128,
  	1,0,0,0,26,131,1,0,0,0,28,146,1,0,0,0,30,161,1,0,0,0,32,176,1,0,0,0,34,
  	180,1,0,0,0,36,182,1,0,0,0,38,184,1,0,0,0,40,186,1,0,0,0,42,192,1,0,0,
  	0,44,195,1,0,0,0,46,197,1,0,0,0,48,199,1,0,0,0,50,201,1,0,0,0,52,203,
  	1,0,0,0,54,211,1,0,0,0,56,218,1,0,0,0,58,226,1,0,0,0,60,228,1,0,0,0,62,
  	230,1,0,0,0,64,232,1,0,0,0,66,236,1,0,0,0,68,70,3,2,1,0,69,68,1,0,0,0,
  	70,73,1,0,0,0,71,69,1,0,0,0,71,72,1,0,0,0,72,1,1,0,0,0,73,71,1,0,0,0,
  	74,88,3,8,4,0,75,88,3,10,5,0,76,88,3,12,6,0,77,88,3,14,7,0,78,88,3,16,
  	8,0,79,88,3,20,10,0,80,88,3,26,13,0,81,88,3,28,14,0,82,88,3,30,15,0,83,
  	88,3,32,16,0,84,88,3,54,27,0,85,88,3,64,32,0,86,88,3,66,33,0,87,74,1,
  	0,0,0,87,75,1,0,0,0,87,76,1,0,0,0,87,77,1,0,0,0,87,78,1,0,0,0,87,79,1,
  	0,0,0,87,80,1,0,0,0,87,81,1,0,0,0,87,82,1,0,0,0,87,83,1,0,0,0,87,84,1,
  	0,0,0,87,85,1,0,0,0,87,86,1,0,0,0,88,3,1,0,0,0,89,90,5,281,0,0,90,5,1,
  	0,0,0,91,92,5,277,0,0,92,7,1,0,0,0,93,94,5,230,0,0,94,95,5,259,0,0,95,
  	96,3,40,20,0,96,97,3,42,21,0,97,9,1,0,0,0,98,99,5,230,0,0,99,100,5,260,
  	0,0,100,101,3,6,3,0,101,102,3,24,12,0,102,11,1,0,0,0,103,104,5,230,0,
  	0,104,105,5,261,0,0,105,106,3,24,12,0,106,13,1,0,0,0,107,108,5,230,0,
  	0,108,109,5,262,0,0,109,15,1,0,0,0,110,111,5,230,0,0,111,112,5,263,0,
  	0,112,113,3,18,9,0,113,17,1,0,0,0,114,115,5,279,0,0,115,19,1,0,0,0,116,
  	117,5,230,0,0,117,118,5,264,0,0,118,119,3,22,11,0,119,120,5,210,0,0,120,
  	121,3,4,2,0,121,21,1,0,0,0,122,123,5,282,0,0,123,23,1,0,0,0,124,127,3,
  	60,30,0,125,127,3,2,1,0,126,124,1,0,0,0,126,125,1,0,0,0,127,130,1,0,0,
  	0,128,126,1,0,0,0,128,129,1,0,0,0,129,25,1,0,0,0,130,128,1,0,0,0,131,
  	132,5,230,0,0,132,133,5,265,0,0,133,134,3,6,3,0,134,138,3,24,12,0,135,
  	137,3,10,5,0,136,135,1,0,0,0,137,140,1,0,0,0,138,136,1,0,0,0,138,139,
  	1,0,0,0,139,142,1,0,0,0,140,138,1,0,0,0,141,143,3,12,6,0,142,141,1,0,
  	0,0,142,143,1,0,0,0,143,144,1,0,0,0,144,145,3,14,7,0,145,27,1,0,0,0,146,
  	147,5,230,0,0,147,148,5,266,0,0,148,149,3,38,19,0,149,153,3,24,12,0,150,
  	152,3,10,5,0,151,150,1,0,0,0,152,155,1,0,0,0,153,151,1,0,0,0,153,154,
  	1,0,0,0,154,157,1,0,0,0,155,153,1,0,0,0,156,158,3,12,6,0,157,156,1,0,
  	0,0,157,158,1,0,0,0,158,159,1,0,0,0,159,160,3,14,7,0,160,29,1,0,0,0,161,
  	162,5,230,0,0,162,163,5,267,0,0,163,164,3,38,19,0,164,168,3,24,12,0,165,
  	167,3,10,5,0,166,165,1,0,0,0,167,170,1,0,0,0,168,166,1,0,0,0,168,169,
  	1,0,0,0,169,172,1,0,0,0,170,168,1,0,0,0,171,173,3,12,6,0,172,171,1,0,
  	0,0,172,173,1,0,0,0,173,174,1,0,0,0,174,175,3,14,7,0,175,31,1,0,0,0,176,
  	177,5,230,0,0,177,178,5,268,0,0,178,179,3,34,17,0,179,33,1,0,0,0,180,
  	181,5,289,0,0,181,35,1,0,0,0,182,183,5,291,0,0,183,37,1,0,0,0,184,185,
  	5,286,0,0,185,39,1,0,0,0,186,187,5,274,0,0,187,41,1,0,0,0,188,191,3,44,
  	22,0,189,191,3,36,18,0,190,188,1,0,0,0,190,189,1,0,0,0,191,194,1,0,0,
  	0,192,190,1,0,0,0,192,193,1,0,0,0,193,43,1,0,0,0,194,192,1,0,0,0,195,
  	196,5,292,0,0,196,45,1,0,0,0,197,198,5,305,0,0,198,47,1,0,0,0,199,200,
  	5,296,0,0,200,49,1,0,0,0,201,202,5,297,0,0,202,51,1,0,0,0,203,204,5,294,
  	0,0,204,207,5,226,0,0,205,208,3,50,25,0,206,208,3,48,24,0,207,205,1,0,
  	0,0,207,206,1,0,0,0,208,209,1,0,0,0,209,210,5,241,0,0,210,53,1,0,0,0,
  	211,212,5,230,0,0,212,216,5,269,0,0,213,217,3,62,31,0,214,217,3,52,26,
  	0,215,217,3,56,28,0,216,213,1,0,0,0,216,214,1,0,0,0,216,215,1,0,0,0,217,
  	55,1,0,0,0,218,219,5,298,0,0,219,222,5,226,0,0,220,223,3,50,25,0,221,
  	223,3,48,24,0,222,220,1,0,0,0,222,221,1,0,0,0,223,224,1,0,0,0,224,225,
  	5,241,0,0,225,57,1,0,0,0,226,227,5,306,0,0,227,59,1,0,0,0,228,229,5,301,
  	0,0,229,61,1,0,0,0,230,231,5,300,0,0,231,63,1,0,0,0,232,233,5,230,0,0,
  	233,234,5,270,0,0,234,235,3,38,19,0,235,65,1,0,0,0,236,237,5,230,0,0,
  	237,238,5,271,0,0,238,240,3,46,23,0,239,241,3,58,29,0,240,239,1,0,0,0,
  	240,241,1,0,0,0,241,67,1,0,0,0,16,71,87,126,128,138,142,153,157,168,172,
  	190,192,207,216,222,240
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  glslpreparserParserStaticData = staticData.release();
}

}

GLSLPreParser::GLSLPreParser(TokenStream *input) : GLSLPreParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

GLSLPreParser::GLSLPreParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  GLSLPreParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *glslpreparserParserStaticData->atn, glslpreparserParserStaticData->decisionToDFA, glslpreparserParserStaticData->sharedContextCache, options);
}

GLSLPreParser::~GLSLPreParser() {
  delete _interpreter;
}

const atn::ATN& GLSLPreParser::getATN() const {
  return *glslpreparserParserStaticData->atn;
}

std::string GLSLPreParser::getGrammarFileName() const {
  return "GLSLPreParser.g4";
}

const std::vector<std::string>& GLSLPreParser::getRuleNames() const {
  return glslpreparserParserStaticData->ruleNames;
}

const dfa::Vocabulary& GLSLPreParser::getVocabulary() const {
  return glslpreparserParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView GLSLPreParser::getSerializedATN() const {
  return glslpreparserParserStaticData->serializedATN;
}


//----------------- Translation_unitContext ------------------------------------------------------------------

GLSLPreParser::Translation_unitContext::Translation_unitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<GLSLPreParser::Compiler_directiveContext *> GLSLPreParser::Translation_unitContext::compiler_directive() {
  return getRuleContexts<GLSLPreParser::Compiler_directiveContext>();
}

GLSLPreParser::Compiler_directiveContext* GLSLPreParser::Translation_unitContext::compiler_directive(size_t i) {
  return getRuleContext<GLSLPreParser::Compiler_directiveContext>(i);
}


size_t GLSLPreParser::Translation_unitContext::getRuleIndex() const {
  return GLSLPreParser::RuleTranslation_unit;
}

void GLSLPreParser::Translation_unitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTranslation_unit(this);
}

void GLSLPreParser::Translation_unitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTranslation_unit(this);
}


std::any GLSLPreParser::Translation_unitContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitTranslation_unit(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Translation_unitContext* GLSLPreParser::translation_unit() {
  Translation_unitContext *_localctx = _tracker.createInstance<Translation_unitContext>(_ctx, getState());
  enterRule(_localctx, 0, GLSLPreParser::RuleTranslation_unit);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(71);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == GLSLPreParser::NUMBER_SIGN) {
      setState(68);
      compiler_directive();
      setState(73);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Compiler_directiveContext ------------------------------------------------------------------

GLSLPreParser::Compiler_directiveContext::Compiler_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

GLSLPreParser::Define_directiveContext* GLSLPreParser::Compiler_directiveContext::define_directive() {
  return getRuleContext<GLSLPreParser::Define_directiveContext>(0);
}

GLSLPreParser::Elif_directiveContext* GLSLPreParser::Compiler_directiveContext::elif_directive() {
  return getRuleContext<GLSLPreParser::Elif_directiveContext>(0);
}

GLSLPreParser::Else_directiveContext* GLSLPreParser::Compiler_directiveContext::else_directive() {
  return getRuleContext<GLSLPreParser::Else_directiveContext>(0);
}

GLSLPreParser::Endif_directiveContext* GLSLPreParser::Compiler_directiveContext::endif_directive() {
  return getRuleContext<GLSLPreParser::Endif_directiveContext>(0);
}

GLSLPreParser::Error_directiveContext* GLSLPreParser::Compiler_directiveContext::error_directive() {
  return getRuleContext<GLSLPreParser::Error_directiveContext>(0);
}

GLSLPreParser::Extension_directiveContext* GLSLPreParser::Compiler_directiveContext::extension_directive() {
  return getRuleContext<GLSLPreParser::Extension_directiveContext>(0);
}

GLSLPreParser::If_directiveContext* GLSLPreParser::Compiler_directiveContext::if_directive() {
  return getRuleContext<GLSLPreParser::If_directiveContext>(0);
}

GLSLPreParser::Ifdef_directiveContext* GLSLPreParser::Compiler_directiveContext::ifdef_directive() {
  return getRuleContext<GLSLPreParser::Ifdef_directiveContext>(0);
}

GLSLPreParser::Ifndef_directiveContext* GLSLPreParser::Compiler_directiveContext::ifndef_directive() {
  return getRuleContext<GLSLPreParser::Ifndef_directiveContext>(0);
}

GLSLPreParser::Line_directiveContext* GLSLPreParser::Compiler_directiveContext::line_directive() {
  return getRuleContext<GLSLPreParser::Line_directiveContext>(0);
}

GLSLPreParser::Pragma_directiveContext* GLSLPreParser::Compiler_directiveContext::pragma_directive() {
  return getRuleContext<GLSLPreParser::Pragma_directiveContext>(0);
}

GLSLPreParser::Undef_directiveContext* GLSLPreParser::Compiler_directiveContext::undef_directive() {
  return getRuleContext<GLSLPreParser::Undef_directiveContext>(0);
}

GLSLPreParser::Version_directiveContext* GLSLPreParser::Compiler_directiveContext::version_directive() {
  return getRuleContext<GLSLPreParser::Version_directiveContext>(0);
}


size_t GLSLPreParser::Compiler_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleCompiler_directive;
}

void GLSLPreParser::Compiler_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCompiler_directive(this);
}

void GLSLPreParser::Compiler_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCompiler_directive(this);
}


std::any GLSLPreParser::Compiler_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitCompiler_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Compiler_directiveContext* GLSLPreParser::compiler_directive() {
  Compiler_directiveContext *_localctx = _tracker.createInstance<Compiler_directiveContext>(_ctx, getState());
  enterRule(_localctx, 2, GLSLPreParser::RuleCompiler_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(87);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(74);
      define_directive();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(75);
      elif_directive();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(76);
      else_directive();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(77);
      endif_directive();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(78);
      error_directive();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(79);
      extension_directive();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(80);
      if_directive();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(81);
      ifdef_directive();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(82);
      ifndef_directive();
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(83);
      line_directive();
      break;
    }

    case 11: {
      enterOuterAlt(_localctx, 11);
      setState(84);
      pragma_directive();
      break;
    }

    case 12: {
      enterOuterAlt(_localctx, 12);
      setState(85);
      undef_directive();
      break;
    }

    case 13: {
      enterOuterAlt(_localctx, 13);
      setState(86);
      version_directive();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BehaviorContext ------------------------------------------------------------------

GLSLPreParser::BehaviorContext::BehaviorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::BehaviorContext::BEHAVIOR() {
  return getToken(GLSLPreParser::BEHAVIOR, 0);
}


size_t GLSLPreParser::BehaviorContext::getRuleIndex() const {
  return GLSLPreParser::RuleBehavior;
}

void GLSLPreParser::BehaviorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBehavior(this);
}

void GLSLPreParser::BehaviorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBehavior(this);
}


std::any GLSLPreParser::BehaviorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitBehavior(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::BehaviorContext* GLSLPreParser::behavior() {
  BehaviorContext *_localctx = _tracker.createInstance<BehaviorContext>(_ctx, getState());
  enterRule(_localctx, 4, GLSLPreParser::RuleBehavior);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(89);
    match(GLSLPreParser::BEHAVIOR);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Constant_expressionContext ------------------------------------------------------------------

GLSLPreParser::Constant_expressionContext::Constant_expressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Constant_expressionContext::CONSTANT_EXPRESSION() {
  return getToken(GLSLPreParser::CONSTANT_EXPRESSION, 0);
}


size_t GLSLPreParser::Constant_expressionContext::getRuleIndex() const {
  return GLSLPreParser::RuleConstant_expression;
}

void GLSLPreParser::Constant_expressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConstant_expression(this);
}

void GLSLPreParser::Constant_expressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConstant_expression(this);
}


std::any GLSLPreParser::Constant_expressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitConstant_expression(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Constant_expressionContext* GLSLPreParser::constant_expression() {
  Constant_expressionContext *_localctx = _tracker.createInstance<Constant_expressionContext>(_ctx, getState());
  enterRule(_localctx, 6, GLSLPreParser::RuleConstant_expression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(91);
    match(GLSLPreParser::CONSTANT_EXPRESSION);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Define_directiveContext ------------------------------------------------------------------

GLSLPreParser::Define_directiveContext::Define_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Define_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Define_directiveContext::DEFINE_DIRECTIVE() {
  return getToken(GLSLPreParser::DEFINE_DIRECTIVE, 0);
}

GLSLPreParser::Macro_nameContext* GLSLPreParser::Define_directiveContext::macro_name() {
  return getRuleContext<GLSLPreParser::Macro_nameContext>(0);
}

GLSLPreParser::Macro_textContext* GLSLPreParser::Define_directiveContext::macro_text() {
  return getRuleContext<GLSLPreParser::Macro_textContext>(0);
}


size_t GLSLPreParser::Define_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleDefine_directive;
}

void GLSLPreParser::Define_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefine_directive(this);
}

void GLSLPreParser::Define_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefine_directive(this);
}


std::any GLSLPreParser::Define_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitDefine_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Define_directiveContext* GLSLPreParser::define_directive() {
  Define_directiveContext *_localctx = _tracker.createInstance<Define_directiveContext>(_ctx, getState());
  enterRule(_localctx, 8, GLSLPreParser::RuleDefine_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(93);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(94);
    match(GLSLPreParser::DEFINE_DIRECTIVE);
    setState(95);
    macro_name();
    setState(96);
    macro_text();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Elif_directiveContext ------------------------------------------------------------------

GLSLPreParser::Elif_directiveContext::Elif_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Elif_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Elif_directiveContext::ELIF_DIRECTIVE() {
  return getToken(GLSLPreParser::ELIF_DIRECTIVE, 0);
}

GLSLPreParser::Constant_expressionContext* GLSLPreParser::Elif_directiveContext::constant_expression() {
  return getRuleContext<GLSLPreParser::Constant_expressionContext>(0);
}

GLSLPreParser::Group_of_linesContext* GLSLPreParser::Elif_directiveContext::group_of_lines() {
  return getRuleContext<GLSLPreParser::Group_of_linesContext>(0);
}


size_t GLSLPreParser::Elif_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleElif_directive;
}

void GLSLPreParser::Elif_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterElif_directive(this);
}

void GLSLPreParser::Elif_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitElif_directive(this);
}


std::any GLSLPreParser::Elif_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitElif_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Elif_directiveContext* GLSLPreParser::elif_directive() {
  Elif_directiveContext *_localctx = _tracker.createInstance<Elif_directiveContext>(_ctx, getState());
  enterRule(_localctx, 10, GLSLPreParser::RuleElif_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(98);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(99);
    match(GLSLPreParser::ELIF_DIRECTIVE);
    setState(100);
    constant_expression();
    setState(101);
    group_of_lines();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Else_directiveContext ------------------------------------------------------------------

GLSLPreParser::Else_directiveContext::Else_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Else_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Else_directiveContext::ELSE_DIRECTIVE() {
  return getToken(GLSLPreParser::ELSE_DIRECTIVE, 0);
}

GLSLPreParser::Group_of_linesContext* GLSLPreParser::Else_directiveContext::group_of_lines() {
  return getRuleContext<GLSLPreParser::Group_of_linesContext>(0);
}


size_t GLSLPreParser::Else_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleElse_directive;
}

void GLSLPreParser::Else_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterElse_directive(this);
}

void GLSLPreParser::Else_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitElse_directive(this);
}


std::any GLSLPreParser::Else_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitElse_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Else_directiveContext* GLSLPreParser::else_directive() {
  Else_directiveContext *_localctx = _tracker.createInstance<Else_directiveContext>(_ctx, getState());
  enterRule(_localctx, 12, GLSLPreParser::RuleElse_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(103);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(104);
    match(GLSLPreParser::ELSE_DIRECTIVE);
    setState(105);
    group_of_lines();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Endif_directiveContext ------------------------------------------------------------------

GLSLPreParser::Endif_directiveContext::Endif_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Endif_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Endif_directiveContext::ENDIF_DIRECTIVE() {
  return getToken(GLSLPreParser::ENDIF_DIRECTIVE, 0);
}


size_t GLSLPreParser::Endif_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleEndif_directive;
}

void GLSLPreParser::Endif_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEndif_directive(this);
}

void GLSLPreParser::Endif_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEndif_directive(this);
}


std::any GLSLPreParser::Endif_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitEndif_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Endif_directiveContext* GLSLPreParser::endif_directive() {
  Endif_directiveContext *_localctx = _tracker.createInstance<Endif_directiveContext>(_ctx, getState());
  enterRule(_localctx, 14, GLSLPreParser::RuleEndif_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(107);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(108);
    match(GLSLPreParser::ENDIF_DIRECTIVE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Error_directiveContext ------------------------------------------------------------------

GLSLPreParser::Error_directiveContext::Error_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Error_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Error_directiveContext::ERROR_DIRECTIVE() {
  return getToken(GLSLPreParser::ERROR_DIRECTIVE, 0);
}

GLSLPreParser::Error_messageContext* GLSLPreParser::Error_directiveContext::error_message() {
  return getRuleContext<GLSLPreParser::Error_messageContext>(0);
}


size_t GLSLPreParser::Error_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleError_directive;
}

void GLSLPreParser::Error_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterError_directive(this);
}

void GLSLPreParser::Error_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitError_directive(this);
}


std::any GLSLPreParser::Error_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitError_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Error_directiveContext* GLSLPreParser::error_directive() {
  Error_directiveContext *_localctx = _tracker.createInstance<Error_directiveContext>(_ctx, getState());
  enterRule(_localctx, 16, GLSLPreParser::RuleError_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(110);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(111);
    match(GLSLPreParser::ERROR_DIRECTIVE);
    setState(112);
    error_message();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Error_messageContext ------------------------------------------------------------------

GLSLPreParser::Error_messageContext::Error_messageContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Error_messageContext::ERROR_MESSAGE() {
  return getToken(GLSLPreParser::ERROR_MESSAGE, 0);
}


size_t GLSLPreParser::Error_messageContext::getRuleIndex() const {
  return GLSLPreParser::RuleError_message;
}

void GLSLPreParser::Error_messageContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterError_message(this);
}

void GLSLPreParser::Error_messageContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitError_message(this);
}


std::any GLSLPreParser::Error_messageContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitError_message(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Error_messageContext* GLSLPreParser::error_message() {
  Error_messageContext *_localctx = _tracker.createInstance<Error_messageContext>(_ctx, getState());
  enterRule(_localctx, 18, GLSLPreParser::RuleError_message);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(114);
    match(GLSLPreParser::ERROR_MESSAGE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Extension_directiveContext ------------------------------------------------------------------

GLSLPreParser::Extension_directiveContext::Extension_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Extension_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Extension_directiveContext::EXTENSION_DIRECTIVE() {
  return getToken(GLSLPreParser::EXTENSION_DIRECTIVE, 0);
}

GLSLPreParser::Extension_nameContext* GLSLPreParser::Extension_directiveContext::extension_name() {
  return getRuleContext<GLSLPreParser::Extension_nameContext>(0);
}

tree::TerminalNode* GLSLPreParser::Extension_directiveContext::COLON() {
  return getToken(GLSLPreParser::COLON, 0);
}

GLSLPreParser::BehaviorContext* GLSLPreParser::Extension_directiveContext::behavior() {
  return getRuleContext<GLSLPreParser::BehaviorContext>(0);
}


size_t GLSLPreParser::Extension_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleExtension_directive;
}

void GLSLPreParser::Extension_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExtension_directive(this);
}

void GLSLPreParser::Extension_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExtension_directive(this);
}


std::any GLSLPreParser::Extension_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitExtension_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Extension_directiveContext* GLSLPreParser::extension_directive() {
  Extension_directiveContext *_localctx = _tracker.createInstance<Extension_directiveContext>(_ctx, getState());
  enterRule(_localctx, 20, GLSLPreParser::RuleExtension_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(116);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(117);
    match(GLSLPreParser::EXTENSION_DIRECTIVE);
    setState(118);
    extension_name();
    setState(119);
    match(GLSLPreParser::COLON);
    setState(120);
    behavior();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Extension_nameContext ------------------------------------------------------------------

GLSLPreParser::Extension_nameContext::Extension_nameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Extension_nameContext::EXTENSION_NAME() {
  return getToken(GLSLPreParser::EXTENSION_NAME, 0);
}


size_t GLSLPreParser::Extension_nameContext::getRuleIndex() const {
  return GLSLPreParser::RuleExtension_name;
}

void GLSLPreParser::Extension_nameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExtension_name(this);
}

void GLSLPreParser::Extension_nameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExtension_name(this);
}


std::any GLSLPreParser::Extension_nameContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitExtension_name(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Extension_nameContext* GLSLPreParser::extension_name() {
  Extension_nameContext *_localctx = _tracker.createInstance<Extension_nameContext>(_ctx, getState());
  enterRule(_localctx, 22, GLSLPreParser::RuleExtension_name);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(122);
    match(GLSLPreParser::EXTENSION_NAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Group_of_linesContext ------------------------------------------------------------------

GLSLPreParser::Group_of_linesContext::Group_of_linesContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<GLSLPreParser::Program_textContext *> GLSLPreParser::Group_of_linesContext::program_text() {
  return getRuleContexts<GLSLPreParser::Program_textContext>();
}

GLSLPreParser::Program_textContext* GLSLPreParser::Group_of_linesContext::program_text(size_t i) {
  return getRuleContext<GLSLPreParser::Program_textContext>(i);
}

std::vector<GLSLPreParser::Compiler_directiveContext *> GLSLPreParser::Group_of_linesContext::compiler_directive() {
  return getRuleContexts<GLSLPreParser::Compiler_directiveContext>();
}

GLSLPreParser::Compiler_directiveContext* GLSLPreParser::Group_of_linesContext::compiler_directive(size_t i) {
  return getRuleContext<GLSLPreParser::Compiler_directiveContext>(i);
}


size_t GLSLPreParser::Group_of_linesContext::getRuleIndex() const {
  return GLSLPreParser::RuleGroup_of_lines;
}

void GLSLPreParser::Group_of_linesContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGroup_of_lines(this);
}

void GLSLPreParser::Group_of_linesContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGroup_of_lines(this);
}


std::any GLSLPreParser::Group_of_linesContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitGroup_of_lines(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Group_of_linesContext* GLSLPreParser::group_of_lines() {
  Group_of_linesContext *_localctx = _tracker.createInstance<Group_of_linesContext>(_ctx, getState());
  enterRule(_localctx, 24, GLSLPreParser::RuleGroup_of_lines);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(128);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(126);
        _errHandler->sync(this);
        switch (_input->LA(1)) {
          case GLSLPreParser::PROGRAM_TEXT: {
            setState(124);
            program_text();
            break;
          }

          case GLSLPreParser::NUMBER_SIGN: {
            setState(125);
            compiler_directive();
            break;
          }

        default:
          throw NoViableAltException(this);
        } 
      }
      setState(130);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- If_directiveContext ------------------------------------------------------------------

GLSLPreParser::If_directiveContext::If_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::If_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::If_directiveContext::IF_DIRECTIVE() {
  return getToken(GLSLPreParser::IF_DIRECTIVE, 0);
}

GLSLPreParser::Constant_expressionContext* GLSLPreParser::If_directiveContext::constant_expression() {
  return getRuleContext<GLSLPreParser::Constant_expressionContext>(0);
}

GLSLPreParser::Group_of_linesContext* GLSLPreParser::If_directiveContext::group_of_lines() {
  return getRuleContext<GLSLPreParser::Group_of_linesContext>(0);
}

GLSLPreParser::Endif_directiveContext* GLSLPreParser::If_directiveContext::endif_directive() {
  return getRuleContext<GLSLPreParser::Endif_directiveContext>(0);
}

std::vector<GLSLPreParser::Elif_directiveContext *> GLSLPreParser::If_directiveContext::elif_directive() {
  return getRuleContexts<GLSLPreParser::Elif_directiveContext>();
}

GLSLPreParser::Elif_directiveContext* GLSLPreParser::If_directiveContext::elif_directive(size_t i) {
  return getRuleContext<GLSLPreParser::Elif_directiveContext>(i);
}

GLSLPreParser::Else_directiveContext* GLSLPreParser::If_directiveContext::else_directive() {
  return getRuleContext<GLSLPreParser::Else_directiveContext>(0);
}


size_t GLSLPreParser::If_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleIf_directive;
}

void GLSLPreParser::If_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIf_directive(this);
}

void GLSLPreParser::If_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIf_directive(this);
}


std::any GLSLPreParser::If_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitIf_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::If_directiveContext* GLSLPreParser::if_directive() {
  If_directiveContext *_localctx = _tracker.createInstance<If_directiveContext>(_ctx, getState());
  enterRule(_localctx, 26, GLSLPreParser::RuleIf_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(131);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(132);
    match(GLSLPreParser::IF_DIRECTIVE);
    setState(133);
    constant_expression();
    setState(134);
    group_of_lines();
    setState(138);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(135);
        elif_directive(); 
      }
      setState(140);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
    }
    setState(142);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx)) {
    case 1: {
      setState(141);
      else_directive();
      break;
    }

    default:
      break;
    }
    setState(144);
    endif_directive();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Ifdef_directiveContext ------------------------------------------------------------------

GLSLPreParser::Ifdef_directiveContext::Ifdef_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Ifdef_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Ifdef_directiveContext::IFDEF_DIRECTIVE() {
  return getToken(GLSLPreParser::IFDEF_DIRECTIVE, 0);
}

GLSLPreParser::Macro_identifierContext* GLSLPreParser::Ifdef_directiveContext::macro_identifier() {
  return getRuleContext<GLSLPreParser::Macro_identifierContext>(0);
}

GLSLPreParser::Group_of_linesContext* GLSLPreParser::Ifdef_directiveContext::group_of_lines() {
  return getRuleContext<GLSLPreParser::Group_of_linesContext>(0);
}

GLSLPreParser::Endif_directiveContext* GLSLPreParser::Ifdef_directiveContext::endif_directive() {
  return getRuleContext<GLSLPreParser::Endif_directiveContext>(0);
}

std::vector<GLSLPreParser::Elif_directiveContext *> GLSLPreParser::Ifdef_directiveContext::elif_directive() {
  return getRuleContexts<GLSLPreParser::Elif_directiveContext>();
}

GLSLPreParser::Elif_directiveContext* GLSLPreParser::Ifdef_directiveContext::elif_directive(size_t i) {
  return getRuleContext<GLSLPreParser::Elif_directiveContext>(i);
}

GLSLPreParser::Else_directiveContext* GLSLPreParser::Ifdef_directiveContext::else_directive() {
  return getRuleContext<GLSLPreParser::Else_directiveContext>(0);
}


size_t GLSLPreParser::Ifdef_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleIfdef_directive;
}

void GLSLPreParser::Ifdef_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIfdef_directive(this);
}

void GLSLPreParser::Ifdef_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIfdef_directive(this);
}


std::any GLSLPreParser::Ifdef_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitIfdef_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Ifdef_directiveContext* GLSLPreParser::ifdef_directive() {
  Ifdef_directiveContext *_localctx = _tracker.createInstance<Ifdef_directiveContext>(_ctx, getState());
  enterRule(_localctx, 28, GLSLPreParser::RuleIfdef_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(146);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(147);
    match(GLSLPreParser::IFDEF_DIRECTIVE);
    setState(148);
    macro_identifier();
    setState(149);
    group_of_lines();
    setState(153);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(150);
        elif_directive(); 
      }
      setState(155);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx);
    }
    setState(157);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      setState(156);
      else_directive();
      break;
    }

    default:
      break;
    }
    setState(159);
    endif_directive();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Ifndef_directiveContext ------------------------------------------------------------------

GLSLPreParser::Ifndef_directiveContext::Ifndef_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Ifndef_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Ifndef_directiveContext::IFNDEF_DIRECTIVE() {
  return getToken(GLSLPreParser::IFNDEF_DIRECTIVE, 0);
}

GLSLPreParser::Macro_identifierContext* GLSLPreParser::Ifndef_directiveContext::macro_identifier() {
  return getRuleContext<GLSLPreParser::Macro_identifierContext>(0);
}

GLSLPreParser::Group_of_linesContext* GLSLPreParser::Ifndef_directiveContext::group_of_lines() {
  return getRuleContext<GLSLPreParser::Group_of_linesContext>(0);
}

GLSLPreParser::Endif_directiveContext* GLSLPreParser::Ifndef_directiveContext::endif_directive() {
  return getRuleContext<GLSLPreParser::Endif_directiveContext>(0);
}

std::vector<GLSLPreParser::Elif_directiveContext *> GLSLPreParser::Ifndef_directiveContext::elif_directive() {
  return getRuleContexts<GLSLPreParser::Elif_directiveContext>();
}

GLSLPreParser::Elif_directiveContext* GLSLPreParser::Ifndef_directiveContext::elif_directive(size_t i) {
  return getRuleContext<GLSLPreParser::Elif_directiveContext>(i);
}

GLSLPreParser::Else_directiveContext* GLSLPreParser::Ifndef_directiveContext::else_directive() {
  return getRuleContext<GLSLPreParser::Else_directiveContext>(0);
}


size_t GLSLPreParser::Ifndef_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleIfndef_directive;
}

void GLSLPreParser::Ifndef_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIfndef_directive(this);
}

void GLSLPreParser::Ifndef_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIfndef_directive(this);
}


std::any GLSLPreParser::Ifndef_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitIfndef_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Ifndef_directiveContext* GLSLPreParser::ifndef_directive() {
  Ifndef_directiveContext *_localctx = _tracker.createInstance<Ifndef_directiveContext>(_ctx, getState());
  enterRule(_localctx, 30, GLSLPreParser::RuleIfndef_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(161);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(162);
    match(GLSLPreParser::IFNDEF_DIRECTIVE);
    setState(163);
    macro_identifier();
    setState(164);
    group_of_lines();
    setState(168);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(165);
        elif_directive(); 
      }
      setState(170);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx);
    }
    setState(172);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      setState(171);
      else_directive();
      break;
    }

    default:
      break;
    }
    setState(174);
    endif_directive();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Line_directiveContext ------------------------------------------------------------------

GLSLPreParser::Line_directiveContext::Line_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Line_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Line_directiveContext::LINE_DIRECTIVE() {
  return getToken(GLSLPreParser::LINE_DIRECTIVE, 0);
}

GLSLPreParser::Line_expressionContext* GLSLPreParser::Line_directiveContext::line_expression() {
  return getRuleContext<GLSLPreParser::Line_expressionContext>(0);
}


size_t GLSLPreParser::Line_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleLine_directive;
}

void GLSLPreParser::Line_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLine_directive(this);
}

void GLSLPreParser::Line_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLine_directive(this);
}


std::any GLSLPreParser::Line_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitLine_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Line_directiveContext* GLSLPreParser::line_directive() {
  Line_directiveContext *_localctx = _tracker.createInstance<Line_directiveContext>(_ctx, getState());
  enterRule(_localctx, 32, GLSLPreParser::RuleLine_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(176);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(177);
    match(GLSLPreParser::LINE_DIRECTIVE);
    setState(178);
    line_expression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Line_expressionContext ------------------------------------------------------------------

GLSLPreParser::Line_expressionContext::Line_expressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Line_expressionContext::LINE_EXPRESSION() {
  return getToken(GLSLPreParser::LINE_EXPRESSION, 0);
}


size_t GLSLPreParser::Line_expressionContext::getRuleIndex() const {
  return GLSLPreParser::RuleLine_expression;
}

void GLSLPreParser::Line_expressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLine_expression(this);
}

void GLSLPreParser::Line_expressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLine_expression(this);
}


std::any GLSLPreParser::Line_expressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitLine_expression(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Line_expressionContext* GLSLPreParser::line_expression() {
  Line_expressionContext *_localctx = _tracker.createInstance<Line_expressionContext>(_ctx, getState());
  enterRule(_localctx, 34, GLSLPreParser::RuleLine_expression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(180);
    match(GLSLPreParser::LINE_EXPRESSION);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Macro_esc_newlineContext ------------------------------------------------------------------

GLSLPreParser::Macro_esc_newlineContext::Macro_esc_newlineContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Macro_esc_newlineContext::MACRO_ESC_NEWLINE() {
  return getToken(GLSLPreParser::MACRO_ESC_NEWLINE, 0);
}


size_t GLSLPreParser::Macro_esc_newlineContext::getRuleIndex() const {
  return GLSLPreParser::RuleMacro_esc_newline;
}

void GLSLPreParser::Macro_esc_newlineContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMacro_esc_newline(this);
}

void GLSLPreParser::Macro_esc_newlineContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMacro_esc_newline(this);
}


std::any GLSLPreParser::Macro_esc_newlineContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitMacro_esc_newline(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Macro_esc_newlineContext* GLSLPreParser::macro_esc_newline() {
  Macro_esc_newlineContext *_localctx = _tracker.createInstance<Macro_esc_newlineContext>(_ctx, getState());
  enterRule(_localctx, 36, GLSLPreParser::RuleMacro_esc_newline);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(182);
    match(GLSLPreParser::MACRO_ESC_NEWLINE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Macro_identifierContext ------------------------------------------------------------------

GLSLPreParser::Macro_identifierContext::Macro_identifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Macro_identifierContext::MACRO_IDENTIFIER() {
  return getToken(GLSLPreParser::MACRO_IDENTIFIER, 0);
}


size_t GLSLPreParser::Macro_identifierContext::getRuleIndex() const {
  return GLSLPreParser::RuleMacro_identifier;
}

void GLSLPreParser::Macro_identifierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMacro_identifier(this);
}

void GLSLPreParser::Macro_identifierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMacro_identifier(this);
}


std::any GLSLPreParser::Macro_identifierContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitMacro_identifier(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Macro_identifierContext* GLSLPreParser::macro_identifier() {
  Macro_identifierContext *_localctx = _tracker.createInstance<Macro_identifierContext>(_ctx, getState());
  enterRule(_localctx, 38, GLSLPreParser::RuleMacro_identifier);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(184);
    match(GLSLPreParser::MACRO_IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Macro_nameContext ------------------------------------------------------------------

GLSLPreParser::Macro_nameContext::Macro_nameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Macro_nameContext::MACRO_NAME() {
  return getToken(GLSLPreParser::MACRO_NAME, 0);
}


size_t GLSLPreParser::Macro_nameContext::getRuleIndex() const {
  return GLSLPreParser::RuleMacro_name;
}

void GLSLPreParser::Macro_nameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMacro_name(this);
}

void GLSLPreParser::Macro_nameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMacro_name(this);
}


std::any GLSLPreParser::Macro_nameContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitMacro_name(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Macro_nameContext* GLSLPreParser::macro_name() {
  Macro_nameContext *_localctx = _tracker.createInstance<Macro_nameContext>(_ctx, getState());
  enterRule(_localctx, 40, GLSLPreParser::RuleMacro_name);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(186);
    match(GLSLPreParser::MACRO_NAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Macro_textContext ------------------------------------------------------------------

GLSLPreParser::Macro_textContext::Macro_textContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<GLSLPreParser::Macro_text_Context *> GLSLPreParser::Macro_textContext::macro_text_() {
  return getRuleContexts<GLSLPreParser::Macro_text_Context>();
}

GLSLPreParser::Macro_text_Context* GLSLPreParser::Macro_textContext::macro_text_(size_t i) {
  return getRuleContext<GLSLPreParser::Macro_text_Context>(i);
}

std::vector<GLSLPreParser::Macro_esc_newlineContext *> GLSLPreParser::Macro_textContext::macro_esc_newline() {
  return getRuleContexts<GLSLPreParser::Macro_esc_newlineContext>();
}

GLSLPreParser::Macro_esc_newlineContext* GLSLPreParser::Macro_textContext::macro_esc_newline(size_t i) {
  return getRuleContext<GLSLPreParser::Macro_esc_newlineContext>(i);
}


size_t GLSLPreParser::Macro_textContext::getRuleIndex() const {
  return GLSLPreParser::RuleMacro_text;
}

void GLSLPreParser::Macro_textContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMacro_text(this);
}

void GLSLPreParser::Macro_textContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMacro_text(this);
}


std::any GLSLPreParser::Macro_textContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitMacro_text(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Macro_textContext* GLSLPreParser::macro_text() {
  Macro_textContext *_localctx = _tracker.createInstance<Macro_textContext>(_ctx, getState());
  enterRule(_localctx, 42, GLSLPreParser::RuleMacro_text);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(192);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == GLSLPreParser::MACRO_ESC_NEWLINE

    || _la == GLSLPreParser::MACRO_TEXT) {
      setState(190);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case GLSLPreParser::MACRO_TEXT: {
          setState(188);
          macro_text_();
          break;
        }

        case GLSLPreParser::MACRO_ESC_NEWLINE: {
          setState(189);
          macro_esc_newline();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(194);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Macro_text_Context ------------------------------------------------------------------

GLSLPreParser::Macro_text_Context::Macro_text_Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Macro_text_Context::MACRO_TEXT() {
  return getToken(GLSLPreParser::MACRO_TEXT, 0);
}


size_t GLSLPreParser::Macro_text_Context::getRuleIndex() const {
  return GLSLPreParser::RuleMacro_text_;
}

void GLSLPreParser::Macro_text_Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMacro_text_(this);
}

void GLSLPreParser::Macro_text_Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMacro_text_(this);
}


std::any GLSLPreParser::Macro_text_Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitMacro_text_(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Macro_text_Context* GLSLPreParser::macro_text_() {
  Macro_text_Context *_localctx = _tracker.createInstance<Macro_text_Context>(_ctx, getState());
  enterRule(_localctx, 44, GLSLPreParser::RuleMacro_text_);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(195);
    match(GLSLPreParser::MACRO_TEXT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumberContext ------------------------------------------------------------------

GLSLPreParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::NumberContext::NUMBER() {
  return getToken(GLSLPreParser::NUMBER, 0);
}


size_t GLSLPreParser::NumberContext::getRuleIndex() const {
  return GLSLPreParser::RuleNumber;
}

void GLSLPreParser::NumberContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNumber(this);
}

void GLSLPreParser::NumberContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNumber(this);
}


std::any GLSLPreParser::NumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitNumber(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::NumberContext* GLSLPreParser::number() {
  NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 46, GLSLPreParser::RuleNumber);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(197);
    match(GLSLPreParser::NUMBER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OffContext ------------------------------------------------------------------

GLSLPreParser::OffContext::OffContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::OffContext::OFF() {
  return getToken(GLSLPreParser::OFF, 0);
}


size_t GLSLPreParser::OffContext::getRuleIndex() const {
  return GLSLPreParser::RuleOff;
}

void GLSLPreParser::OffContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOff(this);
}

void GLSLPreParser::OffContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOff(this);
}


std::any GLSLPreParser::OffContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitOff(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::OffContext* GLSLPreParser::off() {
  OffContext *_localctx = _tracker.createInstance<OffContext>(_ctx, getState());
  enterRule(_localctx, 48, GLSLPreParser::RuleOff);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(199);
    match(GLSLPreParser::OFF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OnContext ------------------------------------------------------------------

GLSLPreParser::OnContext::OnContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::OnContext::ON() {
  return getToken(GLSLPreParser::ON, 0);
}


size_t GLSLPreParser::OnContext::getRuleIndex() const {
  return GLSLPreParser::RuleOn;
}

void GLSLPreParser::OnContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOn(this);
}

void GLSLPreParser::OnContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOn(this);
}


std::any GLSLPreParser::OnContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitOn(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::OnContext* GLSLPreParser::on() {
  OnContext *_localctx = _tracker.createInstance<OnContext>(_ctx, getState());
  enterRule(_localctx, 50, GLSLPreParser::RuleOn);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(201);
    match(GLSLPreParser::ON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Pragma_debugContext ------------------------------------------------------------------

GLSLPreParser::Pragma_debugContext::Pragma_debugContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Pragma_debugContext::DEBUG() {
  return getToken(GLSLPreParser::DEBUG, 0);
}

tree::TerminalNode* GLSLPreParser::Pragma_debugContext::LEFT_PAREN() {
  return getToken(GLSLPreParser::LEFT_PAREN, 0);
}

tree::TerminalNode* GLSLPreParser::Pragma_debugContext::RIGHT_PAREN() {
  return getToken(GLSLPreParser::RIGHT_PAREN, 0);
}

GLSLPreParser::OnContext* GLSLPreParser::Pragma_debugContext::on() {
  return getRuleContext<GLSLPreParser::OnContext>(0);
}

GLSLPreParser::OffContext* GLSLPreParser::Pragma_debugContext::off() {
  return getRuleContext<GLSLPreParser::OffContext>(0);
}


size_t GLSLPreParser::Pragma_debugContext::getRuleIndex() const {
  return GLSLPreParser::RulePragma_debug;
}

void GLSLPreParser::Pragma_debugContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPragma_debug(this);
}

void GLSLPreParser::Pragma_debugContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPragma_debug(this);
}


std::any GLSLPreParser::Pragma_debugContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitPragma_debug(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Pragma_debugContext* GLSLPreParser::pragma_debug() {
  Pragma_debugContext *_localctx = _tracker.createInstance<Pragma_debugContext>(_ctx, getState());
  enterRule(_localctx, 52, GLSLPreParser::RulePragma_debug);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(203);
    match(GLSLPreParser::DEBUG);
    setState(204);
    match(GLSLPreParser::LEFT_PAREN);
    setState(207);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case GLSLPreParser::ON: {
        setState(205);
        on();
        break;
      }

      case GLSLPreParser::OFF: {
        setState(206);
        off();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(209);
    match(GLSLPreParser::RIGHT_PAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Pragma_directiveContext ------------------------------------------------------------------

GLSLPreParser::Pragma_directiveContext::Pragma_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Pragma_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Pragma_directiveContext::PRAGMA_DIRECTIVE() {
  return getToken(GLSLPreParser::PRAGMA_DIRECTIVE, 0);
}

GLSLPreParser::StdglContext* GLSLPreParser::Pragma_directiveContext::stdgl() {
  return getRuleContext<GLSLPreParser::StdglContext>(0);
}

GLSLPreParser::Pragma_debugContext* GLSLPreParser::Pragma_directiveContext::pragma_debug() {
  return getRuleContext<GLSLPreParser::Pragma_debugContext>(0);
}

GLSLPreParser::Pragma_optimizeContext* GLSLPreParser::Pragma_directiveContext::pragma_optimize() {
  return getRuleContext<GLSLPreParser::Pragma_optimizeContext>(0);
}


size_t GLSLPreParser::Pragma_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RulePragma_directive;
}

void GLSLPreParser::Pragma_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPragma_directive(this);
}

void GLSLPreParser::Pragma_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPragma_directive(this);
}


std::any GLSLPreParser::Pragma_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitPragma_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Pragma_directiveContext* GLSLPreParser::pragma_directive() {
  Pragma_directiveContext *_localctx = _tracker.createInstance<Pragma_directiveContext>(_ctx, getState());
  enterRule(_localctx, 54, GLSLPreParser::RulePragma_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(211);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(212);
    match(GLSLPreParser::PRAGMA_DIRECTIVE);
    setState(216);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case GLSLPreParser::STDGL: {
        setState(213);
        stdgl();
        break;
      }

      case GLSLPreParser::DEBUG: {
        setState(214);
        pragma_debug();
        break;
      }

      case GLSLPreParser::OPTIMIZE: {
        setState(215);
        pragma_optimize();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Pragma_optimizeContext ------------------------------------------------------------------

GLSLPreParser::Pragma_optimizeContext::Pragma_optimizeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Pragma_optimizeContext::OPTIMIZE() {
  return getToken(GLSLPreParser::OPTIMIZE, 0);
}

tree::TerminalNode* GLSLPreParser::Pragma_optimizeContext::LEFT_PAREN() {
  return getToken(GLSLPreParser::LEFT_PAREN, 0);
}

tree::TerminalNode* GLSLPreParser::Pragma_optimizeContext::RIGHT_PAREN() {
  return getToken(GLSLPreParser::RIGHT_PAREN, 0);
}

GLSLPreParser::OnContext* GLSLPreParser::Pragma_optimizeContext::on() {
  return getRuleContext<GLSLPreParser::OnContext>(0);
}

GLSLPreParser::OffContext* GLSLPreParser::Pragma_optimizeContext::off() {
  return getRuleContext<GLSLPreParser::OffContext>(0);
}


size_t GLSLPreParser::Pragma_optimizeContext::getRuleIndex() const {
  return GLSLPreParser::RulePragma_optimize;
}

void GLSLPreParser::Pragma_optimizeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPragma_optimize(this);
}

void GLSLPreParser::Pragma_optimizeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPragma_optimize(this);
}


std::any GLSLPreParser::Pragma_optimizeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitPragma_optimize(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Pragma_optimizeContext* GLSLPreParser::pragma_optimize() {
  Pragma_optimizeContext *_localctx = _tracker.createInstance<Pragma_optimizeContext>(_ctx, getState());
  enterRule(_localctx, 56, GLSLPreParser::RulePragma_optimize);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(218);
    match(GLSLPreParser::OPTIMIZE);
    setState(219);
    match(GLSLPreParser::LEFT_PAREN);
    setState(222);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case GLSLPreParser::ON: {
        setState(220);
        on();
        break;
      }

      case GLSLPreParser::OFF: {
        setState(221);
        off();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(224);
    match(GLSLPreParser::RIGHT_PAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ProfileContext ------------------------------------------------------------------

GLSLPreParser::ProfileContext::ProfileContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::ProfileContext::PROFILE() {
  return getToken(GLSLPreParser::PROFILE, 0);
}


size_t GLSLPreParser::ProfileContext::getRuleIndex() const {
  return GLSLPreParser::RuleProfile;
}

void GLSLPreParser::ProfileContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProfile(this);
}

void GLSLPreParser::ProfileContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProfile(this);
}


std::any GLSLPreParser::ProfileContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitProfile(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::ProfileContext* GLSLPreParser::profile() {
  ProfileContext *_localctx = _tracker.createInstance<ProfileContext>(_ctx, getState());
  enterRule(_localctx, 58, GLSLPreParser::RuleProfile);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(226);
    match(GLSLPreParser::PROFILE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Program_textContext ------------------------------------------------------------------

GLSLPreParser::Program_textContext::Program_textContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Program_textContext::PROGRAM_TEXT() {
  return getToken(GLSLPreParser::PROGRAM_TEXT, 0);
}


size_t GLSLPreParser::Program_textContext::getRuleIndex() const {
  return GLSLPreParser::RuleProgram_text;
}

void GLSLPreParser::Program_textContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram_text(this);
}

void GLSLPreParser::Program_textContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram_text(this);
}


std::any GLSLPreParser::Program_textContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitProgram_text(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Program_textContext* GLSLPreParser::program_text() {
  Program_textContext *_localctx = _tracker.createInstance<Program_textContext>(_ctx, getState());
  enterRule(_localctx, 60, GLSLPreParser::RuleProgram_text);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(228);
    match(GLSLPreParser::PROGRAM_TEXT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StdglContext ------------------------------------------------------------------

GLSLPreParser::StdglContext::StdglContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::StdglContext::STDGL() {
  return getToken(GLSLPreParser::STDGL, 0);
}


size_t GLSLPreParser::StdglContext::getRuleIndex() const {
  return GLSLPreParser::RuleStdgl;
}

void GLSLPreParser::StdglContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStdgl(this);
}

void GLSLPreParser::StdglContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStdgl(this);
}


std::any GLSLPreParser::StdglContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitStdgl(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::StdglContext* GLSLPreParser::stdgl() {
  StdglContext *_localctx = _tracker.createInstance<StdglContext>(_ctx, getState());
  enterRule(_localctx, 62, GLSLPreParser::RuleStdgl);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(230);
    match(GLSLPreParser::STDGL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Undef_directiveContext ------------------------------------------------------------------

GLSLPreParser::Undef_directiveContext::Undef_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Undef_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Undef_directiveContext::UNDEF_DIRECTIVE() {
  return getToken(GLSLPreParser::UNDEF_DIRECTIVE, 0);
}

GLSLPreParser::Macro_identifierContext* GLSLPreParser::Undef_directiveContext::macro_identifier() {
  return getRuleContext<GLSLPreParser::Macro_identifierContext>(0);
}


size_t GLSLPreParser::Undef_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleUndef_directive;
}

void GLSLPreParser::Undef_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUndef_directive(this);
}

void GLSLPreParser::Undef_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUndef_directive(this);
}


std::any GLSLPreParser::Undef_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitUndef_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Undef_directiveContext* GLSLPreParser::undef_directive() {
  Undef_directiveContext *_localctx = _tracker.createInstance<Undef_directiveContext>(_ctx, getState());
  enterRule(_localctx, 64, GLSLPreParser::RuleUndef_directive);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(232);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(233);
    match(GLSLPreParser::UNDEF_DIRECTIVE);
    setState(234);
    macro_identifier();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Version_directiveContext ------------------------------------------------------------------

GLSLPreParser::Version_directiveContext::Version_directiveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* GLSLPreParser::Version_directiveContext::NUMBER_SIGN() {
  return getToken(GLSLPreParser::NUMBER_SIGN, 0);
}

tree::TerminalNode* GLSLPreParser::Version_directiveContext::VERSION_DIRECTIVE() {
  return getToken(GLSLPreParser::VERSION_DIRECTIVE, 0);
}

GLSLPreParser::NumberContext* GLSLPreParser::Version_directiveContext::number() {
  return getRuleContext<GLSLPreParser::NumberContext>(0);
}

GLSLPreParser::ProfileContext* GLSLPreParser::Version_directiveContext::profile() {
  return getRuleContext<GLSLPreParser::ProfileContext>(0);
}


size_t GLSLPreParser::Version_directiveContext::getRuleIndex() const {
  return GLSLPreParser::RuleVersion_directive;
}

void GLSLPreParser::Version_directiveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVersion_directive(this);
}

void GLSLPreParser::Version_directiveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<GLSLPreParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVersion_directive(this);
}


std::any GLSLPreParser::Version_directiveContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<GLSLPreParserVisitor*>(visitor))
    return parserVisitor->visitVersion_directive(this);
  else
    return visitor->visitChildren(this);
}

GLSLPreParser::Version_directiveContext* GLSLPreParser::version_directive() {
  Version_directiveContext *_localctx = _tracker.createInstance<Version_directiveContext>(_ctx, getState());
  enterRule(_localctx, 66, GLSLPreParser::RuleVersion_directive);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(236);
    match(GLSLPreParser::NUMBER_SIGN);
    setState(237);
    match(GLSLPreParser::VERSION_DIRECTIVE);
    setState(238);
    number();
    setState(240);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == GLSLPreParser::PROFILE) {
      setState(239);
      profile();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void GLSLPreParser::initialize() {
  std::call_once(glslpreparserParserOnceFlag, glslpreparserParserInitialize);
}
