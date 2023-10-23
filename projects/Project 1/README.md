# Introducere în Robotică: Proiect 1

## Obiectiv:
Controlul fiecărui canal (Roșu, Verde și Albastru) al unui LED RGB folosind potențiometre individuale. Potențiometrele vor ajusta luminozitatea fiecărei culori LED, schimbând astfel culoarea generală emisă de LED-ul RGB.

## Descriere tehnică:
### Configurație:

<p align="center">
  <img src="./media/Project1.jpg" alt="Centered Image" width="400"/>
</p>

Există trei potențiometre, fiecare asociat cu una dintre culorile RGB (Roșu, Verde, Albastru). Aceste potențiometre sunt conectate la pinii Arduino A0, A1 și A2, respectiv.

Un LED RGB este folosit în configurație. Acest LED are conexiuni pentru fiecare dintre culorile sale (Roșu, Verde, Albastru) la pinii Arduino 11, 10 și 9, respectiv.

Valorile minime și maxime citite de la potențiometre au fost definite ca 15 și 1023, respectiv.

Nivelurile minime și maxime de luminozitate pentru LED sunt definite ca 0 și 255.

### Funcționare:

Potențiometrele măsoară nivelurile de tensiune, care pot fi citite de pinii analogi ai Arduino. Rotirea potențiometrului schimbă rezistența acestuia, conducând la o schimbare în tensiune, care este citită ca o valoare analogică de Arduino.

Arduino citește în mod continuu valorile de la potențiometre folosind funcția analogRead().

Aceste valori analogice, care variază de la 0 la 1023, sunt apoi mapate la gama de valori de luminozitate LED (0 la 255). De exemplu, dacă citirea de la potențiometrul roșu (conectat la A0) este la jumătatea dintre 15 și 1023, luminozitatea LED-ului roșu (conectat la pinul 11) va fi setată la jumătate din luminozitatea sa maximă.

Există o caracteristică suplimentară care se ocupă de LED-urile RGB cu anod comun. Dacă LED-ul folosit este de acest tip (commonAnode = 1), valorile de luminozitate sunt inversate. LED-urile RGB cu anod comun funcționează prin scurgerea curentului pentru culorile individuale, astfel că o valoare de 255 oprește LED-ul, iar o valoare de 0 îl face cel mai luminos.

După calculul nivelurilor de luminozitate dorite pentru fiecare culoare, Arduino setează luminozitatea folosind funcția analogWrite().

În plus, în scopuri de depanare, codul imprimă și valorile de luminozitate în monitorul serial.

### Rezultat așteptat:
Când rotiți un potențiometru, luminozitatea culorii LED asociate ar trebui să se schimbe în consecință. Ajustând cele trei potențiometre, se pot combina diferite cantități de roșu, verde și albastru pentru a produce o gamă largă de culori pe LED-ul RGB.

## Proof that it works:

https://youtu.be/3dDfjG6fO6w
