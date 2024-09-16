
// 和 crc32校验算法 有关的代码

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ngx_c_crc32.h"

// CRC32 是一种广泛使用的校验算法，能够检测数据传输或存储过程中的错误。
// 类静态变量初始化
CCRC32 *CCRC32::m_instance = NULL;

// 构造函数
CCRC32::CCRC32()
{
	Init_CRC32_Table();
}
// 释放函数
CCRC32::~CCRC32()
{
}
// 初始化crc32表辅助函数
// unsigned long CCRC32::Reflect(unsigned long ref, char ch)
// Reflect 函数用于反转位。它将 ref 参数的最低位反射到最高位，第二低位反射到第二高位，以此类推。
unsigned int CCRC32::Reflect(unsigned int ref, char ch)
{
	// Used only by Init_CRC32_Table()
	// unsigned long value(0);
	unsigned int value(0);
	// Swap bit 0 for bit 7 , bit 1 for bit 6, etc.
	for (int i = 1; i < (ch + 1); i++)
	{
		if (ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}
// 初始化crc32表
// 这个表用于加速 CRC32 的计算。表的每个条目都是通过将索引值反射，然后通过多次左移和与多项式的 XOR 操作得到的。
void CCRC32::Init_CRC32_Table()
{
	// This is the official polynomial used by CRC-32 in PKZip, WinZip and Ethernet.
	// unsigned long ulPolynomial = 0x04c11db7;
	unsigned int ulPolynomial = 0x04c11db7;

	// 256 values representing ASCII character codes.
	for (int i = 0; i <= 0xFF; i++)
	{
		crc32_table[i] = Reflect(i, 8) << 24;
		// if (i == 1)printf("old1--i=%d,crc32_table[%d] = %lu\r\n",i,i,crc32_table[i]);

		for (int j = 0; j < 8; j++)
		{
			// if(i == 1)
			//{
			//     unsigned int tmp1 = (crc32_table[i] << 1);
			//     unsigned int tmp2 = (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
			//     unsigned int tmp3 = tmp1 ^ tmp2;
			//     tmp3 += 1;
			//     tmp3 -= 1;
			//
			// }

			crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
			// if (i == 1)printf("old3--i=%d,crc32_table[%d] = %lu\r\n",i,i,crc32_table[i]);
		}
		// if (i == 1)printf("old2--i=%d,crc32_table[%d] = %lu\r\n",i,i,crc32_table[i]);
		crc32_table[i] = Reflect(crc32_table[i], 32);
	}
}
// 用crc32_table寻找表来产生数据的CRC值
// int CCRC32::Get_CRC(unsigned char* buffer, unsigned long dwSize)
int CCRC32::Get_CRC(unsigned char *buffer, unsigned int dwSize)
{
	// Be sure to use unsigned variables,
	// because negative values introduce high bits
	// where zero bits are required.
	// unsigned long  crc(0xffffffff);
	unsigned int crc(0xffffffff); // 初始化 crc 变量为 0xffffffff。这是 CRC 计算的常规初始值，用来确保在计算开始时 CRC 寄存器是完全设置的。
	int len;					  // len 变量用于控制循环，它设置为传入数据的大小 dwSize，即 buffer 数组中的字节数。

	len = dwSize;
	// Perform the algorithm on each character
	// in the string, using the lookup table values.
	// crc >> 8：将当前 crc 值右移8位。这是因为我们每次处理8位数据（1字节），右移是为了处理下一个字节的准备。
	// crc & 0xFF：取 crc 当前值的最低8位。
	// *buffer++：取当前 buffer 指向的字节，并将指针向前移动一个字节，以便下一次迭代处理下一个字节。
	// crc32_table[(crc & 0xFF) ^ *buffer++]：使用 XOR 运算结合 crc 的最低8位和当前字节的值，得到一个索引，并使用此索引从预计算的 crc32_table 查找表中获取值。这一步是实际的 CRC 计算，通过查表方式迅速得到对应的 CRC 计算结果。
	while (len--)
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *buffer++]; // 将右移后的 crc 与查找表得到的值进行 XOR，得到新的 crc 值，为下一轮迭代做准备。
	// Exclusive OR the result with the beginning value.
	return crc ^ 0xffffffff; // 将计算完的 crc 值与 0xffffffff 进行 XOR 运算。这通常是 CRC 计算的标准步骤，用于取反所有位，得到最终的 CRC 校验码。
}
