#include <iostream>  

namespace digit_conversion
{
	char digits[] =
	{
		'0', '1', '2', '3', '4', '5',
		'6', '7', '8', '9', 'a', 'b',
		'c', 'd', 'e', 'f'
	};

	std::string ToUnsignedString(int i, int shift)
	{
		char  buf[32];
		char * pBuf = buf;
		int charPos = 32;
		int radix = 1 << shift;
		int mask = radix - 1;
		do
		{
			pBuf[--charPos] = digits[i&mask];
			i = i >> shift;
		} while (i != 0);

		std::string str;
		int strLen = 32 - charPos;
		pBuf = pBuf + charPos;
		while (strLen)
		{
			str.push_back(*pBuf);
			pBuf++;
			strLen--;
		}
		return str;
	}

	std::string ToHexString(int i)
	{
		return ToUnsignedString(i, 4);
	}

	std::string ToOctalString(int i)
	{
		return  ToUnsignedString(i, 3);
	}

	std::string ToBinaryString(int i)
	{
		return  ToUnsignedString(i, 1);
	}

	int main()
	{
		std::cout << ToHexString(200).c_str() << std::endl;
		//std::cout << ToOctalString(200).c_str() << std::endl;
		//std::cout << ToBinaryString(200).c_str() << std::endl;
		std::cin.get();
		return 0;
	}
}