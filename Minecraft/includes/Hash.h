#include <tuple>
#include <functional>
#include <numeric>

namespace std {



	template <class T>
	inline void hash_combine(std::size_t& seed, T const& v)
	{
		seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
	struct HashValueImpl
	{
		static void apply(size_t& seed, Tuple const& tuple)
		{
			HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
			hash_combine(seed, get<Index>(tuple));
		}
	};

	template <class Tuple>
	struct HashValueImpl<Tuple, 0>
	{
		static void apply(size_t& seed, Tuple const& tuple)
		{
			hash_combine(seed, get<0>(tuple));
		}
	};


	template <typename ... TT>
	struct hash<std::tuple<TT...>>
	{
		size_t
			operator()(std::tuple<TT...> const& tt) const
		{
			size_t seed = 0;
			HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
			return seed;
		}

	};


	// maybe use struct instead of tuple for map
	template <>
	struct hash<ChunkPosition> {
		std::size_t operator()(const ChunkPosition& pos) const {
			// Combine the hash of each member using a simple hash combiner
			std::size_t hx = std::hash<int>()(pos.x);
			std::size_t hy = std::hash<int>()(pos.y);
			std::size_t hz = std::hash<int>()(pos.z);

			// XOR and shift to combine hashes
			return hx ^ (hy << 1) ^ (hz << 2);
		}
	};
}
