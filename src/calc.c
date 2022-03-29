/** @file
  Implementacja kalkulatora wielomianów rzadkich wielu zmiennych.
  @authors Jagoda Bracha <jb429153@students.mimuw.edu.pl>
  @date 2021
*/

#define _XOPEN_SOURCE 700 ///< aby znalezc potrzebne funkcje

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "poly.h"

/**
* Sprawdza, czy alokacja sie powiodla.
*/
#define CHECK_PTR(p)    	\
	do {			    	\
		if (p == NULL) {	\
			exit(1);		\
		}					\
	} while (0)

/**
* Maksymalne i minimalne wartosci argumentów.
*/
#define max_coeff LLONG_MAX ///< 9223372036854775807
#define max_coeff_string "9223372036854775807\0" ///< "9223372036854775807"
#define min_coeff LLONG_MIN ///< -9223372036854775808
#define min_coeff_string "-9223372036854775808\0" ///< "-9223372036854775808"
#define max_exp INT_MAX ///< 2147483647
#define max_exp_string "2147483647\0" ///< "2147483647"
#define min_exp 0 ///< 0
#define min_exp_string "0\0" ///< 0
#define max_exp_arg ULLONG_MAX ///< 18446744073709551615
#define max_exp_arg_string "18446744073709551615\0" ///< "18446744073709551615"
#define min_exp_arg 0 ///< 0
#define min_exp_arg_string "0\0" ///< "0"
/**
Rozmiar potrzebny do alokacji stringa, do którego parsuje liczbe.
*/
#define len_of_coeff 23

/**
Typ przechowujacy stos wielomianów.
*/
typedef struct
{
    Poly* arr; ///< tablica przechowywanych wielomianów
    size_t used; ///< liczba przechowywanych wielomianów
    size_t size; ///< zaalokowany rozmiar tablicy
}   Stack;

/**
* Tworzy pusty stos.
* @param[out] s: stos
*/
void StackInit(Stack* s)
{
    s->used = 0;
    s->size = 1;
    s->arr = malloc(sizeof(Poly));
    CHECK_PTR(s->arr);
}

/**
* Sprawdza, czy stos ma mniej elementów niz to konieczne.
* Jesli tak, wypisuje komunikat o bledzie.
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki konieczny do wypisania bledu
* @param[in] a: potrzebna liczba elementów na stosie
* @return bool
*/
bool StackIsUnderflow(Stack* s, unsigned int num_of_lines, unsigned int a)
{
	if (s->used < a)
    {
        fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", num_of_lines);
        return true;
    }
    return false;
}

/**
* Sprawdza, czy stos jest pusty
* @param[in] s: stos
* @return: bool
*/
bool StackIsEmpty(Stack* s)
{
    return (s->used == 0);
}

/**
* Wklada wielomian na wierzcholek stosu.
* @param[in] s: stos
* @param[in] p: wielomian
*/
void StackPush(Stack* s, Poly *p)
{
    if (s->used == s->size)
    {
        s->size = 2 * s->size;
        s->arr = realloc(s->arr, s->size*sizeof(Poly));
        CHECK_PTR(s->arr);
    }
    s->arr[s->used] = *p;
    s->used++;
}

/**
* Odczytuje wielomian z wierzchoku stosu
* @param[in] s: stos
* @return wielomian
*/
Poly StackTop(Stack *s)
{
    return s->arr[s->used-1];
}

/**
* Odczytuje wielomian z wierzchoku i usuwa go ze stosu.
* @param[in] s: stos
* @return wielomian
*/
Poly StackPop(Stack* s)
{
    Poly p = s->arr[s->used - 1];
    s->used--;
    if (s->used < s->size/4 - 1 && s->size / 4 > 0)
    {
        s->size = s->size / 2;
        s->arr = realloc(s->arr, s->size * sizeof(Poly));
        CHECK_PTR(s->arr);
    }
    return p;
}

/**
* Usuwa stos i zwalnia zaalokowana pamiec.
* @param[in] s: stos
*/
void StackDestroy(Stack* s)
{
    for (size_t i = 0; i < s->used; i++)
    {
        PolyDestroy(&(s->arr[i]));
    }
    free(s->arr);
}

/**
* Wklada na wierzcholek stosu wielomian zerowy.
* @param[in] s: stos
*/
void InsertZero(Stack *s)
{
    Poly p = PolyZero();
    StackPush(s, &p);
}

/**
* Sprawdza, czy wielomian z wierzcholka stosu jest wspólczynnikiem.
* Wypisuje 1 jesli jest i 0 jesli nie jest.
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void IsCoeff(Stack *s, unsigned int num_of_lines)
{
    if (!StackIsUnderflow(s, num_of_lines, 1))
    {
        Poly p = StackTop(s);
        bool is = PolyIsCoeff(&p);
        printf("%d\n", is);
    }
}

/**
* Sprawdza, czy wielomian z wierzcholka stosu jest zerowy.
* Wypisuje 1 jesli jest i 0 jesli nie jest.
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void IsZero(Stack *s, unsigned int num_of_lines)
{
    if (!StackIsUnderflow(s, num_of_lines, 1))
    {
        Poly p = StackTop(s);
        bool is = PolyIsZero(&p);
        printf("%d\n", is);
    }
}

/**
* Wstawia na stos kopie wielomianu z wierzcholka.
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void Clone(Stack *s, unsigned int num_of_lines)
{
    if (!StackIsUnderflow(s, num_of_lines, 1))
    {
        Poly p = StackTop(s);
        Poly p_clone = PolyClone(&p);
        StackPush(s, &p_clone);
    }
}

/**
* Dodaje dwa wielomiany z wierzchu stosu, usuwa je,
* wstawia na wierzchołek stosu ich sumę.
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void Add(Stack *s, unsigned int num_of_lines)
{
    if (!StackIsUnderflow(s, num_of_lines, 2))
    {
        Poly p = StackPop(s);
        Poly q = StackPop(s);
        Poly r = PolyAdd(&p, &q);
        PolyDestroy(&p);
        PolyDestroy(&q);
        StackPush(s, &r);
    }
}

/**
* Mnoży dwa wielomiany z wierzchu stosu, usuwa je,
* wstawia na wierzchołek stosu ich iloczyn.
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void Mul(Stack *s, unsigned int num_of_lines)
{
    if (!StackIsUnderflow(s, num_of_lines, 2))
    {
        Poly p = StackPop(s);
        Poly q = StackPop(s);
        Poly r = PolyMul(&p, &q);
        PolyDestroy(&p);
        PolyDestroy(&q);
        StackPush(s, &r);
    }
}

/**
* Neguje wielomian na wierzchołku stosu.
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void Neg(Stack *s, unsigned int num_of_lines)
{
    if (!StackIsUnderflow(s, num_of_lines, 1))
    {
        Poly p = StackPop(s);
        Poly r = PolyNeg(&p);
        PolyDestroy(&p);
        StackPush(s, &r);
    }
}

/**
* Odejmuje od wielomianu z wierzchołka wielomian pod wierzchołkiem, usuwa je,
* wstawia na wierzchołek stosu różnicę.
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void Sub(Stack *s, unsigned int num_of_lines)
{
    if (!StackIsUnderflow(s, num_of_lines, 2))
    {
        Poly p = StackPop(s);
        Poly q = StackPop(s);
        Poly r = PolySub(&p, &q);
        PolyDestroy(&p);
        PolyDestroy(&q);
        StackPush(s, &r);
    }
}

/**
* Sprawdza, czy dwa wielomiany na wierzchu stosu są równe.
* Wypisuje na standardowe wyjście 0 lub 1.
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void IsEq(Stack *s, unsigned int num_of_lines)
{

    if (!StackIsUnderflow(s, num_of_lines, 2))
    {
        Poly p = StackPop(s);
        Poly q = StackTop(s);
        StackPush(s, &p);
        bool is = PolyIsEq(&p, &q);
        printf("%d\n", is);
    }
}

/**
* Wypisuje na standardowe wyjście stopień wielomianu z wierzcholka stosu
* (−1 dla wielomianu tożsamościowo równego zeru).
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void Deg(Stack *s, unsigned int num_of_lines)
{
    if (!StackIsUnderflow(s, num_of_lines, 1))
    {
        Poly p = StackTop(s);
        poly_exp_t res = PolyDeg(&p);
        printf("%d\n", res);
    }
}

/**
* Wypisuje na standardowe wyjście stopień wielomianu,
* ze względu na zmienną o numerze idx
* (−1 dla wielomianu tożsamościowo równego zeru).
* @param[in] s: stos
* @param[in] var_idx: numer zmiennej
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void DegBy(Stack *s, long int var_idx, unsigned int num_of_lines)
{
    if (!StackIsUnderflow(s, num_of_lines, 1))
    {
        Poly p = StackTop(s);
        poly_exp_t res = PolyDegBy(&p, var_idx);
        printf("%d\n", res);
    }
}

/**
* Wylicza wartość wielomianu w punkcie x, usuwa wielomian z wierzchołka,
* wstawia na stos wynik operacji.
* @param[in] s: stos
* @param[in] x: punkt, w którym nalezy obliczyc wartosc
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void At(Stack *s, poly_coeff_t x, unsigned int num_of_lines)
{

    if (!StackIsUnderflow(s, num_of_lines, 1))
    {
        Poly p = StackPop(s);
        Poly res = PolyAt(&p, x);
        PolyDestroy(&p);
        StackPush(s, &res);
    }
}

char* PolyToString(Poly* p);

/**
* Wypisuje na standardowe wyjście wielomian z wierzchołka stosu.
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void Print(Stack *s, unsigned int num_of_lines)
{
    if (!StackIsUnderflow(s, num_of_lines, 1))
    {
        Poly p = StackTop(s);
        char* p_str = PolyToString(&p);
        puts(p_str);
        free(p_str);
    }
}

/**
* Usuwa wielomian z wierzchołka stosu.
* @param[in] s: stos
* @param[in] num_of_lines: numer linijki do wypisania ewentualnego bledu
*/
void Pop(Stack *s, unsigned int num_of_lines)
{
    if (!StackIsUnderflow(s, num_of_lines, 1))
    {
        Poly p = StackPop(s);
        PolyDestroy(&p);
    }
}

/**
* Zwraca zerowy jednomian.
* @return: Mono
*/
Mono MonoZero()
{
    Poly p_zero = PolyZero();
    return MonoFromPoly(&p_zero, 0);
}

/**
* Typ reprezentujacy fragment stringa.
*/
typedef struct
{
    char* str; ///< wskaznik na calego stringa
    size_t beg; ///< numer pozycji bedacej poczatkiem fragmentu
    size_t end; ///< numer pozycji, za która jest koniec fragmentu
    size_t len; ///< dlugosc calego stringa
}   BlockOfString;

/**
* Znajduje ostatni przecinek we fragmencie stringa.
* @param[in] b: fragment stringa
* @param[out] correct: bool, miernik powodzenia calej operacji.
*/
size_t StringFindLastComma(BlockOfString* b, bool* correct)
{
    for (size_t res = b->end-1; res >= b->beg; res--)
    {
        if (b->str[res] == ',')
        {
            return res;
        }
        if (res == 0)
        {
            break;
        }
    }
    *correct = false;
    return 0;
}

/**
* Na podstawie fragmentu stringa, zwraca string zawierajacy tylko ten fragment.
* @param[in] b: fragment stringa
* @return wskaznik na otrzymany string
*/
char* ExtractBlock(BlockOfString* b)
{
    char* to_int = malloc((b->end - b->beg + 1) * sizeof(char));
    CHECK_PTR(to_int);
    memcpy(to_int, &(b->str[b->beg]), (b->end - b->beg));
    to_int[b->end - b->beg] = '\0';
    return to_int;
}

/**
* Konwertuje fragment stringa na liczbe typu wspólczynnik wielomianu.
* @param[in] b: fragment stringa
* @param[out] correct: bool, miernik powodzenia calej operacji.
* @return poly_coeff_t
*/
poly_coeff_t StringToCoeff(BlockOfString* b, bool* correct)
{
    char* to_int = ExtractBlock(b);
    if (to_int[0] == '+' || isspace(to_int[0]))
    {
        *correct = false;
    }
    char* endptr;
    poly_coeff_t res = strtol(to_int, &endptr, 10);
    if (*endptr != '\0')
    {
        *correct = false;
    }
    if (res == 0 && strcmp(to_int, "0\0") != 0)
    {
        *correct = false;
    }
    if (res >= max_coeff && strcmp(to_int, max_coeff_string) != 0)
    {
        *correct = false;
    }
    if (res <= min_coeff && strcmp(to_int, min_coeff_string) != 0)
    {
        *correct = false;
    }
    free(to_int);
    return res;
}

/**
* Konwertuje fragment stringa na liczbe typu wykladnik wielomianu.
* @param[in] b: fragment stringa
* @param[out] correct: bool, miernik powodzenia calej operacji.
* @return poly_exp_t
*/
poly_exp_t StringToExp(BlockOfString* b, bool* correct)
{
    char* to_int = ExtractBlock(b);
    if (to_int[0] == '+' || isspace(to_int[0]) || to_int[0] == '-')
    {
        *correct = false;
    }
    char* endptr;
    long int res = strtoul(to_int, &endptr, 10);
    if (*endptr != '\0')
    {
        *correct = false;
    }
    if (res == 0 && strcmp(to_int, "0\0") != 0)
    {
        *correct = false;
    }
    if (res >= max_exp && strcmp(to_int, max_exp_string))
    {
        *correct = false;
    }
    if (res <= min_exp && strcmp(to_int, min_exp_string))
    {
        *correct = false;
    }
    free(to_int);
    return res;
}

/**
* Konwertuje fragment stringa na liczbe typu argument komendy DEG_BY.
* @param[in] b: fragment stringa
* @param[out] correct: bool, miernik powodzenia calej operacji.
* @return unsigned long long int
*/
unsigned long long int StringToExpArg(BlockOfString* b, bool* correct)
{
    char* to_int = ExtractBlock(b);
    if (to_int[0] == '+' || isspace(to_int[0]) || to_int[0] == '-')
    {
        *correct = false;
    }
    char* endptr;
    unsigned long long int res = strtoull(to_int, &endptr, 10);

    if (*endptr != '\0')
    {
        *correct = false;
    }
    if (res == 0 && strcmp(to_int, "0\0") != 0)
    {
        *correct = false;
    }
    if (res >= max_exp_arg && strcmp(to_int, max_exp_arg_string))
    {
        *correct = false;
    }
    if (res <= min_exp_arg && strcmp(to_int, min_exp_arg_string))
    {
        *correct = false;
    }
    free(to_int);
    return res;
}

Poly StringToPoly(BlockOfString* b, bool* correct);

/**
* Sprawdza warunki konieczne do konwersji fragmentu stringa w jednomian.
* @param[in] b: fragment stringa
* @param[out] correct: bool, miernik powodzenia calej operacji.
* @return bool
*/
bool StringToMonoInitialCheck(BlockOfString* b, bool* correct)
{
    if (!(*correct))
    {
        return false;
    }
    else if (b->end - b->beg <= 4 || b->end > b->len)
    {
        *correct = false;
        return false;
    }
    else if (b->str[b->beg] != '(' || b->str[b->end-1] != ')')
    {
        *correct = false;
        return false;
    }
    return true;
}

/**
* Dzieli fragment stringa bedacy jednomianem na wielomian i wspólczynnik.
* @param[in] b: fragment stringa
* @param[out] only_poly: fragment stringa z wielomianem
* @param[out] only_exp: fragment stringa ze wspólczynnikiem
* @param[in] last_comma: pozycja ostatniego przecinka we fragmencie
*/
void SplitMono(BlockOfString* b, BlockOfString* only_poly, BlockOfString* only_exp, size_t last_comma)
{
    only_exp->beg = last_comma + 1;
    only_exp->end = b->end - 1;
    only_poly->beg = b->beg + 1;
    only_poly->end = last_comma;
}

/**
* Konwertuje fragment stringa do jednomianu.
* @param[in] b: fragment stringa
* @param[out] correct: bool, miernik powodzenia calej operacji.
* @return jednomian
*/
Mono StringToMono(BlockOfString* b, bool* correct)
{
    if (!StringToMonoInitialCheck(b, correct))
    {
        return MonoZero();
    }
    else
    {
        size_t last_comma = StringFindLastComma(b, correct);
        if (!(*correct))
        {
            return MonoZero();
        }

        BlockOfString only_exp = *b;
        BlockOfString only_poly = *b;
        SplitMono(b, &only_poly, &only_exp, last_comma);

        poly_exp_t exp = StringToExp(&only_exp, correct);
        Poly p = StringToPoly(&only_poly, correct);
        return MonoFromPoly(&p, exp);
    }
}

/**
* Dzieli fragment stringa na jednomiany, zwraca ich tablice.
* @param[in] b: fragment stringa
* @param[in] plus_signs: tablica z pozycjami plusów rozdzielajacych jednomiany
* @param[in] plus_signs_size: liczba plusów rozdzielajcych jednoaminy
* @param[out] correct: bool, miernik powodzenia calej operacji.
* @return tablica jednomianów
*/
Mono* SplitToMonos(BlockOfString* b, size_t* plus_signs, size_t plus_signs_size, bool* correct)
{
    Mono* monos = malloc((plus_signs_size + 1) * sizeof(Mono));
    CHECK_PTR(monos);

    if (plus_signs_size == 0)
    {
        monos[0] = StringToMono(b, correct);
    }
    else
    {
        size_t begin = b->beg;
        for (size_t i = 0; i < plus_signs_size; i++)
        {
            BlockOfString temp_mono = *b;
            temp_mono.beg = begin;
            temp_mono.end = plus_signs[i];
            monos[i] = StringToMono(&temp_mono, correct);
            begin = plus_signs[i] + 1;
            if (begin >= b->len)
            {
                correct = false;
                return monos;
            }
        }
        BlockOfString temp_mono = *b;
        temp_mono.beg = begin;
        temp_mono.end = b->end;
        monos[plus_signs_size] = StringToMono(&temp_mono, correct);
    }
    return monos;

}

/**
* Szuka we fragmencie stringa plusów rozdzielajacych jednomiany.
* @param[in] b: fragment stringa
* @param[out] size: liczba plusów
*/
void CountPluses(BlockOfString* b, int* size)
{
    int num_of_brackets = 0;
    for (size_t i = b->beg; i < b->end; i++)
    {
        if (b->str[i] == '(')
        {
            num_of_brackets++;
        }
        if (b->str[i] == ')')
        {
            num_of_brackets--;
        }
        if (b->str[i] == '+' && num_of_brackets == 0)
        {
            (*size)++;
        }
    }
}

/**
* Szuka we fragmencie stringa plusów rozdzielajacych jednomiany,
* wklada je do tablicy.
* @param[in] b: fragment stringa
* @param[out] res: tablica pozycji tych plusów.
*/
void PlusesToArray(BlockOfString* b, size_t* res)
{
    int count = 0;
    int num_of_brackets = 0;

    for (size_t i = b->beg; i < b->end; i++)
    {
        if (b->str[i] == '(')
        {
            num_of_brackets++;
        }
        if (b->str[i] == ')')
        {
            num_of_brackets--;
        }
        if (b->str[i] == '+' && num_of_brackets == 0)
        {
            res[count] = i;
            count++;
        }
    }
}

/**
* Szuka we fragmencie stringa plusów rozdzielajacych jednomiany,
* tworzy ich tablice.
* @param[in] b: fragment stringa
* @param[out] size: rozmiar tablicy
* @return tablica z numerami pozycji plusów
*/
size_t* StringFindPluses(BlockOfString* b, int* size)
{
    CountPluses(b, size);
    size_t* res = malloc((*size) * sizeof(size_t));
    CHECK_PTR(res);
    PlusesToArray(b, res);
    return res;
}

/**
* Sprawdza warunki konieczne do konwersji fragmentu stringa w wielomian.
* @param[in] b: fragment stringa
* @param[out] correct: bool, miernik powodzenia calej operacji.
* @return bool
*/
bool StringToPolyInitialCheck(BlockOfString* b, bool* correct)
{
    if (!(*correct))
    {
        return false;
    }
    if (b->end <= b->beg || b->end > b->len)
    {
        *correct = false;
        return false;
    }
    return true;
}

/**
* Konwertuje fragment stringa do wielomianu.
* @param[in] b: fragment stringa
* @param[out] correct: bool, miernik powodzenia calej operacji.
* @return wielomian
*/
Poly StringToPoly(BlockOfString* b, bool* correct)
{
    if (!StringToPolyInitialCheck(b, correct))
    {
        return PolyZero();
    }
    if (b->str[b->beg] != '(')
    {
        return PolyFromCoeff(StringToCoeff(b, correct));
    }

    int plus_signs_size = 0;
    size_t* plus_signs = StringFindPluses(b, &plus_signs_size);
    Mono* monos = SplitToMonos(b, plus_signs, plus_signs_size, correct);
    free(plus_signs);

    if (!(*correct))
    {
        free(monos);
        return PolyZero();
    }

    Poly res = PolyAddMonos(plus_signs_size + 1, monos);
    free(monos);
    return res;
}

/**
* Konwertuje jednomian do stringa.
* @param[in] m: jednomian
* @return tablica charów
*/
char* MonoToString(Mono* m)
{
    char* p_string = PolyToString(&(m->p));
    char* str = malloc((strlen(p_string) + len_of_coeff) * sizeof(char));
    CHECK_PTR(str);

    str[0] = '\0';
    strcat(str, "(\0");
    strcat(str, p_string);
    strcat(str, ",\0");

    char* exp_str = malloc(len_of_coeff * sizeof(char));
    CHECK_PTR(exp_str);
    sprintf(exp_str, "%d", m->exp);
    strcat(str, exp_str);
    strcat(str, ")\0");
    free(exp_str);
    free(p_string);

    return str;
}

/**
* Konwertuje wielomian do stringa.
* @param[in] p: wielomian
* @return tablica charów
*/
char* PolyToString(Poly* p)
{
    char* str;
    if (PolyIsCoeff(p))
    {
        str = malloc(len_of_coeff * sizeof(char));
        CHECK_PTR(str);
        sprintf(str, "%ld", p->coeff);
        return str;
    }
    str = malloc(2 * sizeof(char));
    CHECK_PTR(str);
    str[0] = '\0';
    size_t len_of_str = 0;

    for (unsigned int i = 0; i < p->size; i++)
    {
        char* mono_str = MonoToString(&(p->arr[i]));
        len_of_str += strlen(mono_str);
        if (i != 0)
        {
            len_of_str++;
            strcat(str, "+\0");
        }
        str = realloc(str, (len_of_str+2)*sizeof(char));
        CHECK_PTR(str);
        strcat(str, mono_str);
        free(mono_str);
    }
    return str;
}

/**
* Sprawdza, czy linijka jest komentarzem, jest pusta, lub zawiera znak null.
* Wypisuje ewentualny komunikat o bledzie.
* @param[in] line: linijka
* @param[in] getline_value: wartosc zwrócona przez funkcje getline
* @param[in] num_of_lines: numer linijki potrzebny do wypisania bledu
* @return bool
*/
bool LineIsEmptyOrNull(char* line, int getline_value, int num_of_lines)
{
    if (line[0] == '#')
    {
        return true;
    }
    else if(getline_value != (int)strlen(line))
    {
        if (isalpha(line[0]))
        {
            fprintf(stderr, "ERROR %d WRONG COMMAND\n", num_of_lines);
        }
        else
        {
            fprintf(stderr, "ERROR %d WRONG POLY\n", num_of_lines);
        }
        return true;
    }
    else if (strlen(line) == 0 || (strlen(line) == 1 && line[0] == '\n'))
    {
        return true;
    }
    return false;
}

/**
* Parsuje linijke do wielomianu. Nastepnie wrzuca go na stos,
* lub wypisuje komunikat o bledzie.
* @param[out] s: stos
* @param[in] line: linijka
* @param[in] num_of_lines: numer linijki potrzebny do wypisania bledu
*/
void LineToPoly(Stack* s, char* line, unsigned int num_of_lines)
{
    bool correct = true;
    BlockOfString b;
    b.str = line;
    b.beg = 0;
    b.end = strlen(line) - 1;
    b.len = b.end + 1;
    Poly p = StringToPoly(&b, &correct);

    if (correct)
    {
        StackPush(s, &p);
    }
    else
    {
        PolyDestroy(&p);
        fprintf(stderr, "ERROR %d WRONG POLY\n", num_of_lines);
    }
}

/**
* Wyodrebnia fragment stringa po komendzie.
* @param[in] line: linijka
* @param[in] command_length: dlugosc komendy
* @return fragment stringa
*/
BlockOfString BlockAfterCommand(char* line, size_t command_length)
{
    BlockOfString b;
    b.str = line;
    b.len = strlen(b.str);
    b.beg = command_length + 1;
    b.end = strlen(b.str);
    return b;
}

/**
* Sprawdza, czy komenda DEG_BY zawiera prawidlowy argument i wykonuje ja,
* lub wypisuje komunikat o bledzie.
* @param[in] s: stos
* @param[in] line: linijka
* @param[in] num_of_lines: numer linijki
*/
void DegByCheckArgs(Stack* s, char* line, unsigned int num_of_lines)
{
    size_t command_length = strlen("DEG_BY\0");
    bool correct = true;
    BlockOfString b = BlockAfterCommand(line, command_length);

    if (b.str[b.end - 1] == '\n')
    {
        b.end--;
    }
    if (b.end <= b.beg)
    {
        fprintf(stderr, "ERROR %d DEG BY WRONG VARIABLE\n", num_of_lines);
        return;
    }
    if (line[command_length] != ' ')
    {
        fprintf(stderr, "ERROR %d WRONG COMMAND\n", num_of_lines);
        return;
    }

    poly_exp_t var_idx = StringToExpArg(&b, &correct);
    if (!correct)
    {
        fprintf(stderr, "ERROR %d DEG BY WRONG VARIABLE\n", num_of_lines);
        return;
    }
    else if (var_idx >= max_exp)
    {
        Poly p = StackTop(s);
        if (PolyIsZero(&p))
        {
            printf("-1\n");
        }
        else
        {
            printf("0\n");
        }
    }
    else
    {
        DegBy(s, var_idx, num_of_lines);
    }
}

/**
* Sprawdza, czy komenda AT zawiera prawidlowy argument i wykonuje ja,
* lub wypisuje komunikat o bledzie.
* @param[in] s: stos
* @param[in] line: linijka
* @param[in] num_of_lines: numer linijki
*/
void AtCheckArgs(Stack* s, char* line, unsigned int num_of_lines)
{
    size_t command_length = strlen("AT\0");
    bool correct = true;
    BlockOfString b = BlockAfterCommand(line, command_length);

    if (b.end <= b.beg)
    {
        fprintf(stderr, "ERROR %d AT WRONG VALUE\n", num_of_lines);
        return;
    }
    if (b.str[b.end - 1] == '\n')
    {
        b.end--;
    }
    if (line[command_length] != ' ')
    {
        fprintf(stderr, "ERROR %d WRONG COMMAND\n", num_of_lines);
        return;
    }

    poly_coeff_t val = StringToCoeff(&b, &correct);
    if (!correct)
    {
        fprintf(stderr, "ERROR %d AT WRONG VALUE\n", num_of_lines);
        return;
    }
    else
    {
        At(s, val, num_of_lines);
    }
}

/**
* Sprawdza, czy linijka jest komenda.
* Jesli tak, to wykonuje ja lub wypisuje komunikat o bledzie.
* @param[in] s: stos
* @param[in] line: linijka
* @param[in] num_of_lines: numer linijki
*/
bool IsCommand(Stack* s, char* line, unsigned int num_of_lines)
{
    if (strcmp(line, "ZERO\n\0") == 0 || strcmp(line, "ZERO\0") == 0)
    {
        InsertZero(s);
        return true;
    }
    else if (strcmp(line, "IS_COEFF\n\0") == 0 || strcmp(line, "IS_COEFF\0") == 0)
    {
        IsCoeff(s, num_of_lines);
        return true;
    }
    else if (strcmp(line, "IS_ZERO\n\0") == 0 || strcmp(line, "IS_ZERO\0") == 0)
    {
        IsZero(s, num_of_lines);
        return true;
    }
    else if (strcmp(line, "CLONE\n\0") == 0 || strcmp(line, "CLONE\0") == 0)
    {
        Clone(s, num_of_lines);
        return true;
    }
    else if (strcmp(line, "ADD\n\0") == 0 || strcmp(line, "ADD\0") == 0)
    {
        Add(s, num_of_lines);
        return true;
    }
    else if (strcmp(line, "MUL\n\0") == 0 || strcmp(line, "MUL\0") == 0)
    {
        Mul(s, num_of_lines);
        return true;
    }
    else if (strcmp(line, "NEG\n\0") == 0 || strcmp(line, "NEG\0") == 0)
    {
        Neg(s, num_of_lines);
        return true;
    }
    else if (strcmp(line, "SUB\n\0") == 0 || strcmp(line, "SUB\0") == 0)
    {
        Sub(s, num_of_lines);
        return true;
    }
    else if (strcmp(line, "IS_EQ\n\0") == 0 || strcmp(line, "IS_EQ\0") == 0)
    {
        IsEq(s, num_of_lines);
        return true;
    }
    else if (strcmp(line, "DEG\n\0") == 0 || strcmp(line, "DEG\0") == 0)
    {
        Deg(s, num_of_lines);
        return true;
    }
    else if (strcmp(line, "PRINT\n\0") == 0 || strcmp(line, "PRINT\0") == 0)
    {
        Print(s, num_of_lines);
        return true;
    }
    else if (strcmp(line, "POP\n\0") == 0 || strcmp(line, "POP\0") == 0)
    {
        Pop(s, num_of_lines);
        return true;
    }
    else if (memcmp(line, "DEG_BY\n\0", strlen("DEG_BY\0")) == 0)
    {
        DegByCheckArgs(s, line, num_of_lines);
        return true;
    }
    else if (memcmp(line, "AT\n\0", strlen("AT\0")) == 0)
    {
        AtCheckArgs(s, line, num_of_lines);
        return true;
    }

    return false;
}

/**
* Tworzy stos wielomianów. Wczytuje ze standardowego wejscia komendy i je wykonuje,
* parsuje wielomiany i umieszcza je na stosie. Wypisuje komunikaty o ewentualnych
* bledach.
*/
int main()
{
    Stack s;
    StackInit(&s);
    char *line = NULL;
    size_t size;
    unsigned int num_of_lines = 0;

    while (1)
    {
        int getline_value = getline(&line, &size, stdin);
        CHECK_PTR(line);
        if (errno == ENOMEM)
        {
            exit(1);
        }
        if (getline_value == -1)
        {
            break;
        }

        num_of_lines++;
        if (LineIsEmptyOrNull(line, getline_value, num_of_lines))
        {
            continue;
        }

        if (!IsCommand(&s, line, num_of_lines))
        {
            if (isalpha(line[0]))
            {
                fprintf(stderr, "ERROR %d WRONG COMMAND\n", num_of_lines);
            }
            else
            {
                LineToPoly(&s, line, num_of_lines);
            }
        }
    }

    free(line);
    StackDestroy(&s);
    return 0;
}
