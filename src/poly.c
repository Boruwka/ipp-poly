/** @file
  Interfejs klasy wielomianów rzadkich wielu zmiennych.
  @authors Jagoda Bracha <jb429153@students.mimuw.edu.pl>
  @date 2021
*/

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "poly.h"

/**
Sprawdza, czy alokacja sie powiodla.
*/
#define CHECK_PTR(p)    	\
	do {			    	\
		if (p == NULL) {	\
			exit(1);		\
		}					\
	} while (0)


/**
 * Zwalnia tablice jednomianów i niszczy jej zawartosc.
 * @param[in] arr: wskaznik na tablice wielomianów
 * @param[in] size: rozmiar tablicy
 */
static void FreeArrOfMonos(Mono** arr, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        MonoDestroy(&((*arr)[i]));
    }
    free(*arr);
    *arr = NULL;
}

/**
 * Usuwa wielomian z pamięci.
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p)
{
    if (PolyIsCoeff(p))
    {
        return;
    }
    FreeArrOfMonos(&(p->arr), p->size);
}



/**
 * Robi pełną, głęboką kopię wielomianu.
 * @param[in] p : wielomian
 * @return skopiowany wielomian
 */
Poly PolyClone(const Poly* p)
{
    assert(p);
    if(PolyIsCoeff(p))
    {
        Poly q;
        q.coeff = p->coeff;
        q.arr = NULL;
        return q;
    }
    Poly q;
    q.size = p->size;
    q.arr = malloc((q.size)*sizeof(Mono));
    CHECK_PTR(q.arr);
    for (size_t i = 0; i < p->size; i++)
    {
        q.arr[i] = MonoClone(&(p->arr[i]));
    }
    return q;
}


/**
 * Sprawdza, czy liczba jest potega dwójki.
 * @param[in] a: liczba dodatnia calkowita
 * @return bool mówiacy, czy jest potega dwójki
 */
static bool IsPowerOfTwo(unsigned int a)
{
    if (a == 0 || a == 1)
    {
        return true;
    }
    else if (a % 2 == 1)
    {
        return false;
    }
    else
    {
        return IsPowerOfTwo(a / 2);
    }
}

/**
 * Dodaje jednomian do tablicy, w razie potrzeby ja realokuje.
 * Przyjmuje na wlasnosc zawartosc jednomianu m.
 * @param[in] array: wskaznik na tablice jednomianów
 * @param[in] m: jednomian
 * @param[in] size: rozmiar tablicy
 */
static void InsertEnd(Mono** array, Mono* m, size_t size)
{
    assert(array && m);
    assert(*array);
    if (size != 0 && IsPowerOfTwo(size))
    {
        // Powieksza tablice dwukrotnie gdy jej rozmiar jest potega dwójki.
        (*array) = realloc((*array), (2*size) * sizeof(Mono));
        CHECK_PTR((*array));
    }
    (*array)[size] = *m;

}

/**
* Klonuje podany jednomian mono_to_insert oraz wielomian p_original;
* Tworzy nowy wielomian p, który jest kopia p_original,
* ale na poczatku tablicy ma dodatkowo jednomian mono_to_insert.
* @param[in] p: wielomian, który jest tworzony
* @param[in] p_original: wielomian do sklonowania
* @param[in] mono_to_insert: jednomian do wstawienia na poczatek
*/
static void CloneAndInsert(Poly* p, const Poly* p_original, const Mono* mono_to_insert)
{
    assert(!PolyIsCoeff(p_original));
    p->size = p_original->size + 1;
    p->arr = malloc(p->size * sizeof(Mono));
    p->arr[0] = MonoClone(mono_to_insert);
    for (size_t i = 1; i < p->size; i++)
    {
        p->arr[i] = MonoClone(&(p_original->arr[i-1]));
    }
}

/**
* Sprawdza, czy wielomian p sklada sie wylacznie z jednomianu,
* którego wykladnik to 0, a wielomian jest wspólczynnikiem.
* @param[in] p: wielomian
* @return: bool
*/
static bool PolyUnreduced(const Poly* p)
{
    return (p->size == 1 && p->arr[0].exp == 0 && PolyIsCoeff(&(p->arr[0].p)));
}

/**
* Jesli wielomian sklada sie tylko z jednomianu o wspolczynniku i wykladniku 0,
* zamienia go na wielomian wspolczynnikowy.
* @param[in] p: wielomian
*/
static void PolyReduce(Poly *p)
{
    assert(p);
    if (PolyIsCoeff(p))
    {
        return;
    }
    else if (p->size == 0)
    {
        free(p->arr);
        *p = PolyZero();
    }
    else if (PolyUnreduced(p))
    {
        poly_coeff_t coeff = p->arr[0].p.coeff;
        FreeArrOfMonos(&p->arr, 1);
        p->coeff = coeff;
    }
}

/**
* Porównuje jednomiany po wykladnikach.
* @param[in] a: jednomian
* @param[in] b: jednomian
* @return -1, 0 lub 1
*/
static int CompareMonos(Mono* a, Mono *b)
{
    assert(a && b);
    if(a->exp < b->exp)
    {
        return -1;
    }
    else if(a->exp == b->exp)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * Sprawdza równość dwóch wielomianów.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p = q@f$
 */
bool PolyIsEq(const Poly* p, const Poly* q)
{
    assert(p && q);
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
    {
        return (p->coeff == q->coeff);
    }
    else if (PolyIsCoeff(p) || PolyIsCoeff(q))
    {
        return false;
    }
    else
    {
        if (p->size != q->size)
        {
            return false;
        }
        for (unsigned int i = 0; i < p->size; i++)
        {
            if (p->arr[i].exp != q->arr[i].exp)
            {
                return false;
            }
            if (!PolyIsEq(&(p->arr[i].p), &(q->arr[i].p)))
            {
                return false;
            }
        }
        return true;
    }
}


Poly PolyAdd(const Poly* p, const Poly* q);

/**
* Dodaje dwa wspolczynnikowe wielomiany.
* @param[in] p: wielomian
* @param[in] q: jednomian
* @return wielomian p+q
*/
static Poly AddTwoEmptyPolys(const Poly* p, const Poly* q)
{
    assert(p && q);
    assert(PolyIsCoeff(p) && PolyIsCoeff(q));
    Poly r;
    r.arr = NULL;
    r.coeff = p->coeff + q->coeff;
    return r;
}

/**
* Dodaje dwa wielomiany, z których jeden jest wspólczynnikiem,
* a drugi ma jednomian z potega 0.
* @param[in] p: wielomian
* @param[in] q: wielomian
* @return wielomian p+q
*/
static Poly AddCoeffToZeroExp(const Poly* p, const Poly* q)
{
    assert(p && q);
    assert(!PolyIsCoeff(q) && PolyIsCoeff(p));
    assert(q->arr[0].exp == 0);

    Mono q_zero_exp = q->arr[0];
    if (PolyIsCoeff(&(q_zero_exp.p)) && q_zero_exp.p.coeff + p->coeff == 0)
    {
        /*
        Jesli suma wspólczynnikowego wielomianu oraz jednomianu
        drugiego wielomianu o wykladniku 0 jest wielomianem zerowym. */
        Poly r;
        r.size = q->size - 1;
        r.arr = malloc(sizeof(Mono));
        CHECK_PTR(r.arr);
        Mono temp_mono;
        for (size_t i = 1; i < q->size; i++)
        {
            temp_mono = MonoClone(&(q->arr[i]));
            InsertEnd(&r.arr, &temp_mono, (i-1));
        }
        return r;
    }

    Poly r;
    r.size = q->size;
    r.arr = malloc(r.size*sizeof(Mono));
    CHECK_PTR(r.arr);
    r.arr[0].p = PolyAdd(&q_zero_exp.p, p);
    r.arr[0].exp = q_zero_exp.exp;
    for (size_t i = 1; i < r.size; i++)
    {
        r.arr[i] = MonoClone(&(q->arr[i]));
    }
    return r;
}

/**
* Dodaje dwa wielomiany, z których jeden jest wspólczynnikiem,
* a drugi nie ma jednomianu z potega 0.
* @param[in] p: wielomian
* @param[in] q: wielomian
* @return wielomian p+q
*/
static Poly AddCoeffToNonZeroExp(const Poly* p, const Poly* q)
{
    assert(p && q);
    assert(!PolyIsCoeff(q) && PolyIsCoeff(p));
    assert(q->arr[0].exp != 0);

    Mono p_mono = MonoFromPoly(p, 0);
    Poly r;

    if (p->coeff != 0)
    {
        CloneAndInsert(&r, q, &p_mono);
    }
    else
    {
        r = PolyClone(q);
    }

    PolyReduce(&r);
    return r;
}

/**
* Dodaje dwa wielomiany, z których jeden jest wspólczynnikiem, a drugi nie.
* @param[in] p: wielomian
* @param[in] q: wielomian
* @return wielomian p+q
*/
static Poly AddEmptyPoly(const Poly* p, const Poly* q)
{
    assert(p && q);
    assert(!PolyIsCoeff(q) && PolyIsCoeff(p));
    if (q->arr[0].exp == 0)
    {
        return AddCoeffToZeroExp(p, q);
    }
    else
    {
        return AddCoeffToNonZeroExp(p, q);
    }
}

/**
* Klonuje i-ty jednomian wielomianu p,
* po czym dodaje go do tablicy wielomianu r.
* @param[in] p: wielomian
* @param[in] i: numer klonowanego jednomianu
* @param[in] r: wielomian
*/
static void InsertClone(const Poly* p, size_t* i, Poly* r)
{
    Mono temp_mono = MonoClone(&(p->arr[(*i)]));
    InsertEnd((&r->arr), &temp_mono, r->size);
    r->size++;
    (*i)++;
}

/**
* Dodaje dwa wielomiany, z których zaden nie jest wspólczynnikowy.
* @param[in] p: wielomian
* @param[in] q: wielomian
* @return wielomian p+q
*/
static Poly AddTwoNotEmptyPolys(const Poly* p, const Poly* q)
{
    assert(p && q);
    assert(!PolyIsCoeff(p) && !PolyIsCoeff(q));
    Poly r;
    r.size = 0;
    r.arr = malloc(sizeof(Mono));
    CHECK_PTR(r.arr);
    size_t i = 0, j = 0;
    while (i < p->size || j < q->size)
    {
        /*
        To MergeSort jednomianów tych wielomianów,
        gdy napotyka na jednomiany o tych samych potegach, scala je. */
        if (i >= p->size)
        {
            InsertClone(q, &j, &r);
        }
        else if (j >= q->size)
        {
            InsertClone(p, &i, &r);
        }
        else if (p->arr[i].exp == q->arr[j].exp)
        {
            Poly s = PolyAdd(&(p->arr[i].p), &(q->arr[j].p));
            if (!(PolyIsCoeff(&s) && s.coeff == 0))
            {
                Mono temp = MonoFromPoly(&s, p->arr[i].exp);
                InsertEnd((&r.arr), &temp, r.size);
                r.size++;
            }
            i++;
            j++;
        }
        else if (p->arr[i].exp < q->arr[j].exp)
        {
            InsertClone(p, &i, &r);
        }
        else
        {
            InsertClone(q, &j, &r);
        }
    }
    PolyReduce(&r);
    return r;
}



/**
 * Dodaje dwa wielomiany.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p + q@f$
 */
Poly PolyAdd(const Poly* p, const Poly* q)
{
    assert(p && q);
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
    {
        return AddTwoEmptyPolys(p, q);
    }
    else if (PolyIsCoeff(p))
    {
        return AddEmptyPoly(p, q);
    }
    else if (PolyIsCoeff(q))
    {
        return AddEmptyPoly(q, p);
    }
    else
    {
        return AddTwoNotEmptyPolys(p, q);
    }
}

/**
 * Zwraca przeciwny wielomian.
 * @param[in] p : wielomian @f$p@f$
 * @return @f$-p@f$
 */
Poly PolyNeg(const Poly *p)
{
    assert(p);
    if (PolyIsCoeff(p))
    {
        Poly res = PolyClone(p);
        res.coeff = -res.coeff;
        return res;
    }
    Poly res;
    res.size = p->size;
    res.arr = malloc(res.size*sizeof(Mono));
    CHECK_PTR(res.arr);
    for (size_t i = 0; i < p->size; i++)
    {
        res.arr[i].exp = p->arr[i].exp;
        res.arr[i].p = PolyNeg(&(p->arr[i].p));
    }
    return res;
}

/**
 * Odejmuje wielomian od wielomianu.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p - q@f$
 */
Poly PolySub(const Poly* p, const Poly* q)
{
    assert(p && q);
    Poly q_neg = PolyNeg(q);
    Poly res = PolyAdd(p, &q_neg);
    PolyDestroy(&q_neg);
    return res;
}

/**
* Przyjmuje posortowana po wykladnikach tablice jednomianów monos,
* modyfikuje tablice new_monos tak, aby zawierala zawartosc tablicy monos
* ze scalonymi jednomianami o równych wykladnikach.
* Przyjmuje zawartosc tablicy monos na wlasnosc.
* @param[in] monos: tablica jednomianów
* @param[in] new_monos: wskaznik na docelowa tablice jednomianów
* @param[in] count: liczba elementów w tablicy monos
* @param[in] new_count: wskaznik na liczbe elementów w tablicy new_monos
*/
static void MergeMonos(Mono* monos, Mono** new_monos, size_t count, size_t* new_count)
{
    assert(monos && new_monos && count > 0 && new_count);
    assert((*new_monos) && (*new_count) == 0);
    Mono temp_mono = monos[0];
    for (size_t i = 1; i < count; i++)
    {
        if (CompareMonos(&monos[i], &monos[i-1]) == 0)
        {
            // Gdy maja równe wykladniki
            Poly temp_poly = PolyAdd(&temp_mono.p, &monos[i].p);
            MonoDestroy(&(monos[i]));
            MonoDestroy(&temp_mono);
            temp_mono.p = temp_poly;
        }
        else
        {
            if (!PolyIsZero(&temp_mono.p))
            {
                InsertEnd(new_monos, &temp_mono, *new_count);
                (*new_count)++;
            }
            temp_mono = monos[i];
        }
    }
    if (!PolyIsZero(&temp_mono.p))
    {
        InsertEnd(new_monos, &temp_mono, (*new_count));
        (*new_count)++;
    }
}

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian.
 * Przejmuje na własność zawartość tablicy @p monos.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyAddMonos(size_t count, const Mono monos[])
{
    /*
    Sortuje jednomiany, a nastepnie je scala. */
    assert(monos);
    if (count == 0)
    {
        return PolyZero();
    }
    Mono* monos_sorted = malloc(count*sizeof(Mono));
    CHECK_PTR(monos_sorted);

    for (size_t i = 0; i < count; i++)
    {
        monos_sorted[i] = monos[i];
    }
    qsort(monos_sorted, count, sizeof(Mono),
     (int(*)(void const*, void const*))CompareMonos);
    Mono* new_monos = malloc(sizeof(Mono));
    CHECK_PTR(new_monos);

    size_t new_count = 0;
    MergeMonos(monos_sorted, &new_monos, count, &new_count);
    if (new_count == 0)
    {
        free(monos_sorted);
        free(new_monos);
        return PolyZero();
    }

    Poly p;
    p.size = new_count;
    p.arr = malloc(new_count*sizeof(Mono));
    CHECK_PTR(p.arr);
    for (size_t i = 0; i < new_count; i++)
    {
        p.arr[i] = new_monos[i];
    }
    free(monos_sorted);
    free(new_monos);
    PolyReduce(&p);
    return p;
}

Poly PolyMul(const Poly *p, const Poly *q);

/**
* Mnozy dwa wielomiany, które sa wspólczynnikami.
* @param[in] p: wielomian
* @param[in] q: wielomian
* @return wielomian p*q
*/
static Poly MulTwoCoeffs(const Poly* p, const Poly* q)
{
    assert(PolyIsCoeff(p) && PolyIsCoeff(q));
    Poly res;
    res.coeff = p->coeff * q->coeff;
    res.arr = NULL;
    return res;
}

/**
* Mnozy dwa wielomiany, z których jeden jest wspólczynnikiem.
* @param[in] p: wielomian
* @param[in] q: wielomian
* @return wielomian p*q
*/
static Poly PolyMulWithCoeff(const Poly* p, const Poly* q)
{
    assert(PolyIsCoeff(q));
    if (PolyIsCoeff(p))
    {
        return MulTwoCoeffs(p, q);
    }
    if (q->coeff == 0)
    {
        return PolyZero();
    }
    Poly r; // Wynikowy wielomian.
    r.size = 0;
    r.arr = malloc(sizeof(Mono));
    CHECK_PTR(r.arr);
    for (size_t i = 0; i < p->size; i++)
    {
        // Przechodzi po kolei po wszystkich jego jednomianach i je wymnaza.
        Poly s = PolyMulWithCoeff(&(p->arr[i].p), q);
        if (!PolyIsZero(&s))
        {
            Mono temp_mono;
            temp_mono.exp = p->arr[i].exp;
            temp_mono.p = s;
            InsertEnd(&r.arr, &temp_mono, r.size);
            r.size++;
        }
        else
        {
            PolyDestroy(&s);
        }
    }
    if (r.size == 0)
    {
        free(r.arr);
        return PolyZero();
    }
    PolyReduce(&r);
    return r;
}

/**
Mnozy dwa jednomiany.
* @param[in] a: jednomian
* @param[in] b: jednomian
* @return jednomian a*b
*/
static Mono MulMonos(Mono *a, Mono *b)
{
    assert(a && b);
    Mono res;
    res.exp = a->exp + b->exp;
    res.p = PolyMul(&(a->p), &(b->p));
    return res;
}

/**
 * Mnoży dwa wielomiany.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p * q@f$
 */
Poly PolyMul(const Poly *p, const Poly *q)
{
    assert(p && q);
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
    {
        return MulTwoCoeffs(p, q);
    }
    if (PolyIsCoeff(p))
    {
        return PolyMulWithCoeff(q, p);
    }
    if (PolyIsCoeff(q))
    {
        return PolyMulWithCoeff(p, q);
    }
    Mono* components = malloc(2*sizeof(Mono));
    CHECK_PTR(components);
    size_t components_size = 0;
    // Mnozy ich jednomiany kazdy z kazdym
    for(size_t i = 0; i < p->size; i++)
    {
        for(size_t j = 0; j < q->size; j++)
        {
            Mono multiplied_mono = MulMonos(&(p->arr[i]), &(q->arr[j]));
            InsertEnd(&components, &multiplied_mono, components_size);
            components_size++;
        }
    }
    Poly res = PolyAddMonos(components_size, components);
    free(components);
    components = NULL;
    PolyReduce(&res);
    return res;
}

/**
* Zwraca wiekszy sposród wykladników.
* @param[in] a: wykladnik
* @param[in] b: wykladnik
* @return max(a, b)
*/
static poly_exp_t max_poly_exp_t(poly_exp_t a, poly_exp_t b)
{
    if(a > b)
    {
        return a;
    }
    return b;
}

/**
 * Zwraca stopień wielomianu ze względu na zadaną zmienną (-1 dla wielomianu
 * tożsamościowo równego zeru). Zmienne indeksowane są od 0.
 * Zmienna o indeksie 0 oznacza zmienną główną tego wielomianu.
 * Większe indeksy oznaczają zmienne wielomianów znajdujących się
 * we współczynnikach.
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopień wielomianu @p p z względu na zmienną o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, size_t var_idx)
{
    assert(p);
    if(PolyIsCoeff(p))
    {
        if(p->coeff == 0)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else if (var_idx == 0)
    {
        return p->arr[p->size-1].exp;
    }
    else
    {
        poly_exp_t max_deg = 0;
        for (unsigned int i = 0; i < p->size; i++)
        {
            max_deg = max_poly_exp_t(max_deg,
             PolyDegBy(&(p->arr[i].p), var_idx-1));
        }
        return max_deg;
    }
}

/**
 * Zwraca stopień wielomianu (-1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] p : wielomian
 * @return stopień wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly* p)
{
    assert(p);
    if (PolyIsCoeff(p))
    {
        if (p->coeff == 0)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        poly_exp_t max_deg = 0;
        for (size_t i = 0; i < p->size; i++)
        {
            max_deg = max_poly_exp_t
                (max_deg, PolyDeg(&(p->arr[i].p)) + p->arr[i].exp);
        }
        return max_deg;
    }
}

/**
* Oblicza potege liczby calkowitej.
* @param[in] base: podstawa potegi
* @param[in] exp: wykladnik
* @param[in] res: obliczony wynik
* @return @f$ @base^ @exp * @res @f$
*/
static poly_coeff_t PowerHelper(poly_coeff_t base, poly_exp_t exp, poly_coeff_t res)
{
    assert(exp >= 0);
    if (exp == 0)
    {
        return res;
    }
    if (exp == 1)
    {
        return res*base;
    }
    else if (exp % 2 == 0)
    {
        return PowerHelper(base * base, exp / 2, res);
    }
    else
    {
        return PowerHelper(base * base, exp / 2, res * base);
    }
}

/**
* Oblicza potege liczby calkowitej.
* @param[in] base: podstawa potegi
* @param[in] exp: wykladnik
* @return @f$ @base^ @exp @f$
*/
static poly_coeff_t Power(poly_coeff_t base, poly_exp_t exp)
{
    assert(exp >= 0);
    return PowerHelper(base, exp, 1);
}



/**
* Oblicza jednomian bedacy wartoscia jednomianu m w x,
* wstawia go do tablicy components.
* @param[in] m: jednomian
* @param[in] x: wartosc
* @param[in] components: wskaznik do tablicy jednomianów
* @param[in] components_size: rozmiar tablicy @f$ @components @f$
*/
static void ComputeComponent(const Mono *m, poly_coeff_t x,
    Mono** components, unsigned int* components_size)
{
    if (PolyIsCoeff(&((*m).p)))
    {
        Poly multiplied_coeffs = PolyFromCoeff
            (Power(x, (*m).exp) * (*m).p.coeff);

        Mono m_temp = MonoFromPoly(&multiplied_coeffs, 0);
        InsertEnd(components, &m_temp, *components_size);
        (*components_size)++;
    }
    else
    {
        for (size_t j = 0; j < (*m).p.size; j++)
        {
            Poly power_coeff = PolyFromCoeff(Power(x, (*m).exp));
            Poly multiplied_polys = PolyMulWithCoeff
                (&((*m).p.arr[j].p), &power_coeff);

            if (!PolyIsZero(&multiplied_polys))
            {
                Mono m_temp = MonoFromPoly
                    (&multiplied_polys, (*m).p.arr[j].exp);

                InsertEnd(components, &m_temp, *components_size);
                (*components_size)++;
            }
            PolyDestroy(&power_coeff);
        }
    }
}


/**
 * Wylicza wartość wielomianu w punkcie @p x.
 * Wstawia pod pierwszą zmienną wielomianu wartość @p x.
 * W wyniku może powstać wielomian, jeśli współczynniki są wielomianami.
 * Wtedy zmniejszane są o jeden indeksy zmiennych w takim wielomianie.
 * Formalnie dla wielomianu @f$p(x_0, x_1, x_2, \ldots)@f$ wynikiem jest
 * wielomian @f$p(x, x_0, x_1, \ldots)@f$.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] x : wartość argumentu @f$x@f$
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly* p, poly_coeff_t x)
{
    assert(p);
    if (PolyIsCoeff(p))
    {
        return PolyClone(p);
    }
    else if (p->size == 1 && p->arr[0].exp == 0 && PolyIsCoeff(&(p->arr[0].p)))
    {
        return PolyClone(&(p->arr[0].p));
    }

    Mono* components = malloc(sizeof(Mono));
    CHECK_PTR(components);
    unsigned int components_size = 0;

    for (size_t i = 0; i < p->size; i++)
    {
        ComputeComponent(&(p->arr[i]), x, &components, &components_size);
    }

    if (components_size == 0)
    {
        FreeArrOfMonos(&components, components_size);
        return PolyZero();
    }

    Poly res;
    if (components_size == 1 && components[0].exp == 0 &&
        PolyIsCoeff(&(components[0].p)))
    {
        res = PolyFromCoeff(components[0].p.coeff);
        FreeArrOfMonos(&components, components_size);
    }
    else
    {
        res = PolyAddMonos(components_size, components);
        free(components);
        components = NULL;
    }

    PolyReduce(&res);
    return res;
}

