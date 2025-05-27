# Navodila uporabe: rudniki_jezik

Ta dokument vsebuje navodila za prevajanje in zagon projekta "rudniki_jezik" s pomočjo CMake. Predpostavljamo, da ste izvorno kodo projekta že pridobili (npr. preko `git clone` ali `git pull`).

## Predpogoji

Preden nadaljujete, se prepričajte, da imate na svojem sistemu nameščeno naslednje:

1.  **C++ Prevajalnik**: Prevajalnik, ki podpira C++17 (npr. GCC, Clang, MSVC).
2.  **CMake**: Minimalna priporočena različica je 3.10.
3.  **Internetna povezava**: Morda potrebna, če vaš sistem potrebuje posodobitev upraviteljev paketov za namestitev C++ prevajalnika ali CMake.

## Postopek prevajanja in zagona s CMake

1.  **Odprite terminal ali ukazno vrstico.**

2.  **Pomaknite se v korensko mapo projekta** (mapa, ki vsebuje datoteko `CMakeLists.txt`):
    ```bash
    cd pot/do/projekta/rudniki_jezik
    ```

3.  **Konfigurirajte projekt s CMake.** Ta korak bo ustvaril gradbene datoteke v podmapi `build`.
    ```bash
    cmake -S . -B build
    ```

4.  **Prevedite projekt.** Ta ukaz bo zagnal dejanski proces prevajanja.
    ```bash
    cmake --build build
    ```
    *   Če želite zgraditi specifično konfiguracijo (npr. Debug ali Release), lahko dodate zastavico `--config`:
        *   Za Debug konfiguracijo: `cmake --build build --config Debug`
        *   Za Release konfiguracijo: `cmake --build build --config Release`
        (To je bolj relevantno za večkonfiguracijske generatorje, kot je Visual Studio. Pri enokonfiguracijskih generatorjih, kot so Makefiles, se konfiguracija običajno nastavi med korakom `cmake -S . -B build` z `-DCMAKE_BUILD_TYPE=Debug` ali `Release`.)

5.  **Zagon glavnega programa.**
    Izvedljiva datoteka `rudniki_jezik` (ali `rudniki_jezik.exe` na Windows) bo ustvarjena neposredno v mapi `build` (ali v podmapi, kot je `build/Debug` ali `build/Release`, če ste eksplicitno uporabili `--config` pri gradnji z večkonfiguracijskim generatorjem).

    *   Na Linux/macOS (iz mape `build`):
        ```bash
        ./rudniki_jezik
        ```
        (Če je v podmapi Debug: `./Debug/rudniki_jezik`)

    *   Na Windows (iz mape `build`):
        ```powershell
        .\rudniki_jezik.exe
        ```
        (Če je v podmapi Debug: `.\Debug\rudniki_jezik.exe`)