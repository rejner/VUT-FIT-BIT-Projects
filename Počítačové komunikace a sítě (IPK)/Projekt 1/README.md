# IPK - Projekt 1
# HTTP resolver doménových jmen

Cílem projektu je implementace severu, který bude komunikovat protokolem HTTP a bude zajišťovat překlad doménových jmen. Pro překlad jmen bude server používat lokální resolver stanice na které běží - užijte přímo API, které poskytuje OS (například getnameinfo, getaddrinfo pro C/C++). 

Autor: Michal Rein (xreinm00)

Řešení je implementováno v programovací jazyce Python3, především prostřednictvím knihovny 'socket', která je vestavěnou knihovnou pro programování síťových socketů.

### Použité knihovny:
 - socket
 - sys
 - re
 - urllib (modul parse)
 - time

## Spuštění

V adresáři projektu je připravený soubor 'makefile', pomocí kterého lze spustit server příkazem:
```sh
$ make run PORT=$port
```
Kde za $port dosaďte číslo portu, na kterém má server běžet. V případě vynechání proměnné $port bude za číslo portu dosazeno 8080 jako výchozí. Server je dostupný na adrese: "localhost:$port". 


## Implementace

Zdrojový kód se serverem je dostupný v podadresáři /src jako server.py.

Pro činnost serveru byly implementovány tyto funkce:

 - main()
 - create_socket(argv)
 - bind_socket()
 - socket_accept()
 - process_request(client_socket)
 - do_GET(client_socket, data)
 - do_POST(client_socket, data)
 - send_response(client_socket, header, data, name, op_type, ip, post_method)

Program tyto funkce volá chronologicky, přičemž smyčka serveru je tvořena ve funkci socket_accept().
 
### main()

Výchozí funkce volaná při spuštění programu, má na starosti volání funkcí pro vytvoření server socketu a předání vyžadovaného čísla portu funkci create_socket().

### create_socket(argv)

Má na starosti vytvoření nového socketu pro server. V parametru argv se nachází číslo portu, případně výchozí hodnota. Jsou zde vytvořeny globální proměnné 's', 'host' a 'port' (server socket, adresa serveru, port serveru).

### bind_socket()

Navázání socketu na požadovanou adresu a čekání na první příchozí komunikaci.

### socket_accept()

Dochází k přijetí spojení a zároveň vytvoření běhové smyčky pro server. Po vykonání požadavku se program vrací zpět do této funkce a navazuje další spojení s klienty.

Po přijetí socketu klienta je volána funkce process_request(client_socket).

### process_request()

Po úspěšném navázání spojení jsou v této funkci přijata data z klientovy strany. Celý proces je realizován pomocí timeoutu, přičemž proměnná timeout je nastavena na hodnotu 2. Data jsou přijímána po balících o velikosti 1024 bytů, každá část dat je dekódována a uložena do jedné společné proměnné. Přijímání dat je ukončeno, jestliže byla přijata některá data a již uběhl čas, po který jsou očekávána data na vstupu, specifikovaný proměnnou 'timeout'. Jestliže nebyla celkově přijata žádná data, doba čekání je prodloužena maximálně na dvojnásobek doby daný proměnnou 'timeout'.

Po úspěšném přijetí všech balíků dat je rozpoznán požadavek a dle situace volány funkce do_GET() nebo do_POST().

### do_GET(client_socket, url)

Funkce pro vykonání požadavku 'GET'.

Data ve formě url jsou zpracována a následně odeslána funkcí send_response()

### do_POST(client_socket, data)

Funkce pro vykonání požadavku 'POST'.

Nejdříve se identifikuje a přeskočí hlavička požadavku, následně jsou zpracovávána data, přičemž je očekáván správný formát daný zadáním projektu. Jsou tolerovány libovolné mezery a tabulátory v jednotlivých adresách, včetně prázdných řádků. Adresy, které nebyly nalezeny, nebo nesplňovaly požadovaný formát jsou pouze vynechány (nedojde hned k odeslání chybné odpovědi) a data jsou prohledávána dále.

### send_response(client_socket, header, data, name, op_type, ip, post_method)

Funkce slouží k vyhodnocení výsledků funkcí do_GET() a do_POST().

Dochází zde k odeslání odpovědi klientovi, následně se program vrací do smyčky vytvořené funkcí socket_accept() a čeká na další připojení a požadavek klienta.
 
