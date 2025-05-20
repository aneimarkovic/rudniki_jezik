# LEXER Dokumentacija

## Atributi:
- `input`:
Shrani datoteko
- `lastToken`:
Shranjen najbolj "svež" kovanec
- `row`:
št. vrstice
- `col`:
št. stolpca
- `dictionary`:
Imenik, ki hrani imena spremenljivk in njihove vrednosti

## Metode:

- `initAutomata`: 
Ustvari tabelo povezav 
- `isFiniteState`:
Preveri ali je stanje končno
- `getFiniteState`:
pridobi stanje iz polja stanj
- `peek`:
vrne naslednji char
- `read`:
pridobi vrni char
- `nextTokenImp`:
Zajame naslednji token
- `eof`:
preveri ali smo na koncu datoteke
- `nextToken`:
vrne naslednji token
- `currentToken`:
vrne trenutni token
- `printAllTerminal`:
izpiše vse terminale
- `insertVariable`:
vstavi novo spremenljivko v imenik
- `getVarValue`:
pridobi vrednost spremenljivke iz imenika
- `updateVarValue`:
posodobi vrednost spremenljivke
- `getPosition`:
vrne trenutno pozicijo v datoteki
- `changePosition`:
spremeni trenutno pozicijo v datoteki
