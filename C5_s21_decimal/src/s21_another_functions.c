#include "s21_decimal.h"
// Округляет указанное Decimal число до ближайшего целого числа в сторону
// отрицательной бесконечности.
int s21_floor(s21_decimal value, s21_decimal *result) {
  int rez = 0;
  int scale = s21_get_scale(value);
  s21_decimal buffer = {0};
  s21_truncate(value, &buffer);
  s21_decimal_to_decimal(buffer, result);
  if (s21_get_sign(value) == 1 && scale) {  // нужно увеличить  decimal на 1
    s21_decimal one_decimal = {0};
    one_decimal.bits[0] = 1;
    s21_set_sign(&buffer, 0);
    s21_set_sign(result, 0);
    s21_add(buffer, one_decimal, result);
    s21_set_sign(result, 1);
  }
  return rez;
}
// Округляет Decimal до ближайшего целого числа.
int s21_round(s21_decimal value, s21_decimal *result) {
  int rez = 0;
  int reminder = 0;
  int reminder_buff = 0;
  s21_extend_decimal value_ext = {0};
  s21_origin_to_extend(value, &value_ext);
  while (value_ext.scale) {
    reminder = s21_pointright(&value_ext);
    reminder_buff += reminder;
  }
  if (reminder == 5) {
    reminder_buff -= reminder;
  }
  if (reminder > 5 || (reminder == 5 && reminder_buff != 0)) {
    value_ext.bits[0] += 1;
  }
  if (reminder == 5 && reminder_buff == 0 && s21_getexbit(value_ext, 0) == 1) {
    value_ext.bits[0] += 1;
  }
  s21_get_overflow(&value_ext);
  s21_extend_to_origin(value_ext, result);
  return rez;
}
// Возвращает целые цифры указанного Decimal числа; любые дробные цифры
// отбрасываются, включая конечные нули.
int s21_truncate(s21_decimal value, s21_decimal *result) {
  int rez = 0;
  s21_extend_decimal value_ext = {0};
  s21_origin_to_extend(value, &value_ext);
  while (value_ext.scale) {
    s21_pointright(&value_ext);
  }
  s21_extend_to_origin(value_ext, result);
  return rez;
}
// Возвращает результат умножения указанного Decimal на -1.
int s21_negate(s21_decimal value, s21_decimal *result) {
  int rez = 0;
  s21_decimal_to_decimal(value, result);
  result->bits[3] ^= MINUS;
  return rez;
}