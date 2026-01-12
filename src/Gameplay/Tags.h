#pragma once

enum class Tag // Game Objects Tags
{
    None,
    TPlayer,
    TWeapon,
    TMeleeWeapon,
    TThrowableWeapon,
    TCollectible,
    TAmmo,
    TGround,
    TEnemy,
    TProjectile,
    TBullet,
    TMiscellaneous,
    TSpawner,
    TMapObject,
    TDestructible,
    THeal,

    //Weapon
    TSpear,
    TBlunderBuss,
    TMusket,
    TStarwheel,

    //Collectibles
    TBomb,
    TDynamite,
    TNoodles,
    TNem,
    TRice,
    TCollectibleAmmo,

    //Ammos
    THeavyAmmo,
    TNormalAmmo,
    TLightAmmo,

    //Mobs
    TMogwai,
    TJiangshi,
    TGuHuoNiao,
    TDragon
};
