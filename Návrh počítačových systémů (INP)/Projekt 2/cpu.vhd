-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2019 Brno University of Technology,
--                    Faculty of Information Technology
-- Author: Michal Rein (xreinm00)
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

	type fsm_state is (
	
	START,
	DECODE,
	PTR_INCREMENT_STATE, PTR_DECREMENT_STATE,
	DATA_INCREMENT_STATE1, DATA_INCREMENT_STATE2,
	DATA_DECREMENT_STATE1, DATA_DECREMENT_STATE2,
	PRINT_DATA,
	SAVE_ADRESS_VALUE, GET_AUX_VALUE, LOAD_ADRESS_VALUE, SAVE_AUX_VALUE,
	WHILE_START_STATE1, WHILE_START_STATE2,
	WHILE_END_STATE1, WHILE_END_STATE2,
	GET_USER_INPUT,
	DEFAULT,
	STOP_PROGRAM);
	
	signal next_state : fsm_state;
	signal present_state : fsm_state;
	
	
	signal pc_increment : std_logic;
	signal pc_decrement : std_logic;
	signal pc_data : std_logic_vector(12 downto 0);
	
	signal ptr_increment : std_logic;
	signal ptr_decrement : std_logic;
	signal ptr_data: std_logic_vector(12 downto 0);
	signal ptr_data_tmp: std_logic_vector(12 downto 0);
	
	signal sel1 : std_logic;
	signal sel2 : std_logic;
	signal sel3 : std_logic_vector(1 downto 0);
	
	type instruction_type is (
	ptr_next, ptr_prev,
	data_increment, data_decrement,
	data_print,
	adress_aux, aux_adress, user_input,
	while_start, while_end,
	not_an_instruction,
	stop);
	
	signal instruction : instruction_type;

 -- zde dopiste potrebne deklarace signalu

begin

 -- zde dopiste vlastni VHDL kod


 -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze 
 --   - nelze z vice procesu ovladat stejny signal,
 --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
 --   - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a 
 --   - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly.
		
		-- PC regist
	pc: process(CLK, RESET)
		begin
		
			if (RESET = '1') then
				pc_data <= (others => '0');
				
			elsif (CLK'event and CLK = '1') then
			
				if (pc_increment = '1') then
				
					if (pc_data = "0111111111111") then
							pc_data <= (others => '0');
					else
							pc_data <= pc_data + 1;
					end if;
					
				elsif (pc_decrement = '1') then
					pc_data <= pc_data - 1;
					
				end if;			
			end if;
			
		end process;

		-- PTR registr
	ptr: process(CLK, RESET)
		begin
		
			if (RESET = '1') then
				ptr_data <=  "1000000000000";
				
			elsif (CLK'event and CLK = '1') then
			
				if (ptr_increment = '1') then
				
						if (ptr_data = "1111111111111") then
								ptr_data <= "1000000000000";
						else
								ptr_data <= ptr_data + 1;
						end if;
					
				elsif (ptr_decrement = '1') then
				
						if (ptr_data = "1000000000000") then
								ptr_data <= "1111111111111";
						else
								ptr_data <= ptr_data - 1;
						end if;
					
				end if;			
			end if;
			
		end process;
		
	-- MUX2, selekce mezi TMP prom�nnou a ukazatelem na DATA
	mux2: process(CLK, sel2, ptr_data, ptr_data_tmp)
	begin
			
			case sel2 is
			
					when '0' => ptr_data_tmp <= ptr_data; -- DATA					
					when '1' => ptr_data_tmp <= "1000000000000"; -- TMP ( adresa 0x1000)
					when others =>
					
			end case;
	end process;

	
	-- MUX1, selekce mezi PC adresou PTR adresou
	mux1: process(CLK, sel1, ptr_data_tmp, pc_data)
	begin
			
			case sel1 is
			
					when '0' => DATA_ADDR <= pc_data; -- PC adresa				
					when '1' => DATA_ADDR <= ptr_data_tmp; -- PTR adresa
					when others =>
					
			end case;
	end process;
	

	-- MUX3, selekce zapisov�n�
	mux3: process(CLK, sel3, DATA_RDATA, IN_DATA)
	begin
			
			case sel3 is
			
					when "00" => DATA_WDATA <= IN_DATA; -- zapi� u�ivatelsk� vstup
					when "01"=> DATA_WDATA <= DATA_RDATA - 1; -- data na aktu�ln� adrese dekrementovan� o 1
					when "10"=> DATA_WDATA <= DATA_RDATA + 1; -- data na aktu�ln� adrese inkrementovan� o 1
					when "11"=> DATA_WDATA <= DATA_RDATA; -- data na aktu�ln� adrese
					when others =>
					
			end case;
	end process;
	
	-- DEK�DER instrukc�
	decoder: process(DATA_RDATA)
	begin
			case (DATA_RDATA) is
			
					when X"3E" => instruction <= ptr_next; -- > 
					when X"3C" => instruction <= ptr_prev; -- <
					when X"2B" => instruction <= data_increment; -- +
					when X"2D" => instruction <= data_decrement; -- -
					when X"2E" => instruction <= data_print; -- vytiskni data
					when X"00" => instruction <= stop; -- zastavit program
					when X"24" => instruction <= adress_aux; -- $
					when X"21" => instruction <= aux_adress; -- !
					when X"2C" => instruction <= user_input; -- ,
					when X"5B" => instruction <= while_start; -- [
					when X"5D" => instruction <= while_end; -- ] 
					when others => instruction <= not_an_instruction;
				
			end case;
	end process;
	
	-- Kone�n� automat pro present_state
	fsm_present_state: process(RESET, CLK, present_state, next_state)
	begin
	
			if (RESET = '1') then
					present_state <= START;
					
			elsif (CLK'event and CLK = '1') then
			
					if (EN = '1') then
							present_state <= next_state;
					end if;
					
			end if;
	end process;
	
	-- Kone�n� automat pro next_state
	fsm_next_state: process(DATA_RDATA, IN_DATA, OUT_BUSY, IN_VLD, present_state, instruction, sel1, sel2, sel3)
	begin
	
			next_state <= START;
			DATA_EN <= '0';
			DATA_RDWR <= '0';
			OUT_WE <= '0';
			IN_REQ <= '0';			
			
			ptr_increment <= '0';
			ptr_decrement <= '0';
			pc_increment <= '0';
			pc_decrement <= '0';
			
			
			case present_state is
			
					when START =>
							sel1 <= '0';
							DATA_EN <= '1';
							next_state <= DECODE;
							
					when DECODE =>
					
							case instruction is
							
									when stop => next_state <= STOP_PROGRAM;
									when ptr_next => next_state <= PTR_INCREMENT_STATE;
									when ptr_prev => next_state <= PTR_DECREMENT_STATE;
									when data_increment  => next_state <= DATA_INCREMENT_STATE1;
									when data_decrement => next_state <= DATA_DECREMENT_STATE1;
									when data_print => 
											next_state <= PRINT_DATA;
											sel1 <= '1';
											sel2 <= '0';
											sel3 <= "11";
											DATA_EN <= '1';
											DATA_RDWR <= '0';
											
									when adress_aux =>
											next_state <= LOAD_ADRESS_VALUE;
											sel1 <= '1';
											sel2 <= '0';
											sel3 <= "11";
											DATA_EN <= '1';
											DATA_RDWR <= '0';
									
									when aux_adress =>
											sel1 <= '1';
											sel2 <= '1';
											sel3 <= "11";
											DATA_EN <= '1';
											DATA_RDWR <= '0';
											next_state <= GET_AUX_VALUE;
									
									when user_input =>
											sel1 <= '1';
											sel2 <= '0';
											sel3 <= "00";
											IN_REQ <= '1';
											next_state <= GET_USER_INPUT;
									
									when while_start =>
											sel1 <= '1';
											sel2 <= '0';
											sel3 <= "11";
											DATA_EN <= '1';
											DATA_RDWR <= '0';
											next_state <= WHILE_START_STATE1;
									
									when while_end =>
											sel1 <= '1';
											sel2 <= '0';
											sel3 <= "11";
											DATA_EN <= '1';
											DATA_RDWR <= '0';
											next_state <= WHILE_END_STATE1;
											
									when not_an_instruction => next_state <= DEFAULT;
									when others => next_state <= DEFAULT;
							
							end case;
					
					-- zv��en� ukazatele na data
					when PTR_INCREMENT_STATE =>
							ptr_increment <= '1';
							pc_increment <= '1';
							next_state <= START;
					
					-- sn�en� ukazatele na data
					when PTR_DECREMENT_STATE =>
							ptr_decrement <= '1';
							pc_increment <= '1';
							next_state <= START;
					
					-- inkrementace hodnoty aktu�ln� bu�ky (f�ze 1)
					when DATA_INCREMENT_STATE1 =>
							sel1 <= '1';
							sel2 <= '0';
							sel3 <= "10";
							DATA_EN <= '1';
							DATA_RDWR <= '0';									
							next_state <= DATA_INCREMENT_STATE2;
					
					-- inkrementace hodnoty aktu�ln� bu�ky (f�ze 2)
					when DATA_INCREMENT_STATE2 =>
							sel1 <= '1';
							sel2 <= '0';
							sel3 <= "10";
							DATA_RDWR <= '1';
							DATA_EN <= '1';
							pc_increment <= '1';
							next_state <= START;
					
					-- dekrementace hodnoty aktu�ln� bu�ky (f�ze 1)
					when DATA_DECREMENT_STATE1 =>
							sel1 <= '1';
							sel2 <= '0';
							sel3 <= "01";
							DATA_RDWR <= '0';
							DATA_EN <= '1';
							next_state <= DATA_DECREMENT_STATE2;
					
					-- dekrementace hodnoty aktu�ln� bu�ky (f�ze 2) 
					when DATA_DECREMENT_STATE2 =>					
							sel1 <= '1';
							sel2 <= '0';
							sel3 <= "01";
							DATA_RDWR <= '1';
							DATA_EN <= '1';
							pc_increment <= '1';
							next_state <= START;
					
					-- vytiskne data na display
					when PRINT_DATA =>
							
							-- za��zen� je zanepr�zdn�n� ---> mus�me po�kat 
							if (OUT_BUSY = '1') then
									OUT_WE <= '1';
									next_state <= PRINT_DATA;
									
							else
									DATA_EN <= '1';
									DATA_RDWR <= '0';
									OUT_DATA <= DATA_RDATA;
									OUT_WE <= '1';
									pc_increment <= '1';
									next_state <= START;
									
							end if;
					
					-- na�te hodnotu na dan� adrese
					when LOAD_ADRESS_VALUE =>
							sel1 <= '1';
							sel2 <= '0';
							DATA_EN <= '1';
							DATA_RDWR <= '0';
							next_state <= SAVE_ADRESS_VALUE;
					
					-- ulo�� hodnotu na adresse do aux prom�nn�
					when SAVE_ADRESS_VALUE =>
							sel1 <= '1';
							sel2 <= '1';
							sel3 <= "11";
							pc_increment <= '1';
							DATA_EN <= '1';
							DATA_RDWR <= '1';
							next_state <= START;
					
					-- na�te aux hodnotu z 0x1000 
					when GET_AUX_VALUE =>
							sel1 <= '1';
							sel2 <= '1';
							DATA_EN <= '1';
							DATA_RDWR <= '0';
							next_state <= SAVE_AUX_VALUE;
					
					-- ulo�� aux na aktu�ln� adressu
					when SAVE_AUX_VALUE =>
							sel1 <= '1';
							sel2 <= '0';
							sel3 <= "11";
							pc_increment <= '1';
							DATA_EN <= '1';
							DATA_RDWR <= '1';
							next_state <= START;
					
					-- na�te u�ivatelsk� data
					when GET_USER_INPUT =>
							
							if (IN_VLD = '1') then
									next_state <= START;
									DATA_EN <= '1';
									DATA_RDWR <= '1';
									sel1 <= '1';
									sel2 <= '0';
									sel3 <= "00";
									pc_increment <= '1';
							else
									IN_REQ <= '1';
									next_state <= GET_USER_INPUT;
							
							end if;
					
					-- za��tek while cyklu
					when WHILE_START_STATE1 =>
							
							-- podm�nka se vyhodnot� jako false, projede na konec
							if (DATA_RDATA = "00000000") then
									next_state <= WHILE_START_STATE2;
									sel1 <= '0';
									sel2 <= '0';
									sel3 <= "11";
									DATA_EN <= '1';
									DATA_RDWR <= '0';
									pc_increment <= '1';
							else
									next_state <= START;
									sel1 <= '0';
									sel2 <= '0';
									sel3 <= "11";
									pc_increment <= '1';
							end if;
					
					-- podm�nka p�ed cyklem byla 0, projedeme na konec cyklu a za�neme d�le vykon�vat instrukce za n�m
					when WHILE_START_STATE2 =>
					
							if (instruction = while_end) then
									next_state <= START;
									pc_increment <= '1';
							else
									next_state <= WHILE_START_STATE2;
									pc_increment <= '1';
							end if;
					
					-- konec cyklu
					when WHILE_END_STATE1 =>
					
							-- podm�nka se vyhodnot� jako false, pokra�uje d�le v programu
							if (DATA_RDATA = "00000000") then
									pc_increment <= '1';
									next_state <= START;
							else
									next_state <= WHILE_END_STATE2;
									sel1 <= '0';
									sel2 <= '0';
									sel3 <= "11";
									DATA_EN <= '1';
									DATA_RDWR <= '0';
									pc_decrement <= '1';
									
							end if;
							
					-- hled�me za��tek cyklu
					when WHILE_END_STATE2 =>
							
							-- jestli�e jsme na za��tku, projedeme cyklus znova
							if (instruction = while_start) then
									next_state <= START;
									pc_increment <= '1';
									
							else
									sel1 <= '0';
									sel2 <= '0';
									sel3 <= "11";
									DATA_EN <= '1';
									DATA_RDWR <= '0';
									next_state <= WHILE_END_STATE2;
									pc_decrement <= '1';
									
							end if;
							
									
					-- na�tena nezn�m� instrukce
					when DEFAULT =>
							pc_increment <= '1';
							next_state <= START;
											
					-- ukon�en� programu, z�stane vyset ve stavu STOP_PROGRAM
					when STOP_PROGRAM => next_state <= STOP_PROGRAM;
					
					when others =>
							pc_increment <= '1';
							next_state <= START;
					
			end case;
	end process;
	
end behavioral;
