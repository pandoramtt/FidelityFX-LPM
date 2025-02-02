// AMD Cauldron code
// 
// Copyright(c) 2018 Advanced Micro Devices, Inc.All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//--------------------------------------------------------------------------------------
// Constant Buffer
//--------------------------------------------------------------------------------------
cbuffer cbPerFrame : register(b0)
{
    bool u_shoulder;
    bool u_con;
    bool u_soft;
    bool u_con2;
    bool u_clip;
    bool u_scaleOnly;
    uint u_displayMode;
    uint pad;
    matrix u_inputToOutputMatrix;
    uint4 u_ctl[24];
}

//--------------------------------------------------------------------------------------
// I/O Structures
//--------------------------------------------------------------------------------------
struct VERTEX
{
    float2 vTexcoord : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Texture definitions
//--------------------------------------------------------------------------------------
Texture2D        sceneTexture     : register(t0);
SamplerState     samLinearWrap    : register(s0);

#define A_GPU 1
#define A_HLSL 1
#include "ffx_a.h"

uint4 LpmFilterCtl(uint i) { return u_ctl[i]; }

#define LPM_NO_SETUP 1
#include "ffx_lpm.h"

#include "transferFunction.h"

//--------------------------------------------------------------------------------------
// Main function
//--------------------------------------------------------------------------------------
float4 mainPS(VERTEX Input) : SV_Target
{
    float4 color = sceneTexture.Sample(samLinearWrap, Input.vTexcoord);

    color = mul(u_inputToOutputMatrix, color);

    // This code is there to make sure no negative values make it down to LPM. Make sure to never hit this case and convert content to correct colourspace
    color.r = max(0, color.r);
    color.g = max(0, color.g);
    color.b = max(0, color.b);
    //

    LpmFilter(color.r, color.g, color.b, u_shoulder, u_con, u_soft, u_con2, u_clip, u_scaleOnly);

    switch (u_displayMode)
    {
        case 1:
            // FS2_DisplayNative
            // Apply gamma
            color.xyz = ApplyGamma(color.xyz);
            break;

        case 3:
            // HDR10_ST2084
            // Apply ST2084 curve
            color.xyz = ApplyPQ(color.xyz);
            break;
    }

    return color;
}