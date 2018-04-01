//
// Created by adunstudio on 2018-01-07.
//

#pragma once

#include "FBXHeader.h"

namespace sunny
{
	namespace maths
	{
		const float SUNNY_PI = 3.14159265358f;

		// ���� ������ ������ ���ڸ� ���� ������ ȯ���Ͽ� ��ȯ�Ѵ�.
		inline float toRadians(float degree)
		{
			return static_cast<float>(degree * (SUNNY_PI / 180.0f));
		};

		// ���� ������ ������ ���ڸ� ������ ȯ���Ͽ� ��ȯ�Ѵ�.
		inline float toDegrees(float radians)
		{
			return static_cast<float>(radians * (180.0f / SUNNY_PI));
		}

		// ������ ���� ���� ��ȣ�� ��ȯ�Ѵ�. ����̸� 1��, �����̸� -1��, 0�̸� 0�� ��ȯ�Ѵ�
		inline int sign(float value)
		{
			//  1:  true  -  false =  1
			//  0: false  -  false =  0
			// -1: false  -   true = -1
			return (value > 0) - (value < 0);
		}

		// ������ ����(sine)���� ��ȯ�ϸ�, ���� angle�� ���� ���̾�� �Ѵ�.
		inline float sin(float angle)
		{   // ���� / ����
			return ::sin(angle);
		}

		// ������ �ڻ���(cosine)���� ��ȯ�ϸ�, ���� angle�� ���� ���̾�� �Ѵ�.
		inline float cos(float angle)
		{   // �غ� / ����
			return ::cos(angle);
		}

		// ������ ź��Ʈ(tangent)���� ��ȯ�ϸ�, ���� angle�� ���� ���̾�� �Ѵ�.
		inline float tan(float angle)
		{   // ���� / �غ�
			return ::tan(angle);
		}

		// ���ڰ��� �������� ��ȯ�Ѵ�.
		inline float sqrt(float value)
		{
			return ::sqrt(value);
		}

		// ���ڰ� �������� ������ ��ȯ�Ѵ�.
		inline float rsqrt(float value)
		{
			return 1.0f / ::sqrt(value);
		}

		// ������ ��ũ����(arcsine)���� ��ȯ�Ѵ�.
		inline float asin(float value)
		{
			return ::asin(value);
		}

		// ������ ��ũ�ڻ���(arccosine)���� ��ȯ�Ѵ�.
		inline float acon(float value)
		{
			return ::acos(value);
		}

		// ������ ��ũź��Ʈ(arctangent)���� ��ȯ�Ѵ�.
		inline float atan(float value)
		{
			return ::atan(value);
		}

		// ������ ��ũź��Ʈ(arctangent)���� ��ȯ�Ѵ�.
		//inline float atan2(float y, float x)
		//{
		//	return ::atan2(y, x);
		//}

		// �ּҰ� ���� �Լ�
		inline float _min(float value, float minimum)
		{
			return (value < minimum) ? minimum : value;
		}

		// �ִ밪 ���� �Լ�
		inline float _max(float value, float maximum)
		{
			return (value > maximum) ? maximum : value;
		}

		// �ּҰ��� �ִ밪 ���� �Լ�
		inline float clamp(float value, float minimum, float maximum)
		{
			return (value > minimum) ? (value < maximum) ? value : maximum : minimum;
		}
	}
}
