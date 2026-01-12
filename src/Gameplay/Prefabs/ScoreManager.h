#pragma once
#include <iostream>
#include <Engine.h>

class ScoreManager
{
	static int m_amount;

public:
	static int GetScore() { return m_amount; }
	static void Reset() { m_amount = 0; }
	static void Set(int val) { m_amount = val; }
	static void Add(float val) { m_amount = (int)gce::Clamp(m_amount + (int)val, 0.f, INT_MAX); }
	static void Multiply(float val) { m_amount *= val; }
};

