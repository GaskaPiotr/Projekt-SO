# Symulacja ula

## Cel symulacji
Celem projektu jest stworzenie symulacji życia roju pszczół, uwzględniając interakcje pomiędzy pszczołami robotnicami, królową oraz pszczelarzem. 
Symulacja obejmuje kluczowe aspekty, takie jak ograniczone miejsce w ulu, instynktowne opuszczanie ula przez pszczoły, składanie jaj przez królową, oraz ingerencje pszczelarza w rozmiar ula.

## Elementy do implementacji
1. **Pszczoły robotnice:**
   - Mogą wchodzić do ula przez dwa wąskie wejścia, przy czym ruch w każdym z wejść jest jednokierunkowy w danej chwili.
   - Przebywanie w ulu jest ograniczone czasem `Ti`, po którym pszczoła opuszcza ul.
   - Pszczoła umiera po określonej liczbie odwiedzin w ulu `Xi`.

2. **Królowa:**
   - Co określony czas `Tk` składa jaja.
   - Składanie jaj jest możliwe tylko wtedy, gdy w ulu jest wystarczająca ilość miejsca (każde jajo zajmuje miejsce jednego dorosłego osobnika).

3. **Ul:**
   - Pojemność początkowa ula to `P` (gdzie `P < N/2`).
   - Pszczelarz może:
     - Zwiększyć pojemność ula do `2*N` (sygnał1).
     - Zmniejszyć pojemność ula o 50% (sygnał2).

4. **Pszczelarz:**
   - Odpowiada za zarządzanie pojemnością ula poprzez dodawanie i usuwanie ramek.

5. **Synchronizacja i współbieżność:**
   - Program musi zapewniać odpowiednią synchronizację dostępu do ula, aby uniknąć konfliktów w wejściach oraz przegrzania ula.

## Testy
1. **Testy podstawowe:**
   - Sprawdzenie poprawności działania ula przy różnych wartościach `N`, `P`, `Ti`, `Tk`, i `Xi`.
   - Weryfikacja, czy królowa składa jaja tylko wtedy, gdy w ulu jest miejsce.
   - Sprawdzenie, czy pszczoły robotnice opuszczają ul po upływie czasu `Ti`.

2. **Testy skrajnych przypadków:**
   - Ul jest przepełniony, a królowa próbuje składać jaja.
   - Ul jest przepełniony, a pszczoły próbują się dostać do środka.
   - Szybkie zmiany pojemności ula przez pszczelarza (dodawanie i usuwanie ramek w krótkim czasie).

3. **Testy wielowątkowości:**
   - Symulacja wielu pszczół próbujących jednocześnie wejść do ula przez oba wejścia.
   - Weryfikacja poprawności synchronizacji w przypadku intensywnego ruchu w ulu.

4. **Testy wydajności:**
   - Symulacja dużego roju pszczół (`N > 1000`) z dynamicznymi zmianami pojemności ula.

## Podsumowanie
Projekt symulacji ula ma na celu ukazanie kluczowych mechanizmów synchronizacji oraz współbieżności w systemach wielowątkowych. 
Implementacja zapewnia realistyczne odwzorowanie cyklu życia roju pszczół, uwzględniając ograniczenia przestrzeni, czas przebywania pszczół w ulu oraz interwencje pszczelarza. 
Dzięki odpowiednim testom symulacja jest wszechstronnie przetestowana pod kątem poprawności i wydajności.
