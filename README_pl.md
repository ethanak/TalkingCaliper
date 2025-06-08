# TalkingCaliper

Mówiąca w wielu językach przystawka do suwmiarki.

**UWAGA - plik nie jest ukończony!**

## Krótki opis

![widok ogólny](/images/vfront.jpg)

Krótki film: [YouTube video](https://youtu.be/fLb1sWjNS7g)

Urządzenie stanowi przystawkę do popularnej suwmiarki Vorel, może być również stosowane we wszystkich urządzeniach pomiarowych tej firmy posiadających kompatybilny interfejs (głębokościomierz do bieżnika opon, czujnik zegarowy itd). W przypadku suwmiarki jest montowane na tylnej ścianie obudowy w sposób, który nie przeszkadza w jej normalnym użytkowaniu.

Przystawka umożliwia wygodne posługiwanie się suwmiarką;

* przez osoby niewidome i słabo widzące
* w złych warunkach oświetleniowych
* w sytuacjach, kiedy obserwowanie wyświetlacza jest utrudnione lub niemożliwe

W przeciwieństwie do poprzednich konstrukcji wykorzystujących pełny mechanizm TTS (a tym samym ograniczonych do jednego konkretnego języka) tu został użyty syntezator korpusowy. Z uwagi na stosunkowo niewielką ilość komunikatów, jest możliwe ich wstępne nagranie w praktycznie dowolnym języku. Kod programu umożliwia wybór (w trakcie kompilacji) jednego z czterech języków:

* angielski (RHVoice, dwa głosy bdl i slt);
* polski (RHVoice, dwa głosy magda i natan)
* hiszpański (RHVoice, jeden głos mateo)
* niemiecki (Piper-TTS, jeden głos thorsten)

Możliwe jest również wykorzystanie telefonu z systemem Android i użycie wbudowanego w telefon systemu TTS, ale nie jest to zbyt wygodne (szczególnie dla pierwszej grupy użytkowników)

Przystawka może pracować w następujących trybach:

* odczyt ciągły - podawane są aktualne wyniki pomiaru z sekundową przerwą
* odczyt zmian - podawane są wyłącznie wyniki pomiaró jeśli wartość się zmieniła lub zostało wymuszone odczytanie.
* odczyt na żądanie - po naciśnięciu klawisza.

Jednostki (milimetry lub cale) zależą od ustawień samej suwmiarki. Każdy pierwszy odczyt zakończony jest podaniem jednostki, kolejne to same liczby. Ponieważ wynik w milimetrach zawiera zawsze dwie cyfry dziesiętne,  w calach trzy, odczyt jest zawsze jednoznaczny. Dodatkowo jest możliwość ustawienia możliwości czytania w tysięcznych częściach cala dla wyników poniżej jednego cala (jednostką jest wtedy mil), w celu skrócenia komunikatu głosowego bez utraty jego zrozumiałości.


## Obsługa

Przystawka posiada tylko jeden przycisk. Możliwości są następujące

* pojedyncze kliknięcie - w trybie odczytu zmian lub na żądanie odczytuje bieżący wynik;
* przytrzymanie - zmienia tryb czytania;
* podwójne kliknięcie - odczytuje stan akumulatora;
* kliknięcie plus przytrzymanie - wejście w tryb ustawień.

W trybie ustawień:

* pojedyncze kliknięcie - zmiana wartości
* przytrzymanie - wyjściw z trybu ustawien bez zapisania i powrót do poprzednich ustawień
* podwójne kliknięcie - zmiana parametru
* kliknięcie plus przytrzymanie - zapis.

Możliwe jest ustawienie następujących parametrów:

* prędkość mowy (siedem możliwości, od absurdalnie wolnej do bardzo szybkiej);
* głośność (trzy możliwości);
* głos (tylko jeśli jest więcej niż jeden);
* sposób czytania małych wartości;
* rozdzielenie cyfr przy czytaniu.


Dodatkowo po podłączeniu ładowania uruchamiany jest tryb ładowania, gdzie urządzenie nie mówi nic. Można z niego wyjść przytrzymując klawisz.

## Program

Program na chwilę obecną jest przystosowany wyłącznie do płytki XIAO ESP32S3

### Wymagania:

* Arduino IDE z zainstalowanym ESP32 Board w wersji co najmniej 3.1
* Zainstalowana biblioteka ESP8266Audio

### Ustawienia IDE:

- Wybrana płytka: XIAO ESP32-S3
- CPU Frequency: 80 MHz
- Partition Scheme: Maximum APP (7.9 MB APP, No OTA/No FS) 

### Ustawienia programu:

W pliku common.h:

```c
//Odkomentuj jeśli nie masz/nie potrzebujesz aplikacji BlueTooth
//#DISABLE_BLE

//Odkomentuj jeśli nie potrzebujesz kontroli ładowania
//#DISABLE_CHARGER
```

W pliku i18n.h:

```c

//Odkomentuj dla języka polskiego
//#define VOICE_IS_PL
```

Wszystkie inne definicje VOICE_IS powinny być zakomentowane.

Dla innych języków:

* dla angielskiego zakomentować wszystkie definicje VOICE_IS
* dla pozostałych odkomentować wyłącznie właściwą definicję.


## Elektronika

### Potrzebne elementy

* XIAO ESP32-S3
* Moduł MAX98357A
* 2 x dioda Schottky (min. 0.5A)
* 2 x dioda prostownicza (najmniejsza, parametry dowolne) do zasilania suwmiarki
* przycisk tact switch
* wyłącznik
* rezystory, kondensatory, elementy montażowe wg. schematu

### Wersja testowa

![schemat wersji uproszczonej](/images/testsch.png)

Ta wersja służy wyłącznie do przetestowania współpracy programu z suwmiarką. Wymaga jedynie trzech dodatkowych elementów poza główną płytką Xiao i wzmacniaczem (dwa kondensatory i przycisk), ale może być użyta zarówno do oceny przydaności programu, jak i pracy przy wprowadzeniu nowego głosu czy języka. Układ może być zmontowany na małej (170 otworów) płytce stykowej.

![przykładowe rozmieszczenie elementów na płytce stykowej](/images/miniboard.png)

W tej wersji suwmiarka musi być zasilana z wewnętrznej baterii. Przy kompilacji należy w pliku common.h odkomentować

```c
#DISABLE_CHARGER
```

aby program nie próbował sprawdzać podłączenia ładowarki do nieistniejącego akumulatora.

Ponieważ akumulatora nie ma, polecenie sprawdzenia stanu baterii będzie generowało błędne odczyty, ale reszta będzie działać.

### Wersja bazowa

![schemat wersji pełnej](/images/fullsch.png)

Najprostsza realnie możliwa do wykonania działającego urządzenia. Różni się od poprzedniej tym, że zasilanie wzmacniacza pobierane jest albo bezpośrednio z akumulatora, albo (w przypadku podłączenia USB) z wejścia Vin. Dodatkowo jest już podłączony  miernik stanu baterii.

Podobnie jak poprzednio, wykrywanie podłączenia ładowarki musi być wyłączone.

Dodatkowe elementy: dwie diody Schottky, dwa rezystory, opcjonalny kondensator.

### Zasilanie suwmiarki z akumulatora przystawki

Jeśli przystawka nie jest na stałe przymocowana do suwmiarki, nie należy tego stosować i pozostawić zasilanie suwmiarki z wewnętrznej baterii. W przypadku stałego połączenia należy to rozważyć: z reguły czas potrzebny do podładowania akumulatora jest o co najmniej rząd wielkości niższy, niż czas potrzebny do znalezienia sklepu z bateriami i kupienia jednej sztuki. A wiadomo, że baterie rozładowują się w najmniej oczekiwanym momencie... Poza tym układ (łącznie z suwmiarką) może być wtedy zasilany jakiejkolwiek ładowarki (jeśli akumulator jest rozładowany).

Koszt: dwie diody prostownicze, dwa rezystory, podłączenie wszystkich czterech przewodów do wtyczki suwmiarki. Dodatkowo ok. 1 mA poboru prądu z akumulatora.

Należy umiemożliwić włożenie baterii do suwmiarki!


### Czujnik ładowania

Pozwala na automatyczne przełączanie urządzenia w tryb cichy (w czasie ładowania nie jest wskazane, aby urządzenie coś gadało).

Koszt: dwa rezystory.

Konieczne zakomentowanie

```c
#DISABLE_CHARGER
```

 w pliku common.h

 ### Uwagi
 
 Do wykonania kalibracji czujnika stanu akumulatora konieczny jest dostęp do gnd i pada +bat. O ile przed zamontowaniem przystawki jest to trywialne (+bat jest dostępny na wyprowadzeniach wyłącznika, gnd w kilku miejscach) - o tyle po zamontowaniu nie ma już takiej możliwości. Należy o tym pamiętać i dokonać kalibracji przed zamontowaniem przystawki na stałe.
 
 
## Konstrukcja mechaniczna

### Wtyczka do suwmiarki

Powinna być wydrukowana z TPU [plik STL](/connector/mini_connector.jpg),
najlepiej z warstwą 0.1 mm. Sposób zamocowania przewodu pokazują zdjęcia w folderze [Connector](/connector/).

### Obudowa

Z uwagi na możliwość użycia różnych elementów i konfiguracji nie ma możliwości zamieszczenia plików STL. Załączony plik [OpenSCAD](/caliper.scad) może stanowić bazę do zaprojetowania obudowy

*dokończenie wkrótce*

