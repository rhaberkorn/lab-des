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
        expandR[47 - i] = R[32 - E[i]];  //expandR[i] = R[E[i] - 1];
    // XOR
    expandR = expandR ^ k;
    // вместо этого S-поле
    bitset<32> output;
    int x = 0;
    for (int i = 0; i < 48; i = i + 6)
    {
        int row = expandR[i] * 2 + expandR[i + 5];
        int col = expandR[i + 1] * 8 + expandR[i + 2] * 4 + expandR[i + 3] * 2 + expandR[i + 4];
        int num = S_BOX[i / 6][row][col];
        bitset<4> temp(num);
        output[x + 3] = temp[0];
        output[x + 2] = temp[1];
        output[x + 1] = temp[2];
        output[x] = temp[3];
        x += 4;
    }
    // Замена P-бокса
    bitset<32> tmp = output;
    for (int i = 0; i < 32; i++)
        output[i] = tmp[P[i] - 1];

    return output;
}
// Функция сдвига влево
bitset<28> DES::leftshift(bitset<28> k, int shift) {
    bitset<28> temp = k;
    if (shift == 1)
    {
        for (int i = 0; i < 27; i++)
        {
            if (i - shift < 0)
                k[i - shift + 28] = temp[i];
            else
                k[i] = temp[i + shift];
        }
    }
    if (shift == 2)
    {
        for (int i = 0; i < 26; i++)
        {
            if (i - shift < 0)
                k[i - shift + 28] = temp[i];
            else
                k[i] = temp[i + shift];
        }
    }
    return k;
}

void DES::generateKeys() {
    bitset<56> real_key;
    bitset<28> left;
    bitset<28> right;
    bitset<48> compressKey;

    // Во-первых, после выбора и замены ПК-1 удаляем 8-битный бит четности исходного ключа
    // И переставляем
    for (int i = 0; i < 56; i++)
        real_key[i] = key[PC_1[i] - 1];

    for (int round = 0; round < 16; round++)
    {
        for (int i = 0; i < 28; i++)
            left[i] = real_key[i];
        for (int i = 28; i < 56; i++)
            right[i - 28] = real_key[i];
        // Сдвиг влево
        left = leftshift(left, shiftBits[round]);
        right = leftshift(right, shiftBits[round]);
        // Подключаем, заменяем и выбираем ПК-2 для перестановки и сжатия
        for (int i=0; i < 28; i++)
            real_key[i] = left[i];
        for (int i = 28; i < 56; i++)
            real_key[i] = right[i - 28];
        for (int i = 0; i < 48; i++)
        {
            int m = PC_2[i];
            compressKey[i] = real_key[m - 1];
        }

        subkey[round] = compressKey;
    }

}


// Функция инструмента: преобразовать массив символов char в двоичный
bitset<64> DES::char_to_bit(const char s[8]) {
    bitset<64> bits;
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

    return bits;
}
// Функция инструмента: выполнить двоичное обратное преобразование
bitset<64> DES::change(bitset<64> temp) {
    bitset<64> bits;
    bitset<8> n;
    for (int i = 0; i < 64; i = i + 8)
    {
        for (int j = 0; j < 8; j++)
        {
            bits[i + j] = temp[i + 7 - j];
        }
    }
    return bits;
}

bitset<64> DES::DES_encryp(bitset<64> &plain) {
    bitset<64> cipher;
    bitset<64> currentBits;
    bitset<32> left;
    bitset<32> right;
    bitset<32> newLeft;
    // Первоначальная замена IP
    for (int i = 0; i < 64; i++)
        currentBits[i] = plain[IP[i] - 1];//

    for (int i = 0; i < 32; i++)
        left[i] = currentBits[i];
    for (int i = 32; i < 64; i++)
        right[i - 32] = currentBits[i];
    // Вводим 16 раундов изменения
    for (auto round : subkey)
    {
        newLeft = right;
        right = left ^ F(right, round);
        left = newLeft;
    }
    // Слияние
    for (int i = 0; i < 32; i++)
        cipher[i] = right[i];
    for (int i = 32; i < 64; i++)
        cipher[i] = left[i - 32];
    // Обратная инициализация замены
    currentBits = cipher;
    for (int i = 0; i < 64; i++)
        cipher[i] = currentBits[IP_1[i] - 1];

    return cipher;
}

bitset<64> DES::DES_decrypt(bitset<64> & cipher) {
    bitset<64> plain;
    bitset<64> currentBits;
    bitset<32> left;
    bitset<32> right;
    bitset<32> newLeft;
    // Заменить IP
    for (int i = 0; i < 64; i++)
        currentBits[i] = cipher[IP[i] - 1];

    for (int i = 0; i < 32; i++)
        left[i] = currentBits[i];
    for (int i = 32; i < 64; i++)
        right[i - 32] = currentBits[i];
    // Вводим 16 итераций (подключи применяются в обратном порядке)
    for (int round = 0; round < 16; round++)
    {
        newLeft = right;
        right = left ^ F(right, subkey[15 - round]);
        left = newLeft;
    }
    // Слияние
    for (int i = 0; i < 32; i++)
        plain[i] = right[i];
    for (int i = 32; i < 64; i++)
        plain[i] = left[i - 32];
    // Обратная инициализация замены
    currentBits = plain;
    for (int i = 0; i < 64; i++)
        plain[i] = currentBits[IP_1[i] - 1];

    return plain;
}


void DES::show_encryp() {
    bitset<64> plain = char_to_bit(s.c_str());
    key = char_to_bit(k.c_str());
    // Создание 16 подключей
    generateKeys();
    // зашифрованный текст в 1.txt
    bitset<64> cipher = DES_encryp(plain);
    fstream file1;
    file1.open("/Users/a8752355675/CLionProjects/untitled2/1.txt", ios::binary | ios::out);
    file1.write((char*)&cipher, sizeof(cipher));
    file1.close();
}

void DES::show_decrypt() {
    // читаем файл
    bitset<64> temp;
    fstream file2;
    file2.open("/Users/a8752355675/CLionProjects/untitled2/1.txt", ios::binary | ios::in);
    file2.read((char *) &temp, sizeof(temp));
    file2.close();

    // дешифр и в файл 2.txt
    bitset<64> temp_plain = DES_decrypt(temp);
    bitset<64> temp_1 = change(temp_plain);

    file2.open("/Users/a8752355675/CLionProjects/untitled2/2.txt", ios::binary | ios::out);
    file2.write((char *) &temp_1, sizeof(temp_1) );
    file2.close();
}
