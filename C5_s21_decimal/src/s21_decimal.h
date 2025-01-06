#ifndef SPRINTF_DECIMAL_H
#define SPRINTF_DECIMAL_H

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MINUS 0x80000000     // битовая маска для минуса
#define SC 0x00ff0000        // битовая маска для scale
#define MAX4BITE 0xffffffff  // максимальный размер 4 байт
#define FLOATMANTIS 0x400000
#define MAX8BITE 0x8000000000000000

// стандартный децимал
typedef struct {
  int bits[4];
} s21_decimal;

// расширенный децимал, пока не знаю зачем нужен :3 //upd: нужен для крайних
// значений
typedef struct {
  uint64_t bits[7];
  uint16_t scale;
  uint16_t minus;
} s21_extend_decimal;

// Битовые операции с decimal
// --------------------------------------------------------------------------------
// функция возвращает значение нужного бита в децимал (отсчёт идёт с нуля)
int s21_getbit(s21_decimal value, int bit_poz);
// функция возвращает значение нужного бита в бигдецимал(отсчёт идёт с нуля)
int s21_getexbit(s21_extend_decimal value, int bit_poz);
// функция устанавливает нужный бит в указанную позицию (отсчёт идёт с нуля)
int s21_setbit(s21_decimal *value, int bit_poz, int bit_value);
// setbit для расширенного децимал
int s21_set_extbit(s21_extend_decimal *value, int bit_poz, int bit_value);
// setbit для расширенного децимал
int s21_set_extbit(s21_extend_decimal *value, int bit_poz, int bit_value);
// функция вовзращает значение scale
int s21_get_scale(s21_decimal value);
// функция устанавливает scale
int s21_set_scale(s21_decimal *value, int scale);
// Функция чтобы узнать знак
int s21_get_sign(s21_decimal value);
// Функция чтобы установить знак
int s21_set_sign(s21_decimal *value, int sign);
// Битовый cдвиг децимал влево на direction битов
void s21_decimal_shift(s21_extend_decimal *value, int direction);
// --------------------------------------------------------------------------------

// Вспомогательные функции
// --------------------------------------------------------------------------------
// функция для проверки переполнения. В случае переполнения вернёт не ноль
int s21_get_overflow(s21_extend_decimal *value);
// Функция для нахождения порядка в числе float
int s21_get_exp(float number);
// Функция зануления децимал
void s21_decimal_to_zero(s21_decimal *value);
// Проверка decimal на 0
int s21_is_zero(s21_decimal value);
// Деление децимал на 10
uint64_t s21_pointright(s21_extend_decimal *value);
// Умножение децимал на 10
int s21_pointleft(s21_extend_decimal *value);
// Нормализация расширенного децмала
int s21_normalization(s21_extend_decimal *value);
// Приведение к extend decimal к одному скейлу
int s21_align_scale(s21_extend_decimal *value_1, s21_extend_decimal *value_2,
                    int scale1, int scale2);
// Функция для вывода двочиного представления float в компьютере
unsigned int s21_float_bits(float number);
// Функция для вывода двочиного представления decimal в компьютере
unsigned int s21_decimal_bits(s21_decimal value);
// Функция для вывода десятичного представления decimal в компьютере
void s21_decimal_dect(s21_decimal value);
// Функция для вывода расширенного децимл
unsigned int s21_extended_decimal_bits(s21_extend_decimal value);
void s21_float_str(char *float_str, int *count, int *reminder, int *scale);
void s21_str_round(char *float_str, int *scale);
void s21_float_str_round(char *float_str, int *scale);
// --------------------------------------------------------------------------------

// Арифмитические операции
//  --------------------------------------------------------------------------------
// Сложение
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
// Вычитание
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
// Умножение
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
// Деление
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
// --------------------------------------------------------------------------------

// Конвертеры
// --------------------------------------------------------------------------------
//  конвертация децимал в расширенный дицимал
void s21_origin_to_extend(s21_decimal value_1, s21_extend_decimal *value_2);
// конвертация расширенный децимал в дицимал
void s21_extend_to_origin(s21_extend_decimal value_1, s21_decimal *value_2);
// конвертация одного децимал в другой
void s21_decimal_to_decimal(s21_decimal src, s21_decimal *dst);
// конвертация расширенных децмалов
void s21_extend_to_extend(s21_extend_decimal src, s21_extend_decimal *dst);
// преобразование int в decimal
int s21_from_int_to_decimal(int src, s21_decimal *dst);
// преобразование float в decimal (при ошибке возвращает 10)
int s21_from_float_to_decimal(float src, s21_decimal *dst);
// преобразование decimal в int
int s21_from_decimal_to_int(s21_decimal src, int *dst);
// преобразование decimal в float
int s21_from_decimal_to_float(s21_decimal src, float *dst);
// --------------------------------------------------------------------------------

// Другие функции
// --------------------------------------------------------------------------------
// Округляет указанное Decimal число до ближайшего целого числа в сторону
// отрицательной бесконечности.
int s21_floor(s21_decimal value, s21_decimal *result);
// Округляет Decimal до ближайшего целого числа.
int s21_round(s21_decimal value, s21_decimal *result);
// Возвращает целые цифры указанного Decimal числа; любые дробные цифры
// отбрасываются, включая конечные нули.
int s21_truncate(s21_decimal value, s21_decimal *result);
// Возвращает результат умножения указанного Decimal на -1.
int s21_negate(s21_decimal value, s21_decimal *result);
// --------------------------------------------------------------------------------
// Операторы сравнения
// Меньше
// <
int s21_is_less(s21_decimal value_1, s21_decimal value_2);
// Меньше или равно
// <=
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
// Больше
// >
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
// Больше или равно
// >=
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
// Равно
// ==
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);
// Не равно
// !=
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
// --------------------------------------------------------------------------------

#endif