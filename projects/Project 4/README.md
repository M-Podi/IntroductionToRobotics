
# Tema #4 - Cronometru: Introducere în Robotică

## Obiectiv:
Crearea unui cronometru cu afișaj pe 4 cifre și 7 segmente, controlat prin 2 butoane și un joystick. 
Cronometrul trebuie să numere în zecimi de secundă și să aibă funcționalitatea de salvare a tururilor (laps).

## Descriere tehnică:
### Configurație:

<p align="center">
  <img src="../../media/Homework4CircuitView.jpg" alt="Centered Image" width="400"/>
</p>

Componentele includ un afișaj de 4 cifre a câte 7 segmente, 2 butoane, un joystick și un buzzer, alături de rezistori și fire necesare. 
Fiecare buton are funcționalități specifice:
- Butonul 1: Începere/Pauză.
- Butonul 2: Resetare (în modul pauză) și resetarea tururilor salvate (în modul vizualizare tururi).
- Joystick: Salvarea turului (în modul de numărare) și ciclarea prin ultimele tururi salvate (până la 4).

Valorile inițiale pe afișaj sunt "000.0". 

### Funcționare:

La apăsarea butonului de start, cronometrul începe să numere. În modul de numărare, apăsarea joystick-ului salvează valoarea cronometrului în memorie (până la 4 tururi). 
În modul pauză, joystick-ul nu mai funcționează. Apăsarea butonului de resetare în modul pauză resetează cronometrul la "000.0". 
Joystick-ul este folosit pentru a cicla prin timpurile tururilor salvate.

<p align="center">
  <img src="../../media/Homework4.jpg" alt="Centered Image" width="400"/>
</p>

## Rezultat așteptat:
Cronometrul ar trebui să numere precis în zecimi de secundă, să permită salvarea și vizualizarea tururilor, și să fie controlabil prin butoanele dedicate.

## Proof that it works:
Link către un videoclip demonstrativ:

https://youtu.be/UorVCmpK93o
