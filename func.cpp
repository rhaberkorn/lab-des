//
// Created by 8752355675 on 3/6/23.
//

#include<iostream>
#include<string>
#include<bitset>
#include "des.h"

using namespace std;

DES::DES()
= default;


DES::~DES()
= default;

bitset<32> DES::F(bitset<32> R, bitset<48> k) {
    // Расширение электронного блока
    bitset<48> expandR;
    for (int i = 0; i < 48; i++)
        expandR[47 - i] = R[32 - E[i]];
    //cout << "E: " << expandR << endl;
    // XOR
    expandR = expandR ^ k;
    // вместо этого S-поле
    bitset<32> output;
    int x = 0;
    for (int i = 0; i < 48; i = i + 6)
    {
        int row = ((int)expandR[i+5] << 1) | expandR[i];
        int col = (expandR >> (i+1)).to_ulong() & 0xF;
        int num = S_BOX[7 - i/6][row][col];
        bitset<4> temp(num); // FIXME
        output[x + 3] = temp[3];
        output[x + 2] = temp[2];
        output[x + 1] = temp[1];
        output[x + 0] = temp[0];
        x += 4;
    }
    //cout << "S: " << output << endl;
    // Замена P-бокса
    bitset<32> tmp = output;
    for (int i = 0; i < 32; i++)
        output[31 - i] = tmp[32 - P[i]];
    //cout << "P: " << output << endl;

    return output;
}
// Функция сдвига влево
// NOTE: Это операция обычно называется left rotation!
bitset<28> DES::leftshift(bitset<28> k, int shift) {
//    return (k >> shift) | (k << (28 - shift));
    return (k << shift) | (k >> (28 - shift));
}

void DES::generateKeys() {
    bitset<56> real_key;
    bitset<28> left;
    bitset<28> right;
    bitset<48> compressKey;

    // Во-первых, после выбора и замены ПК-1 удаляем 8-битный бит четности исходного ключа
    // И переставляем
    for (int i = 0; i < 56; i++)
        real_key[55 - i] = key[64 - PC_1[i]];

    // NOTE: Обычно real_key[0] == младший разряд
    for (int i = 0; i < 28; i++)
        right[i] = real_key[i];
    for (int i = 28; i < 56; i++)
        left[i - 28] = real_key[i];

    for (int round = 0; round < 16; round++)
    {
        // Сдвиг влево
        left = leftshift(left, shiftBits[round]);
        right = leftshift(right, shiftBits[round]);
        // Подключаем, заменяем и выбираем ПК-2 для перестановки и сжатия
        for (int i=0; i < 28; i++)
            real_key[i] = right[i];
        for (int i = 28; i < 56; i++)
            real_key[i] = left[i - 28];
        for (int i = 0; i < 48; i++)
            compressKey[47 - i] = real_key[56 - PC_2[i]];

        subkey[round] = compressKey;

	//cout << "K" << (round+1) << ": " << subkey[round] << endl;
    }

}


// Функция инструмента: преобразовать массив символов char в двоичный
bitset<64> DES::char_to_bit(const char s[8]) {
    bitset<64> bits(htobe64(*(uint64_t *)s));
#if 0
    int x = 0;
    for (int i = 0; i < 8; i++)
    {
        int num = int(s[i]);
        bitset<8> temp(num);
        for (int j = 7; j >= 0; j--)
        {
            bits[x + j] = temp[7 - j];
        }
        x += 8;
    }
#endif
    return bits;
}

bitset<64> DES::transcrypt(bitset<64> &plain, bool decrypt) {
    bitset<64> cipher;
    bitset<64> currentBits;
    bitset<32> left;
    bitset<32> right;
    // Первоначальная замена IP
    for (int i = 0; i < 64; i++)
        currentBits[63 - i] = plain[64 - IP[i]];
    //cout << "IP: " << currentBits << endl;

    for (int i = 0; i < 32; i++)
        right[i] = currentBits[i];
    for (int i = 32; i < 64; i++)
        left[i - 32] = currentBits[i];
    // Вводим 16 раундов изменения
    for (int i = 0; i < 16; i++)
    {
        bitset<48> key = decrypt ? subkey[15-i] : subkey[i];

        bitset<32> oldLeft = left;
        left = right;
        right = oldLeft ^ F(right, key);

        //cout << "R: " << right << endl;
    }
    // Слияние: как раз сначала right и потом left!
    for (int i = 0; i < 32; i++)
        cipher[i] = left[i];
    for (int i = 32; i < 64; i++)
        cipher[i] = right[i - 32];
    // Обратная инициализация замены
    currentBits = cipher;

    for (int i = 0; i < 64; i++)
        cipher[63 - i] = currentBits[64 - IP_1[i]];

    return cipher;
}

static_assert(sizeof(uint64_t) == sizeof(unsigned long long));

void DES::show_encryp() {
    bitset<64> plain = char_to_bit(s.c_str());
    key = char_to_bit(k.c_str());
    cout << "Plain: " << plain << endl << "K: " << key << endl;
    // Создание 16 подключей
    generateKeys();
    // зашифрованный текст в 1.txt
    bitset<64> cipher = transcrypt(plain, false);
    cout << "Cipher: " << cipher << endl;
    fstream file1;
    file1.open("1.txt", ios::binary | ios::out);
    uint64_t cipher_uint = htobe64(cipher.to_ullong());
    file1.write((char*)&cipher_uint, sizeof(cipher_uint));
    file1.close();

    plain = transcrypt(cipher, true);
    fstream file2;
    file2.open("2.txt", ios::binary | ios::out);
    uint64_t plain_uint = htobe64(plain.to_ullong());
    file2.write((char*)&plain_uint, sizeof(plain_uint));
    file2.close();
}
