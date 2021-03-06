#include <cstdio>
#include <cstdint>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <unordered_map>

#pragma comment (lib, "assimp-vc140-mt.lib")

#include "../../ModelFormat.h"
#include "../../TofuMath.h"

using tofu::math::float2;
using tofu::math::float3;
using tofu::math::float4;
using tofu::math::quat;
using tofu::math::float4x4;
using tofu::math::int4;

typedef tofu::model::ModelMesh Mesh;
typedef tofu::model::ModelBone Bone;
typedef tofu::model::ModelAnimation Animation;
typedef tofu::model::ModelAnimChannel Channel;
typedef tofu::model::ModelFloat3Frame VFrame;
typedef tofu::model::ModelQuatFrame QFrame;

typedef std::vector<Bone> BoneTree;
typedef std::unordered_map<std::string, uint32_t> BoneTable;

void Basename(char* buf, size_t bufSize, const char* path)
{
	size_t start = 0;
	size_t end = SIZE_MAX;
	size_t i = 0;
	while (path[i])
	{
		if (path[i] == '\\' || path[i] == '/')
		{
			start = i + 1;
		}
		if (path[i] == '.')
		{
			end = i;
		}
		i++;
	}

	if (end == SIZE_MAX || end < start)
	{
		end = i;
	}

	strncpy_s(buf, bufSize, path + start, end - start);
}

void Directory(char* buf, size_t bufSize, const char* path)
{
	size_t start = 0;
	size_t end = 0;
	size_t i = 0;
	while (path[i])
	{
		if (path[i] == '\\' || path[i] == '/')
		{
			end = i;
		}
		i++;
	}

	if (end == start)
	{
		strncpy_s(buf, bufSize, "./", 3);
	}

	strncpy_s(buf, bufSize, path + start, end - start + 1);
}

inline void CopyMatrix(float4x4& dstMatrix, const aiMatrix4x4& srcMatrix)
{
	memcpy(&dstMatrix, &srcMatrix, sizeof(float4x4));
}

inline bool IsIdentity(const float4x4& mat)
{
	const float* m = reinterpret_cast<const float*>(&mat);
	for (uint32_t i = 0; i < 16; i++)
	{
		if ((i % 5 == 0 && m[i] != 1.0f) || (i % 5 != 0 && m[i] != 0.0f))
			return false;
	}
	return true;
}

inline bool IsEqual(const float4x4& a, const aiMatrix4x4& b)
{
	const float* ma = reinterpret_cast<const float*>(&a);
	const float* mb = reinterpret_cast<const float*>(&b);
	for (uint32_t i = 0; i < 16; i++)
	{
		if (std::fabsf(ma[i] - mb[i]) > FLT_EPSILON)
			return false;
	}
	return true;
}

uint32_t loadBoneHierarchy(aiNode* node, BoneTree& bones, BoneTable& table, uint32_t parentId = UINT32_MAX, uint32_t lastSibling = UINT32_MAX)
{
	uint32_t boneId = static_cast<uint32_t>(bones.size());
	bones.push_back(Bone());
	Bone& bone = bones[boneId];
	bone.id = boneId;
	bone.parent = parentId;
	bone.nextSibling = UINT32_MAX;

	if (lastSibling != UINT32_MAX)
	{
		bones[lastSibling].nextSibling = boneId;
	}

	if (node->mName.length > 0)
	{
		table.insert(std::pair<std::string, uint32_t>(node->mName.C_Str(), boneId));
	}

	CopyMatrix(bone.transform, node->mTransformation);
	bone.offsetMatrix = tofu::math::matrix::identity();

	uint32_t firstChild = UINT32_MAX;
	uint32_t lastChild = UINT32_MAX;
	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		uint32_t id = loadBoneHierarchy(node->mChildren[i], bones, table, boneId, lastChild);
		if (firstChild == UINT32_MAX)
		{
			firstChild = id;
		}
		lastChild = id;
	}
	bones[boneId].firstChild = firstChild;

	return boneId;
}

struct ModelFile
{
	Assimp::Importer importer;
	tofu::model::ModelHeader	header;
	uint32_t					numVertices;
	uint32_t					numIndices;
	std::vector<Mesh>			meshes;
	std::vector<uint8_t>		vertices;
	std::vector<uint16_t>		indices;
	BoneTree					bones;
	BoneTable					boneTable;
	std::vector<Animation>		anims;
	std::vector<Channel>		channels;
	std::vector<VFrame>			tFrames;
	std::vector<QFrame>			rFrames;
	std::vector<VFrame>			sFrames;

	int Init(const char* filename)
	{
		importer.FreeScene();

		unsigned int flags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace;

		bool bConvertToLeftHanded = false;
		bool bStructureOfArray = false;

		if (bConvertToLeftHanded)
		{
			flags |= aiProcess_ConvertToLeftHanded;
		}
		flags |= aiProcess_FlipUVs;

		const aiScene* scene = importer.ReadFile(std::string(filename), flags);

		if (nullptr == scene)
		{
			printf("failed to read input file.\n%s\n", importer.GetErrorString());
			return __LINE__;
		}

		header = {};
		header.Magic = tofu::model::MODEL_FILE_MAGIC;
		header.Version = tofu::model::MODEL_FILE_VERSION;
		header.StructOfArray = bStructureOfArray ? 1 : 0;
		header.HasIndices = 1;
		header.HasTangent = 1;
		header.NumTexcoordChannels = 0;

		header.NumMeshes = scene->mNumMeshes;
		header.NumBones = 0;
		header.NumAnimations = 0;

		// gathering bone information
		if (scene->mNumAnimations > 0 && scene->mRootNode->mNumChildren > 0)
		{
			// load bone hierarchy
			loadBoneHierarchy(scene->mRootNode, bones, boneTable);

			header.NumBones = static_cast<uint32_t>(bones.size());
			header.NumAnimations = scene->mNumAnimations;
			header.HasAnimation = 1;
		}

		// get number of texcoord channels
		for (uint32_t i = 0; i < header.NumMeshes; ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];
			uint32_t numUVChns = mesh->GetNumUVChannels();
			if (numUVChns > header.NumTexcoordChannels)
			{
				header.NumTexcoordChannels = numUVChns;
			}
		}

		if (header.NumTexcoordChannels > tofu::model::MODEL_FILE_MAX_TEXCOORD_CHANNELS)
		{
			header.NumTexcoordChannels = tofu::model::MODEL_FILE_MAX_TEXCOORD_CHANNELS;
		}

		// gathering vertices information
		if (header.NumMeshes > 0)
		{
			meshes.resize(header.NumMeshes);
		}

		for (uint32_t i = 0; i < header.NumMeshes; ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];
			tofu::model::ModelMesh& meshInfo = meshes[i];
			meshInfo.NumVertices = mesh->mNumVertices;
			meshInfo.NumIndices = mesh->mNumFaces * 3;

			numVertices += meshInfo.NumVertices;
			numIndices += meshInfo.NumIndices;
		}

		// align index data size to dword
		if (numIndices % 2 != 0)
		{
			numIndices += 1;
		}

		uint32_t vertexSize = header.CalculateVertexSize();
		uint32_t verticesDataSize = vertexSize * numVertices;
		vertices.resize(verticesDataSize);
		indices.resize(numIndices);

		// populate vertices data
		if (bStructureOfArray)
		{
			printf("not implemented yet.\n");
			return __LINE__;
		}
		else
		{
			uint8_t* ptr = &(vertices[0]);
			for (uint32_t i = 0; i < header.NumMeshes; ++i)
			{
				const aiMesh* mesh = scene->mMeshes[i];
				uint8_t* meshBaseAddr = ptr;

				for (uint32_t j = 0; j < mesh->mNumVertices; ++j)
				{
					aiVector3D& pos = mesh->mVertices[j];
					aiVector3D& nor = mesh->mNormals[j];

					*reinterpret_cast<float3*>(ptr) = float3{ pos.x, pos.y, pos.z };
					ptr += sizeof(float3);

					*reinterpret_cast<float3*>(ptr) = float3{ nor.x, nor.y, nor.z };
					ptr += sizeof(float3);

					if (header.HasTangent == 1)
					{
						aiVector3D& tan = mesh->mTangents[j];

						*reinterpret_cast<float3*>(ptr) = float3{ tan.x, tan.y, tan.z };
						ptr += sizeof(float3);
					}

					if (header.HasAnimation)
					{
						// reserve space for bone Ids and weights
						*reinterpret_cast<int4*>(ptr) = int4();
						ptr += sizeof(int4);
						*reinterpret_cast<float4*>(ptr) = float4();
						ptr += sizeof(float4);
					}

					for (uint32_t k = 0; k < header.NumTexcoordChannels; ++k)
					{
						if (k < mesh->GetNumUVChannels())
						{
							aiVector3D& uv = mesh->mTextureCoords[k][j];
							*reinterpret_cast<float2*>(ptr) = float2{ uv.x, uv.y };
							ptr += sizeof(float2);
						}
						else
						{
							*reinterpret_cast<float2*>(ptr) = float2{ 0.0f, 0.0f };
							ptr += sizeof(float2);
						}
					}
				}

				if (!header.HasAnimation)
				{
					continue;
				}

				for (uint32_t j = 0; j < mesh->mNumBones; ++j)
				{
					aiBone* bone = mesh->mBones[j];
					std::string boneName(bone->mName.C_Str());

					auto iter = boneTable.find(boneName);
					if (iter == boneTable.end())
					{
						printf("cannot find bone '%s' in hierarchy.\n", boneName.c_str());
						return __LINE__;
					}

					uint32_t boneId = iter->second;

					if (IsIdentity(bones[boneId].offsetMatrix))
					{
						CopyMatrix(bones[boneId].offsetMatrix, bone->mOffsetMatrix);
					}
					else if (!IsEqual(bones[boneId].offsetMatrix, bone->mOffsetMatrix))
					{
						printf("different offset matrix for the same bone %d.", boneId);
						return __LINE__;
					}

					for (uint32_t k = 0; k < bone->mNumWeights; k++)
					{
						uint32_t vertexId = bone->mWeights[k].mVertexId;
						float weight = bone->mWeights[k].mWeight;

						int4* boneIds = reinterpret_cast<int4*>(meshBaseAddr
							+ vertexSize * vertexId
							+ sizeof(float3) * (header.HasTangent ? 3 : 2));

						float4* weights = reinterpret_cast<float4*>(boneIds + 1);

						if (weights->x == 0.0f)
						{
							boneIds->x = boneId;
							weights->x = weight;
						}
						else if (weights->y == 0.0f)
						{
							boneIds->y = boneId;
							weights->y = weight;
						}
						else if (weights->z == 0.0f)
						{
							boneIds->z = boneId;
							weights->z = weight;
						}
						else if (weights->w == 0.0f)
						{
							boneIds->w = boneId;
							weights->w = weight;
						}
						else
						{
							printf("mesh %d vertex %d has more than 4 bones bound.\n", i, vertexId);
							return __LINE__;
						}
					}

				}
			}
		}

		// populate indices data
		{
			uint16_t* pIndices = &(indices[0]);

			for (uint32_t i = 0; i < header.NumMeshes; ++i)
			{
				const aiMesh* mesh = scene->mMeshes[i];

				for (uint32_t j = 0; j < mesh->mNumFaces; ++j)
				{
					uint32_t a = mesh->mFaces[j].mIndices[0];
					uint32_t b = mesh->mFaces[j].mIndices[1];
					uint32_t c = mesh->mFaces[j].mIndices[2];

					if (a > UINT16_MAX || b > UINT16_MAX || c > UINT16_MAX)
					{
						printf("index larger overflow 16 bits.\n");
						return __LINE__;
					}

					*pIndices = a;
					pIndices++;
					*pIndices = b;
					pIndices++;
					*pIndices = c;
					pIndices++;
				}
			}
		}

		// gathering animation information
		for (uint32_t iAnim = 0; iAnim < header.NumAnimations; iAnim++)
		{
			aiAnimation* anim = scene->mAnimations[iAnim];

			anims.push_back(Animation
			{
				static_cast<float>(anim->mDuration),
				static_cast<float>(anim->mTicksPerSecond),
				anim->mNumChannels,
				static_cast<uint32_t>(channels.size())
			});

			for (uint32_t iChan = 0; iChan < anim->mNumChannels; iChan++)
			{
				aiNodeAnim* chan = anim->mChannels[iChan];
				std::string boneName(chan->mNodeName.C_Str());
				auto iter = boneTable.find(boneName);
				if (iter == boneTable.end())
				{
					printf("unable to find bone %s\n", boneName.c_str());
					return __LINE__;
				}

				uint32_t boneId = iter->second;
				uint32_t numT = chan->mNumPositionKeys;
				uint32_t numR = chan->mNumRotationKeys;
				uint32_t numS = chan->mNumScalingKeys;
				uint32_t startT = (0 == numT) ? UINT32_MAX : static_cast<uint32_t>(tFrames.size());
				uint32_t startR = (0 == numR) ? UINT32_MAX : static_cast<uint32_t>(rFrames.size());
				uint32_t startS = (0 == numS) ? UINT32_MAX : static_cast<uint32_t>(sFrames.size());

				channels.push_back(Channel
				{
					boneId,
					startT,
					numT,
					startR,
					numR,
					startS,
					numS
				});

				// translation keys
				for (uint32_t iFrame = 0; iFrame < numT; iFrame++)
				{
					aiVectorKey& key = chan->mPositionKeys[iFrame];
					VFrame frame;
					frame.time = static_cast<float>(key.mTime);
					frame.value.x = key.mValue.x;
					frame.value.y = key.mValue.y;
					frame.value.z = key.mValue.z;

					tFrames.push_back(frame);
				}

				// rotation keys
				for (uint32_t iFrame = 0; iFrame < numT; iFrame++)
				{
					aiQuatKey& key = chan->mRotationKeys[iFrame];
					QFrame frame;
					frame.time = static_cast<float>(key.mTime);
					frame.value.x = key.mValue.x;
					frame.value.y = key.mValue.y;
					frame.value.z = key.mValue.z;
					frame.value.w = key.mValue.w;

					rFrames.push_back(frame);
				}

				// scale keys
				for (uint32_t iFrame = 0; iFrame < numT; iFrame++)
				{
					aiVectorKey& key = chan->mScalingKeys[iFrame];
					VFrame frame;
					frame.time = static_cast<float>(key.mTime);
					frame.value.x = key.mValue.x;
					frame.value.y = key.mValue.y;
					frame.value.z = key.mValue.z;

					sFrames.push_back(frame);
				}
			}
		}
		header.NumAnimChannels = static_cast<uint32_t>(channels.size());
		header.NumTotalTranslationFrames = static_cast<uint32_t>(tFrames.size());
		header.NumTotalRotationFrames = static_cast<uint32_t>(rFrames.size());
		header.NumTotalScaleFrames = static_cast<uint32_t>(sFrames.size());

		return 0;
	}

	int MergeAnimation(const ModelFile& other)
	{
		if (!other.header.HasAnimation
			|| other.bones.empty()
			|| other.anims.empty())
			return 0;

		if (header.NumBones != other.header.NumBones)
		{
			printf("bone count doesn't match.\n");
			return __LINE__;
		}

		//anims.resize(anims.size() + other.anims.size());
		anims.insert(anims.end(), other.anims.begin(), other.anims.end());
		channels.insert(channels.end(), other.channels.begin(), other.channels.end());
		tFrames.insert(tFrames.end(), other.tFrames.begin(), other.tFrames.end());
		rFrames.insert(rFrames.end(), other.rFrames.begin(), other.rFrames.end());
		sFrames.insert(sFrames.end(), other.sFrames.begin(), other.sFrames.end());

		for (uint32_t i = 0; i < other.header.NumAnimations; i++)
		{
			Animation& anim = anims[i + header.NumAnimations];
			anim.startChannelId += header.NumAnimChannels;
		}

		for (uint32_t i = 0; i < other.header.NumAnimChannels; i++)
		{
			Channel& chan = channels[i + header.NumAnimChannels];
			if (chan.startTranslationFrame != UINT32_MAX)
				chan.startTranslationFrame += header.NumTotalTranslationFrames;

			if (chan.startRotationFrame != UINT32_MAX)
				chan.startRotationFrame += header.NumTotalRotationFrames;

			if (chan.startScaleFrame != UINT32_MAX)
				chan.startScaleFrame += header.NumTotalScaleFrames;
		}

		header.NumAnimations = static_cast<uint32_t>(anims.size());
		header.NumAnimChannels = static_cast<uint32_t>(channels.size());
		header.NumTotalTranslationFrames = static_cast<uint32_t>(tFrames.size());
		header.NumTotalRotationFrames = static_cast<uint32_t>(rFrames.size());
		header.NumTotalScaleFrames = static_cast<uint32_t>(sFrames.size());

		return 0;
	}

	int Write(const char* filename)
	{
		FILE* file = fopen(filename, "wb");
		if (nullptr == file)
		{
			printf("failed to create output file.\n");
			return __LINE__;
		}

		// writing header
		if (1 != fwrite(&header, sizeof(header), 1, file))
		{
			printf("failed to write header data.\n");
			return __LINE__;
		}

		// writing mesh data
		if (1 != fwrite(&meshes[0], sizeof(Mesh) * header.NumMeshes, 1, file))
		{
			printf("failed to write mesh data.\n");
			return __LINE__;
		}


		// write vertices to file
		if (1 != fwrite(&vertices[0], header.CalculateVertexSize() * numVertices, 1, file))
		{
			printf("failed to write vertices to the file.\n");
			return __LINE__;
		}

		// write indices to file
		if (1 != fwrite(&indices[0], sizeof(uint16_t) * numIndices, 1, file))
		{
			printf("failed to write indices to the file.\n");
			return __LINE__;
		}

		// bone data
		if (header.NumBones > 0)
		{
			if (1 != fwrite(&(bones[0]), sizeof(Bone) * header.NumBones, 1, file))
			{
				printf("failed to write bone list to the file.\n");
				return __LINE__;
			}
		}

		// animation data
		if (header.HasAnimation)
		{
			if (1 != fwrite(&(anims[0]), sizeof(Animation) * header.NumAnimations, 1, file))
			{
				printf("failed to write animation list to the file.\n");
				return __LINE__;
			}

			if (1 != fwrite(&(channels[0]), sizeof(Channel) * header.NumAnimChannels, 1, file))
			{
				printf("failed to write channel list to the file.\n");
				return __LINE__;
			}

			if (1 != fwrite(&(tFrames[0]), sizeof(VFrame) * header.NumTotalTranslationFrames, 1, file))
			{
				printf("failed to write translation frame list to the file.\n");
				return __LINE__;
			}

			if (1 != fwrite(&(rFrames[0]), sizeof(QFrame) * header.NumTotalRotationFrames, 1, file))
			{
				printf("failed to write rotation frame list to the file.\n");
				return __LINE__;
			}

			if (1 != fwrite(&(sFrames[0]), sizeof(VFrame) * header.NumTotalScaleFrames, 1, file))
			{
				printf("failed to write scale frame list to the file.\n");
				return __LINE__;
			}
		}

		fclose(file);
		return 0;
	}

	bool HasTextures()
	{
		const aiScene* scene = importer.GetScene();
		return nullptr != scene && scene->HasTextures();
	}

	int WriteTextures(const char* basepath)
	{
		const aiScene* scene = importer.GetScene();

		if (nullptr != scene && scene->HasTextures())
		{
			char path[1024] = {};

			for (uint32_t i = 0; i < scene->mNumTextures; i++)
			{
				aiTexture* tex = scene->mTextures[i];
				if (tex->CheckFormat("png"))
				{
					sprintf_s(path, 1024, "%s_%u.png", basepath, i);
					FILE* f = fopen(path, "wb");
					if (1 != fwrite(tex->pcData, tex->mWidth, 1, f))
					{
						printf("failed to write texture to the file.\n");
						return __LINE__;
					}
				}
				else
				{
					printf("unsupported texture format.\n");
					return __LINE__;
				}
			}
		}

		return 0;
	}
};

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("model_converter output_file input_file1 [input_file2 ...]\n");
		return 0;
	}

	ModelFile model = {};
	int err = model.Init(argv[2]);
	if (err) return err;

	for (int i = 3; i < argc; i++)
	{
		ModelFile model2 = {};
		err = model2.Init(argv[i]);
		if (err) return err;

		model.MergeAnimation(model2);
	}

	// write
	err = model.Write(argv[1]);
	if (err) return err;

	if (model.HasTextures())
	{
		char directory[1024] = {};
		char basename[1024] = {};

		Directory(directory, 1024, argv[1]);
		Basename(basename, 1024, argv[1]);

		strcat_s(directory, 1024, basename);

		err = model.WriteTextures(directory);
		if (err) return err;
	}

	return 0;
}