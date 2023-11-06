
# Introducere în Robotică: Desen cu afișaj cu 7 segmente

## Obiectiv
Scopul acestei sarcini este de a utiliza un joystick pentru a controla poziția unui segment pe un afișaj cu 7 segmente și a "desena" prin comutarea stării segmentelor individuale. Mișcarea între segmente trebuie să fie intuitivă și permisă doar între segmente adiacente, fără a trece prin "pereți".

## Configurație
Componentele necesare includ:
- Un afișaj cu 7 segmente, cu fiecare din cele 7 segmente și punctul zecimal (DP) conectate la pinii digitali ai Arduino, specificați în codul sursă.
- Un joystick cu două axe și un buton, conectat la pinii analogici pentru citirea pozițiilor axelor și la un pin digital pentru buton.
- Buzzer conectat la un pin Arduino, folosit pentru a oferi un feedback la input-ul utilizatorului.

Fiecare segment al afișajului cu 7 segmente este controlat de un pin digital diferit pe placa Arduino, iar starea fiecărui pin (aprins sau stins) este determinată de poziția joystick-ului și de interacțiunea cu butonul. Acest lucru permite afișajului să "deseneze" prin aprinderea sau stingerea segmentelor individuale.

Poziția inițială a cursorului este la punctul zecimal (DP) al afișajului, iar poziția curentă clipește constant pentru a indica selecția activă.

## Funcționare
- Joystick-ul citit prin pinii analogici permite utilizatorului să deplaseze poziția activă între segmentele afișajului.
- Apăsarea scurtă a butonului joystick-ului, detectată de un pin digital și gestionată prin întreruperi, comută starea segmentului curent de la ON la OFF sau invers.
- Apăsarea lungă pe butonul joystick-ului va reseta afișajul, oprind toate segmentele și mutând poziția activă înapoi la DP.
- Codul include o logică de 'debounce' pentru a asigura că apăsările butonului sunt înregistrate o singură dată, evitând astfel citirile false.
- O funcție separată gestionează direcția joystick-ului, iar o altă parte a codului se ocupă de actualizarea stării segmentelor și de gestionarea clipirii segmentului activ.
- Întreruperile hardware sunt folosite pentru a îmbunătăți responsivitatea la acțiunile utilizatorului, permițând sistemului să reacționeze rapid la apăsările butonului.

## Rezultat Așteptat
La final, utilizatorul va putea controla afișajul cu 7 segmente pentru a crea diverse "desene" prin aprinderea și stingerea segmentelor. Această interacțiune va oferi o înțelegere practică a modului în care intrările utilizatorului (prin joystick) pot fi folosite pentru a controla ieșiri hardware (LED-uri pe afișajul cu 7 segmente).
