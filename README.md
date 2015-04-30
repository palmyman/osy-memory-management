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
