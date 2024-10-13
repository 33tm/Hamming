#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

const int G[7][4] = {
    { 1, 1, 0, 1 },
    { 1, 0, 1, 1 },
    { 1, 0, 0, 0 },
    { 0, 1, 1, 1 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
};

uint8_t *encodeBlock(uint8_t block[4]) {
    uint8_t *encoded = malloc(8);

    for (int i = 0; i < 7; i++) {
        int dp = 0;
        for (int j = 0; j < 4; j++)
            dp += G[i][j] * block[j];
        encoded[i] = dp % 2;
    }

    encoded[7] = 0;

    return encoded;
}

void encode(char *input_file, char *output_file) {
    FILE *input = fopen(input_file, "rb");

    fseek(input, 0L, SEEK_END);
    long size = ftell(input);
    fseek(input, 0L, SEEK_SET);

    uint8_t *buffer = malloc(size);
    fread(buffer, size, 1, input);
    fclose(input);

    uint8_t *blocks = malloc(size * 8);
    for (long i = 0; i < size; i++)
        for (int j = 0; j < 8; j++)
            blocks[i * 8 + j] = (buffer[i] >> (7 - j)) & 1;
    free(buffer);

    FILE *output = fopen(output_file, "wb");
    for (size_t i = 0; i < size * 8; i += 4) {
        uint8_t *block = encodeBlock(blocks + i);

        uint8_t byte = 0;
        for (int j = 0; j < 7; j++)
            byte |= (block[j] << (6 - j));

        fwrite(&byte, 1, 1, output);
        free(block);
    }

    fclose(output);
    free(blocks);
}

const int H[3][7] = {
    { 1, 0, 1, 0, 1, 0, 1 },
    { 0, 1, 1, 0, 0, 1, 1 },
    { 0, 0, 0, 1, 1, 1, 1 }
};

const int D[4][7] = {
    { 0, 0, 1, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 1 },
};

uint8_t *decodeBlock(uint8_t block[7]) {
    uint8_t *error = malloc(3);

    for (int i = 0; i < 3; i++) {
        int dp = 0;
        for (int j = 0; j < 7; j++)
            dp += H[i][j] * block[j];
        error[i] = dp % 2;
    }

    int index = error[0] * 1 + error[1] * 2 + error[2] * 4;

    if (index != 0)
        block[index - 1] ^= 1;

    free(error);

    uint8_t *decoded = malloc(4);

    for (int i = 0; i < 4; i++) {
        int dp = 0;
        for (int j = 0; j < 7; j++)
            dp += D[i][j] * block[j];
        decoded[i] = dp % 2;
    }

    return decoded;
}

void decode(char *input_file, char *output_file) {
    FILE *input = fopen(input_file, "rb");

    fseek(input, 0L, SEEK_END);
    long size = ftell(input);
    fseek(input, 0L, SEEK_SET);

    uint8_t *buffer = malloc(size);
    fread(buffer, size, 1, input);
    fclose(input);

    uint8_t *blocks = malloc(size * 7);
    for (long i = 0; i < size; i++)
        for (int j = 0; j < 7; j++)
            blocks[i * 7 + j] = (buffer[i] >> (6 - j)) & 1;
    free(buffer);

    FILE *output = fopen(output_file, "wb");
    for (size_t i = 0; i < size * 7; i += 14) {
        uint8_t *block1 = decodeBlock(blocks + i);
        uint8_t *block2 = decodeBlock(blocks + i + 7);

        uint8_t byte = 0;
        for (int j = 0; j < 4; j++) {
            byte |= (block1[j] << (7 - j));
            byte |= (block2[j] << (3 - j));
        }

        free(block1);
        free(block2);

        fwrite(&byte, 1, 1, output);
    }

    fclose(output);
    free(blocks);
}

void corrupt(char *input_file, char *output_file) {
    FILE *input = fopen(input_file, "rb");

    fseek(input, 0L, SEEK_END);
    long size = ftell(input);
    fseek(input, 0L, SEEK_SET);

    uint8_t *buffer = malloc(size);
    fread(buffer, size, 1, input);
    fclose(input);

    for (int i = 0; i < size; i++)
        buffer[i] ^= (1 << rand() % 8);

    FILE *output = fopen(output_file, "wb");
    fwrite(buffer, size, 1, output);
    fclose(output);
    free(buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("%s <encode/decode> <input> <output>", argv[0]);
        return 1;
    }

    switch (argv[1][0]) {
        case 'e':
            encode(argv[2], argv[3]);
            break;
        case 'd':
            decode(argv[2], argv[3]);
            break;
        case 'c':
            corrupt(argv[2], argv[3]);
            break;
    }

    return 0;
}
