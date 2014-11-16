#pragma once

#include "Common.h"

struct SurfaceMaterial
{
	std::wstring matName;
	float4 difColor;
	int texArrayIndex;
	bool hasTexture;
	bool transparent;
};

class MyObjLoader
{
public:
    MyObjLoader();
    ~MyObjLoader();

    void Init(ID3D11Device* d3dDevice)
    {
        m_pd3dDevice = d3dDevice;
        
        meshSRV.clear();
        textureNameArray.clear();

        vertBuff = NULL;;
        indexBuff = NULL;;

        subsetIndexStart.clear();
        subsetMaterialArray.clear();
        material.clear();

        subsetCount = 0;
    }

    void Destroy()
    {
        m_pd3dDevice = NULL;

        for(uint32 i=0; i<meshSRV.size(); i++)
            meshSRV[i]->Release();

        textureNameArray.clear();

        vertBuff->Release();
        indexBuff->Release();

        subsetIndexStart.clear();
        subsetMaterialArray.clear();
        material.clear();

        subsetCount = 0;
    }

    bool LoadObjModel(bool isRHCoordSys,						//true if model was created in right hand coord system
	                  bool computeNormals);						//true to compute the normals, false to use the files normals

    ID3D11Device*   m_pd3dDevice;

    //Textures and material variables, used for all mesh's loaded
    std::vector<ID3D11ShaderResourceView*> meshSRV;
    std::vector<std::wstring> textureNameArray;

    std::wstring filename;	                    //.obj filename
	ID3D11Buffer* vertBuff;					    //mesh vertex buffer
	ID3D11Buffer* indexBuff;					//mesh index buffer
	std::vector<int> subsetIndexStart;			//start index of each subset
	std::vector<int> subsetMaterialArray;		//index value of material for each subset
	std::vector<SurfaceMaterial> material;		//vector of material structures
	int subsetCount;							//Number of subsets in mesh
};