#include "s21_decimal.h"

int s21_index(s21_extend_decimal value) {
  int rez = 0;
  for (int i = 447; i >= 0; i--) {
    if (s21_getexbit(value, i) == 1) {
      rez = i;
      break;
    }
  }
  return rez;
}

void s21_additional_code(s21_extend_decimal *value, int poz) {
  for (int i = 0; i <= poz; i++) {
    unsigned long mask = MAX8BITE;
    for (int j = 0; j < 32; j++, mask >>= 1) {
      value->bits[i] |= mask;
    }
    value->bits[i] = ~value->bits[i];
  }
  value->bits[0] += 1;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = 0;
  int scale1 = s21_get_scale(value_1), scale2 = s21_get_scale(value_2);
  int sign1 = s21_get_sign(value_1), sign2 = s21_get_sign(value_2);
  if (sign1 != sign2) {
    s21_set_sign(&value_1, 0);
    s21_set_sign(&value_2, 0);
    if (s21_is_greater(value_2, value_1)) {
      error = s21_sub(value_2, value_1, result);
      s21_set_sign(result, sign2);
    } else {
      error = s21_sub(value_1, value_2, result);
      s21_set_sign(result, sign1);
    }
  } else {
    s21_extend_decimal val_1 = {0};
    s21_extend_decimal val_2 = {0};
    s21_extend_decimal val_rez = {0};
    s21_origin_to_extend(value_1, &val_1);
    s21_origin_to_extend(value_2, &val_2);
    s21_origin_to_extend(*result, &val_rez);
    s21_align_scale(&val_1, &val_2, scale1, scale2);
    for (int i = 0; i < 7; i++) {
      val_rez.bits[i] = val_1.bits[i] + val_2.bits[i];
    }
    s21_get_overflow(&val_rez);
    val_rez.minus = sign1;
    val_rez.scale = fmax(scale1, scale2);
    if (s21_normalization(&val_rez) == 1) {
      if (val_rez.minus == 1) {
        error = 2;
      } else {
        error = 1;
      }
    } else {
      s21_extend_to_origin(val_rez, result);
    }
  }
  return error;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = 0;
  int scale1 = s21_get_scale(value_1), scale2 = s21_get_scale(value_2);
  int sign1 = s21_get_sign(value_1), sign2 = s21_get_sign(value_2);
  s21_decimal buffer = {0};
  s21_set_sign(&value_2, sign1);
  if (sign1 != sign2) {
    error = s21_add(value_1, value_2, result);
  } else {
    s21_set_sign(&value_1, 0);
    s21_set_sign(&value_2, 0);
    s21_set_sign(result, sign1);
    if (s21_is_greater(value_2, value_1)) {
      s21_decimal_to_decimal(value_1, &buffer);
      s21_decimal_to_decimal(value_2, &value_1);
      s21_decimal_to_decimal(buffer, &value_2);
      s21_set_sign(result, !sign2);
    }
    s21_extend_decimal val_1 = {0};
    s21_extend_decimal val_2 = {0};
    s21_extend_decimal val_rez = {0};
    s21_origin_to_extend(value_1, &val_1);
    s21_origin_to_extend(value_2, &val_2);
    s21_align_scale(&val_1, &val_2, scale1, scale2);
    int poz = 0;
    for (int i = 6; i > -1 && !poz; i--) {
      if (val_1.bits[i] != 0) {
        poz = i;
      }
    }
    s21_additional_code(&val_2, poz);
    for (int i = 0; i < 7; i++) {
      val_rez.bits[i] = val_1.bits[i] + val_2.bits[i];
    }
    s21_get_overflow(&val_rez);
    s21_set_extbit(&val_rez, s21_index(val_rez), 0);
    val_rez.scale = fmax(scale1, scale2);
    val_rez.minus = s21_get_sign(*result);
    s21_normalization(&val_rez);
    s21_extend_to_origin(val_rez, result);
  }
  return error;
}

// Умножение
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int rez = 0;
  int sign_1 = s21_get_sign(value_1), sign_2 = s21_get_sign(value_2);
  int sign = (sign_1 == 1 || sign_2 == 1) ? 1 : 0;
  if (sign_1 == sign_2) {
    sign = 0;
  }
  int scale = s21_get_scale(value_1) + s21_get_scale(value_2);
  if (!s21_is_zero(value_1) || !s21_is_zero(value_2)) {
    s21_decimal value_buffer = {0};
    s21_decimal_to_decimal(value_2, &value_buffer);
    s21_extend_decimal value_1_ext = {0};
    s21_extend_decimal value_2_ext = {0};
    s21_extend_decimal value_result_ext = {0};
    s21_extend_decimal value_buffer_ext = {0};
    s21_origin_to_extend(value_1, &value_1_ext);
    s21_origin_to_extend(value_2, &value_2_ext);
    s21_origin_to_extend(value_buffer, &value_buffer_ext);
    for (int i = 0; i < 223; i++) {
      int shift = 1;
      shift <<= i % 32;
      int poz = i / 32;
      if (!!(value_1_ext.bits[poz] & shift)) {
        s21_extend_to_extend(value_2_ext, &value_buffer_ext);
        s21_decimal_shift(&value_buffer_ext, i);
        for (int i = 0; i < 7; i++) {
          value_result_ext.bits[i] += value_buffer_ext.bits[i];
        }
        s21_get_overflow(&value_result_ext);
      }
    }
    value_result_ext.minus = sign;
    value_result_ext.scale = scale;
    if (s21_normalization(&value_result_ext) == 1) {
      if (value_result_ext.minus == 1) {
        rez = 2;
      } else {
        rez = 1;
      }
    } else {
      s21_extend_to_origin(value_result_ext, result);
    }
  }
  return rez;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int rez = 0;
  int sign_1 = s21_get_sign(value_1), sign_2 = s21_get_sign(value_2);
  int sign = (sign_1 == 1 || sign_2 == 1) ? 1 : 0;
  if (sign_1 == sign_2) {
    sign = 0;
  }
  int scale = abs(s21_get_scale(value_1) - s21_get_scale(value_2));
  s21_set_scale(&value_1, 0);
  s21_set_scale(&value_2, 0);
  s21_set_sign(&value_1, 0);
  s21_set_sign(&value_2, 0);
  if (!s21_is_zero(value_2)) {
    s21_decimal val_buff = {0};
    s21_decimal val_buff_1 = {0};
    s21_decimal sub_rez = {0};
    s21_decimal reminder = {0};
    reminder.bits[0] = 1;
    s21_decimal one = {0};
    s21_decimal ten = {0};
    s21_decimal mantiss = {0};
    s21_decimal_to_decimal(value_1, &val_buff);
    int exit = 0;
    one.bits[0] = 1;
    ten.bits[0] = 10;
    while (!s21_is_zero(reminder) && scale < 28 && !exit) {
      s21_decimal_to_zero(&reminder);
      s21_decimal_to_zero(&sub_rez);
      s21_decimal_to_decimal(val_buff, &val_buff_1);
      while (s21_is_greater_or_equal(val_buff, value_2)) {
        s21_sub(val_buff, value_2, &val_buff);
        s21_add(one, sub_rez, &sub_rez);
      }
      exit = s21_add(sub_rez, mantiss, &mantiss);
      if (!exit) exit = s21_mul(value_2, sub_rez, &sub_rez);
      s21_sub(val_buff_1, sub_rez, &reminder);
      if (!s21_is_zero(reminder) && !exit) {
        s21_decimal_to_decimal(reminder, &val_buff);
        exit = s21_mul(mantiss, ten, &mantiss);
        if (!exit) {
          exit = s21_mul(val_buff, ten, &val_buff);
          scale++;
        }
      }
    }
    s21_decimal_to_decimal(mantiss, result);
    s21_set_scale(result, scale);
    s21_set_sign(result, sign);
  } else {
    rez = 3;
  }
  return rez;
}