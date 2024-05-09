# Projekt - Programowanie systemowe i współbieżne

## 1. Zasady ogólne
Celem projektu jest implementacja systemu przekazywania (rozgłaszania) wiadomości do wszystkich procesów, które zasubskrybowały dany typ wiadomości (zarejestrowały się na ich odbór). Tworzona aplikacja powinna wykorzystywać mechanizm kolejek komunikatów. W ramach realizacji projektu konieczne będzie opracowanie protokołu komunikacyjnego i 2 programów: klienta i serwera. Każdy klient może wysyłać i otrzymywać wiadomości do/od pozostałych użytkowników systemu. W wymianie wiadomości pomiędzy klientami zawsze pośredniczy serwer (bezpośrednia komunikacja pomiędzy klientami jest zabroniona). Otrzymywane wiadomości wyświetlane są na ekranie.


## 2. Funkcjonalność szczegółowa

1. Logowanie:

    • Proces loguje się do systemu rozgłaszania podając swój identyfikator i nazwę.
   
    • Rezultat: dopisanie przez serwer identyfikatora procesu i jego nazwy do bazy użytkowników.


2. Rejestracja obiorcy:
   
    • Proces rejestruje się w systemie rozgłaszania podając swoją nazwę, typ wiadomości, które chciałby otrzymywać w przyszłości oraz sposób ich odbioru (subskrypcja trwała lub przejściowa - w subskrypcji trwałej proces 
odczytuje wiadomość danego typu, kiedy tylko pojawi się ona w systemie; w subskrypcji przejściowej proces podaje liczbę wiadomości danego typu, a po otrzymaniu zadanej liczby serwer przestaje wysyłać wiadomości tego typu do klienta).

    • Rezultat: dopisanie przez serwer nazwy procesu wraz z inymi informacjami do bazy odbiorców danego typu wiadomości w celu wykorzystania w dalszej komunikacji.


3. Rejestracja typu wiadomości (tematu):

    • Proces przekazuje informacje o nowym typie wiadomości lub żąda utworzenia nowego (unikalnego) typu wiadomości przez system.

    • Rezultat: przekazanie przez serwer informacji o nowym typie wiadomości do zalogowanych uzytkowników lub wysłanie informacji o błędzie, gdy taki typ już istnieje do procesu rejestrującego nowy typ wiadomości.


4. Rozgłosznie nowej wiadomości

    • Proces wysyła treść rozgłaszanej wiadomości wraz z jej typem i priorytetem.

    • Rezultat: odebranie wiadomości przez serwer i przekazanie jej odpowiednim subskrybentom.


5. Odbór wiadomości w sposób synchroniczny:
   
    • Proces odbiera przeznaczoną dla niego wiadomość (na którą wcześniej się zarejestrował) poprzez wywołanie odpowiedniej funkcji, przy czym wywołanie to może być blokujące, jeśli wiadomość nie jest jeszcze dostępna.

    • Możliwość zablokowania odbioru wiadomości (od użytkownika)

    • Rezultat: Odebranie wiadomości o ile nie pochodzi od zablokowanego użytkownika (wyświetlenie jej na ekranie).


6. Odbór wiadomości w sposób asynchroniczny
   
    • Proces rejestruje funkcję, która zostanie przez system wywołana gdy pojawi się wiadomość w subskrybowanym temacie.

    • Rezultat: Odebranie wiadomości (wyświetlenie jej na ekranie).
