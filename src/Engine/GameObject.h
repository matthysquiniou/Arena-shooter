#ifndef ENGINE_GAME_OBJECT_H_INCLUDED
#define ENGINE_GAME_OBJECT_H_INCLUDED

#pragma once
#include "define.h"
#include "GameTransform.h"
#include "Components.h"
#include "Maths/Vector3.h"
#include "GamePlay/Tags.h"
#include <unordered_set>

template <typename ...Args>
struct Event;

namespace gce {

class Scene;

class GameObject
{
public:
    GameTransform transform;

    // May be temporary 
    // For the physic collision
    Vector3f32 m_movingDirection;

    static GameObject& Create(Scene& scene);
    void Destroy();
    virtual void Cast(); // just here for dynamic_cast

    [[nodiscard]] uint32 GetID() const;
    [[nodiscard]] cstr GetName() const;

    void SetName(cstr name);
    void AddTags(std::vector<Tag> tags);
    const Tag& GetUniqueTag(std::vector<Tag> tags);

    const bool& HasTags(std::vector<Tag> tags) const;

    [[nodiscard]] bool IsActive() const;
    void SetActive(bool active);

    [[nodiscard]] bool HasParent() const;
    [[nodiscard]] GameObject* GetParent();
    [[nodiscard]] GameObject const* GetParent() const;
    void RemoveParent();
    void SetParent(GameObject& parent);

    [[nodiscard]] bool HasChildren() const;
    [[nodiscard]] uint64 ChildrenCount() const;
    [[nodiscard]] Vector<GameObject*>& GetChildren();
    [[nodiscard]] Vector<GameObject*> const& GetChildren() const;
    void AddChild(GameObject& child);
    void RemoveChild(GameObject& child);

    template <class ComponentClass> [[nodiscard]] bool HasComponent() const;
    template <class ComponentClass> [[nodiscard]] bool HasComponentOfSameFamily() const;
    template <class ComponentClass> [[nodiscard]] ComponentClass* GetComponent();
    template <class ComponentClass> [[nodiscard]] ComponentClass const* GetComponent() const;
    template <class ComponentClass> ComponentClass* AddComponent();
    template <class ComponentClass> void RemoveComponent();

    template <class ScriptClass> [[nodiscard]] bool HasScript() const;
    template <class ScriptClass> [[nodiscard]] ScriptClass* GetScript();
    template <class ScriptClass> [[nodiscard]] ScriptClass const* GetScript() const;
    template <class ScriptClass> ScriptClass* AddScript();
    template <class ScriptClass> void RemoveScript();

    Event<GameObject*>* pCollisionEvents = nullptr;
    Event<GameObject*>* pCollision2DEvents = nullptr;

    UnorderedMap<Component::TypeEnum, uint16>& GetAllComponents() { return m_components; }
    UnorderedMap<uint16, uint16>&  GetAllScripts() { return m_scripts; }

    gce::Texture* GetDefaultTexture() { return m_pDefaultTexture; }
protected:
    explicit GameObject( cstr name = "GameObject" );
    // GameObject( GameObject const& ) = default;
    ~GameObject() = default;

    bool m_created = false;
    bool m_destroyed = false;

    bool m_active = true;

    inline static uint32 s_nextID = 0;
    uint32 m_id = s_nextID++;
    cstr m_name;
  

    Scene* m_pScene = nullptr;
    
    int64 m_componentsBitmask = 0;
    UnorderedMap<Component::TypeEnum, uint16> m_components {};
    UnorderedMap<uint16, uint16> m_scripts {};

    std::unordered_set<Tag> m_tags;

    Event<> m_destroyEvent;

    Vector<GameObject*> m_children;
    GameObject* m_pParent = nullptr;

    gce::Texture* m_pDefaultTexture = nullptr;

    friend struct GameTransform;
    friend struct Script;
    friend class Scene;
    friend class LifespanSystem;
    friend class PhysicSystem;
};


}

#include "GameObject.inl"

#endif