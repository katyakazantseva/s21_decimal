#include "s21_decimal.h"

// функция возвращает значение нужного бита в децимал (отсчёт идёт с нуля)
int s21_getbit(s21_decimal value, int bit_poz) {
  int rez = 1;
  int poz = bit_poz / 32;
  rez <<= bit_poz % 32;
  rez = !!(value.bits[poz] & rez);
  return rez;
}
// функция возвращает значение нужного бита в бигдецимал(отсчёт идёт с нуля)
int s21_getexbit(s21_extend_decimal value, int bit_poz) {
  uint64_t rez = 1;
  int poz = bit_poz / 64;
  rez <<= bit_poz % 64;
  rez = !!(value.bits[poz] & rez);
  return rez;
}
// функция устанавливает нужный бит в указанную позицию (отсчёт идёт с нуля)
int s21_setbit(s21_decimal *value, int bit_poz, int bit_value) {
  int rez = 1;
  int poz = bit_poz / 32;
  rez <<= bit_poz % 32;
  if (bit_value == 1) {
    value->bits[poz] |= rez;
  } else {
    value->bits[poz] &= ~rez;
  }
  return rez;
}
// setbit для расширенного децимал
int s21_set_extbit(s21_extend_decimal *value, int bit_poz, int bit_value) {
  uint64_t rez = 1;
  int poz = bit_poz / 64;
  rez <<= bit_poz % 64;
  if (bit_value == 1) {
    value->bits[poz] |= rez;
  } else {
    value->bits[poz] &= ~rez;
  }
  return rez;
}
// функция вовзращает значение scale
int s21_get_scale(s21_decimal value) {
  int scale = 0;
  scale = (value.bits[3] & SC) >> 16;
  return scale;
}
// функция устанавливает scale
int s21_set_scale(s21_decimal *value, int scale) {
  value->bits[3] &= MINUS;
  value->bits[3] |= (scale << 16) & SC;
  return scale;
}
// Функция чтобы узнать знак
int s21_get_sign(s21_decimal value) {
  int sign = 0;
  sign = !!(value.bits[3] & MINUS);
  return sign;
}
// Функция чтобы установить знак
int s21_set_sign(s21_decimal *value, int sign) {
  s21_setbit(value, 127, sign);
  return sign;
}
// функция для проверки переполнения. В случае переполнения вернёт не ноль
int s21_get_overflow(s21_extend_decimal *value) {
  int overflow = 0;
  for (int i = 0; i < 7; i++) {
    value->bits[i] += overflow;
    overflow = value->bits[i] >> 32;
    value->bits[i] &= MAX4BITE;
  }
  return overflow;
}
// Функция для нахождения порядка в числе float
int s21_get_exp(float number) {
  int rez = 0;
  unsigned int fbits = *((unsigned int *)&number);
  fbits >>= 23;
  fbits &= 0xFF;
  rez = fbits - 127;
  return rez;
}
// Функция зануления децимал
void s21_decimal_to_zero(s21_decimal *value) {
  for (int i = 0; i < 4; i++) {
    value->bits[i] = 0;
  }
}
// Проверка decimal на 0
int s21_is_zero(s21_decimal value) {
  int result = 1;
  for (int i = 0; i < 3 && result; i++) {
    if (value.bits[i] != 0) {
      result = 0;
    }
  }
  return result;
}
// Деление децимал на 10
uint64_t s21_pointright(s21_extend_decimal *value) {
  uint64_t remainder = 0;
  for (int i = 6; i > -1; i--) {
    value->bits[i] += remainder << 32;
    remainder = value->bits[i] % 10;
    value->bits[i] /= 10;
  }
  value->scale -= 1;
  s21_get_overflow(value);
  return remainder;
}
// Умножение децимал на 10
int s21_pointleft(s21_extend_decimal *value) {
  int output = 0;
  for (int i = 6; i > -1 && !output; i--) {
    value->bits[i] *= 10;
    if (s21_get_overflow(value)) {
      output = 1;
    }
  }
  if (!output) {
    value->scale++;
  }
  return output;
}
// Битовый cдвиг децимал влево на direction битов
void s21_decimal_shift(s21_extend_decimal *value, int direction) {
  for (int j = 0; j < direction; j++) {
    for (int i = 0; i < 7; i++) {
      value->bits[i] <<= 1;
    }
    s21_get_overflow(value);
  }
}
// Нормализация расширенного децмала
int s21_normalization(s21_extend_decimal *value) {
  int output = 0;
  int reminder = 0;
  int reminder_buff = 0;
  while (value->scale > 0 && (value->bits[6] != 0 || value->bits[5] != 0 ||
                              value->bits[4] != 0 || value->bits[3] != 0)) {
    reminder = s21_pointright(value);
    reminder_buff += reminder;
  }
  while (value->scale > 28) {
    reminder = s21_pointright(value);
    reminder_buff += reminder;
  }
  if (reminder == 5) {
    reminder_buff -= 5;
  }
  if (reminder > 5 || (reminder == 5 && reminder_buff != 0)) {
    value->bits[0] += 1;
  }
  if (reminder == 5 && reminder_buff == 0 && s21_getexbit(*value, 0) == 1) {
    value->bits[0] += 1;
  }
  s21_get_overflow(value);
  if (value->scale == 0 && (value->bits[6] != 0 || value->bits[5] != 0 ||
                            value->bits[4] != 0 || value->bits[3] != 0)) {
    output = 1;
  }
  return output;
}
// Приведение к extend decimal к одному скейлу
int s21_align_scale(s21_extend_decimal *value_1, s21_extend_decimal *value_2,
                    int scale1, int scale2) {
  if (scale1 != scale2) {
    if (scale1 > scale2) {
      int rez1 = scale1 - scale2;
      while (rez1 > 0) {
        s21_pointleft(value_2);
        rez1--;
      }
    } else {
      int rez2 = scale2 - scale1;
      while (rez2 > 0) {
        s21_pointleft(value_1);
        rez2--;
      }
    }
  }
  s21_get_overflow(value_1);
  s21_get_overflow(value_2);
  return 0;
}
// // Функция для вывода двочиного представления float в компьютере
// // Пример
// // sign   exp          mantisa
// // [0][10011100][10111100011110001010001]
// unsigned int s21_float_bits(float number) {
//   unsigned int fbits = *((unsigned int *)&number);
//   printf("float in binary = |\n");
//   for (unsigned int mask = MINUS, i = 0; mask; mask >>= 1, i++) {
//     if (i == 9 || i == 1 || i == 0) putchar('[');
//     printf("%d", !!(fbits & mask));
//     if (i == 31 || i == 8 || i == 0) putchar(']');
//   }
//   putchar('\n');
//   return fbits;
// }
// // Функция для вывода двочиного представления decimal в компьютере
// // Пример
// //  bits[3] bits[2] bits[1] bits[0] порядок в котором записываются числа
// //
// [00000000000000000000000000000000][00000000000000000000000000000000][00000000000000000000000000000000][00110111100011110001010001000000]
// unsigned int s21_decimal_bits(s21_decimal value) {
//   unsigned int fbits = *((unsigned int *)&value);
//   printf("decimal in binary |\n");
//   for (int i = 3; i > -1; i--) {
//     putchar('[');
//     for (unsigned int mask = MINUS; mask; mask >>= 1) {
//       if (i == 3 &&
//           (mask == 0b1000000000000000 || mask == 0b100000000000000000000000))
//           {
//         putchar('|');
//       }
//       printf("%d", !!(value.bits[i] & mask));
//     }
//     putchar(']');
//   }
//   putchar('\n');
//   return fbits;
// }
// // Функция для вывода десятичного представления decimal в компьютере
// void s21_decimal_dect(s21_decimal value) {
//   printf("decimal = [%d][%d][%d][%d]\n", value.bits[3], value.bits[2],
//          value.bits[1], value.bits[0]);
// }
// // Функция для вывода расширенного децимл
// unsigned int s21_extended_decimal_bits(s21_extend_decimal value) {
//   unsigned int fbits = *((unsigned int *)&value);
//   printf("decimal in binary |\n");
//   for (int i = 6; i > -1; i--) {
//     putchar('[');
//     for (uint64_t mask =
//              0b1000000000000000000000000000000000000000000000000000000000000000;
//          mask; mask >>= 1) {
//       printf("%d", !!(value.bits[i] & mask));
//     }
//     putchar(']');
//   }
//   putchar('\n');
//   return fbits;
// }
