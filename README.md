# Implementacija metoda tabloa za iskaznu logiku

Program za ispitivanje **tautologičnosti** i **zadovoljivosti** iskaznih formula
metodom analitičkih tabloa. Za formulu koja nije tautologija vraća kontramodel,
za zadovoljivu formulu model, a opciono iscrtava i konstruisani tablo.

Seminarski rad na predmetu Automatsko rezonovanje (MATF). Detaljan opis je u
`rad/seminarski.pdf`.

## Zahtevi

- Prevodilac sa podrškom za C++17 (npr. `g++` verzije 7 ili novije)
- `GNU make`
- (samo za prevođenje rada) `LaTeX` sa paketima `tikz` i `microtype`

## Prevođenje

```bash
make            # prevodi u build/tablo
```

## Pokretanje

```bash
./build/tablo valid "<formula>" [--tree]   # provera tautologije
./build/tablo sat   "<formula>" [--tree]   # provera zadovoljivosti
./build/tablo                              # ugradjeni demo primeri
```

- režim `valid` ispituje tautologičnost,
- režim `sat` ispituje zadovoljivost,
- opcija `--tree` dodatno iscrtava konstruisani tablo.

Prečica preko Make-a (bez argumenata pokreće demo):

```bash
make run                                   # demo primeri
```

Sintaksa formula:

- veznici: `~` (negacija), `&`, `|`, `->`, `<->`
- konstante: `true`, `false`
- atomi: identifikatori poput `p`, `q`, `r`, ...

## Primeri

```
$ ./build/tablo valid "(p -> q) -> (~q -> ~p)"
(p -> q) -> (~q -> ~p)  =>  TAUTOLOGIJA

$ ./build/tablo valid "p -> q"
p -> q  =>  NIJE TAUTOLOGIJA, kontramodel {p=1, q=0}

$ ./build/tablo sat "(p | q) & (~p | r)"
(p | q) & (~p | r)  =>  ZADOVOLJIVA, model {p=1, r=1}

$ ./build/tablo sat "p -> q" --tree
p -> q  =>  ZADOVOLJIVA, model {p=0}

T (p -> q)
|-- F p
|   o (otvorena) {p=0}
`-- T q
    o (otvorena) {q=1}
```

## Čišćenje

```bash
make clean      # briše build/ i LaTeX medjufajlove
```
