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
    MyObjLoader()
    {}

    bool LoadObjModel(bool isRHCoordSys,						//true if model was created in right hand coord system
	                  bool computeNormals);						//true to compute the normals, false to use the files normals
private:
    std::wstring filename;	                    //.obj filename
	ID3D11Buffer** vertBuff;					//mesh vertex buffer
	ID3D11Buffer** indexBuff;					//mesh index buffer
	std::vector<int>& subsetIndexStart;			//start index of each subset
	std::vector<int>& subsetMaterialArray;		//index value of material for each subset
	std::vector<SurfaceMaterial>& material;		//vector of material structures
	int subsetCount;							//Number of subsets in mesh
}