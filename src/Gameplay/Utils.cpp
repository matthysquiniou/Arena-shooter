#include <fstream>
#include <regex>
#include <Engine.h>
#include <RenderContext.h>
#include "json.h"
#include "Utils.h"
#include "NavMesh.h"
#include "Scripts/HealSpawnerBehavior.hpp"
#include "Scripts/DestructibleBehavior.hpp"

std::vector<gce::GameObject*> ImportBlenderScene(std::wstring jsonFile)
{
	const std::string pathTexture = "res/Texture/";
	
	gce::GameManager* pGameManager = gce::GameManager::s_pInstance;
    gce::Scene& pScene = pGameManager->GetScene();

	std::ifstream f(WRES_PATH L"res/BlenderScene/" + jsonFile);
	nlohmann::json data;

	try {
		data = nlohmann::json::parse(f);
	}
	catch (const std::exception& e) {
		std::cerr << "Erreur JSON: " << e.what() << std::endl;
		return std::vector<gce::GameObject*>();
	}

    std::regex patternPhysic("^[^_]*P");
    std::regex patternCollider("^[^_]*C");
    std::regex patternDestructible("^[^_]*D");
    std::regex patternNavMesh("^[^_]*NM");
    std::regex patternSpawnerHeal("^[^_]*SH");
    std::regex patternSpawnerGuHuoNiao("^[^_]*SG");
    std::regex patternSpawnerJiangshi("^[^_]*SJ");
    std::regex patternSpawnerMogwai("^[^_]*SM");

    gce::Vector<gce::Vertex> navmeshVertices;
    gce::Vector<uint32> navmeshIndices;
    std::unordered_set<gce::GameObject*> obstaclesGameObject;

    std::vector<gce::GameObject*> allCreatedObj;

    for (auto& obj : data)
    {
        std::string type = obj.value("t", "NONE");
        if (type != "MESH")
            continue;

        std::string name = obj.value("n", "Unnamed");
        bool hasPhysic = std::regex_search(name, patternPhysic);
        bool hasCollider = std::regex_search(name, patternCollider);
        bool isDestructible = std::regex_search(name, patternDestructible);
        bool isNavMesh = std::regex_search(name, patternNavMesh);
        bool isSpawnerHeal = std::regex_search(name, patternSpawnerHeal);
        bool isSpawnerGuHuoNiao = std::regex_search(name, patternSpawnerGuHuoNiao);
        bool isSpawnerJiangshi = std::regex_search(name, patternSpawnerJiangshi);
        bool isSpawnerMogwai = std::regex_search(name, patternSpawnerMogwai);
        bool isobstacle = !isNavMesh && hasCollider;
        std::string baseColorTex = "";
        std::string metallicTex = "";
        std::string roughnessTex = "";
        std::string normalMapTex = "";
        std::string emissionTex = "";
        gce::Vector3f32 position;
        gce::Quaternion rotation;
        gce::Vector3f32 scale;

        if (obj.contains("tx"))
        {
            auto& tex = obj["tx"];

            if (tex.contains("Base Color"))
                baseColorTex = tex["Base Color"].get<std::string>();

           /* if (tex.contains("metallic") && !tex["metallic"].is_null())
                metallicTex = tex["metallic"].get<std::string>();

            if (tex.contains("Roughness"))
                roughnessTex = tex["Roughness"].get<std::string>();

            if (tex.contains("normal_map") && !tex["normal_map"].is_null())
                normalMapTex = tex["normal_map"].get<std::string>();*/
        }

        if (obj.contains("p"))
        {
            position.x = obj["p"][0];
            position.y = obj["p"][2];
            position.z = obj["p"][1];
        }

        if (obj.contains("r"))
            rotation = gce::Quaternion{ obj["r"][0], obj["r"][2], obj["r"][1], obj["r"][3] };
        else
            rotation = gce::Quaternion{ 0.f, 0.f, 0.f, 0.f };

        if (obj.contains("s"))
            scale = gce::Vector3f32{ obj["s"][0], obj["s"][2], obj["s"][1] };
        else
            scale = gce::Vector3f32{ 1.f, 1.f, 1.f };

        gce::GameObject& gameObject = gce::GameObject::Create(pScene);

        if (isSpawnerHeal || isSpawnerGuHuoNiao || isSpawnerJiangshi || isSpawnerMogwai) {
            gameObject.AddTags({ Tag::TSpawner });
            if (isSpawnerHeal)
            {
                gameObject.AddTags({ Tag::THeal });
                gameObject.AddScript<HealSpawnerBehavior>();
            }
            if (isSpawnerGuHuoNiao)
                gameObject.AddTags({ Tag::TGuHuoNiao });
            if (isSpawnerJiangshi)
                gameObject.AddTags({ Tag::TJiangshi });
            if (isSpawnerMogwai)
                gameObject.AddTags({Tag::TMogwai});
        }
        if (isDestructible)
        {
            gameObject.AddTags({ Tag::TDestructible });

        }

        // gameObject.SetName("importedScene"); // maybe change later just taging everything with the same name
        gameObject.transform.SetWorldPosition(position);
        gameObject.transform.SetWorldRotation(rotation);
        gameObject.transform.SetWorldScale(scale);

        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        if (obj.contains("m"))
        {
            auto& mesh = obj["m"];

            if (mesh.contains("v"))
                vertices = mesh["v"].get<std::vector<float>>();

            if (mesh.contains("i"))
                indices = mesh["i"].get<std::vector<uint32_t>>();
        }
        gce::MeshRenderer* pMeshRenderer = gameObject.AddComponent<gce::MeshRenderer>();
        gce::Vector<gce::Vertex> gceVertices;
        gce::Vector<uint32> gceIndices;
        pMeshRenderer->pGeometry = MakeCustomGeometry(vertices, indices, gceVertices, gceIndices);
        pMeshRenderer->pPso = pGameManager->GetSceneManager().GetPSO();
        pMeshRenderer->pMaterial->subsurface = 1;

        if (isNavMesh)
        {
            navmeshVertices = gceVertices;
            navmeshIndices = gceIndices;
        }
        else if (hasCollider)
        {
            obstaclesGameObject.insert(&gameObject);
            gameObject.AddScript<DestructibleBehavior>();
        }
        
        if (!baseColorTex.empty())
        {
            gce::Texture* pNewTexture = new gce::Texture(pathTexture + baseColorTex);
            pMeshRenderer->pMaterial->albedoTextureID = pNewTexture->GetTextureID();
            pMeshRenderer->pMaterial->useTextureAlbedo = 1;
        }
        /*
        if (!metallicTex.empty())
        {
            gce::Texture* pNewTexture = new gce::Texture(pathTexture + metallicTex);
            pMeshRenderer->pMaterial->metalnessTextureID = pNewTexture->GetTextureID();
            pMeshRenderer->pMaterial->useTextureMetalness = 1;
        }

        if (!roughnessTex.empty())
        {
            gce::Texture* pNewTexture = new gce::Texture(pathTexture + roughnessTex);
            pMeshRenderer->pMaterial->roughnessTextureID = pNewTexture->GetTextureID();
            pMeshRenderer->pMaterial->useTextureRoughness = 1;
        }
        
        if (!normalMapTex.empty())
        {
            gce::Texture* pNewTexture = new gce::Texture(pathTexture + normalMapTex);
            pMeshRenderer->pMaterial->normalTextureID = pNewTexture->GetTextureID();
            pMeshRenderer->pMaterial->useTextureNormal = 1;
        }
        */
        if (hasCollider)
            gce::BoxCollider* pBoxCollider = gameObject.AddComponent<gce::BoxCollider>();

        if (hasPhysic)
            gce::PhysicComponent* pPhysic = gameObject.AddComponent<gce::PhysicComponent>();

        allCreatedObj.push_back(&gameObject);
    }

    if (!navmeshVertices.Empty())
    {
        NavMesh::Create(navmeshVertices, navmeshIndices, obstaclesGameObject);
    }
    
    return allCreatedObj;
}

gce::Geometry* MakeCustomGeometry(
    std::vector<float> vertices,
    std::vector<uint32_t> indices,
    gce::Vector<gce::Vertex>& gceVertices,
    gce::Vector<uint32>& gceIndices
)
{
    gce::Geometry* customGeo = nullptr;

    size_t vertexCount = vertices.size() / 5;

    gceVertices.Resize(vertexCount);

    for (size_t i = 0; i < vertexCount; i++)
    {
        size_t vIndex = i * 5;
        gce::Vertex v;
        v.pos = {
            vertices[vIndex],
            vertices[vIndex + 2],
            vertices[vIndex + 1]
        };

        v.normal = { 0.f, 0.f, 0.f }; // it's not right but it will stay like this for now

        v.uv = {
            vertices[vIndex + 3],
            1.f - vertices[vIndex + 4]
        };

        gceVertices[i] = v;
    }

    gceIndices.Resize(indices.size());
    size_t reverseIndice = indices.size() - 1;
    for (size_t i = 0; i < indices.size(); ++i)
        gceIndices[reverseIndice - i] = indices[i];

    customGeo = new gce::Geometry(gceVertices.Data(), gceVertices.Size(), gceIndices.Data(), gceIndices.Size());
    return customGeo;
}

gce::Vector3f32 ClosestPointOnLine(const gce::Vector3f32& a, const gce::Vector3f32& b, const gce::Vector3f32& p)
{

    gce::Vector3f32 d = b - a;
    gce::Vector3f32 ap = p - a;

    float denom = d.DotProduct(d);
    if (denom == 0.0f)  
        return a;

    float t = ap.DotProduct(d) / denom;

    t = max(0.f,min(1.f, t));

    return a + d * t;
}

bool IsPointNearLine(const gce::Vector3f32& a, const gce::Vector3f32& b, const gce::Vector3f32& p, float radius)
{
    gce::Vector3f32 d = b - a;
    gce::Vector3f32 ap = p - a;

    float denom = d.DotProduct(d);
    float r2 = radius * radius;

    if (denom < 1e-6f)
        return ap.DotProduct(ap) <= r2;

    float t = ap.DotProduct(d) / denom;
    t = std::clamp(t, 0.0f, 1.0f);

    gce::Vector3f32 diff = p - (a + d * t);
    return diff.DotProduct(diff) <= r2;
}

bool SegmentIntersectsRectXZ(const gce::Vector3f32& A, const gce::Vector3f32& B, float minX, float maxX, float minZ, float maxZ, float radius)
{
    minX -= radius; maxX += radius;
    minZ -= radius; maxZ += radius;

    gce::Vector3f32 p1{ minX, 0, minZ };
    gce::Vector3f32 p2{ maxX, 0, minZ };
    gce::Vector3f32 p3{ maxX, 0, maxZ };
    gce::Vector3f32 p4{ minX, 0, maxZ };

    return
        SegmentsIntersectXZ(A, B, p1, p2) ||
        SegmentsIntersectXZ(A, B, p2, p3) ||
        SegmentsIntersectXZ(A, B, p3, p4) ||
        SegmentsIntersectXZ(A, B, p4, p1);
}

bool SegmentsIntersectXZ(const gce::Vector3f32& A, const gce::Vector3f32& B, const gce::Vector3f32& C, const gce::Vector3f32& D)
{
    auto orient = [](float ax, float az,
        float bx, float bz,
        float cx, float cz)
        {
            return (bx - ax) * (cz - az) - (bz - az) * (cx - ax);
        };

    float o1 = orient(A.x, A.z, B.x, B.z, C.x, C.z);
    float o2 = orient(A.x, A.z, B.x, B.z, D.x, D.z);
    float o3 = orient(C.x, C.z, D.x, D.z, A.x, A.z);
    float o4 = orient(C.x, C.z, D.x, D.z, B.x, B.z);

    if ((o1 > 0 && o2 < 0 || o1 < 0 && o2 > 0) &&
        (o3 > 0 && o4 < 0 || o3 < 0 && o4 > 0))
        return true;

    return false;
}
