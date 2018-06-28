# blackjack

Simple three-deck blackjack simulator I wrote on a plane to Vegas in 2005.

## Quickstart

Compile:
```
$ make
```

Run:
```
$ ./blackjack
```

Use `CTRL-C` to exit the game.

## Known Bugs

1. When you exhaust the deck, the dealer will begin to draw `(null)` cards.
2. The random number generator used to shuffle the deck via heapsort is not seeded. On macOS, this leads to the same card draw sequence on every execution.
