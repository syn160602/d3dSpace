#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "myMath.h"

class Sphere
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTextured(int latDiv, int longDiv)
	{
		namespace dx = DirectX;
		assert(latDiv >= 3);
		assert(longDiv >= 3);

		constexpr float radius = 1.0f;
		const auto base = dx::XMVectorSet(0.0f, radius, 0.0f, 0.0f);
		const float lattitudeAngle = PI / latDiv;
		const float longitudeAngle = -(2.0f * PI / longDiv);

		std::vector<V> vertices;
		for (int iLat = 1; iLat < latDiv; iLat++)
		{
			const auto latBase = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationX(lattitudeAngle * iLat)
			);
			float tcY = (float)iLat / (float)latDiv;
			for (int iLong = 0; iLong <= longDiv; iLong++)
			{
				vertices.emplace_back();
				auto v = dx::XMVector3Transform(
					latBase,
					dx::XMMatrixRotationY(longitudeAngle * iLong)
				);
				float tcX = (float)iLong / (float)longDiv;
				dx::XMStoreFloat3(&vertices.back().pos, v);
				vertices.back().tc = {tcX, tcY};
			}
		}

		// add the cap vertices
		const auto iNorthPole = (unsigned short)vertices.size();
		for (int iLong = 0; iLong < longDiv; iLong++) 
		{
			vertices.emplace_back();
			dx::XMStoreFloat3(&vertices.back().pos, base);
			vertices.back().tc = { ((float)iLong + 0.5f) / (float)longDiv, 0.0f };
		}
		const auto iSouthPole = (unsigned short)vertices.size();
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			dx::XMStoreFloat3(&vertices.back().pos, base);
			vertices.back().tc = { ((float)iLong + 0.5f) / (float)longDiv, 1.0f };
		}

		const auto calcIdx = [latDiv, longDiv](unsigned short iLat, unsigned short iLong)
		{ return iLat * (longDiv + 1) + iLong; };
		std::vector<unsigned short> indices;
		for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
		{
			for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
			{
				indices.push_back(calcIdx(iLat, iLong));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong));
			}
			// wrap band
			indices.push_back(calcIdx(iLat, longDiv - 1));
			indices.push_back(calcIdx(iLat, longDiv));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat, longDiv));
			indices.push_back(calcIdx(iLat + 1, longDiv));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
		}

		// cap fans
		for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
		{
			// north
			indices.push_back(iNorthPole + iLong);
			indices.push_back(calcIdx(0, iLong + 1));
			indices.push_back(calcIdx(0, iLong));
			// south
			indices.push_back(calcIdx(latDiv - 2, iLong + 1));
			indices.push_back(iSouthPole + iLong);
			indices.push_back(calcIdx(latDiv - 2, iLong));
		}
		// wrap triangles
		// north
		indices.push_back(iNorthPole + longDiv - 1);
		indices.push_back(calcIdx(0, longDiv));
		indices.push_back(calcIdx(0, longDiv - 1));
		// south
		indices.push_back(calcIdx(latDiv - 2, longDiv));
		indices.push_back(iSouthPole + longDiv - 1);
		indices.push_back(calcIdx(latDiv - 2, longDiv - 1));

		return { std::move(vertices),std::move(indices) };
	}
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(12, 24);
	}

	template<class V>
	static IndexedTriangleList<V> MakeTexturedIndependentCapNormals(int latDiv, int longDiv)
	{
		namespace dx = DirectX;
		assert(latDiv >= 3);
		assert(longDiv >= 3);

		constexpr float radius = 1.0f;
		const auto base = dx::XMVectorSet(0.0f, radius, 0.0f, 0.0f);
		const float lattitudeAngle = PI / latDiv;
		const float longitudeAngle = -(2.0f * PI / longDiv);

		std::vector<V> vertices;
		for (int iLat = 1; iLat < latDiv; iLat++)
		{
			const auto latBase = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationX(lattitudeAngle * iLat)
			);
			float tcY = (float)iLat / (float)latDiv;
			for (int iLong = 0; iLong <= longDiv; iLong++)
			{
				vertices.emplace_back();
				auto v = dx::XMVector3Transform(
					latBase,
					dx::XMMatrixRotationY(longitudeAngle * iLong)
				);
				float tcX = (float)iLong / (float)longDiv;
				dx::XMStoreFloat3(&vertices.back().pos, v);
				vertices.back().n = { vertices.back().pos.x,vertices.back().pos.y,vertices.back().pos.z };
				vertices.back().tc = { tcX, tcY };
			}
		}

		// add the cap vertices
		const auto iNorthPole = (unsigned short)vertices.size();
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			dx::XMStoreFloat3(&vertices.back().pos, base);
			vertices.back().n = { vertices.back().pos.x,vertices.back().pos.y,vertices.back().pos.z };
			vertices.back().tc = { ((float)iLong + 0.5f) / (float)longDiv, 0.0f };
		}
		const auto iSouthPole = (unsigned short)vertices.size();
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			dx::XMStoreFloat3(&vertices.back().pos, dx::XMVectorNegate(base));
			vertices.back().n = { vertices.back().pos.x,vertices.back().pos.y,vertices.back().pos.z };
			vertices.back().tc = { ((float)iLong + 0.5f) / (float)longDiv, 1.0f };
		}

		const auto calcIdx = [latDiv, longDiv](unsigned short iLat, unsigned short iLong)
		{ return iLat * (longDiv + 1) + iLong; };
		std::vector<unsigned short> indices;
		for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
		{
			for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
			{
				indices.push_back(calcIdx(iLat, iLong));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong));
			}
			// wrap band
			indices.push_back(calcIdx(iLat, longDiv - 1));
			indices.push_back(calcIdx(iLat, longDiv));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat, longDiv));
			indices.push_back(calcIdx(iLat + 1, longDiv));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
		}

		// cap fans
		for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
		{
			// north
			indices.push_back(iNorthPole + iLong);
			indices.push_back(calcIdx(0, iLong + 1));
			indices.push_back(calcIdx(0, iLong));
			// south
			indices.push_back(calcIdx(latDiv - 2, iLong + 1));
			indices.push_back(iSouthPole + iLong);
			indices.push_back(calcIdx(latDiv - 2, iLong));
		}
		// wrap triangles
		// north
		indices.push_back(iNorthPole + longDiv - 1);
		indices.push_back(calcIdx(0, longDiv));
		indices.push_back(calcIdx(0, longDiv - 1));
		// south
		indices.push_back(calcIdx(latDiv - 2, longDiv));
		indices.push_back(iSouthPole + longDiv - 1);
		indices.push_back(calcIdx(latDiv - 2, longDiv - 1));

		return { std::move(vertices),std::move(indices) };
	}
};