#include "pch.h"
#include "BonusManager.h"
#include "GameManager.h"
#include "../Scripts/BonusManagerBehavior.hpp"

EntityWrapper* BonusManager::CreateNem(gce::Vector3f32 pos)
{
    EntityWrapper& nem = EntityWrapper::Create();
    nem.SetProperties("Nem", { Tag::TCollectible, Tag::THeal, Tag::TNem }, pos, { 0, 0, 0 }, { 8, 8, 8 });
    nem.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/nem/nem.obj"), "res/Assets/nem/nem_base_color.png");
    nem.AddScript<BonusManagerBehavior>()->SetProperties(20);
    return &nem;
}

EntityWrapper* BonusManager::CreateRiceBowl(gce::Vector3f32 pos)
{
    EntityWrapper& rice = EntityWrapper::Create();
    rice.SetProperties("Rice", { Tag::TCollectible, Tag::THeal, Tag::TRice }, pos, { 0, 0, 0 }, { 4, 4, 4 });
    rice.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/rice/rice.obj"), "res/Assets/rice/rice_base_color.png");
    rice.AddScript<BonusManagerBehavior>()->SetProperties(35);
    return &rice;
}

EntityWrapper* BonusManager::CreateNoodles(gce::Vector3f32 pos)
{
    EntityWrapper& noodles = EntityWrapper::Create();
    noodles.SetProperties("Noodles", { Tag::TCollectible, Tag::THeal, Tag::TNoodles }, pos, { 0, 0, 0 }, { 4, 4, 4 });
    noodles.AddMeshRenderer(gce::GeometryFactory::GetCustomGeometry("res/Assets/noodles/noodles.obj"), "res/Assets/noodles/noodles_base_color.png");
    noodles.AddScript<BonusManagerBehavior>()->SetProperties(50);
    return &noodles;
}
