#pragma once
#include "../Tags.h"
#include <iostream>
#include <algorithm>

class Ammos
{
	Tag m_type = Tag::None;
	int m_amount = 0;
	int m_maxAmountStockage = 0;
public:
	Ammos(Tag type, int beginAmmoCount, int maxAmountStockage = 999) { m_type = type, m_amount = beginAmmoCount, m_maxAmountStockage = maxAmountStockage; }

	const bool& IsEmpty() const
	{
		if (m_amount <= 0)
			return true;

		return false;
	}

	const bool& IsFull() const
	{
		if (m_amount >= m_maxAmountStockage)
			return true;

		return false;
	}

	const bool& IsType(Tag type) const
	{
		if (m_type == type)
			return true;

		return false;
	}

	void SetAmmos(int val)
	{
		m_amount = std::clamp(val, 0, m_maxAmountStockage);
	}

	void UseAmmos(int val = 1)
	{
		m_amount = std::clamp(m_amount - val, 0, m_maxAmountStockage);
	}

	void EarnAmmos(int val = 1)
	{
		m_amount = std::clamp(m_amount + val, 0, m_maxAmountStockage);
	}

	const int& GetAmount() const { return m_amount; }
	const int& GetMaxAmountStockage() const { return m_maxAmountStockage; }
};

