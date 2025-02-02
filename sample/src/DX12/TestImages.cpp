// AMD AMDUtils code
// 
// Copyright(c) 2019 Advanced Micro Devices, Inc.All rights reserved.
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

#include "stdafx.h"
#include "TestImages.h"

namespace CAULDRON_DX12
{
    void TestImages::OnCreate(Device* pDevice, UploadHeap* pUploadHeap, ResourceViewHeaps* pResourceViewHeaps, StaticBufferPool* pStaticBufferPool, DynamicBufferRing* pDynamicBufferRing, DXGI_FORMAT outFormat)
    {
        m_pDynamicBufferRing = pDynamicBufferRing;

        D3D12_STATIC_SAMPLER_DESC SamplerDesc = {};
        SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        SamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        SamplerDesc.MinLOD = 0.0f;
        SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
        SamplerDesc.MipLODBias = 0;
        SamplerDesc.MaxAnisotropy = 1;
        SamplerDesc.ShaderRegister = 0;
        SamplerDesc.RegisterSpace = 0;
        SamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        static const char* s_TextureNameList[_countof(m_testImagesData)] = {
            "..\\media\\color_ramp_bt2020_dcip3\\LuxoDoubleChecker_EXR_ARGB_16F_1.DDS",
            "..\\media\\color_ramp_bt2020_dcip3\\dcip3_1000_EXR_ARGB_16F_1.DDS",
            "..\\media\\color_ramp_bt2020_dcip3\\bt2020_1000_EXR_ARGB_16F_1.DDS"
        };

        for (int i = 0; i < _countof(m_testImagesData); ++i)
        {
            m_testImagesData[i].m_testImageTexture.InitFromFile(pDevice, pUploadHeap, s_TextureNameList[i], false);
            pUploadHeap->FlushAndFinish();
            pResourceViewHeaps->AllocCBV_SRV_UAVDescriptor(1, &m_testImagesData[i].m_testImageTextureSRV);
            m_testImagesData[i].m_testImageTexture.CreateSRV(0, &m_testImagesData[i].m_testImageTextureSRV);
        }

        m_testImagePS.OnCreate(pDevice, "TestImagesPS.hlsl", pResourceViewHeaps, pStaticBufferPool, 1, 1, &SamplerDesc, outFormat);
    }

    void TestImages::OnDestroy()
    {
        for (int i = 0; i < _countof(m_testImagesData); ++i)
        {
            m_testImagesData[i].m_testImageTexture.OnDestroy();
        }

        m_testImagePS.OnDestroy();
    }

    void TestImages::Draw(ID3D12GraphicsCommandList* pCommandList, int testPattern)
    {
        D3D12_GPU_VIRTUAL_ADDRESS cbTestImagesHandle;
        TestImagesConsts *pTestImagesConsts;
        m_pDynamicBufferRing->AllocConstantBuffer(sizeof(TestImagesConsts), (void **)&pTestImagesConsts, &cbTestImagesHandle);

        pTestImagesConsts->testPattern = testPattern - 1;

        m_testImagePS.Draw(pCommandList, 1, &m_testImagesData[pTestImagesConsts->testPattern % _countof(m_testImagesData)].m_testImageTextureSRV, cbTestImagesHandle);
    }
}
