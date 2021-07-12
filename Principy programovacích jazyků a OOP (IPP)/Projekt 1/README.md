# Implementační dokumentace k 1. úloze do IPP 2019/2020
Jméno a příjmení: Michal Rein (xreinm00)

Program provádí lexikální a syntaktickou analýzu jazyka IPPcode20.

## Spuštění:

```sh
$ php7.4 parse.php [OPTIONS] < IPPcode20
```
Pro více informací o parametrech spusťte skript s parametrem --help.

## Implementace

Program je sestaven z následujících tříd:
 - Controller (parse_classes/controller.php)
 - ArgumentManager_parse (parse_classes/arguments_parse.php)
 - Scanner (parse_classes/scanner.php)
 - Parser (parse_classes/parser.php)

### Controller
Po spuštění skriptu *parse.php* je vytvořena nová instance třídy **Controller**, která řídí průběh programu a odchycuje chyby, které vypisuje na standardní chybový výstup.

### ArgumentManager
První operací třídy **Controller** je vytvoření nové instance třídy **ArgumentManager**, která zkontroluje vstupní argumenty, přičemž případné přepínače s dalšími informacemi uchovává ve své struktuře. Jestliže je potřeba přistoupit a otestovat zadané argumenty později za běhu programu, přistupuje se k informacím právě přes již vytvořenou instanci třídy **ArgumentManager**.

### Scanner
Po dokončení kontroly vstupních parametrů je třídou **Controller** vytvořena nová instance třídy **Scanner**. Tato entita je spuštěna metodou *run_scanner()*. Po spuštění je vytvořena nová instance třídy **Parser**, které jsou metodou *receive_command()* odesílány validní řádky s instrukcemi zapsané v jazyce IPPcode20. Každý příkaz je odeslán spolu s informací o parametrech, které jsou vyžadovány přesně v pořadí, které udává jedno z následujících označení: *VAR*, *SYMB*, *LABEL*, *VAR_SYMB*, *VAR_TYPE*, *VAR_SYMB_SYMB*, *LABEL_SYMB_SYMB*, případně *NONE*. Všechny tyto informace jsou vyhledávány v předdefinovaném asociativním poli, kdy klíčem je jméno instrukce a hodnotou právě jedno z označení. Žádná instrukce nebude předána třídě **Parser** dříve, než je nalezena validní hlavička s označením jazyka IPPcode20, jinak je reportována chyba zpět třídě **Controller**, stejně jako všechny ostatní lexikální chyby nalezené při lexikální analýze.

### Parser
Třída **Parser** příjímá vždy jeden validní řádek programu zapsaného v jazyce IPPcode20 přes metodu *receive_command()* a vytváří jeho XML reprezentaci pomocí třídy XMLWriter, která je standardní součástí interpretu PHP verze 7.4. Pro danou instrukci jsou převzaty vyžadované parametry a je provedena syntaktická kontrola řádku s příkazem. Kontroly jsou prováděny především pomocí regulárních výrazů, které jsou definovány při konstrukci instance této třídy. Jestliže není nalezena žádná chyba, dojde po kontrole k vygenerování příslušné XML reprezentace dané instrukce. Po přijetí posledního řádku kódu, je třídou **Scanner** volána metoda *print_output()*, která vypíše vygenerovaný obsah třídy XMLWriter na standardní výstup.

#### Získáno bodů: 7/7
