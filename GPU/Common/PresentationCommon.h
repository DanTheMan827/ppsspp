// Copyright (c) 2012- PPSSPP Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#pragma once

#include "GPU/Common/ShaderCommon.h"

struct CardboardSettings {
	bool enabled;
	float leftEyeXPosition;
	float rightEyeXPosition;
	float screenYPosition;
	float screenWidth;
	float screenHeight;
};

struct PostShaderUniforms {
	float texelDelta[2]; float pixelDelta[2];
	float time[4];
	float video; float pad[3];
	// Used on Direct3D9.
	float gl_HalfPixel[4];
};

void CenterDisplayOutputRect(float *x, float *y, float *w, float *h, float origW, float origH, float frameW, float frameH, int rotation);

namespace Draw {
class Buffer;
class DrawContext;
class Framebuffer;
class Pipeline;
class SamplerState;
class ShaderModule;
class Texture;
}

struct ShaderInfo;
class TextureCacheCommon;

enum class OutputFlags {
	LINEAR = 0x0000,
	NEAREST = 0x0001,
	RB_SWIZZLE = 0x0002,
	BACKBUFFER_FLIPPED = 0x0004,
	POSITION_FLIPPED = 0x0008,
};

inline OutputFlags operator | (const OutputFlags &lhs, const OutputFlags &rhs) {
	return OutputFlags((int)lhs | (int)rhs);
}
inline OutputFlags operator |= (OutputFlags &lhs, const OutputFlags &rhs) {
	lhs = lhs | rhs;
	return lhs;
}
inline bool operator & (const OutputFlags &lhs, const OutputFlags &rhs) {
	return ((int)lhs & (int)rhs) != 0;
}

class PresentationCommon {
public:
	PresentationCommon(Draw::DrawContext *draw);
	~PresentationCommon();

	void UpdateSize(int w, int h, int rw, int rh) {
		pixelWidth_ = w;
		pixelHeight_ = h;
		renderWidth_ = rw;
		renderHeight_ = rh;
	}
	void SetLanguage(ShaderLanguage lang) {
		lang_ = lang;
	}

	bool HasPostShader() {
		return usePostShader_;
	}

	bool UpdatePostShader();
	void UpdateShaderInfo(const ShaderInfo *shaderInfo);

	void DeviceLost();
	void DeviceRestore(Draw::DrawContext *draw);

	void GetCardboardSettings(CardboardSettings *cardboardSettings);
	void CalculatePostShaderUniforms(int bufferWidth, int bufferHeight, bool hasVideo, PostShaderUniforms *uniforms);

	void SourceTexture(Draw::Texture *texture);
	void SourceFramebuffer(Draw::Framebuffer *fb);
	void CopyToOutput(OutputFlags flags, int uvRotation, float u0, float v0, float u1, float v1, const PostShaderUniforms &uniforms);

protected:
	void CreateDeviceObjects();
	void DestroyDeviceObjects();
	void DestroyPostShader();

	void ShowPostShaderError(const std::string &errorString);

	Draw::ShaderModule *CompileShaderModule(Draw::ShaderStage stage, ShaderLanguage lang, const std::string &src, std::string *errorString);
	Draw::Pipeline *CreatePipeline(std::vector<Draw::ShaderModule *> shaders, bool postShader, const Draw::UniformBufferDesc *uniformDesc);

	void BindSource();

	Draw::DrawContext *draw_;
	Draw::Pipeline *texColor_ = nullptr;
	Draw::Pipeline *texColorRBSwizzle_ = nullptr;
	Draw::SamplerState *samplerNearest_ = nullptr;
	Draw::SamplerState *samplerLinear_ = nullptr;
	Draw::Buffer *vdata_ = nullptr;
	Draw::Buffer *idata_ = nullptr;

	std::vector<Draw::ShaderModule *> postShaderModules_;
	std::vector<Draw::Pipeline *> postShaderPipelines_;
	std::vector<Draw::Framebuffer *> postShaderFramebuffers_;

	Draw::Texture *srcTexture_ = nullptr;
	Draw::Framebuffer *srcFramebuffer_ = nullptr;

	int pixelWidth_ = 0;
	int pixelHeight_ = 0;
	int renderWidth_ = 0;
	int renderHeight_ = 0;

	bool usePostShader_ = false;
	bool restorePostShader_ = false;
	bool postShaderAtOutputResolution_ = false;
	bool postShaderIsUpscalingFilter_ = false;
	ShaderLanguage lang_;
};
