#ifndef ENGINE_SCENE_H_INCLUDED
#define ENGINE_SCENE_H_INCLUDED

#include "define.h"
#include <unordered_map>

namespace gce {

class GameObject;


class Scene final
{
public:
    static Scene& Create();

    UnorderedMap<uint32, GameObject*> m_gameObjects;
private:
    Scene() = default;
    ~Scene() = default;

    void UpdateMatrix();
 
    friend class LifespanSystem;
    friend class GameManager;
};


}

#endif