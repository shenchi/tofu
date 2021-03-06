#pragma once

#include "Common.h"

namespace tofu
{
	struct RendererCommand
	{
		enum
		{
			None,
			CreateBuffer,
			UpdateBuffer,
			DestroyBuffer,
			CreateTexture,
			UpdateTexture,
			DestroyTexture,
			CreateSampler,
			DestroySampler,
			CreateVertexShader,
			DestroyVertexShader,
			CreatePixelShader,
			DestroyPixelShader,
			CreatePipelineState,
			DestroyPipelineState,
			ClearRenderTargets,
			Draw,
			MaxRendererCommands
		};
	};

	enum PixelFormat
	{
		FORMAT_AUTO,
		FORMAT_R8G8B8A8_UNORM,
		FORMAT_R8G8B8A8_SNORM,
		FORMAT_R16G16B16B16_UNORM,
		FORMAT_R16G16B16B16_SNORM,
		FORMAT_R32G32B32A32_FLOAT,
		FORMAT_R16_SINT,
		FORMAT_R32_SINT,
		FORMAT_R16_UINT,
		FORMAT_R32_UINT,
		FORMAT_D24_UNORM_S8_UINT,
		NUM_PIXEL_FORMAT
	};

	enum CullMode
	{
		CULL_NONE,
		CULL_FRONT,
		CULL_BACK
	};

	enum ComparisonFunc
	{
		COMPARISON_NEVER,
		COMPARISON_LESS,
		COMPARISON_EQUAL,
		COMPARISON_LESS_EQUAL,
		COMPARISON_GREATER,
		COMPARISON_NOT_EQUAL,
		COMPARISON_GREATER_EQUAL,
		COMPARISON_ALWAYS
	};

	enum BindingFlag
	{
		BINDING_VERTEX_BUFFER = 1 << 0,
		BINDING_INDEX_BUFFER = 1 << 1,
		BINDING_CONSTANT_BUFFER = 1 << 2,
		BINDING_SHADER_RESOURCE = 1 << 3,
		BINDING_RENDER_TARGET = 1 << 5,
		BINDING_DEPTH_STENCIL = 1 << 6,
	};

	enum VertexFormat
	{
		VERTEX_FORMAT_NORMAL,
		VERTEX_FORMAT_SKINNED
	};

	struct RendererCommandBuffer
	{
		uint32_t*			cmds;
		void**				params;

		uint32_t			capacity;
		uint32_t			size;

		// create a new command buffer from allocator[allocNo]
		static RendererCommandBuffer* Create(uint32_t capacity, uint32_t allocNo);

		// append a command into the command buffer
		void Add(uint32_t cmd, void* param);
	};

	struct CreateBufferParams
	{
		BufferHandle		handle;
		uint32_t			dynamic : 1;
		uint32_t			format : 15;
		uint32_t			bindingFlags : 16;
		uint32_t			size;
		uint32_t			stride;
		void*				data;
	};

	struct UpdateBufferParams
	{
		BufferHandle		handle;
		uint32_t			offset;
		uint32_t			size;
		void*				data;
	};

	struct CreateTextureParams
	{
		TextureHandle		handle;
		uint32_t			dynamic : 1;
		uint32_t			cubeMap : 1;
		// if set, data: file's content; width: file's size, other fields are ignored
		uint32_t			isFile : 1;
		uint32_t			_reserved : 5;
		uint32_t			format : 8;
		uint32_t			arraySize : 8;
		uint32_t			bindingFlags : 8;
		uint32_t			width;
		uint32_t			height;
		uint32_t			pitch;
		void*				data;
	};

	struct UpdateTextureParams
	{
		TextureHandle		handle;
		uint32_t			pitch;
		void*				data;
	};

	struct CreateSamplerParams
	{
		SamplerHandle		handle;
		// TODO
	};

	struct CreateVertexShaderParams
	{
		VertexShaderHandle	handle;
		void*				data;
		size_t				size;
	};

	struct CreatePixelShaderParams
	{
		PixelShaderHandle	handle;
		void*				data;
		size_t				size;
	};

	struct CreatePipelineStateParams
	{
		PipelineStateHandle	handle;
		VertexFormat		vertexFormat;
		VertexShaderHandle	vertexShader;
		PixelShaderHandle	pixelShader;
		union
		{
			struct
			{
				uint32_t			CullMode : 2;
				uint32_t			_Reserved1 : 30;
			};
			uint32_t				RasterizerState;
		};

		union
		{
			struct
			{
				uint32_t			DepthEnable : 1;
				uint32_t			DepthWrite : 1;
				uint32_t			DepthFunc : 3;
				uint32_t			_Reserved2 : 27;
			};
			uint32_t				DepthStencilState;
		};

		CreatePipelineStateParams()
			:
			handle(),
			vertexFormat(VERTEX_FORMAT_NORMAL),
			vertexShader(),
			pixelShader(),
			CullMode(CULL_BACK),
			DepthEnable(1),
			DepthWrite(1),
			DepthFunc(COMPARISON_LESS)
		{}
	};

	struct ClearParams
	{
		TextureHandle		renderTargets[MAX_RENDER_TARGET_BINDINGS];
		float				clearColor[4];
		TextureHandle		depthRenderTarget;
		float				clearDepth;
		uint8_t				clearStencil;

		inline ClearParams()
			:
			renderTargets(),
			clearColor(),
			depthRenderTarget(MAX_TEXTURES),
			clearDepth(1.0f),
			clearStencil(0u)
		{
			renderTargets[0] = TextureHandle(MAX_TEXTURES + 1);
		}

		inline void SetClearColor(float r, float g, float b, float a)
		{
			clearColor[0] = r;
			clearColor[1] = g;
			clearColor[2] = b;
			clearColor[3] = a;
		}
	};

	struct ConstantBufferBinding
	{
		BufferHandle		bufferHandle;
		uint16_t			offsetInVectors;
		uint16_t			sizeInVectors;
	};

	struct DrawParams
	{
		PipelineStateHandle		pipelineState;
		BufferHandle			vertexBuffer;
		BufferHandle			indexBuffer;
		uint32_t				startIndex;
		uint32_t				startVertex;
		uint32_t				indexCount;
		ConstantBufferBinding	vsConstantBuffers[MAX_CONSTANT_BUFFER_BINDINGS];
		ConstantBufferBinding	psConstantBuffers[MAX_CONSTANT_BUFFER_BINDINGS];
		TextureHandle			vsTextures[MAX_TEXTURE_BINDINGS];
		TextureHandle			psTextures[MAX_TEXTURE_BINDINGS];
		SamplerHandle			vsSamplers[MAX_SAMPLER_BINDINGS];
		SamplerHandle			psSamplers[MAX_SAMPLER_BINDINGS];
	};

	class Renderer
	{
	public:
		virtual ~Renderer() {}

		virtual int32_t Init() = 0;

		virtual int32_t Release() = 0;

		virtual int32_t Submit(RendererCommandBuffer* buffer) = 0;

		virtual int32_t Present() = 0;

		virtual int32_t GetFrameBufferSize(int32_t& width, int32_t& height) = 0;

		static Renderer* CreateRenderer();
	};

}

