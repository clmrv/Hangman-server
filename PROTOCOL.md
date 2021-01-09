# Protokół do wiadomości

## K > S `login`
- 0 / 2 bajty - id gracza

## S > K `loggedIn`
- 2 bajty - id gracza

## K > S `setName` 
- do 65k bajtów - nick

## K > S `joinRoom`
- 6 bajtów - 000001

## S > K `error`
- 1 bajt
    - roomFull
    - roomNotFound
    - newHostNotFound
    - kickPlayerNotFound

## K > S `createRoom`
- 2 bajty - język
- 1 bajt - długość słowa
- 2 bajt - czas gry
- 1 bajt - ilość punktów gry
- 1 bajt - liczba graczy

## S > K `roomSettings`
- 1 bajt - l - ilość języków
- 2 * l bajtów - identyfikatory języków
- 1 bajt - min. długość słowa
- 1 bajt - max. długość słowa
- 2 bajt - min. czas gry
- 2 bajt - max. czas gry
- 1 bajt - min. ilość punktów zdrowia
- 1 bajt - max. ilość punktów zdrowia
- 1 bajt - min. liczba graczy
- 1 bajt - max. liczba graczy

## S > K `roomStatus`
- 2 bajty - język
- 1 bajt - długość słowa
- 2 bajty - czas gry
- 1 bajt - ilość punktów gry
- 6 bajtów - identyfikator pokoju
- 1 bajt - max. liczba graczy
- 1 bajt - p - liczba graczy
- p razy:
    - 2 bajty - id gracza
    - 1 bajt - długość
    - długość * bajt - nazwa 
- 2 bajty - id hosta

## K > S `setNewHost`
- 2 bajty id gracza

## K > S `kickPlayer`
- 2 bajty id gracza

## S > K `kicked`
- nic

## K > S `leaveRoom`
- nic

## K > S `startGame`
- nic

## S > K `gameStatus`
- 8 bajtów - moment końca gry (czas uniksowy, epoch time, unix timestamp)
- 1 bajt - liczba graczy p
- p razy:
    - 2 bajty - id gracza
    - 1 bajt - długość
    - długość * bajt - nazwa 
    - 2 bajty - punkty
    - 1 bajt - pozostałe życie
    - 1 bajt - czy odgadnął już hasło
- 1 bajt - długość słowa w (ilość liter - każda litera w UTF-32 ma 4 bajty)
- 4 * w bajtów - odgadnięte litery w UTF-32 (string, \0 - puste miejsce)

Przykład:
```
0x00000077 0x00000000 0x00000000 0x00000000
w _ _ _

0x00000077 0x00000105 0x00000000 0x00000000
w ą _ _

0x00000077 0x00000105 0x0000017c 0x00000079
wąży
```

## K > S `guessWord`
- słowo w UTF-32

## K > S `guessLetter`
- 4 bajty - litera w UTF-32

## S > K `scoreBoard`
- 1 bajt - liczba graczy p
- p razy:
    - 2 bajty - id gracza
    - 1 bajt - długość
    - długość * bajt - nazwa 
    - 2 bajt - punkty
    - 1 bajt - czy odgadnął hasło
- 1 bajt - długość słowa w (ilość liter - każda litera w UTF-32 ma 4 bajty)
- 4 * w bajtów - litery słowa w UTF-32
