#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include <initializer_list>

class Cube
{
public:
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		namespace dx = DirectX;

		constexpr float side = 1.0f / 2.0f;

		std::vector<V> vertices(8);
		vertices[0].pos = { -side,-side,-side };
		vertices[1].pos = { side,-side,-side };
		vertices[2].pos = { -side,side,-side };
		vertices[3].pos = { side,side,-side };
		vertices[4].pos = { -side,-side,side };
		vertices[5].pos = { side,-side,side };
		vertices[6].pos = { -side,side,side };
		vertices[7].pos = { side,side,side };

		return{
			std::move(vertices),{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			}
		};
	}

	template<class V>
	static IndexedTriangleList<V> MakeSkinned()
	{
		namespace dx = DirectX;

		constexpr float side = 1.0f / 2.0f;

		std::vector<V> vertices(14);

		vertices[0].pos = {-side,-side,-side};
		vertices[0].tex = { 0.0f / 4.0f, 2.0f / 3.0f };
		vertices[1].pos = { side,-side,-side };
		vertices[1].tex = { 0.0f / 4.0f, 1.0f / 3.0f };
		vertices[2].pos = { -side,side,-side };
		vertices[2].tex = { 1.0f / 4.0f, 2.0f / 3.0f };
		vertices[3].pos = { side,side,-side };
		vertices[3].tex = { 1.0f / 4.0f, 1.0f / 3.0f };
		vertices[4].pos = { -side,-side,side };
		vertices[4].tex = { 3.0f / 4.0f, 2.0f / 3.0f };
		vertices[5].pos = { side,-side,side };
		vertices[5].tex = { 3.0f / 4.0f, 1.0f / 3.0f };
		vertices[6].pos = { -side,side,side };
		vertices[6].tex = { 2.0f / 4.0f, 2.0f / 3.0f };
		vertices[7].pos = { side,side,side };
		vertices[7].tex = { 2.0f / 4.0f, 1.0f / 3.0f };
		vertices[8].pos = { -side,-side,-side };
		vertices[8].tex = { 4.0f / 4.0f, 2.0f / 3.0f };
		vertices[9].pos = { side,-side,-side };
		vertices[9].tex = { 4.0f / 4.0f, 1.0f / 3.0f };
		vertices[10].pos = { -side,-side,-side };
		vertices[10].tex = { 1.0f / 4.0f, 3.0f / 3.0f };
		vertices[11].pos = { -side,-side,side };
		vertices[11].tex = { 2.0f / 4.0f, 3.0f / 3.0f };
		vertices[12].pos = { side,-side,-side };
		vertices[12].tex = { 1.0f / 4.0f, 0.0f / 3.0f };
		vertices[13].pos = { side,-side,side };
		vertices[13].tex = { 2.0f / 4.0f, 0.0f / 3.0f };
		
		return{
			std::move(vertices),{
				0,1,2,   2,1,3,
				4,5,8,   5,9,8,
				2,3,6,   3,7,6,
				4,7,5,   4,6,7,
				2,11,10, 2,6,11,
				12,7,3,  12,13,7
			}
		};
	}

	template<class V>
	static IndexedTriangleList<V> MakeIndependent()
	{
		constexpr float side = 1.0f / 2.0f;

		std::vector<V> vertices(24);
		vertices[0].pos = { -side,-side,-side };// 0 near side
		vertices[1].pos = { side,-side,-side };// 1
		vertices[2].pos = { -side,side,-side };// 2
		vertices[3].pos = { side,side,-side };// 3
		vertices[4].pos = { -side,-side,side };// 4 far side
		vertices[5].pos = { side,-side,side };// 5
		vertices[6].pos = { -side,side,side };// 6
		vertices[7].pos = { side,side,side };// 7
		vertices[8].pos = { -side,-side,-side };// 8 left side
		vertices[9].pos = { -side,side,-side };// 9
		vertices[10].pos = { -side,-side,side };// 10
		vertices[11].pos = { -side,side,side };// 11
		vertices[12].pos = { side,-side,-side };// 12 right side
		vertices[13].pos = { side,side,-side };// 13
		vertices[14].pos = { side,-side,side };// 14
		vertices[15].pos = { side,side,side };// 15
		vertices[16].pos = { -side,-side,-side };// 16 bottom side
		vertices[17].pos = { side,-side,-side };// 17
		vertices[18].pos = { -side,-side,side };// 18
		vertices[19].pos = { side,-side,side };// 19
		vertices[20].pos = { -side,side,-side };// 20 top side
		vertices[21].pos = { side,side,-side };// 21
		vertices[22].pos = { -side,side,side };// 22
		vertices[23].pos = { side,side,side };// 23

		return{
			std::move(vertices),{
				0,1, 2,    2,1,3,
				4,7, 5,    4,6,7,
				8,9, 10,  10,9,11,
				12,15,13, 12,14,15,
				16,18,17, 18,19,17,
				20,21,23, 20,23,22
			}
		};
	}
	template<class V>
	static IndexedTriangleList<V> MakeIndependentTextured()
	{
		auto itl = MakeIndependent<V>();

		itl.vertices[0].tc = { 0.0f,1.0f };
		itl.vertices[1].tc = { 1.0f,1.0f };
		itl.vertices[2].tc = { 0.0f,0.0f };
		itl.vertices[3].tc = { 1.0f,0.0f };
		itl.vertices[4].tc = { 0.0f,1.0f };
		itl.vertices[5].tc = { 1.0f,1.0f };
		itl.vertices[6].tc = { 0.0f,0.0f };
		itl.vertices[7].tc = { 1.0f,0.0f };
		itl.vertices[8].tc = { 0.0f,0.0f };
		itl.vertices[9].tc = { 1.0f,0.0f };
		itl.vertices[10].tc = { 0.0f,1.0f };
		itl.vertices[11].tc = { 1.0f,1.0f };
		itl.vertices[12].tc = { 0.0f,0.0f };
		itl.vertices[13].tc = { 1.0f,0.0f };
		itl.vertices[14].tc = { 0.0f,1.0f };
		itl.vertices[15].tc = { 1.0f,1.0f };
		itl.vertices[16].tc = { 1.0f,1.0f };
		itl.vertices[17].tc = { 1.0f,0.0f };
		itl.vertices[18].tc = { 0.0f,1.0f };
		itl.vertices[19].tc = { 0.0f,0.0f };
		itl.vertices[20].tc = { 1.0f,1.0f };
		itl.vertices[21].tc = { 1.0f,0.0f };
		itl.vertices[22].tc = { 0.0f,1.0f };
		itl.vertices[23].tc = { 0.0f,0.0f };

		for (int i = 0; i < 6; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				itl.vertices[4 * i + j].id = (char)i;
			}
		}

		return itl;
	}
};