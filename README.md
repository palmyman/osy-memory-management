# Paměťový manažer

Zadaný úkol je zaměřen na podobnější pochopení přidělování paměti v OS. Jedná se z velmi zjednodušenou variantu, která spočívá v implementaci jednoduchého správce dynamicky alokované paměti uživatelského procesu.

V jednoduchém modelovém prostředí neexistuje správce haldy (v testovacím prostředí na Progtestu je odpojen). Tedy neexistují funkce malloc, free, ..., ani operátory new a delete. Vaším úkolem je naimplementovat jejich zjednodušené ekvivalenty.

Vaše rozhraní bude pracovat nad blokem svěřené paměti zadané velikosti (při inicializaci Vám bude předán ukazatel na existující blok paměti a informace o velikosti tohoto bloku paměti). Předaný blok paměti bude souvislý a relativně velký (jednotky až stovky MiB). Vámi implementovaný paměťový subsystém bude mít za úkol tento svěřený blok paměti obhospodařovat. Tedy umožnit jeho využití v menších částech podle přicházejících požadavků, udržovat si informace o alokovaných a nepoužitých částech svěřeného bloku paměti, umožnit alokaci bloku, uvolnění alokovaného bloku a slévání uvolněných bloků do souvislých celků.

Odevzdávat budete zdrojový kód, který bude obsahovat funkce se zadaným rozhraním. Testovací prostředí k sobě připojí Vámi odevzdaný zdrojový kód a začne používat funkce z definovaného rozhraní. Na základě jejich odezvy pak rozhodne o funkčnosti nebo nefunkčnosti odevzdávané implementace. Je tedy potřeba přesně dodržet specifikaci rozhraní (jména funkcí, typy parametrů, ...). Vámi odevzdávaný kód nebude plnohodnotný program. Nebude mít např. funkci main, bude se jednat spíše o modul (knihovnu funkcí).

Požadované funkce a jejich význam:

```c
void HeapInit ( void * startAddr, int size );
```
Funkce HeapInit, bude zavolána na počátku při inicializaci subsystému, před voláním ostatních implementovaných funkcí. Funkce má parametrem ukazatel startAddr, který udává paměťovou adresu bloku, který má Váš paměťový subsystém obhospodařovat. Druhým parametrem je size - velikosti svěřeného bloku paměti v bajtech.

V tomto paměťovém bloku (začíná na adrese startAddr a má velikost size bajtů) bude realizována halda. Tedy budou zde uložené veškeré alokované bloky i Vaše servisní informace o tom, jaké části Vaší haldy jsou jak využívané / volné.

```c
void HeapDone ( int * pendingBlk );
```
Funkce HeapDone, bude zavolána na konci používání haldy. Funkce zjistí, kolik bloků paměti na haldě ještě zůstalo alokovaných, tuto informaci předá volajícímu prostřednictvím výstupního parametru pendingBlk.

```c
void * HeapAlloc ( int size );
```
Funkce HeapAlloc bude sloužit jako vstupní bod pro alokaci bloku paměti ve spravované haldě. Funkce má parametrem velikost požadovaného bloku a vrací ukazatel na přidělený prostor paměti. V případě selhání funkce vrací hodnotu NULL. K selhání může dojít pokud je prostor ve svěřené paměti již vyčerpán nebo pokud svěřený blok paměti po předchozích alokacích již neobsahuje dostatečně velký souvislý prostor pro požadovanou alokaci.

```c
bool HeapFree ( void * blk );
```
Funkce HeapFree slouží k uvolnění bloku dříve alokovaného pomocí HeapAlloc. Funkce vrací hodnotu true, pokud blok úspěšně uvolnila nebo hodnotu false, pokud se blok nepodařilo uvolnit (zadaná neplatná adresa, blok již byl uvolněn, ...).

Odevzdávejte soubor, který obsahuje implementaci požadovaných funkcí, potřebných deklarací globálních proměnných, datových typů a podpůrných funkcí. Za základ využijte přiložený ukázkový kód. Pokud ponecháte direktivy pro podmíněný překlad, lze zdrojový soubor rovnou odevzdávat Progtestu.

Při implementaci máte velmi omezené prostředky (viz seznam hlavičkových souborů). Není k dispozici STL, nejsou k dispozici standardní funkce pro dynamickou alokaci (malloc, ...) ani jejich C++ ekvivalenty (new). Odevzdávaný kód bude spouštěn v omezeném prostředí. Budete omezeni dostupnou pamětí (kromě svěřeného bloku paměti je k dispozici pouze pár kilobyte na uložení několika málo globálních proměnných). Tato extra dostupná paměť nepostačuje pro uložení všech servisních informací o alokacích (tyto informace budete muset ukládat do spravované paměti a řídit si jejich rozmisťování).

Hodnocení je rozděleno na povinnou a nepovinnou část. V povinné části se testuje správnost alokátoru. Je potřeba, aby správně přiděloval paměť, aby se alokované bloky nepřekrývaly a aby se uvolněná paměť správně spojovala.

Nepovinné testy kontrolují, jak se Vaše implementace vypořádá s fragmentací a jak rychle dokáže pracovat pro velké množství alokovaných bloků. Pokud tyto testy neprojdete, bude Vaše hodnocení kráceno.
