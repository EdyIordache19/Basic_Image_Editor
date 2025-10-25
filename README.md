
> Iordache Eduard-Stefanut, 314CA

# README Tema 3

Acesta este README-ul pentru *Tema 3*. Contine explicatii, clarificari si procesul prin care am 
rezolvateu problema propusa pentru *Tema 3*.

## Problema Image Editor
Tema acestei probleme este aceea de a edita fotografii in formaturile P2, P3, P5 si P6, 
cu ajutorul alocarilor dinamice de memorie si al lucrului cu fisierele. Astfel, implementarea
mea se bazeaza pe aceste tehnici, incercand sa fie cat mai optima posibil. Pentru a incarca
imaginea am folosit o structura, `image_t` care are ca atribute header-ul imaginii (width, height,
type), precum si valorile pixelilor, retinute sub forma unei matrice alocata dinamic, `data`. 
Initial, aceasta matrice are tipul `void **`, deoarece in functie de formatul imaginii, ea poate fi
alcatuita ori din valori de la 0 la 255 ale pixelilor, in cazul imaginilor grayscale (P2, P5), ori din triplete
de forma RGB, cu valori tot de la 0 la 255, in cazul imaginilor color (P3, P6). Aceasta matrice este apoi
transformata prin conversii de tip in una `unsigned char **` (grayscale), sau `pixel_t **` (color).
`pixel_t` este alta structura, care doar retine valorile R, G, B ale fiecarui pixel, tot in tipul de date
`unsigned char`. Tipul `unsigned char` este folosit pe tot parcursul rezolvarii pentru a optimiza
memoria folosita, un char ocupand 1byte, in timp ce un int ocupa 4bytes. 
In functia `main()` se apeleaza doar functia `read_command()`, care citeste fiecare comanda de
la tastatura. Ea citeste rand cu rand tot ce se trimite programului, iar fiecare rand il
interpreteaza pana ajunge la comanda "EXIT". Fiecare comanda se proceseaza in functia 
`process_command()`, care preia comenzile cu `sscanf`, le verifica parametrii si se asigura ca acestia
corespund enuntului si apeleaza functiile aferente fiecarei comanda.
Exista 11 tipuri de comenzi, si anume:
<ul>
	<li>LOAD</li>
	<li>SELECT</li>
	<li>SELECT ALL</li>
	<li>ROTATE</li>
	<li>EQUALIZE</li>
	<li>CROP</li>
	<li>APPLY</li>
	<li>SAVE</li>
	<li>HISTOGRAM</li>
	<li>EXIT</li>
</ul>

### I. Functiile `load()` si `save()`
Aceste functii sunt singurele care pot lucra in mod direct cu fisiere din afara programului. Asadar,
in functia `load()`se aceseaza un nou fisier, se citeste header-ul imaginii in mod ascii, iar apoi,
alocandu-se dinamic matricea `img->data`, se citesc valorile. In functie de formatul imaginii, valorile
sunt citite fie in mod text, fie in mod binar, iar data are fie tipul `unsigned char **`, fie tipul `pixel_t **`
Functia `save()` salveaza imaginea incarcata curent in fisierul al carui nume este transmis de la tastatura.
In modul default, se salveaza intr-un fisier binar, cu functia `save_bin()`, insa daca este prezent si parametrul `ascii` dupa numele fisierului acesta devine fisier "plain text", si se foloseste functia `save_ascii()`.

### II. Functiile `alloc_data()` si `free_data()`
Cu ajutorul celor doua functii se aloca sau se elibereaza memorie pentru matricea `img->data`. Ca parametru este transmisa doar imaginea curenta, de tipul `image_t`, care contine toate informatiile necesare pentru
alocarea, respectiv eliberarea memoriei (width, height pentru dimensiuni; type pentru tipul de date al matricei).

### III. Functia de selectare a unei zone
Cu ajutorul acestei functii se selecteaza o anumita zona a imaginii, de la coordonatele x1, y1, la coordonatele
x2, y2. In cazul in care programul primeste comanda SELECT ALL, `x1 = y1 = 0`, iar `x2 = img->width;
y2 = img->height`. De asemenea, exista si variabila de natura booleana `selected`, care verifica daca
exista o selectie curenta valida pe imaginea actuala sau nu.

### IV. Functiile de editare ale imaginii
Totate functiile sunt construite asemanator, dupa instructiunile prevazute in enuntul problemei. Pentru
fiecare functie se genereaza o noua imagine, `new_img`, tot de tip `image_t`, care ii preia headerul
imaginii initiale. In functie de fiecare comanda, se aplica modificari pentru `new_img`, intr-un final urmand
ca imaginea initiala sa ii copieze valorile pixelilor imaginii noua.
Avem mai multe functii pentru editare:
<ol>
<li> <strong>Functia apply()</strong> - Aceasta aplica anumite nuclee (kernel) pe selectii din matricea 
<code>img->data</code>, mai exact face anumite convolutii cu ele. Nucleele sunt niste matrici 3*3, alocate si ele dinamic, care se aloca, se elibereaza si se genereaza cu ajutorul functiilor  <code>alloc_kernel()</code>, <code>free_kernel()</code> si <code>generate_kernel()</code>. Ca in enunt fiecare pixel al imaginii se inmulteste cu un pixel din kernel, aplicandu-se astfel un filtru, imaginea finala fiind putin diferita. Filtrele posibile sunt: <i>SHARPEN</i>, <i>EDGE</i>, <i>BLUR</i> si <i>GAUSSIAN_BLUR</i>. <u>Poate fi folosita doar pe imagini color</u> </li>
<li>  <strong>Functia rotate()</strong> - Aceasta are doua optiuni: se poate roti o anumita selectie din 
imagine, doar daca aceasta este patrata, acest lucru fiind verificat cu functia <code>is_square()</code>, sau 
poate roti toata imaginea, caz in care nu trebuie sa fie neaparat patrata. Astfel, se apeleaza una din cele doua 
functii, <code>rotate_all()</code>, sau <code>rotate_selection()</code>, pe baza variabilei <code>selected
</code>.</li>
<li> <strong>Functia equalize()</strong> - Ea egalizeaza anumite zone ale unei imagini, cu ajutorul unei
histograme generate de aceasta. Ia numarul de aparitii ale fiecarui pixel din imagine si netezeste curba 
histogramei imaginii. Astfel, se face suma aparitiilor pixelilor pana la pixelul <code>i</code>, iar suma se 
inmulteste cu 255, valoarea maxima a unui pixel si se imparte la numarul total de pixeli 
(aria imaginii = width*height). <u>Poate fi folosita doar pe imagini grayscale</u> </li>
<li> <strong>Functia crop()</strong> - Aceasta taie o selectie dintr-o poza, ramanand astfel dupa
executarea comenzii doar selectia facuta. Mai apoi pe aceasta se pot efectua restul de operatii, sau salva asa
intr-o dimensiune mai mica sau egala cu cea initiala.</li>
</ol>

### V. Functia `histogram()`
Functia histogram realizeaza, conform enuntului, o diagrama cu numarul aparitiilor unei anumite valori ale
pixelilor, raportate la un numar maxim de aparitii. Astfel, pot fi maxim `max_stars` aparitii, iar acestea sunt calculate pe `bins` intervale, adica un interval de aparitii are dimensiunea `256 / bins`. Pentru fiecare interval se calculeaza numarul total al aparitiilor si se face aproximarea in functie  de `max_stars`. Apoi, 
se afiseaza conform formatului din enunt, cu ajutorul unor stelute.

