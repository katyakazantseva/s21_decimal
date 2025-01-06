#include "s21_decimal.h"

// конвертация децимал в расширенный дицимал
void s21_origin_to_extend(s21_decimal value_1, s21_extend_decimal *value_2) {
  for (int i = 0; i < 3; i++) {
    value_2->bits[i] = value_1.bits[i] & MAX4BITE;
  }
  value_2->scale = (value_1.bits[3] & SC) >> 16;
  if (value_1.bits[3] & MINUS) {
    value_2->minus = 1;
  }
}
// конвертация расширенный децимал в дицимал
void s21_extend_to_origin(s21_extend_decimal value_1, s21_decimal *value_2) {
  for (int i = 0; i < 3; i++) {
    value_2->bits[i] = value_1.bits[i] & MAX4BITE;
  }
  s21_set_scale(value_2, value_1.scale);
  s21_set_sign(value_2, value_1.minus);
}
// конвертация одного децимал в другой
void s21_decimal_to_decimal(s21_decimal src, s21_decimal *dst) {
  for (int i = 0; i < 4; i++) {
    dst->bits[i] = src.bits[i];
  }
}
// конвертация расширенных децмалов
void s21_extend_to_extend(s21_extend_decimal src, s21_extend_decimal *dst) {
  for (int i = 0; i < 7; i++) {
    dst->bits[i] = src.bits[i];
  }
}

// преобразование int в decimal
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int rez = 0;
  if (abs(src) == INFINITY || src == !src) {
    rez = 1;
  } else {
    int flag = 0;
    s21_decimal_to_zero(dst);
    if (src < 0) {
      if (src == INT_MIN) {
        src++;
        flag = 1;
      }
      src = -src;
      dst->bits[3] |= MINUS;
    }
    if (flag) {
      s21_setbit(dst, 31, 1);
    } else {
      dst->bits[0] = src;
    }
  }
  return rez;
}
// преобразование float в decimal (при ошибке возвращает 1)
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  s21_decimal_to_zero(dst);
  int rez = 0;
  if (src != 0 && src == src && fabs(src) != INFINITY && fabs(src) > 1e-28) {
    if (src < 0) {
      src = -src;
      s21_set_sign(dst, 1);
    }
    int binary_exp = 0;
    int binary_exp_except = 0;
    int scale = 0;
    unsigned int fbits = 0;
    char float_str[4096] = {0};
    int count = 0;
    int reminder = 0;
    s21_extend_decimal dst_ext = {0};
    binary_exp_except = s21_get_exp(src);
    sprintf(float_str, "%.30f", src);
    s21_float_str(float_str, &count, &reminder, &scale);
    sscanf(float_str, "%f", &src);
    if (reminder > 4) src++;
    sprintf(float_str, "%.f", src);
    s21_float_str_round(float_str, &scale);
    sscanf(float_str, "%f", &src);
    binary_exp = s21_get_exp(src);
    fbits = *((unsigned int *)&src);
    if (binary_exp_except > 95 || binary_exp_except < -94) {
      s21_decimal_to_zero(dst);
      rez = 1;  // 1 - ошибка
    } else {
      s21_setbit(dst, binary_exp, 1);
      binary_exp--;
      for (unsigned int mask = FLOATMANTIS; mask && binary_exp > -1;
           mask >>= 1, binary_exp--) {
        s21_setbit(dst, binary_exp, !!(fbits & mask));
      }
      s21_set_scale(dst, scale);
      s21_origin_to_extend(*dst, &dst_ext);
      for (int i = 0; i < count; i++) {
        s21_pointleft(&dst_ext);
        dst_ext.scale--;
      }
      s21_extend_to_origin(dst_ext, dst);
    }
  } else if (src != src || fabs(src) == INFINITY || fabs(src) < 1e-28) {
    if (src != 0) rez = 1;
  }
  return rez;
}
void s21_float_str(char *float_str, int *count, int *reminder, int *scale) {
  int dot_poz = strchr(float_str, '.') - float_str;
  int fract_flag = 0;
  if (dot_poz < 7) {
    if (float_str[0] != '0')
      *scale = 7 - dot_poz;
    else
      fract_flag = 1;
  }
  *count = dot_poz - 7;
  for (int i = dot_poz; float_str[i] != '\0'; i++) {
    float_str[i] = float_str[i + 1];
  }
  for (int i = 0, count = 0; float_str[i] == '0'; count++) {
    for (int j = 0; float_str[j] != '\0'; j++) {
      float_str[j] = float_str[j + 1];
    }
    if (count > 0) *scale += 1;
  }
  *reminder = float_str[7] - '0';
  float_str[7] = '\0';
  if (fract_flag) *scale += strlen(float_str);
  if (*reminder < 5) {
    s21_str_round(float_str, scale);
  }
}
void s21_float_str_round(char *float_str, int *scale) {
  float_str[7] = '\0';
  s21_str_round(float_str, scale);
}
void s21_str_round(char *float_str, int *scale) {
  for (int i = 6; *scale && float_str[i] == '0' && i > -1; i--) {
    float_str[i] = '\0';
    *scale -= 1;
  }
}
// преобразование decimal в int
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int rez = 0;
  s21_decimal buffer = {0};
  *dst = 0;
  s21_truncate(src, &buffer);
  s21_decimal_to_decimal(buffer, &src);
  if (src.bits[1] == 0 && src.bits[2] == 0) {
    if (s21_getbit(src, 31) == 0) {
      *dst = src.bits[0];
      if (s21_get_sign(src) == 1) {
        *dst = -*dst;
      }
    } else {
      if (((src.bits[0] | MINUS) == MINUS) && ((s21_get_sign(src) == 1))) {
        *dst = INT_MIN;
      } else {
        rez = 1;
      }
    }
  } else {
    rez = 1;
  }
  return rez;
}
// преобразование decimal в float
int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int rez = 0;
  int scale = s21_get_scale(src);
  double buffer = 0;
  *dst = 0;
  for (int i = 0; i < 96; i++) {
    if (s21_getbit(src, i) == 1) {
      buffer += powl(2, i);
    }
  }
  while (scale) {
    buffer /= 10.0l;
    scale--;
  }
  if (s21_get_sign(src) == 1) {
    buffer = -buffer;
  }
  *dst = buffer;
  return rez;
}