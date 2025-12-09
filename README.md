# Kalkulator Reprezentacji IEEE 754

Ten projekt konwertuje liczby dziesiętne na ich binarną reprezentację zgodną z IEEE 754 w precyzji poczwórnej (128-bit) i oferuje interfejs graficzny zbudowany na Raylibie, który wizualizuje kolejne etapy konwersji.

## Funkcjonalności

-   Konwersja liczb dziesiętnych na binarne
-   Obliczanie formatu IEEE 754 w precyzji poczwórnej (128-bit)
-   Kolorowe oznaczenie wyniku (bit znaku na czerwono, wykładnik na niebiesko, mantysa na zielono)
-   Wykorzystuje Boost.Multiprecision dla arytmetyki wysokiej precyzji
-   Lekki interfejs graficzny (Raylib) z polem tekstowym, przyciskiem **Convert** oraz wizualizacją segmentów (bit znaku / wykładnik / mantysa)

## Wymagania

### Wszystkie systemy operacyjne

Potrzebujesz:

-   Kompilatora zgodnego z C++17 (g++ 7+ lub clang++ 5+)
-   Biblioteki Boost (konkretnie Boost.Multiprecision)
-   Biblioteki [Raylib](https://www.raylib.com/) w wersji 4.0+ (nagłówki + biblioteki)
-   Narzędzia `make` do budowania

### Linux (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install build-essential libboost-all-dev libquadmath0 libraylib-dev
```

### Linux (Fedora/RHEL)

```bash
sudo dnf install gcc-c++ boost-devel libquadmath-devel raylib-devel make
```

### Linux (Arch)

```bash
sudo pacman -S base-devel boost raylib
```

### macOS

Zainstaluj Xcode Command Line Tools i Homebrew, następnie:

```bash
brew install boost raylib
```

**Uwaga:** macOS może nie mieć `libquadmath`. Może być konieczne zmodyfikowanie Makefile i usunięcie `-lquadmath` lub zainstalowanie GCC przez Homebrew:

```bash
brew install gcc
```

Następnie zaktualizuj Makefile, aby używał `g++-13` (lub Twojej wersji GCC) zamiast `g++`.

### Windows

**Opcja 1: WSL (Zalecane)**

-   Zainstaluj WSL2 i Ubuntu, następnie postępuj zgodnie z instrukcjami dla Linuxa powyżej

**Opcja 2: MinGW-w64**

1. Zainstaluj [MinGW-w64](https://www.mingw-w64.org/)
2. Zainstaluj Boost ze źródeł lub użyj [vcpkg](https://vcpkg.io/)
3. Zainstaluj Raylib (np. `vcpkg install raylib` albo pobierz [prekompilowane paczki](https://github.com/raysan5/raylib/releases))
4. Zaktualizuj zmienne `BOOST_INCLUDE`, `RAYLIB_INCLUDE` oraz `RAYLIB_LIBS` w Makefile lub przekazuj je do `make` (przykład poniżej)

**Opcja 3: Visual Studio**

-   Zainstaluj Visual Studio z obsługą C++
-   Zainstaluj Boost przez vcpkg lub ręcznie
-   Będziesz musiał stworzyć projekt VS lub dostosować komendy budowania

## Instalacja

1. Sklonuj repozytorium:

```bash
git clone https://github.com/bartosz-skejcik/ask-project.git
cd ask-project
```

2. Sprawdź instalację Boost (Linux/macOS):

```bash
# Sprawdź czy nagłówki Boost są dostępne
ls /usr/include/boost/multiprecision/float128.hpp
# lub
ls /usr/local/include/boost/multiprecision/float128.hpp
```

3. Jeśli Boost znajduje się w niestandardowej lokalizacji, edytuj `Makefile` i zaktualizuj ścieżkę `BOOST_INCLUDE`.

4. Jeśli Raylib nie jest w standardowych ścieżkach, ustaw zmienne środowiskowe podczas kompilacji:

```bash
make RAYLIB_INCLUDE="-I/path/to/raylib/include" RAYLIB_LIBS="-L/path/to/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm"
```

## Kompilacja

```bash
make
```

Lub zbuduj i uruchom od razu (domyślnie uruchamia GUI):

```bash
make run
```

## Użycie

### Interfejs graficzny (domyślny)

```bash
./bin/main
```

GUI wyświetla pole tekstowe, przyciski **Convert**/**Clear** oraz panele z wizualizacją bitów. Wyniki aktualizują się natychmiast po naciśnięciu przycisku Convert.

### Tryb konsolowy

```bash
./bin/main --cli
```

Tryb ten zachowuje dotychczasowe kolorowanie znaków w terminalu.

## Czyszczenie

Usuń skompilowane pliki:

```bash
make clean
```

## Rozwiązywanie problemów

### "boost/multiprecision/float128.hpp: No such file or directory"

Boost nie jest zainstalowany lub nie znajduje się w ścieżce include. Zainstaluj Boost lub zaktualizuj `BOOST_INCLUDE` w Makefile.

### "cannot find -lquadmath"

Biblioteka quadmath nie jest dostępna. Na macOS może być konieczne usunięcie `-lquadmath` z Makefile lub użycie GCC zamiast clang.

### Ostrzeżenia kompilacji o nieużywanych parametrach

To ostrzeżenia niekrytyczne i mogą być zignorowane. Kod będzie działał poprawnie.

### "Values with |x| < 1 are not supported"

Aktualna logika normalizacji obsługuje liczby, których część całkowita jest większa od zera. Dla wartości 0 < |x| < 1 aplikacja zwróci komunikat o braku wsparcia. Wymaga to osobnej implementacji normalizacji części ułamkowej.

## Struktura projektu

```
.
├── main.cpp          # Główny punkt wejścia programu
├── utils.cpp         # Funkcje pomocnicze do konwersji
├── utils.h           # Plik nagłówkowy z deklaracjami funkcji
├── Makefile          # Konfiguracja budowania
├── bin/              # Skompilowany plik wykonywalny (generowany)
└── build/            # Pliki obiektowe (generowane)
```

## Szczegóły techniczne

-   **Precyzja**: IEEE 754 precyzja poczwórna (128-bit)
-   **Format**: 1 bit znaku + 15 bitów wykładnika + 112 bitów mantysy
-   **Bias wykładnika**: 16383
-   **Obsługiwane dane wejściowe**: Liczby dziesiętne (dodatnie i ujemne)

## Licencja

Ten projekt ma cele edukacyjne.
