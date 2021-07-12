-- Autor reseni: Michal Rein, xreinm00

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( -- Sem doplnte popis rozhrani obvodu.

	RESET : in STD_LOGIC;
	SMCLK : in STD_LOGIC;
	ROW : out STD_LOGIC_VECTOR (7 downto 0);
	LED : out STD_LOGIC_VECTOR (7 downto 0)

);
end ledc8x8;

architecture main of ledc8x8 is

    -- Sem doplnte definice vnitrnich signalu.
	signal CE : STD_LOGIC := '1';
	signal CE_state : STD_Logic := '1';
	signal cnt : STD_LOGIC_VECTOR (7 downto 0) := (others => '0');
	signal state_cnt : STD_LOGIC_Vector (23 downto 0) := (others => '0');
	signal row_active : STD_LOGIC_VECTOR (7 downto 0) := (others => '0');
	signal col_active : STD_LOGIC_VECTOR (7 downto 0) := (others => '0');

begin

    -- Sem doplnte popis obvodu. Doporuceni: pouzivejte zakladni obvodove prvky
    -- (multiplexory, registry, dekodery,...), jejich funkce popisujte pomoci
    -- procesu VHDL a propojeni techto prvku, tj. komunikaci mezi procesy,
    -- realizujte pomoci vnitrnich signalu deklarovanych vyse.

    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL KODU OBVODOVYCH PRVKU,
    -- JEZ JSOU PROBIRANY ZEJMENA NA UVODNICH CVICENI INP A SHRNUTY NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html.

    -- Nezapomente take doplnit mapovani signalu rozhrani na piny FPGA
    -- v souboru ledc8x8.ucf.
	
	-- dìlièka frekvence SMCLK/256 (8 bitù)
	-- nezbytná pro správnou funkci diod
	counter: process(RESET, SMCLK)
	begin
	
		if (RESET = '1') then
		
			cnt <= (others => '0');
			CE <= '0';
			
		elsif (SMCLK'event) and (SMCLK = '1') then
		
			
				if (cnt = "1111111") then
			
					CE <= '1';
					cnt <= (others => '0');
				
				else
			
					CE <= '0';		
					cnt <= cnt + 1;
				
				end if;
		end If;	
	end process;
	
	-- èítaè pro poèítání poèáteèních mezer v zobrazení iniciál
	-- prvních 500 ms display zobrazuje iniciály, poté CE_state
	-- pøejde do stavu '0' a zabrání pøepínání øádkù a diod
	-- po dalších 500 ms èítaè opìt pøepne signál CE_state a již
	-- po zbytek chodu zaøízení generuje '1' na CE_state -> display
	-- zobrazuje iniciály po zbytek chodu
	state_counter: process(RESET, SMCLK, CE_state)
	begin
	
		if (RESET = '1') then
		
			state_cnt <= (others => '0');
			CE_state <= '1';
			
		-- FitKit bìží na frekvenci 7372800 HZ -> za jednu sekundu napoèítá 7372800 (dec)	
		elsif (SMCLK'event) and (SMCLK = '1') then
				
				-- 3686400(dec) (0 - 1/2 sec)
				if (state_cnt = "001110000100000000000000") then
			
					CE_state <= '0';
					state_cnt <= state_cnt + 1;
					
				-- 7372800(dec) (1/2 sec - 1 sec)
				elsif (state_cnt = "011100001000000000000000") then
				
					CE_state <= '1';
					
				else
				
					state_cnt <= state_cnt + 1;
					
				end if;
				
		end If;		
	end process;
	
	-- pøepínaè øádkù (posuvný registr)
	-- MSB je postupnì opakovanì pøesouván konkatenací na konec vektoru
	-- jednièka se posouvá doleva
	rows_switch: process(RESET, SMCLK, CE, CE_state, row_active)
	begin
	
	   if (RESET = '1') then
	   
		  row_active <= "10000000";
		  
	   elsif (SMCLK'event) and (SMCLK = '1') then
			
			if (CE = '1') and (CE_state = '1') then
			
				if (row_active = "00000000") then
				
					row_active <= "10000000";
				
				else
				
					row_active <= row_active(6 downto 0) & row_active(7);
				
				end if;
				
			elsif (CE = '1') and (CE_state = '0') then
			
				row_active <= (others => '0');
											 		
			end if;
			
	   end if;
		
		ROW <= row_active;
	   	   
	end process;
	
	-- aktivuje specifické diody na maticové disp. podle aktuálního aktivního øádku
	-- (tvoøí jako celek iniciály MR)
	diode_activator: process(SMCLK, RESET, row_active, CE, col_active)
	begin
		if (RESET = '1') then
		
			-- vynuluje display
			LED <= (others => '1');
		
		elsif (SMCLK'event) and (SMCLK = '1') then
					
			case row_active is

				when "00000001" => col_active <= "11101110";
				when "00000010" => col_active <= "11100100";
				when "00000100" => col_active <= "11101010";
				when "00001000" => col_active <= "10001110";
				when "00010000" => col_active <= "01101110";
				when "00100000" => col_active <= "10001111";
				when "01000000" => col_active <= "01101111";
				when "10000000" => col_active <= "01101111";
				when others => col_active <= "11111111";					

			end case;
				
		end if;
		
		LED <= col_active;
				
	end process;
	
		

end main;




-- ISID: 75579
