# Implementační dokumentace k 2. úloze do IPP 2019/2020
Jméno a příjmení: Michal Rein (xreinm00)

# Interpret

Program slouží jako interpret jazyka IPPcode20.

## Spuštění:

```sh
$ python3.8 interpret.py [OPTIONS] < (input.xml)
```
Pro více informací o parametrech spusťte skript s parametrem --help.

## Implementace

Program je sestaven z následujících modulů:
 - interpret.py
 - strings.py (interpret_lib/strings.py)
 - type_tests.py (interpret_lib/type_tests.py)

Většina implementace se nachází v samotném skriptu *interpret.py*. Modul *strings.py* obsahuje pouze funkce pro práci s řetězci a *type_tests.py* funkce pro ověřování datových typů konstant a proměnných.

Po spuštění skriptu se inicializují tyto globální struktury:
 - *program_counter* (proměnná typu integer)
   - interní čítač programu, reprezentuje aktuálně prováděnou instrukci
 - *program* (list příkazů)
   - list uchovávající veškeré nalezené příkazy, seřazené podle atributu *order*
 - *global_frame* (list proměnných)
   - reprezentuje globální rámec programu
   - proměnné jsou uloženy jako elementy listu ve tvaru: {"name" : ["type", "value"]}
 - *tmp_frame* (list proměnných)
   - reprezentuje dočasný rámec programu
 - *frame_stack* (zásobník listů proměnných)
   - reprezentuje datovou strukturu typu zásobník, uchovávající lokální rámce
 - *data_stack* (zásobník dat)
   -  datová struktura typu zásobník uchovávající data ve tvaru: ["type", "value"]
 - *label_list* (list návětší)
   -  list uchovávající návětší ve tvaru: {"label_name" : "order"}
 - *call_stack* (zásobník volání) 
   - zásobník uchovávající hodnoty programového čítače při skoku do funkce voláním CALL

### Analýza vstupního souboru

Po inicializaci výše uvedených datových struktur je nutné načíst a analyzovat patřičnou XML reprezentaci programu. Tuto funkčnost zajišťuje funkce ***parse_xml(source_file)***, která pomocí interní knihovny jazyka Python3.8 ***xml.dom.minidom*** zkontroluje formální správnost přijatého XML a extrahuje každý element *<instruction>* do následujícího interního formátu: *[order, instruction_name, symbol1_type, symbol1_value, symbol2_type, symbol2_value, symbol3_type, symbol3_value]*. Tento formát je obecně očekávaný a společný pro všechny instrukce, přičemž *order* reprezentuje index, na kterém se příkaz nachází ve struktuře *program*, a jednotlivé elementy *symbol* proměnlivé parametry jednotlivých instrukcí. Příkaz je následně uložen do struktury *program* na index *order*. V této fázi jsou všechna nalezená návětší uložena do struktury *label_list*.

Po extrakci všech instrukcí ze vstupního souboru, je celá struktura *program* seřazena podle atributu *order*. Jestliže je porušena souvislá posloupnost atributů *order* (např. 1, 2, 10, 13...), jsou chybějící pořádí doplněna příslušným indexem s interní instrukcí [order, "PASS"].

### Interpretace programu

Samotná interpretace je obsluhována ve funkci ***execute_program(command)***. Interním čítačem *program_counter* je ze struktury *program* vybrán patřičný příkaz k provedení a předán této funkci. Nejdříve je identifikována instrukce podle jejího názvu, poté provedena syntaktická a typová kontrola, a případně je proveden celý příkaz.

Všechny operandy instrukcí jsou extrahovány funkcí ***extract_operands(command)***, která identifikuje příslušné datové typy, přetypuje hodnoty, případně konvertuje speciální znaky pomocí funkce ***convert_special_chars(string)*** do podoby vhodné pro práci v prostředí programovacího jazyka Python3.8. Hodnoty, se kterými se následně pracuje jsou vždy ve formátu: [type, value], přičemž *type* může být jedna z hodnot "int", "bool", "string", "var" nebo "nil".

Jestliže je nutné vyhledat hodnotu uloženou v proměnné na některém z rámců, je použita funkce ***get_variable_value(variable)***, opačně pak k ukládání hodnot do proměnných slouží funkce ***assign_to_variable(variable, value)***.

V případě provádění instrukce některé ze společných "rodin" příkazů jsou volány specifické funkce zprostředkovávající tuto funkcionalitu, např. pro artimetické instrukce slouží funkce ***calculate_arithmetic(order, operation, destination, operand_1, operand_2)***, k vyhodnocení podmínek ***evaluate_condition(operation, destination, operand_1, operand_2)*** a provedení skokových instrukcí pak ***perform_jump(label)***. 

# Test

## Spuštění:

```sh
$ php7.4 test.php [OPTIONS] > (FILE.html)
```
Pro více informací o parametrech spusťte skript s parametrem --help.

## Implementace

Program je sestaven z následujících tříd a modulů:
 - test.php (řídící skript programu)
 - arguments_test.php (test_classes/arguments_test.php)
    - třída pro extrakci a kontrolu argumentů, uchovává v sobě informace pro řídící strukturu
 - html_gen.php (test_classes/html_gen.php)
   - třída zajišťující generování výsledného HTML výstupu

Po spuštění testovacího skriptu je vytvořena nová instance třídy ***ArgumentManager_test***, která zkontroluje vstupní argumenty a nastaví patřičné příznaky, kterými se testovací skript následně řídí.
Následně je vyvořena instance třídy HTMLGenerator, která disponuje metodami pro generování předpřipraveného HTML strukturovaného kódu.

Hlavním tělem skriptu je metoda ***iterate_directory($dir_path, $args, $final_output, $html_gen)***, která v první části sestaví list všech nalezených souborů (vyhledává pouze unikátní názvy souborů bez ohledu na přípony). K procházení adresářů je využita vestavěná funkce ***scandir()*** a následná iterace jejím výstupem. Jestliže je skript spuštěn s parametrem *'recursive'*, je po nalezení dalšího odkazu na adresář v aktuálně prohledávaném adresáři, spuštěna celá funkce ***iterate_directory()*** rekurzivně znova. Každé volání této funkce tedy zpracovává zvlášť jeden adresář s testy. V druhé části je ověřena existence všech potřebných souborů, přípustné chybějící soubory jsou vytvořeny s výchozími hodnotami a na základě nastavených příznaků třídy ***ArgumentManager_test*** je spuštěna část programu pro daný režim (both/int-only/parse-only). Po zpracování a vyhodnocení testu je vygenerována patřičná struktura HTML a uložena do proměnné *$final_output*, která je po dokončení testů vypsána na standardní výstup.

#### Získáno bodů: 9,8/13

