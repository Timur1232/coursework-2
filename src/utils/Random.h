#pragma once

#include <random>

namespace CW_E{

	class Random
	{
	public:
		Random(const Random&) = delete;
		Random(Random&&) = delete;

		static Random& normal();

	private:
		std::default_random_engine m_Gen;
		std::normal_distribution<double>
	};

} // CW_E