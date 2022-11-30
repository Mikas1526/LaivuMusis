# Laivų mūšis
## Įvadas ir taisyklės
Laivų mūšis – 2 dalyvių žaidimas, kuriame kiekvienas iš žaidėjų turi savo vandeninę erdvę, kurioje turi prišvartuoti savo laivus, ir apšaudyti priešininko teritoriją tam, kad susprogdintų priešo laivus. Pirmasis žaidėjas, susprogdinęs visus priešininko laivus, laimi.
- Kiekvienas žaidėjas savo teritorijoje, kurios dydis yra 12x12 langelių, turi sustatyti laivus:

Laivo ilgis langeliais|Laivų kiekis
-|-
5|1
4|1
3|3
2|3

Nė vienas iš laivų negali „liesti“ šalia esančio laivo, tai yra, kiekvienas laivas aplink save turi turėti tuščius langelius.
- Žaidimą pradeda pirmasis žaidėjas.
- Jeigu žaidėjas nepataiko į jokį priešo laivą, žaidimą perima priešininkas. Jeigu žaidėjas pataiko, jis gauna teisę šauti dar kartą iki tol, kol nepataikys
## Techninė programos pusė
### Žaidimo valdymo klavišai

Klavišas|Paskirtis
-|-
W, A, S, D|**Pasiruošimo metu**: laivo būvimo vietos nurodymas. **Žaidimo metu**: taikymasis į priešininko lentą.
Enter|**Pasiruošimo metu**: patvirtinama laivo parkavimo vieta. **Žaidimo metu**: patvirtinimas, kad pasikeitė žaidėjas (kai keičiasi žaidėjo eilė) arba patvirtinimas, kur šauti.
Space|**Pasiruošimo metu**: keičiama laivo padėtis iš horizontalios į vertikalią ir atvirkščiai

Paspaustų klavišų fiksavimui naudojama biblioteka pavadinimu „conio.h“
## Žaidimo pradžia
1. Reikia paspausti Enter mygtuką, kai ekraną matys tik pirmas žaidėjas.
2. Paspaudus, pasirodo lenta. Joje taškas (.) reprezentuoja tuščią vietą, <===> reprezentuoja 5 langelių ilgio laivą, <==> - 4, <=> - 3, <> - 2. Jeigu laivas stovi vertikaliai:

5|4|3|2
-|-|-|-
<|<|<|<
=|=|=|>
=|=|>|
=|>||
\>|||

Jeigu norima pasukti laivą, reikia spausti Space. Jeigu laivą bandoma pastumti už lentos ribų, tai laivas atsiduria kitoje pusėje visas (tai reiškia, kad negali laivo dalys būti skirtingose teritorijos vietose. Taip pat laivai negali būti arčiau vienas prie kito kaip 1 langelis.
4. Pradedama statyti nuo ilgiausio laivo iki trumpiausio. Pasirinkus tam tikrą laivo statymo vietą, spaudžiama Enter.
5. Viskas kartojama nuo 1 žingsnio antram žaidėjui.
## Žaidimo eiga
1. Laukiama, kol ekrane rašomas žaidėjas paspaus Enter mygtuką.
2. Ekrane pasirodo lenta, kurioje:
  - taškas (.) vaizduoja nepaliestą kvadratą,
  - X vaizduoja pažeistą laivą,
  - O vaizduoja vietą, kurioje buvo šauta, bet joje nebuvo laivo,
  - \+ rodo, kur yra taikomasi,
ir statusą, kuriame rašo, kiek tavo priešas nukovė tavo laivų.
3. Enter spaudžiama, kai nusprendžiama, kur šauti. Negalima šauti ten, kur jau buvo šauta. Jeigu pataikoma, leidžiama šauti dar. Jeigu ne – keičiasi žaidėjas ir grįžtama į 1 punktą.
