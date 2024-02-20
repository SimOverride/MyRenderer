#include "model.h"
#include <iostream>
#include <fstream>
#include <limits>

using std::string;

static float floatMin = std::numeric_limits<float>::min();
static float floatMax = std::numeric_limits<float>::max();

std::istringstream& operator >>(std::istringstream& iss, Face& f)
{
    char slash;
    for (int i = 0; i < 3; i++)
    {
        iss >> f.vi[i] >> slash >> f.ti[i] >> slash >> f.ni[i];
        // obj文件中的索引是从1开始的，因此需要减1
        f.vi[i]--;
        f.ti[i]--;
        f.ni[i]--;
    }
    return iss;
}

Model::Model(const string& modelFile, const std::string& textureFile)
{
    LoadOBJ(modelFile);
    LoadTexture(textureFile);
}

Model::Model(const string& modelFile, const std::string& textureFile, Vector3 position, Vector3 rotation, Vector3 scale)
    : Object(position, rotation, scale)
{
    LoadOBJ(modelFile);
    LoadTexture(textureFile);
}

Model::Model(const Model& model) : Object(model)
{
    diffuseMap = model.diffuseMap == nullptr ? nullptr : new Bitmap(*model.diffuseMap);
    
    verts = model.verts;
    texCoords = model.texCoords;
    normals = model.normals;
    faces = model.faces;
    boundsSize = model.boundsSize;
}

Model::~Model()
{
    if (diffuseMap != nullptr)
        delete diffuseMap;
}

int Model::nVerts() const
{
    return verts.size();
}

int Model::nFaces() const
{
    return faces.size();
}

Vector3 Model::vert(int index) const
{
    return verts[index];
}

Vector2 Model::texCoord(int index) const
{
    return texCoords[index];
}

Vector3 Model::normal(int index) const
{
    return normals[index];
}

Face Model::face(int index) const
{
    return faces[index];
}

Vector3 Model::GetBoundsSize() const
{
    return boundsSize;
}

Vector3 Model::operator [](int index) const
{
    return verts[index];
}

void Model::LoadOBJ(const string& modelFile)
{
    std::ifstream in;
    in.open(modelFile, std::ifstream::in);
    if (in.fail())
    {
        std::cerr << "Can't open " + modelFile + ".obj" << "\n";
        return;
    }
    
    Vector3 minPoint = Vector3(floatMax, floatMax, floatMax);
    Vector3 maxPoint = Vector3(floatMin, floatMin, floatMin);

    string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        
        string prefix;
        iss >> prefix;
        if (prefix == "v")
        {
            Vector3 v;
            iss >> v;
            verts.push_back(v);

            minPoint.x = std::min(minPoint.x, v.x);
            minPoint.y = std::min(minPoint.y, v.y);
            minPoint.z = std::min(minPoint.z, v.z);
            maxPoint.x = std::max(maxPoint.x, v.x);
            maxPoint.y = std::max(maxPoint.y, v.y);
            maxPoint.z = std::max(maxPoint.z, v.z);
        }
        else if (prefix == "vt")
        {
            Vector2 t;
            iss >> t;
            texCoords.push_back(t);
        }
        else if (prefix == "vn")
        {
            Vector3 n;
            iss >> n;
            normals.push_back(n);
        }
        else if (prefix == "f")
        {
            Face f;
            iss >> f;
            faces.push_back(f);
        }
    }
    Vector3 boundsCenter = (minPoint + maxPoint) / 2;
    std::cerr << "BoundsCenter:\t" << boundsCenter << "\n";
    for (int i = 0; i < verts.size(); i++)
        verts[i] -= boundsCenter;   // 模型统一以边界框中心为原点
    boundsSize = maxPoint - minPoint;
    std::cerr << "BoundsSize:\t" << boundsSize << "\n";
}

void Model::LoadTexture(const string& textureFile)
{
    if (textureFile == "")
    {
        diffuseMap = nullptr;
        return;
    }
    diffuseMap = new Bitmap();
    diffuseMap->Read(textureFile);
}