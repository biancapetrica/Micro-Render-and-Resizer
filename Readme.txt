================================ Tema 1 APD ====================================
=========================== Petrica Bianca 333CB ===============================

Partea 1.
	Pentru a putea lucra cu imagini de tip PNM/PGM am creat structura 
typedef struct {
	char type[3];
	int width;
	int height;
	int maxVal;
	void** data; -- poate fi de tipul unsigned char** pentru PGM
					sau pixel** pentru PNM
} image;

	Pentru a redimensiona imaginea data, folosind tehnica SSAA si thread-uri, am 
realizat o copie globala a imaginii de input pe care am citit-o din fisier, 
pentru ca thread-urile sa aiba access la campul data, respectiv la matricea de 
pixeli.  
	
	Dorind ca noua imagine sa fie de resize_factor ori mai mica decat cea 
originala, iar fiecare pixel din imaginea finala este reprezentat de o formula 
aplicata pe pixelii unui patrat cu latura de resize factor din poza initiala, 
am parcurs matricea pozei finale pixel cu pixel. Valoarea fiecarui pixel am 
calculat-o parcurgand patratul corespunzator din matricea pozei originale si 
realizand media aritmetica a resizefactor^2 pixel (pentru resizefactor par) sau 
inmultind submatricea cu valorile Kernel-ului Gaussian, elemenent cu element, 
apoi impartind rezultatul la 16 (resizefactor = 3).
	In cazul pozei color, am realizat aceste operatii pe fiecare pixel (red, 
green, blue). 

	Pentru a obtine paralelizarea codului, am paralelizat for-ul principal care 
parcurge inaltimea imaginii finale. For-ul pe latime nu poate fi paralelizate 
deoarece pasii ar trebui executati in ordine, iar urmatoarele 2 for-uri executa 
doar resizefactor^2 pasi.
	Cum calcularea fiecarui pixel din noua imagine nu depinde decat de patratul 
corespunzator cu latura de resize_factor pixeli din imaginea originala, iar 
toate astfel de patrate  sunt diferite, calcularea noii imagini poate fi 
excutata pe mai multe threaduri.  
	Am creat 2 functii threadFunction care sa calculeze matricea noii poze in 
functie de formatul acesteia (color sau alb-negru) si in functie de resizefactor.

Partea 2.
	Am creat o imagine de tip PGM cu structura 
typedef struct {
	char type[3];
	int width;
	int height;
	int maxVal;
	unsigned char** data;
} image;
	
	Pentru a creea o imagine alba cu o linie a carei ecuatie este -x +2y + 0 = 0, 
de grosime 3 cm, intr-un spatiu patratic de 100cm x 100cm, am calculat distanta 
centrului fiecarui pixel din poza la ecuatia dreptei si am scalat aceasta 
distanta in functie de rezolutie.
	Pentru pixelul din pozitia (i,j) am folosit formula 
|a*(j+0.5)*100/resolution + b*(resolution-(i+0,5))*100/resolution| / sqrt(a^2+b^2). 
	Daca aceasta distanta e mai mica decat 3 cm, atunci pixelul este negru, 
altfel acesta esta alb.
 
	Pentru a obtine paralelizarea codului, am paralelizat for-ul principal care 
parcurge inaltimea imaginii. For-ul pe latime nu poate fi paralelizat 
deoarece pasii ar trebui executati in ordine.
	
	Pentru implementarea thread-urilor prin API-ul oferit de libraria pthread am
urmarit structura data la laborator.

Scalabilitate si timpi de executie
	La rularea pe clusterul facultatii, am obtinut urmatorii timpi de executie (am folosit pozele lenna_bw.pgm si lenna_color.pnm)
Color 1 thread : 
	RF: 2 - 0.006671s	3 - 0.012436s	8 - 0.002586s 
Color 2 thread-uri: 
	RF: 2 - 0.003434s	3 - 0.006319s	8 - 0.002388s 
Color 4 thread-uri:
	RF: 2 - 0.001873s	3 - 0.003366s	8 - 0.001352s 
Color 8 thread-uri: 
	RF: 2 - 0.001167s	3 - 0.001844s	8 - 0.000802s
 
Grayscale 1 thread : 
	RF: 2 - 0.002392s	3 - 0.008129s	8 - 0.002356s 
Grayscale 2 thread-uri: 
	RF: 2 - 0.002079s	3 - 0.004542s	8 - 0.001245s 
Grayscale 4 thread-uri:
	RF: 2 - 0.001049s	3 - 0.002078s	8 - 0.000721s 
Grayscale 8 thread-uri: 
	RF: 2 - 0.000688s	3 - 0.001157s	8 - 0.000539s 

Render 1 thread :
	Resolution: 1000 - 0.040397s	8000 - 1.432641s
Render 2 thread-uri:
	Resolution: 1000 - 0.020018s	8000 - 0.699649s
Render 4 thread-uri:
	Resolution: 1000 - 0.009111s	8000 - 0.354037s
Render 8 thread-uri: 
	Resolution: 1000 - 0.004772s	8000 - 0.188238s

