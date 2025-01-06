#include "s21_decimal.h"

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  int rez = 0;
  int first = s21_getbit(value_1, 127);
  int second = s21_getbit(value_2, 127);
  s21_extend_decimal value_3 = {0};
  s21_extend_decimal value_4 = {0};
  s21_origin_to_extend(value_1, &value_3);
  s21_origin_to_extend(value_2, &value_4);
  int scale1 = s21_get_scale(value_1);
  int scale2 = s21_get_scale(value_2);
  s21_align_scale(&value_3, &value_4, scale1, scale2);
  if ((first == 1 && second == 1) || (first == 0 && second == 0)) {
    for (int i = 6; i > -1; i--) {
      if (first == 0 && second == 0) {
        if (value_3.bits[i] < value_4.bits[i] &&
            (value_4.bits[i] != 0 || value_3.bits[i] != 0)) {
          rez = 1;
          i = -1;
        } else if (value_4.bits[i] != 0 && value_3.bits[i] != 0)
          i = -1;
      } else {
        if (value_3.bits[i] > value_4.bits[i] &&
            (value_4.bits[i] != 0 || value_3.bits[i] != 0)) {
          rez = 1;
          i = -1;
        } else if (value_4.bits[i] != 0 && value_3.bits[i] != 0)
          i = -1;
      }
    }
  } else if (first == 0 && second == 1)
    rez = 0;
  else if (first == 1 && second == 0)
    rez = 1;
  return rez;
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int rez = 0;
  if (s21_is_less(value_1, value_2) || s21_is_equal(value_1, value_2)) rez = 1;
  return rez;
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  int rez = 0;
  if (!s21_is_less(value_1, value_2) && !s21_is_equal(value_1, value_2))
    rez = 1;
  return rez;
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int rez = 0;
  if (s21_is_greater(value_1, value_2) || s21_is_equal(value_1, value_2))
    rez = 1;
  return rez;
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int rez = 1;
  int first = s21_getbit(value_1, 127);
  int second = s21_getbit(value_2, 127);
  s21_extend_decimal value_3 = {0};
  s21_extend_decimal value_4 = {0};
  s21_origin_to_extend(value_1, &value_3);
  s21_origin_to_extend(value_2, &value_4);
  int scale1 = s21_get_scale(value_1);
  int scale2 = s21_get_scale(value_2);
  s21_align_scale(&value_3, &value_4, scale1, scale2);
  if ((first == 1 && second == 1) || (first == 0 && second == 0)) {
    for (int i = 6; i > -1; i--) {
      if (value_3.bits[i] != value_4.bits[i]) {
        rez = 0;
        i = -1;
      }
    }
  } else if (first == 0 && second == 1)
    rez = 0;
  else if (first == 1 && second == 0)
    rez = 0;
  return rez;
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  int rez = 0;
  if (!s21_is_equal(value_1, value_2)) rez = 1;
  return rez;
}