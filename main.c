#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int32_t is_ascii(char str[]) {
    for (int i = 0; i < strlen(str); i += 1) {
        unsigned u = str[i];
        if (u >= 128) {
            return 0;
        }
    }
    return 1;
}

int32_t capitalize_ascii(char str[]) {
    int counter = 0;
    for (int i = 0; i < strlen(str); i += 1) {
        if (str[i] >= 97 && str[i] <= 122) {
            str[i] -= 32;
            counter += 1;
        }
    }
    return counter;
}

int32_t width_from_start_byte(char start_byte) {
    if ((start_byte & 0b11110000) == 0b11110000) {
        return 4;
    } else if ((start_byte & 0b11100000) == 0b11100000) {
        return 3;
    } else if ((start_byte & 0b11000000) == 0b11000000) {
        return 2;
    } else if ((start_byte & 0b10000000) == 0b10000000) {
        return -1;
    } else {
        return 1;
    }
}

int32_t utf8_strlen(char str[]) {
    int len = 0;
    for (int i = 0; i < strlen(str); i += 1) {
        int width = width_from_start_byte(str[i]);
        if (width != -1) {
            len += 1;
        }
    }
    return len;
}

int32_t codepoint_index_to_byte_index(char str[], int32_t cpi) {
    int byte_index = 0;
    for (int i = 0; i < cpi; i += 1) {
        int width = width_from_start_byte(str[i]);
        if (width != -1) {
            byte_index += width;
        } else {
            cpi += 1;
        }
    }
    return byte_index;
}

void utf8_substring(char str[], int32_t cpi_start, int32_t cpi_end, char result[]) {
    if (cpi_start > cpi_end || cpi_start < 0 || cpi_end < 0) {
        return;
    }
    if (cpi_end > utf8_strlen(str)) {
        cpi_end = utf8_strlen(str);
    }
    int start = codepoint_index_to_byte_index(str, cpi_start);
    int end = codepoint_index_to_byte_index(str, cpi_end);
    int len = end - start;
    int index = 0;
    for (int i = 0; i < len; i += 1) { // i: start byte location
        int width = width_from_start_byte(str[start + i]);
        if (width != -1) {
            for (int j = 0; j < width; j += 1) { // j: iterate through all the bytes of the current character
                result[index] = str[start + i + j];
                index += 1;
            }
        }
    }
    result[index] = 0;
}

int32_t code_point2(char c1, char c2) {
    return (c1 & 0b00011111) * 64 + (c2 & 0b00111111);
}

int32_t code_point3(char c1, char c2, char c3) {
    return (c1 & 0b00001111) * 4096 + (c2 & 0b00111111) * 64 + (c3 & 0b00111111);
}

int32_t code_point4(char c1, char c2, char c3, char c4) {
    return (c1 & 0b00000111) * 4096 * 64 + (c2 & 0b00111111) * 4096 + (c3 & 0b00111111) * 64 + (c4 & 0b00111111);
}

int32_t codepoint_at(char str[], int32_t cpi) {
    int index = codepoint_index_to_byte_index(str, cpi);
    int width = width_from_start_byte(str[index]);
    if (width == 1) {
        return str[index];
    }
    if (width == 2) {
        return code_point2(str[index], str[index + 1]);
    }
    if (width == 3) {
        return code_point3(str[index], str[index + 1], str[index + 2]);
    }
    if (width == 4) {
        return code_point4(str[index], str[index + 1], str[index + 2], str[index + 3]);
    }
    return -1;
}

char is_animal_emoji_at(char str[], int32_t cpi) {
    char boundary[] = "🐀🐿🦀🦮";
    int d1 = codepoint_at(boundary, 0);
    int d2 = codepoint_at(boundary, 1);
    int d3 = codepoint_at(boundary, 2);
    int d4 = codepoint_at(boundary, 3);
    int d = codepoint_at(str, cpi);
    if ((d1 <= d && d <= d2) || (d3 <= d && d <= d4)) {
        return 1;
    } else {
        return 0;
    }
}

void next_utf8_char(char str[], int32_t cpi, char result[]) {
    int index = codepoint_index_to_byte_index(str, cpi);
    int width = width_from_start_byte(str[index]);
    if (width == 1) {
        result[0] = str[index] + 1;
        result[1] = '\0';
    }
    if (width == 2) {
        result[0] = str[index];
        result[1] = str[index + 1] + 1;
        if ((result[1] & 0x3f) == 0) {
            result[0] += 1;
        }
        result[2] = '\0';
    }
    if (width == 3) {
        result[0] = str[index];
        result[1] = str[index + 1];
        result[2] = str[index + 2] + 1;
        if ((result[2] & 0x3f) == 0) {
            result[1] += 1;
        }
        if ((result[1] & 0x3f) == 0) {
            result[0] += 1;
        }
        result[3] = '\0';
    }
    if (width == 4) {
        result[0] = str[index];
        result[1] = str[index + 1];
        result[2] = str[index + 2];
        result[3] = str[index + 3] + 1;
        if ((result[3] & 0x3f) == 0) {
            result[2] += 1;
        }
        if ((result[2] & 0x3f) == 0) {
            result[1] += 1;
        }
        if ((result[1] & 0x3f) == 0) {
            result[0] += 1;
        }
        result[4] = '\0';
    }
}

int main() {
    char input[256];
    printf("Enter a UTF-8 encoded string: ");
   
    fgets(input, 256, stdin);
    printf("\n");
    if (is_ascii(input) == 0) {
        printf("Valid ASCII: false\n");
    } else {
        printf("Valid ASCII: true\n");
    }
    
    char capitalized[256];
    strcpy(capitalized, input);
    int32_t ret = 0;
    ret = capitalize_ascii(capitalized);
    printf("Uppercased ASCII: %s", capitalized);
    printf("\n");
    printf("Length in bytes: %ld\n", strlen(input));
   
    printf("Number of code points: %d\n", utf8_strlen(input));

    printf("Bytes per code point: ");
    for (int i = 0; i < strlen(input); i += 1) {
        int width = width_from_start_byte(input[i]);
        if (width != -1) {
            printf("%d ", width);
        }
    }
    printf("\n");

    char substring[256];
    utf8_substring(input, 0, 6, substring);
    printf("Substring of the first 6 code points: \"%s\"\n", substring);

    printf("Code points as decimal numbers: ");
    for (int i = 0; i < utf8_strlen(input); i += 1) {
        printf("%d ", codepoint_at(input, i));
    }
    printf("\n");

    printf("Animal emojis: ");
    for (int i = 0; i < utf8_strlen(input); i += 1) {
        if (is_animal_emoji_at(input, i)) {
            int byte_index = codepoint_index_to_byte_index(input, i);
            int width = width_from_start_byte(input[byte_index]);
            for (int j = 0; j < width; j += 1) {
                printf("%c", input[byte_index + j]);
            }
            printf(" ");
        }
    }
    printf("\n");
    
    char result[256];
    next_utf8_char(input, 3, result);
    printf("Next Character of Codepoint at Index 3: %s\n", result);

    return 0;
}