#include "AnimationComponent.h"

#include "Engine.h"
#include "TofuMath.h"
#include "Transform.h"
#include "RenderingComponent.h"

#include <cassert>

namespace tofu
{
	int32_t AnimationComponentData::Play(uint32_t animId)
	{
		if (animId != currentAnimation)
		{
			// cancel cross fading if there is one
			crossFadeFactor = 0.0f;

			currentAnimation = animId;
			currentTime = 0.0f;
		}
		return TF_OK;
	}

	int32_t AnimationComponentData::CrossFade(uint32_t animId, float duration)
	{
		// test if we are cross fading or trying to switch to the same animation
		if (crossFadeFactor > 0.0f || animId == currentAnimation)
			return TF_OK;

		// record old animation and set new animation
		lastAnimation = currentAnimation;
		currentAnimation = animId;

		lastAnimationTime = currentTime;
		currentTime = 0.0f;

		// start cross fading, set cross fading speed
		crossFadeFactor = 1.0f;
		crossFadeSpeed = 1.0f / duration;

		return TF_OK;
	}

	void AnimationComponentData::UpdateTiming()
	{
		if (crossFadeFactor > 0.0f)
		{
			// update cross fading and old animation parameters
			crossFadeFactor -= crossFadeSpeed * Time::DeltaTime;
			lastAnimationTime += Time::DeltaTime * playbackSpeed;
		}

		// update current animation play back time
		currentTime += Time::DeltaTime * playbackSpeed;
	}

	int32_t AnimationComponentData::FillInBoneMatrices(void* buffer, uint32_t bufferSize)
	{
		math::float4x4* matrices = reinterpret_cast<math::float4x4*>(buffer);

		if (nullptr == model)
		{
			return TF_UNKNOWN_ERR;
		}

		if (currentAnimation >= model->header->NumAnimations ||
			static_cast<uint32_t>(sizeof(math::float4x4)) * model->header->NumBones > bufferSize)
		{
			return TF_UNKNOWN_ERR;
		}

		model::ModelAnimation& anim = model->animations[currentAnimation];
		
		// convert time in seconds to ticks
		float ticks = currentTime * anim.ticksPerSecond;
		ticks = std::fmodf(ticks, anim.durationInTicks);

		// load bone matrices
		for (uint32_t i = 0; i < model->header->NumBones; i++)
		{
			matrices[i] = model->bones[i].transform;
		}

		// update bone matrices for each channel
		for (uint32_t i = 0; i < anim.numChannels; i++)
		{
			model::ModelAnimChannel& chan = model->channels[anim.startChannelId + i];

			uint32_t boneId = chan.boneId;

			// get interlopated matrix
			Transform t;
			t.SetTranslation(SampleFrame(model->translationFrames, chan.startTranslationFrame, chan.numTranslationFrame, ticks));
			t.SetRotation(SampleFrame(model->rotationFrames, chan.startRotationFrame, chan.numRotationFrame, ticks));
			t.SetScale(SampleFrame(model->scaleFrames, chan.startScaleFrame, chan.numScaleFrame, ticks));

			matrices[boneId] = t.GetMatrix();
		}

		// if we are cross fading
		if (crossFadeFactor > 0.0f)
		{
			model::ModelAnimation& lastAnim = model->animations[lastAnimation];

			float lastAnimTicks = currentTime * lastAnim.ticksPerSecond;
			lastAnimTicks = std::fmodf(lastAnimTicks, lastAnim.durationInTicks);

			// interplotate matrices between new and old animtion
			for (uint32_t i = 0; i < lastAnim.numChannels; i++)
			{
				model::ModelAnimChannel& chan = model->channels[lastAnim.startChannelId + i];

				uint32_t boneId = chan.boneId;

				Transform t;
				t.SetTranslation(SampleFrame(model->translationFrames, chan.startTranslationFrame, chan.numTranslationFrame, lastAnimTicks));
				t.SetRotation(SampleFrame(model->rotationFrames, chan.startRotationFrame, chan.numRotationFrame, lastAnimTicks));
				t.SetScale(SampleFrame(model->scaleFrames, chan.startScaleFrame, chan.numScaleFrame, lastAnimTicks));
				
				math::float4x4 m = t.GetMatrix();
				matrices[boneId] = math::lerp(matrices[boneId], m, crossFadeFactor);
			}
		}

		// convert to world space 
		for (uint32_t i = 0; i < model->header->NumBones; i++)
		{
			uint32_t p = model->bones[i].parent;
			if (UINT32_MAX != p)
			{
				matrices[i] = matrices[p] * matrices[i];
			}
		}

		// append the offset matrices ( convert vertices from model space to bone local space )
		for (uint32_t i = 0; i < model->header->NumBones; i++)
		{
			matrices[i] = matrices[i] * model->bones[i].offsetMatrix;
		}

		return TF_OK;
	}

	math::float3 AnimationComponentData::SampleFrame(model::ModelFloat3Frame* frames, uint32_t startFrame, uint32_t numFrames, float ticks)
	{
		if (nullptr != frames && numFrames > 0)
		{
			// if we have only 1 frame ...
			if (numFrames < 2)
				return frames[startFrame].value;

			// find the 2 consecutive frames we are in between
			for (uint32_t i = 1, last = 0; i < numFrames; i++)
			{
				model::ModelFloat3Frame& fb = frames[startFrame + i];

				if (fb.time > ticks)
				{
					// lerp between these 2 frames
					model::ModelFloat3Frame& fa = frames[startFrame + last];
					float t = (ticks - fa.time) / (fb.time - fa.time);
					assert(!std::isnan(t) && !std::isinf(t) && t >= 0.0f && t <= 1.0f);
					return math::lerp(fa.value, fb.value, t);
				}
				last = i;
			}
		}
		return math::float3();
	}

	math::quat AnimationComponentData::SampleFrame(model::ModelQuatFrame* frames, uint32_t startFrame, uint32_t numFrames, float ticks)
	{
		if (nullptr != frames && numFrames > 0)
		{
			// if we have only 1 frame ...
			if (numFrames < 2)
				return frames[startFrame].value;

			// find the 2 consecutive frames we are in between
			for (uint32_t i = 1, last = 0; i < numFrames; i++)
			{
				model::ModelQuatFrame& fb = frames[startFrame + i];

				if (fb.time > ticks)
				{
					// slerp between these 2 frames
					model::ModelQuatFrame& fa = frames[startFrame + last];
					float t = (ticks - fa.time) / (fb.time - fa.time);
					assert(!std::isnan(t) && !std::isinf(t) && t >= 0.0f && t <= 1.0f);
					return math::slerp(fa.value, fb.value, t);
				}
				last = i;
			}
		}
		return math::quat();
	}

}
