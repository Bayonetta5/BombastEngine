//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

#include "BasicMath.h"
#include <Windows.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla

using namespace std;

struct BasicVertex
{
	Vector3<float> pos;  // position
	Vector2<float> tex;  // texture coordinate
	Vector3<float> norm; // surface normal vector
};

struct IndexTriplet
{
	unsigned short pos;
	unsigned short norm;
	unsigned short tex;
};

bool operator <(const IndexTriplet& lhs, const IndexTriplet& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(IndexTriplet)) < 0;
}

void usage()
{
	cout << "usage: obj2vbo.exe [input obj] [output vbo] [/normalize]" << endl;
	cout << "  input obj    specifies the input OBJ mesh file" << endl;
}

#pragma warning(disable : 4447)

int main(int argc, char *argv[])
{
	int x;

	if (argc != 2)
	{
		usage();
		cin >> x;
		return -1;
	}

	std::vector<aiMesh> meshes;
	std::vector<aiMaterial> materials;

	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(argv[1], aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenSmoothNormals);

	if (!pScene) {
		cerr << "Error Parsing: " << importer.GetErrorString() << endl;
		cin >> x;
		return -1;
	}

	cout << pScene->mNumMeshes << " Shapes found" << endl;
	
	const auto szui = sizeof(unsigned int);

	//remove extension
	string filePath = string(argv[1]);
	size_t lastDot = filePath.find_last_of(".");
	string finalFilename = (lastDot == std::string::npos) ? filePath : filePath.substr(0, lastDot);

	// Dump vertex and index data to the output VBO file
	ofstream vboFile("Meshes\\" + std::string(finalFilename) + ".bmdl", ofstream::out | ofstream::binary);
	if (!vboFile.is_open())
	{
		cerr << "error: could not open file \"Meshes\\" << finalFilename << "\" for write" << endl;
		cin >> x;
		return -1;
	}
	unsigned int numObjects = pScene->mNumMeshes;

	vboFile.write(reinterpret_cast<char*>(&numObjects), szui);

	for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
	{
		const aiMesh* pAiMesh = pScene->mMeshes[i];
		unsigned int materialIndex = pAiMesh->mMaterialIndex;
		vboFile.write(reinterpret_cast<char*>(&materialIndex), szui);

		//Normal/tex is same size as verts
		unsigned int numVertices = pAiMesh->mNumVertices;
		unsigned int numIndices = pAiMesh->mNumFaces * 3;
		vboFile.write(reinterpret_cast<char*>(&numVertices), szui);
		vboFile.write(reinterpret_cast<char*>(&numIndices), szui);

		BasicVertex* vertices = new BasicVertex[numVertices];
		for (int j = 0; j < numVertices; j++)
		{
			BasicVertex vert;
			vert.pos = Vector3<float>(pAiMesh->mVertices[j].x, pAiMesh->mVertices[j].y, pAiMesh->mVertices[j].z);
			vert.norm = Vector3<float>(pAiMesh->mNormals[j].x, pAiMesh->mNormals[j].y, pAiMesh->mNormals[j].z);
			if (pAiMesh->HasTextureCoords(0))
			{
				vert.tex = Vector2<float>(pAiMesh->mTextureCoords[0][j].x, pAiMesh->mTextureCoords[0][j].y);
			}
			else 
			{
				vert.tex = Vector2<float>(0, 0);
			}

			vertices[j] = vert;
		}

		vboFile.write(reinterpret_cast<char*>(&vertices[0]), numVertices * sizeof(BasicVertex));

		unsigned int* indices = new unsigned int[numIndices];
		for (int j = 0; j < pAiMesh->mNumFaces; j++)
		{
			const aiFace& face = pAiMesh->mFaces[j];

			indices[j * 3] = face.mIndices[0];
			indices[j * 3 + 1] = face.mIndices[1];
			indices[j * 3 + 2] = face.mIndices[2];
		}
		vboFile.write(reinterpret_cast<char*>(&indices[0]), numIndices * szui);
	}

	vboFile.close();

	//Convert mtl to set of material files
	cout << pScene->mNumMaterials << " Materials found" << endl;
	std::vector<std::string> materialNames;
	for (int i = 0; i < pScene->mNumMaterials; i++)
	{
		aiMaterial* pMat = pScene->mMaterials[i];

		aiString name;
		pMat->Get(AI_MATKEY_NAME, name);
		std::string materialName = string(name.C_Str());
		
		aiColor3D aiDiffuse (0.f, 0.f, 0.f);
		pMat->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuse);

		aiColor3D aiAmbient(0.f, 0.f, 0.f);
		pMat->Get(AI_MATKEY_COLOR_AMBIENT, aiAmbient);

		aiColor3D aiEmissive(0.f, 0.f, 0.f);
		pMat->Get(AI_MATKEY_COLOR_EMISSIVE, aiEmissive);

		aiColor3D aiSpecular(0.f, 0.f, 0.f);
		pMat->Get(AI_MATKEY_COLOR_SPECULAR, aiSpecular);

		float specularStrength = 0.f;
		pMat->Get(AI_MATKEY_SHININESS, specularStrength);

		unsigned int texCount = pMat->GetTextureCount(aiTextureType_DIFFUSE);
		string temp = "Materials\\" + finalFilename;
		std::wstring matDir(temp.length(), L' ');
		std::copy(temp.begin(), temp.end(), matDir.begin());

		CreateDirectory(matDir.c_str(), NULL);
		ofstream mtlFile("Materials\\" + finalFilename + "\\" + materialName + ".bmtl", ofstream::out | ofstream::binary);
		if (!mtlFile.is_open())
		{
			cerr << "error: could not open file \"Materials\\" << materialName + "\\" + materialName << ".bmtl" << "\" for write" << endl;
			cin >> x;
			return -1;
		}

		mtlFile << "<Material name=\"" << materialName << "\">" << endl;
		mtlFile << "\t<Ambient r=\"" << aiAmbient.r << "\" g=\"" << aiAmbient.g << "\" b=\"" << aiAmbient.b << "\" a=\"1\"/>" << endl;
		mtlFile << "\t<Diffuse r=\"" << aiDiffuse.r << "\" g=\"" << aiDiffuse.g << "\" b=\"" << aiDiffuse.b << "\" a=\"1\"/>" << endl;
		mtlFile << "\t<Specular r=\"" << aiSpecular.r << "\" g=\"" << aiSpecular.g << "\" b=\"" << aiSpecular.b << "\" a=\"1\" power=\"" << specularStrength << "\" />" << endl;
		mtlFile << "\t<Emmisive r=\"" << aiEmissive.r << "\" g=\"" << aiEmissive.g << "\" b=\"" << aiEmissive.b << "\" a=\"1\"/>" << endl;
		
		if (texCount > 0) 
		{
			mtlFile << "\t<Textures>" << endl;
			for (int j = 0; j < texCount; j++)
			{
				aiString aiTexPath;
				if (pMat->GetTexture(aiTextureType_DIFFUSE, j, &aiTexPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					mtlFile << "\t\t<Texture path=\"textures\\" << finalFilename << "\\" << aiTexPath.C_Str() << "\"/>";
				}
			}
			mtlFile << "\t</Textures>" << endl;
		}

		mtlFile << "\t<Shader type=\"LitTextured\"/>" << endl;
		mtlFile << "</Material>";

		materialNames.push_back(materialName);

		mtlFile.close();
	}

	//Export Actor file
	ofstream actorFile("Actors\\" + std::string(finalFilename) + ".xml", ofstream::out);
	if (!actorFile.is_open())
	{
		cerr << "error: could not open file \"Actors\\" << finalFilename << "\" for write" << endl;
		cin >> x;
		return -1;
	}

	actorFile << "<Actor type=\"" << finalFilename << "\" resource=\"actors\\" << finalFilename << ".xml\">" << endl;
	actorFile << "\t<TransformComponent/>" << endl;
	actorFile << "\t<MeshRenderComponent>" << endl;
	actorFile << "\t\t<Mesh path=\"meshes\\" << finalFilename << ".bmdl\"/>" << endl;
	actorFile << "\t\t<Materials>" << endl;
	for (auto it = materialNames.begin(); it != materialNames.end(); it++)
	{
		actorFile << "\t\t\t<Material path=\"materials\\" << finalFilename << "\\" << *it << ".bmtl\"/>" << endl;

	}
	actorFile << "\t\t</Materials>" << endl;
	actorFile << "\t</MeshRenderComponent>" << endl;
	actorFile << "</Actor>" << endl;

	actorFile.close();

	return 0;
}