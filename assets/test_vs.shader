DXBC°EV!+{ ≥mЩ3Kыы$   8<     8   d  ш  P  Ф  0  RDEF$     †      <    ю€  р  RD11<          (   $          |                             О                            InstanceConstants FrameConstants ЂЂЂ|      –   @           О      0  Р           ш       @            €€€€    €€€€    matModel float4x4 ЂЂ                              ®      @            €€€€    €€€€    ∞  @   @            €€€€    €€€€    Є  А          ћ      €€€€    €€€€    matView matProj cameraPos float4 ЂЂЂ                            ¬  Microsoft (R) HLSL Shader Compiler 6.3.9600.16384 ЂЂISGNМ         h                    q                    x                    А                   POSITION NORMAL TANGENT TEXCOORD ЂЂЂOSGNP         8                    D                   SV_POSITION TEXCOORD ЂЂЂSHEX<  P  П   jИ Y  FО         Y  FО        _  т     _  2    g  т         e  2     h             F     FО            "      F     FО           B      F     FО           В      F     FО                F     FО           "     F     FО          B     F     FО          В     F     FО                F    FО          "      F    FО          B      F    FО          В      F    FО        6  т      F     6  2     F    6  т      F     6  2     F     >  STATФ                                                                                                                                                 SPDB 6  Microsoft C/C++ MSF 7.00
DS            Ф                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ј€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€8  ш€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€       <       €€€€                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         Ф.1∞aZ   Ю9amE‘юNШEњеѓј.W                          AС2                                                                                                                                                                                                                                                                                                                                                                                                                                                                    ;

VS_OUTPUT main(VS_INPUT input)
{
	//float4 pos = mul(mul(mul(input.position, matModel), matView), matProj);
	//float4 pos = mul(mul(input.position, matView), matProj);

	float4 pos = mul(input.position, matModel);
	pos = mul(pos, matView);
	pos = mul(pos, matProj);

	VS_OUTPUT output;
	output.position = pos;
	output.texcoord = input.texcoord;

	return output;
}
    V   +               ,                                                                                                    ws  3M  /,  “m  IL  L  √I  ‘{  р1  ‘(  №|  ≠Z  б/                                                                                                                                                                                                                                                                                                                                                                                                                                                                             cbuffer InstanceConstants : register (b0)
{
	matrix		matModel;
}

cbuffer FrameConstants : register (b1)
{
	matrix		matView;
	matrix		matProj;
	float4		cameraPos;
};

struct VS_INPUT
{
	float4		position	: POSITION;
	float3		normal		: NORMAL;
	float3		tangent		: TANGENT;
	float2		texcoord	: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4		position	: SV_POSITION;
	float2		texcoord	: TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
	//float4 pos = mul(mul(mul(input.position, matModel), matView), matProj);
	//float4 pos = mul(mul(input.position, matView), matProj);

	float4 pos = mul(input.position, matModel);
	pos = mul(pos, matView);
	pos = mul(pos, matProj);

	VS_OUTPUT output;
	output.position = pos;
	output.texcoord = input.texcoord;

	return output;
}
                                                                                                                                                                                                                                  юпюп   u   D:\Profiles\cs2083\tofu\tofu\test_vs.hlsl  d:\profiles\cs2083\tofu\tofu\test_vs.hlsl cbuffer InstanceConstants : register (b0)
{
	matrix		matModel;
}

cbuffer FrameConstants : register (b1)
{
	matrix		matView;
	matrix		matProj;
	float4		cameraPos;
};

struct VS_INPUT
{
	float4		position	: POSITION;
	float3		normal		: NORMAL;
	float3		tangent		: TANGENT;
	float2		texcoord	: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4		position	: SV_POSITION;
	float2		texcoord	: TEXCOORD0;
}в0А   "оp)^”                                                               ,   (   в0ѕ%%9     +   ,                                                                                                                                                                                                                                                                                                                                                                                                                  J <      @А%   @А%Microsoft (R) HLSL Shader Compiler 6.3.9600.16384 6 =hlslFlags 0x5 hlslTarget vs_5_0 hlslEntry main     *     М      ‘      ‘  	  h    †main . >  	 input                                  P     h    ‘     P    h    ‘    P    h    ‘    P    h    ‘    P    h    ‘    P    h    ‘    P    h    ‘    P    h    ‘     P     h    ‘$    P  $  h    ‘(    P  (  h    ‘0    P  ,  h    ‘4   : >  И <main return value>                                P    h    ‘    P    h    ‘    P     h    ‘     P    h    ‘    P    h    ‘    P    h    ‘   * >    pos                                P      И    іА А      P     ®    ФА А     P     »    tА А     P     и    TА А     P         А     P     (   А     P     H   А     P     h   А    . >   output                                 P      ь   @      P     ь   @     P     ь   @     P     ь   @     P        ,     P        ,      ф         эЌ°ЄшдіJD=ш`	Ђ  т   ∞        <      "   §  h      Аh       И      АИ       ®      А®       »      А»       и   !  Аи   !     !  А  !   (  !  А(  !   H  !  АH  !   h  "  Аh  "   И  "  АИ  "   ®  "  А®  "   »  "  А»  "   и  %  Аи  %   ь  &  Аь  &     (  А  (   $  (  А$  (   8  (  А8  (    ,  +  ,  +  ,  +  ,  +                                      "  !             ц                    4   P                                                                                                                                                                                                                                                                                                                                                                                            18        h  
 €€   А      4   4      <        @       float4 утс @       float3 утс @       float2 утсR       position с    normal утс    tangent тс   ( texcoord с              0 VS_INPUT с
      *       position с    texcoord с               VS_OUTPUT 
             @             @ float4x4 
 
    
                                                                                                      18              €€   А                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  D3DSHDR <                             `                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        €€€€	/с      Q                  5                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  $                                                                                                                                                                                                                                                                                                                                                                                                                                                                %    И    main   Q       €€€€€€matModel     Q      €€€€€€matView  Q    @ €€€€€€matProj                                                                                                                                                                                                                                                                                                                                                                                                                                                €€€€	/с                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            €€€€w	1     Л Ј√   L       ,   8                                    <     `             	 Р      д     иmf        main none   -Ї.с a     <     `  e                 €€€€    <        €€€€    €€€€         D:\Profiles\cs2083\tofu\tofu\test_vs.hlsl   юпюп                  €€€€€€€€€€ €€€€€€€€€€                                                                                                                                                                                                 Ф.1∞aZ   Ю9amE‘юNШEњеѓј.WW   /LinkInfo /names /src/headerblock /src/files/d:\profiles\cs2083\tofu\tofu\test_vs.hlsl                 "            
                 AС2                                                                                                                                                                                                                                                                                                                                                    ≥   †  ?  8       •  А     И  <       (   D  ,   l                  
            	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            