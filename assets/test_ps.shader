DXBCdS1en¶С9?”   $2     <     t  |0  ш0  Ш1  р1  Aon9ƒ  ƒ   €€Ь  (     (   (   (  $   (     €€ю€W DBUG(   $         D      H      ь   h   D:\cs2083\tofu\test_ps.hlsl (     €€d    €€p     |     М  main input position            texcoord ЂЂЂ           s   |   М   Ш          ®         €€€€                           h   m   Є      »       h   ‘      д   Microsoft (R) HLSL Shader Compiler 6.3.9600.16384 ЂЂ     А  ∞     Р †B    А  д∞ д†   А  дА€€  SHDRd   @      Z   `     X  p     UU  b 2    e  т      E  	т      F    F~      `     >  SPDB .  Microsoft C/C++ MSF 7.00
DS            Д                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ј€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€8 А€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€       <       €€€€                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         Ф.1mн	Z   4c5x\хO∞Z“∞(h6                           AС2                                                                                                                                                                                                                                                                                                                                                                                                                                                                    D3DSHDR d                              `                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ws  /,  JN  OL  “A  Кn  ≠"  пW  р1  ,  Mu  ≠Z  б/                                                                                                                                                                                                                                                                                                                                                                                                                                                                             struct PS_INPUT
{
	float4		position	: SV_POSITION;
	float2		texcoord	: TEXCOORD0;
};

Texture2D tex : register(t0);

SamplerState samp : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	return tex.Sample(samp, input.texcoord);
}                                                                                                                                                                                                                                                                      юпюп   5   D:\cs2083\tofu\test_ps.hlsl  d:\cs2083\tofu\test_ps.hlsl struct PS_INPUT
{
	float4		position	: SV_POSITION;
	float2		texcoord	: TEXCOORD0;
};

Texture2D tex : register(t0);

SamplerState samp : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	return tex.Sample(samp, input.texcoord);
}                         :                                                                                                                                                                 в0А   љЙ§≤\”                                                                  (   в0АBъ                                                                                                                                                                                                                                                                                                                                                                                                                           J <      @А%   @А%Microsoft (R) HLSL Shader Compiler 6.3.9600.16384 > =hlslFlags 0x5 hlslTarget ps_4_0_level_9_3 hlslEntry main   *           (       (     <    †main . >  	 input                                  P     <    (      P    <    (     P    <    (     P    <    (     P    <    (     P    <    (    : >  И <main return value>                                P     <    (      P    <    (     P    <    (     P    <    (      ф         РЁ]∆}XЌжQI  т   H         d          <   <     А<      `     А`       ) 	 (  )  ) ц                    ,                                                                                                                                                                                                                                                                                                                                                            18        ф   
 €€   А      4   4      <        @       float4 утс @       float2 утс*       position с    texcoord с               PS_INPUT с
      
                 0  тс
     
          
    тс
 
    
                                                                                                                                                                                                                          18              €€   А                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  €€€€	/с                 -                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           %    Р    main   Q	   €€€€  €€€€tex  Q   €€€€€€  €€samp                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       €€€€	/с                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            €€€€w	1     Л Ј√   L       ,   (                                    d      `             	       |      xzS        main none   -Ї.с       d      `                    €€€€    d         €€€€    €€€€         D:\cs2083\tofu\test_ps.hlsl юпюп                  €€€€€€€€€€ €€€€€€€€€€                                                                                                                                                                                                                 Ф.1mн	Z   4c5x\хO∞Z“∞(h6 I   /LinkInfo /names /src/headerblock /src/files/d:\cs2083\tofu\test_ps.hlsl                 "            
                 AС2                                                                                                                                                                                                                                                                                                                                                                  •   ,  /  8       e  А   ъ   ®  <       (   8  ,   H                  	   
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         STATt                                                                                                                   RDEFШ                  €€  e   \                            a            €€€€          samp tex Microsoft (R) HLSL Shader Compiler 6.3.9600.16384 ЂISGNP         8                    D                   SV_POSITION TEXCOORD ЂЂЂOSGN,                               SV_TARGET ЂЂ