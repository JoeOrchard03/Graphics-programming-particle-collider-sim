#include "LoadModel.h"

bool LoadModel(const char* filePath, std::vector<Vertex>& ModelVertices, std::vector<unsigned>& ModelIndices, std::string& texturePath)
{
	//Calls the asset importer library
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals |
		aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_FixInfacingNormals);
	//block based off learningspace tutorials
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode || !scene->HasMeshes()) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Model import failed", importer.GetErrorString(), NULL);
		return false;
	}
	//JUST A DEMO, assuming just one texture! You can use other textures, see documentation!
	//http://assimp.sourceforge.net/lib_html/structai_material.html
	bool hasTexture = false;
	aiString texPath("");
	if (scene->HasMaterials()) {
		aiMaterial* material = scene->mMaterials[0];
		hasTexture = material && material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) >= 0;
	}
	texturePath = texPath.C_Str(); //converts to const char
	//JUST A DEMO, assuming one mesh!
	aiMesh* mesh = scene->mMeshes[0];
	if (!mesh) return false;

	//Clears the vertices from the previous call of load model, if previous model is done loading this wont delete it
	ModelVertices.clear();
	ModelIndices.clear();

	//Assigns new vertices and indices from the fbx file
	ModelVertices.resize(mesh->mNumVertices);
	aiVector3D* texCoords = hasTexture ? mesh->mTextureCoords[0] : nullptr;
	for (unsigned i = 0; i < mesh->mNumVertices; i++)
	{
		ModelVertices[i].x = mesh->mVertices[i].x;
		ModelVertices[i].y = mesh->mVertices[i].y;
		ModelVertices[i].z = mesh->mVertices[i].z;

		if (mesh->HasNormals()) {
			ModelVertices[i].nx = mesh->mNormals[i].x;
			ModelVertices[i].ny = mesh->mNormals[i].y;
			ModelVertices[i].nz = mesh->mNormals[i].z;
		}

		if (texCoords) {
			ModelVertices[i].u = texCoords[i].x;
			ModelVertices[i].v = texCoords[i].y;
		}
	}

	for (unsigned i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for (unsigned j = 0; j < face.mNumIndices; j++)
		{
			ModelIndices.push_back(face.mIndices[j]);
		}
	}

	return !(ModelVertices.empty() || ModelIndices.empty());
}
