# Sada aplikací nad FreeRTOS

Autor: Michal Rein (xreinm00)

Smyslem a zadáním projektu je vytvoření sady demoaplikací za použití 
vstupních a výstupních periferií nad real-time operačním systémem FreeRTOS 
na platformě FITkit3, která disponuje mikrokontrolérem Kinetis K-60 s jádrem 
ARM Cortex-M4. Na FITkit3 najdeme sadu LED diod, tlačítka a piezzo, které 
lze využít pro demonstraci a ovládání výsledné aplikace.

Jak již bylo zmíněno, aplikace poběží nad operačním systémem 
FreeRTOS. Jedná se o real-time operační systém, který se zpravidla využívá 
v real-time systémech, ve kterých běží současně více úloh a je nutné těmto 
procesům přidělovat procesorový čas a prostředky se závislostí na prioritách.

#### Získáno bodů: 12,7/14