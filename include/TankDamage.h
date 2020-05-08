#pragma once

struct TankDamage
{
	/// <summary>
	/// @brief Reset all tank damage
	/// </summary>
	void reset() { m_leftTrackDamaged = false, m_rightTrackDamaged = false; }

	bool m_leftTrackDamaged{ false };
	bool m_rightTrackDamaged{ false };
};