#pragma once
#include "cjw.hpp"

class DES {
public:
	static std::string SHA1(std::string message) {
		unsigned H[5] = { 0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476,0xC3D2E1F0 }, Message_Digest[5]{0};
		unsigned Length_Low = 0, Length_High = 0 , Message_Block_Index =0;
		unsigned char Message_Block[64];    // 512-bit message blocks
		bool Corrupted = false;
		// 处理接下来512位数据的信息
		auto ProcessMessageBlock = [&]() {
			// SHA-1 常量
			const unsigned K[] = { 0x5A827999,0x6ED9EBA1,0x8F1BBCDC,0xCA62C1D6 };
			unsigned list[80], buffer[5];

			//循环左移位
			auto CircularShift = [&](int bits, unsigned word) {
				return ((word << bits) & 0xFFFFFFFF) | ((word & 0xFFFFFFFF) >> (32 - bits));
			};

			auto Operation = [&](int type, int pos) {
				unsigned A = buffer[0], B = buffer[1], C = buffer[2], D = buffer[3], E = buffer[4];
				auto temp = 0;
				switch (type) {
				case 0:
					temp = (CircularShift(5, A) + ((B & C) | (~B & D)) + E + list[pos] + K[0]);
					break;
				case 1:
					temp = CircularShift(5, A) + (B ^ C ^ D) + E + list[pos] + K[1];
					break;
				case 2:
					temp = CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + list[pos] + K[2];
					break;
				case 3:
					temp = CircularShift(5, A) + (B ^ C ^ D) + E + list[pos] + K[3];
					break;
				default:
					break;
				}
				temp &= 0xFFFFFFFF;
				E = D;	D = C;	C = CircularShift(30, B);	B = A;	A = temp;
				return;
			};

			for (int i = 0; i < 16; i++)
				list[i] = ((unsigned)Message_Block[i * 4]) << 24
				| ((unsigned)Message_Block[i * 4 + 1]) << 16
				| ((unsigned)Message_Block[i * 4 + 2]) << 8
				| ((unsigned)Message_Block[i * 4 + 3]);

			for (int i = 16; i < 80; i++)
				list[i] = CircularShift(1, list[i - 3] ^ list[i - 8] ^ list[i - 14] ^ list[i - 16]);

			for (int i = 0; i < 5; i++)
				buffer[i] = H[i];

			for (int i = 0; i < 80; i++) {
				auto type = i / 20;
				Operation(type, i);
			}

			for (int i = 0; i < 5; i++)
				H[i] = (H[i] + buffer[i]) & 0xFFFFFFFF;

			Message_Block_Index = 0;
		};
		//填充数据至512位
		auto PadMessage = [&]() {
			Message_Block[Message_Block_Index] = 0x80;
			if (Message_Block_Index++ > 55) {
				while (Message_Block_Index < 64)
					Message_Block[Message_Block_Index++] = 0;
				ProcessMessageBlock();
			}
			while (Message_Block_Index < 56)
				Message_Block[Message_Block_Index++] = 0;

			//将数据长度储存至最后八字节
			for (int i = 0, j = 24; i < 4; i++, j -= 8) {
				Message_Block[56 + i] = (Length_High >> j) & 0xFF;
				Message_Block[60 + i] = (Length_Low >> j) & 0xFF;
			}
			ProcessMessageBlock();
		};

		auto str = (const unsigned char*)message.data();
		while (*str) {
			Message_Block[Message_Block_Index++] = (*str & 0xFF);
			Length_Low = (Length_Low + 8) & 0xFFFFFFFF;
			if (Length_Low == 0) {
				Length_High = (Length_High + 1) & 0xFFFFFFFF;
				if (Length_High == 0) 
					_ERROR_(ENCRYPT_DATA_ERROR);
			}
			if (Message_Block_Index == 64)
				ProcessMessageBlock();
			str++;
		}
		PadMessage();
		//将主机数转换成无符号长整型的网络字节顺序，（将可能以小端储存的主机数换成大端存储）
		for (int i = 0; i < 5; i++)
			Message_Digest[i] = htonl(H[i]);

		return std::string((char *)reinterpret_cast<const unsigned char*>(Message_Digest)).substr(0, 20);
	}

	static std::string MD5(std::string message,bool flag = false) {
		unsigned count[2] = { 0 }, state[4] = { 0x67452301 ,0xEFCDAB89 ,0x98BADCFE ,0x10325476 };
		unsigned char buffer[64], bits[8], result[16];
		unsigned char PADDING[] = {
			0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		auto MD5Encode = [](unsigned char *output, unsigned int *input, unsigned int len) {
			for (unsigned i = 0; i < len; i++)
				output[i] = input[int(i / 4)] >> (i % 4) * 8 & 0xFF;
		};
		auto MD5Update = [&](unsigned char *input, unsigned int len) {
			auto MD5Transform = [](unsigned int state[4], unsigned char block[64]) {

				auto NLF = [=](unsigned type, unsigned &a, unsigned b, unsigned c, unsigned d, unsigned x, unsigned s, unsigned ac) {
					if (type == 0)		a += ((b & c) | (~b & d)) + x + ac;
					else if (type == 1)		a += ((b & d) | (c & ~d)) + x + ac;
					else if (type == 2)	a += (b^c^d) + x + ac;
					else if (type == 3)	a += (c ^ (b | ~d)) + x + ac;
					a = ((a << s) | (a >> (32 - s))) + b;

				};

				unsigned int a = state[0], b = state[1], c = state[2], d = state[3], x[64];

				for (unsigned i = 0, j = 0; j < 64; i++, j += 4)
					x[i] = (block[j]) | (block[j + 1] << 8) | (block[j + 2] << 16) | (block[j + 3] << 24);

				NLF(0, a, b, c, d, x[0], 7, 0xd76aa478);
				NLF(0, d, a, b, c, x[1], 12, 0xe8c7b756);
				NLF(0, c, d, a, b, x[2], 17, 0x242070db);
				NLF(0, b, c, d, a, x[3], 22, 0xc1bdceee);
				NLF(0, a, b, c, d, x[4], 7, 0xf57c0faf);
				NLF(0, d, a, b, c, x[5], 12, 0x4787c62a);
				NLF(0, c, d, a, b, x[6], 17, 0xa8304613);
				NLF(0, b, c, d, a, x[7], 22, 0xfd469501);
				NLF(0, a, b, c, d, x[8], 7, 0x698098d8);
				NLF(0, d, a, b, c, x[9], 12, 0x8b44f7af);
				NLF(0, c, d, a, b, x[10], 17, 0xffff5bb1);
				NLF(0, b, c, d, a, x[11], 22, 0x895cd7be);
				NLF(0, a, b, c, d, x[12], 7, 0x6b901122);
				NLF(0, d, a, b, c, x[13], 12, 0xfd987193);
				NLF(0, c, d, a, b, x[14], 17, 0xa679438e);
				NLF(0, b, c, d, a, x[15], 22, 0x49b40821);

				NLF(1, a, b, c, d, x[1], 5, 0xf61e2562);
				NLF(1, d, a, b, c, x[6], 9, 0xc040b340);
				NLF(1, c, d, a, b, x[11], 14, 0x265e5a51);
				NLF(1, b, c, d, a, x[0], 20, 0xe9b6c7aa);
				NLF(1, a, b, c, d, x[5], 5, 0xd62f105d);
				NLF(1, d, a, b, c, x[10], 9, 0x2441453);
				NLF(1, c, d, a, b, x[15], 14, 0xd8a1e681);
				NLF(1, b, c, d, a, x[4], 20, 0xe7d3fbc8);
				NLF(1, a, b, c, d, x[9], 5, 0x21e1cde6);
				NLF(1, d, a, b, c, x[14], 9, 0xc33707d6);
				NLF(1, c, d, a, b, x[3], 14, 0xf4d50d87);
				NLF(1, b, c, d, a, x[8], 20, 0x455a14ed);
				NLF(1, a, b, c, d, x[13], 5, 0xa9e3e905);
				NLF(1, d, a, b, c, x[2], 9, 0xfcefa3f8);
				NLF(1, c, d, a, b, x[7], 14, 0x676f02d9);
				NLF(1, b, c, d, a, x[12], 20, 0x8d2a4c8a);

				NLF(2, a, b, c, d, x[5], 4, 0xfffa3942);
				NLF(2, d, a, b, c, x[8], 11, 0x8771f681);
				NLF(2, c, d, a, b, x[11], 16, 0x6d9d6122);
				NLF(2, b, c, d, a, x[14], 23, 0xfde5380c);
				NLF(2, a, b, c, d, x[1], 4, 0xa4beea44);
				NLF(2, d, a, b, c, x[4], 11, 0x4bdecfa9);
				NLF(2, c, d, a, b, x[7], 16, 0xf6bb4b60);
				NLF(2, b, c, d, a, x[10], 23, 0xbebfbc70);
				NLF(2, a, b, c, d, x[13], 4, 0x289b7ec6);
				NLF(2, d, a, b, c, x[0], 11, 0xeaa127fa);
				NLF(2, c, d, a, b, x[3], 16, 0xd4ef3085);
				NLF(2, b, c, d, a, x[6], 23, 0x4881d05);
				NLF(2, a, b, c, d, x[9], 4, 0xd9d4d039);
				NLF(2, d, a, b, c, x[12], 11, 0xe6db99e5);
				NLF(2, c, d, a, b, x[15], 16, 0x1fa27cf8);
				NLF(2, b, c, d, a, x[2], 23, 0xc4ac5665);

				NLF(3, a, b, c, d, x[0], 6, 0xf4292244);
				NLF(3, d, a, b, c, x[7], 10, 0x432aff97);
				NLF(3, c, d, a, b, x[14], 15, 0xab9423a7);
				NLF(3, b, c, d, a, x[5], 21, 0xfc93a039);
				NLF(3, a, b, c, d, x[12], 6, 0x655b59c3);
				NLF(3, d, a, b, c, x[3], 10, 0x8f0ccc92);
				NLF(3, c, d, a, b, x[10], 15, 0xffeff47d);
				NLF(3, b, c, d, a, x[1], 21, 0x85845dd1);
				NLF(3, a, b, c, d, x[8], 6, 0x6fa87e4f);
				NLF(3, d, a, b, c, x[15], 10, 0xfe2ce6e0);
				NLF(3, c, d, a, b, x[6], 15, 0xa3014314);
				NLF(3, b, c, d, a, x[13], 21, 0x4e0811a1);
				NLF(3, a, b, c, d, x[4], 6, 0xf7537e82);
				NLF(3, d, a, b, c, x[11], 10, 0xbd3af235);
				NLF(3, c, d, a, b, x[2], 15, 0x2ad7d2bb);
				NLF(3, b, c, d, a, x[9], 21, 0xeb86d391);

				state[0] += a;
				state[1] += b;
				state[2] += c;
				state[3] += d;
			};

			unsigned i = 0, index = (count[0] >> 3) & 0x3F, partlen = 64 - index;
			count[0] += len << 3;
			if (count[0] < (len << 3))
				count[1]++;
			count[1] += len >> 29;

			if (len >= partlen) {
				memcpy(&buffer[index], input, partlen);
				MD5Transform(state, buffer);
				for (i = partlen; i + 64 <= len; i += 64)
					MD5Transform(state, &input[i]);
				index = 0;
			}
			memcpy(&buffer[index], &input[i], len - i);
		};

		MD5Update((unsigned char *)message.data(), strlen(message.data()));
		MD5Encode(bits, count, 8);
		auto temp = (count[0] >> 3) & 0x3F;
		MD5Update(PADDING, (temp < 56) ? (56 - temp) : (120 - temp));
		MD5Update(bits, 8);
		MD5Encode(result, state, 16);

		return flag ? std::string((char *)result).substr(4, 8) : std::string((char *)result).substr(0, 16);

	}
};

