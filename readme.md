# Laivų mūšis
## Įvadas ir taisyklės
Laivų mūšis – 2 dalyvių (jūsų ir kompiuterio) žaidimas, kuriame kiekvienas iš žaidėjų turi savo vandeninę erdvę, kurioje turi prišvartuoti savo laivus, ir apšaudyti priešininko teritoriją tam, kad susprogdintų priešo laivus. Pirmasis žaidėjas, susprogdinęs visus priešininko laivus, laimi.
- Kiekvienas žaidėjas savo teritorijoje, kurios dydis yra 12x24 langelių, turi sustatyti laivus:

Laivo ilgis langeliais|Laivų kiekis
-|-
5|1
4|1
3|3
2|3

Nė vienas iš laivų negali „liesti“ šalia esančio laivo, tai yra, kiekvienas laivas aplink save turi turėti tuščius langelius.
- Žaidimą pradedate jūs.
- Jeigu žaidėjas nepataiko į jokį priešo laivą, žaidimą perima priešininkas. Jeigu žaidėjas pataiko, jis gauna teisę šauti dar kartą iki tol, kol nepataikys
## Techninė programos pusė
### Žaidimo valdymo klavišai

Klavišas|Paskirtis
-|-
W, A, S, D|**Pasiruošimo metu**: laivo būvimo vietos nurodymas. **Žaidimo metu**: taikymasis į priešininko lentą.
Enter|**Pasiruošimo metu**: patvirtinama laivo parkavimo vieta. **Žaidimo metu**: šūvis
Space|**Pasiruošimo metu**: keičiama laivo padėtis iš horizontalios į vertikalią ir atvirkščiai

Paspaustų klavišų fiksavimui naudojama biblioteka pavadinimu `conio.h`
## Žaidimo pradžia
1. Jūs turite pasirinkti savo laivų buvimo vietą.
2. Lentoje ╞---╡ reprezentuoja 5 langelių ilgio laivą, ╞--╡ - 4, ╞-╡ - 3, ╞╡ - 2. Jeigu laivas stovi vertikaliai:

5|4|3|2
-|-|-|-
╥|╥|╥|╥
║|║|║|╨
║|║|╨|
║|╨||
╨|||

Jeigu norima pasukti laivą, reikia spausti Space.
4. Pradedama statyti nuo trumpiausio laivo iki ilgiausio. Pasirinkus tam tikrą laivo statymo vietą, spaudžiama Enter.
## Žaidimo eiga
1. Laukiama, jūs paspausite Enter mygtuką.
2. Ekrane pasirodo lenta, kurioje:
  - tusčias langelis vaizduoja nepaliestą kvadratą,
  - ryškus langelis vaizduoja pažeistą laivą,
  - blankus langelis vaizduoja vietą, kurioje buvo šauta, bet joje nebuvo laivo,
  - \+ rodo, kur yra taikomasi
3. Enter spaudžiama, kai nusprendžiama, kur šauti. Negalima šauti ten, kur jau buvo šauta. Jeigu pataikoma, leidžiama šauti dar. Jeigu ne – keičiasi žaidėjas ir grįžtama į 1 punktą.
