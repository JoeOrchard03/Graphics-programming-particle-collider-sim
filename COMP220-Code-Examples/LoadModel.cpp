#include "LoadModel.h"

#include <iostream>

bool LoadModel(const char* filePath, std::vector<Vertex>& ModelVertices, std::vector<unsigned>& ModelIndices, std::string& texturePath)
{
	//Calls the asset importer library
	Assimp::Importer importer;
	//Loads the model into an asset importer scene object
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals |
		aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_FixInfacingNormals);
	//Checks if model import was sucessful by checking scene is not empty, that it is not incomplete according to assimp and that it has a root node and a mesh
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode || !scene->HasMeshes()) {
		//If scene is messed up display an error and return false to quit out of the function
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Model import failed", importer.GetErrorString(), NULL);
		return false;
	}
	//JUST A DEMO, assuming just one texture! You can use other textures, see documentation!
	//http://assimp.sourceforge.net/lib_html/structai_material.html
	bool hasTexture = false;
	aiString texPath("");
	//Check if model has materials using an assimp function
	if (scene->HasMaterials()) {
		//Creates and sets a material object using the first material in the scene
		aiMaterial* material = scene->mMaterials[0];
		//hasTexture is a bool, checks if material is valid(exists), also checks if the found material is a diffuse material using GetTexture function
		hasTexture = material && material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) >= 0;
	}
	//If material contains a diffuse material stores it in the texturePath
	texturePath = texPath.C_Str(); //converts to const char
	//JUST A DEMO, assuming one mesh!
	//Sets mesh to be the first mesh that is found in the scene

	aiMesh* mesh = scene->mMeshes[0];
	//Quits out function if no mesh is found
	if (!mesh) return false;

	ModelVertices.clear();
	ModelIndices.clear();

	//Vertices counter for keeping track of how many vertices have been looped through, so they do not get overwritten
	unsigned int verticesCounter = 0;

	//Clears the vertices from the previous call of load model, if previous model is done loading this wont delete it
	for (unsigned i = 0; i < scene->mNumMeshes; i++)
	{
		mesh = scene->mMeshes[i];
		std::cout << (mesh->mName).C_Str()<<std::endl;

		//Resizes the size of model vertices to match the amount the new mesh has as different loaded models likely have a different amount of vertices
		ModelVertices.resize(verticesCounter + mesh->mNumVertices);

		//assigns the mesh's texture coordinates to texCoords object
		aiVector3D* texCoords = hasTexture ? mesh->mTextureCoords[0] : nullptr;
		//Loops through each vertex in the mesh and assigns the meshes' co-ords to the corresponding axis of the model vertices variable
		for (unsigned j = 0; j < mesh->mNumVertices; j++)
		{
			ModelVertices[j + verticesCounter].x = mesh->mVertices[j].x;
			ModelVertices[j + verticesCounter].y = mesh->mVertices[j].y;
			ModelVertices[j + verticesCounter].z = mesh->mVertices[j].z;

			//Does the same for any normals
			if (mesh->HasNormals()) {
				ModelVertices[j + verticesCounter].nx = mesh->mNormals[j].x;
				ModelVertices[j + verticesCounter].ny = mesh->mNormals[j].y;
				ModelVertices[j + verticesCounter].nz = mesh->mNormals[j].z;
			}

			//Gets the uv's from the texCoords of the mesh
			if (texCoords) {
				ModelVertices[j + verticesCounter].u = texCoords[j].x;
				ModelVertices[j + verticesCounter].v = texCoords[j].y;
			}
		}

		//Loops through each face in the mesh
		for (unsigned i = 0; i < mesh->mNumFaces; i++)
		{
			//Loops through each index in each face
			aiFace& face = mesh->mFaces[i];
			for (unsigned j = 0; j < face.mNumIndices; j++)
			{
				//Sends the index data back to model indices, also offset by vertices counter so that the mesh uses the indices for the right set of vertice
				ModelIndices.push_back(face.mIndices[j] + verticesCounter);
			}
		}

		verticesCounter += mesh->mNumVertices;
	}

	//Returns true if modelVertices and modelIndices were populated succesfully, otherwise returns false
	return !(ModelVertices.empty() || ModelIndices.empty());
}
