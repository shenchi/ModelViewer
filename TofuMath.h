#pragma once

#include <cstdint>
#include <cmath>

namespace tofu
{
	namespace math
	{
		template<typename T>
		struct vec2
		{
			T	x;
			T	y;
		};

		template<typename T>
		struct vec3
		{
			T	x;
			T	y;
			T	z;
		};

		template<typename T>
		struct vec4
		{
			T	x;
			T	y;
			T	z;
			T	w;
		};

		typedef vec2<float>	float2;
		typedef vec3<float>	float3;
		typedef vec4<float>	float4;

		struct float4x4
		{
			float4 x;
			float4 y;
			float4 z;
			float4 w;
		};

		typedef vec2<int32_t>	int2;
		typedef vec3<int32_t>	int3;
		typedef vec4<int32_t>	int4;

		typedef vec2<uint32_t>	uint2;
		typedef vec3<uint32_t>	uint3;
		typedef vec4<uint32_t>	uint4;

		// float2

		inline float2& operator += (float2& a, const float2 b)
		{
			a.x += b.x;
			a.y += b.y;
			return a;
		}

		inline float2& operator -= (float2& a, const float2 b)
		{
			a.x -= b.x;
			a.y -= b.y;
			return a;
		}

		inline float2& operator *= (float2& a, const float2 b)
		{
			a.x *= b.x;
			a.y *= b.y;
			return a;
		}

		inline float2& operator *= (float2& a, float b)
		{
			a.x *= b;
			a.y *= b;
			return a;
		}

		inline float2& operator /= (float2& a, float b)
		{
			a.x /= b;
			a.y /= b;
			return a;
		}

		inline float2 operator + (const float2& a, const float2& b)
		{
			return float2{ a.x + b.x, a.y + b.y };
		}

		inline float2 operator - (const float2& a, const float2& b)
		{
			return float2{ a.x - b.x, a.y - b.y };
		}

		inline float2 operator * (const float2& a, const float2& b)
		{
			return float2{ a.x * b.x, a.y * b.y };
		}

		inline float2 operator * (const float2& a, float b)
		{
			return float2{ a.x * b, a.y * b };
		}

		inline float2 operator * (float a, const float2& b)
		{
			return float2{ a * b.x, a * b.y };
		}

		inline float2 operator / (const float2& a, float b)
		{
			return float2{ a.x / b, a.y / b };
		}

		inline float dot(const float2& a, const float2& b)
		{
			return a.x * b.x + a.y * b.y;
		}

		inline float cross(const float2& a, const float2& b)
		{
			return a.x * b.y - a.y * b.x;
		}

		inline float length(const float2& a)
		{
			return std::sqrtf(a.x * a.x + a.y * a.y);
		}


		// float3

		inline float3& operator += (float3& a, const float3 b)
		{
			a.x += b.x;
			a.y += b.y;
			a.z += b.z;
			return a;
		}

		inline float3& operator -= (float3& a, const float3 b)
		{
			a.x -= b.x;
			a.y -= b.y;
			a.z -= b.z;
			return a;
		}

		inline float3& operator *= (float3& a, const float3 b)
		{
			a.x *= b.x;
			a.y *= b.y;
			a.z *= b.z;
			return a;
		}

		inline float3& operator *= (float3& a, float b)
		{
			a.x *= b;
			a.y *= b;
			a.z *= b;
			return a;
		}

		inline float3& operator /= (float3& a, float b)
		{
			a.x /= b;
			a.y /= b;
			a.z /= b;
			return a;
		}

		inline float3 operator + (const float3& a, const float3& b)
		{
			return float3{ a.x + b.x, a.y + b.y, a.z + b.z };
		}

		inline float3 operator - (const float3& a, const float3& b)
		{
			return float3{ a.x - b.x, a.y - b.y, a.z - b.z };
		}

		inline float3 operator * (const float3& a, const float3& b)
		{
			return float3{ a.x * b.x, a.y * b.y, a.z * b.z };
		}

		inline float3 operator * (const float3& a, float b)
		{
			return float3{ a.x * b, a.y * b, a.z * b };
		}

		inline float3 operator * (float a, const float3& b)
		{
			return float3{ a * b.x, a * b.y, a * b.z };
		}

		inline float3 operator / (const float3& a, float b)
		{
			return float3{ a.x / b, a.y / b, a.z / b };
		}

		inline float dot(const float3& a, const float3& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		inline float3 cross(const float3& a, const float3& b)
		{
			return float3{
				a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x
			};
		}

		inline float length(const float3& a)
		{
			return std::sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
		}


		// float4

		inline float4& operator += (float4& a, const float4 b)
		{
			a.x += b.x;
			a.y += b.y;
			a.z += b.z;
			a.w += b.w;
			return a;
		}

		inline float4& operator -= (float4& a, const float4 b)
		{
			a.x -= b.x;
			a.y -= b.y;
			a.z -= b.z;
			a.w -= b.w;
			return a;
		}

		inline float4& operator *= (float4& a, const float4 b)
		{
			a.x *= b.x;
			a.y *= b.y;
			a.z *= b.z;
			a.w *= b.w;
			return a;
		}

		inline float4& operator *= (float4& a, float b)
		{
			a.x *= b;
			a.y *= b;
			a.z *= b;
			a.w *= b;
			return a;
		}

		inline float4& operator /= (float4& a, float b)
		{
			a.x /= b;
			a.y /= b;
			a.z /= b;
			a.w /= b;
			return a;
		}

		inline float4 operator + (const float4& a, const float4& b)
		{
			return float4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
		}

		inline float4 operator - (const float4& a, const float4& b)
		{
			return float4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
		}

		inline float4 operator * (const float4& a, const float4& b)
		{
			return float4{ a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
		}

		inline float4 operator * (const float4& a, float b)
		{
			return float4{ a.x * b, a.y * b, a.z * b, a.w * b };
		}

		inline float4 operator * (float a, const float4& b)
		{
			return float4{ a * b.x, a * b.y, a * b.z, a * b.w };
		}

		inline float4 operator / (const float4& a, float b)
		{
			return float4{ a.x / b, a.y / b, a.z / b, a.w / b };
		}

		// w is ignored
		inline float dot(const float4& a, const float4& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		// w is ignored
		inline float4 cross(const float4& a, const float4& b)
		{
			return float4{
				a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x
			};
		}

		// w is ignored
		inline float length(const float4& a)
		{
			return std::sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
		}


		// quaternion

		inline float4 quat(float theta, const float3& axis)
		{
			float s = std::sinf(theta * 0.5f);
			float c = std::cosf(theta * 0.5f);
			return float4{ s * axis.x, s * axis.y, s * axis.z, c };
		}

		inline float4 quat(float pitch, float yaw, float roll)
		{
			float sp = std::sinf(pitch);
			float sy = std::sinf(yaw);
			float cp = std::cosf(pitch);
			float cy = std::cosf(yaw);
			return quat(roll, float3{cp * sy, sp, cp * cy});
		}

		// float4x4

		// row vector
		// TODO optimize it
		inline float4 mul(const float4& a, const float4x4& b)
		{
			return float4{
				a.x * b.x.x + a.y * b.y.x + a.z * b.z.x + a.w * b.w.x,
				a.x * b.x.y + a.y * b.y.y + a.z * b.z.y + a.w * b.w.y,
				a.x * b.x.z + a.y * b.y.z + a.z * b.z.z + a.w * b.w.z,
				a.x * b.x.w + a.y * b.y.w + a.z * b.z.w + a.w * b.w.w
			};
		}

		// column vector
		// TODO optimize it
		inline float4 mul(const float4x4& a, const float4& b)
		{
			return float4{
				a.x.x * b.x + a.x.y * b.y + a.x.z * b.z + a.x.w * b.w,
				a.y.x * b.x + a.y.y * b.y + a.y.z * b.z + a.y.w * b.w,
				a.z.x * b.x + a.x.y * b.y + a.z.z * b.z + a.z.w * b.w,
				a.w.x * b.x + a.w.y * b.y + a.w.z * b.z + a.w.w * b.w
			};
		}

		// TODO optimize it
		inline float4x4 mul(const float4x4& a, const float4x4& b)
		{
			return float4x4{
				mul(a.x, b),
				mul(a.y, b),
				mul(a.z, b),
				mul(a.w, b)
			};
		}

		inline float4x4 transpose(const float4x4& a)
		{
			return float4x4{
				float4{ a.x.x, a.y.x, a.z.x, a.w.x },
				float4{ a.x.y, a.y.y, a.z.y, a.w.y },
				float4{ a.x.z, a.y.z, a.z.z, a.w.z },
				float4{ a.x.w, a.y.w, a.z.w, a.w.w }
			};
		}

		inline float4x4 identity()
		{
			return float4x4{
				float4{ 1.0f, 0.0f, 0.0f, 0.0f },
				float4{ 0.0f, 1.0f, 0.0f, 0.0f },
				float4{ 0.0f, 0.0f, 1.0f, 0.0f },
				float4{ 0.0f, 0.0f, 0.0f, 1.0f }
			};
		}

		inline float4x4 perspective(float fov, float aspect, float zNear, float zFar)
		{
			float yScale = 1.0f / std::tanf(fov * 0.5f);
			float xScale = yScale / aspect;
			float zScale = zFar / (zFar - zNear);
			float zOffset = zFar * zNear / (zNear - zFar);

			return float4x4{
				float4{ xScale, 0.0f, 0.0f, 0.0f },
				float4{ 0.0f, yScale, 0.0f, 0.0f },
				float4{ 0.0f, 0.0f, zScale, zOffset },
				float4{ 0.0f, 0.0f, 1.0, 0.0f }
			};
		}

		inline float4x4 translate(const float3& t)
		{
			return float4x4{
				float4{ 1.0f, 0.0f, 0.0f, t.x },
				float4{ 0.0f, 1.0f, 0.0f, t.y },
				float4{ 0.0f, 0.0f, 1.0f, t.z },
				float4{ 0.0f, 0.0f, 0.0f, 1.0f }
			};
		}

		inline float4x4 translate(float x, float y, float z)
		{
			return float4x4{
				float4{ 1.0f, 0.0f, 0.0f, x },
				float4{ 0.0f, 1.0f, 0.0f, y },
				float4{ 0.0f, 0.0f, 1.0f, z },
				float4{ 0.0f, 0.0f, 0.0f, 1.0f }
			};
		}

		inline float4x4 rotate(const float4& q)
		{

		}

		inline float4x4 scale(const float3& s)
		{
			return float4x4{
				float4{ s.x, 0.0f, 0.0f, 0.0f },
				float4{ 0.0f, s.y, 0.0f, 0.0f },
				float4{ 0.0f, 0.0f, s.z, 0.0f },
				float4{ 0.0f, 0.0f, 0.0f, 1.0f }
			};
		}

		inline float4x4 scale(float s)
		{
			return float4x4{
				float4{ s, 0.0f, 0.0f, 0.0f },
				float4{ 0.0f, s, 0.0f, 0.0f },
				float4{ 0.0f, 0.0f, s, 0.0f },
				float4{ 0.0f, 0.0f, 0.0f, 1.0f }
			};
		}

		inline float4x4 scale(float x, float y, float z)
		{
			return float4x4{
				float4{ x, 0.0f, 0.0f, 0.0f },
				float4{ 0.0f, y, 0.0f, 0.0f },
				float4{ 0.0f, 0.0f, z, 0.0f },
				float4{ 0.0f, 0.0f, 0.0f, 1.0f }
			};
		}
	}
}