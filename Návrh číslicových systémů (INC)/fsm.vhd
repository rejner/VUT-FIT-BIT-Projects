-- fsm.vhd: Finite State Machine
-- Author: Michal Rein (xreinm00) 
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (T1_A_B, T2_A, T2_B, T3_A, T3_B, T4_A, T4_B, T5_A, T5_B, T6_A, T6_B, T7_A, T7_B, T8_A, T8_B, T9_A, T9_B, T10_A, T10_B, WAIT_FOR_HASH, ALMOST_THERE ,PRINT_DENIED, PRINT_ALLOWED, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= T1_A_B;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
  
   -- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   -- Podminka T1_A_B rozhodne, jakou cestou se automat vyd�  -
   -- Na v�b�r jsou celkem 2 v�tve: "A" a "B" (viz. graf)     -
   -- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   when T1_A_B =>
      next_state <= T1_A_B;

      if (KEY(4) = '1') then
	 next_state <= T2_A;

      elsif (KEY(3) = '1') then
	 next_state <= T2_B;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -
   -- V�tev pro kod "A"                         -
   -- - - - - - - - - - - - - - - - - - - - - - -
   when T2_A => 
      next_state <= T2_A;

       if (KEY(6) = '1') then
	 next_state <= T3_A;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -
   when T3_A => 
      next_state <= T3_A;
  
      if (KEY(0) = '1') then
	 next_state <= T4_A;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;
 
   -- - - - - - - - - - - - - - - - - - - - - - -
   when T4_A => 
      next_state <= T4_A;

      if (KEY(4) = '1') then
	 next_state <= T5_A;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;
 
   -- - - - - - - - - - - - - - - - - - - - - - -
   when T5_A => 
      next_state <= T5_A;

      if (KEY(0) = '1') then
	 next_state <= T6_A;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;
 
   -- - - - - - - - - - - - - - - - - - - - - - -
   when T6_A => 
      next_state <= T6_A;

      if (KEY(5) = '1') then
	 next_state <= T7_A;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;
 
   -- - - - - - - - - - - - - - - - - - - - - - -
   when T7_A => 
      next_state <= T7_A;

      if (KEY(6) = '1') then
	 next_state <= T8_A;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;
 
   -- - - - - - - - - - - - - - - - - - - - - - -
   when T8_A => 
      next_state <= T8_A;

      if (KEY(5) = '1') then
	 next_state <= T9_A;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -
   when T9_A => 
      next_state <= T9_A;

      if (KEY(8) = '1') then
	 next_state <= T10_A;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;
 
   -- - - - - - - - - - - - - - - - - - - - - - -
   when T10_A => 
      next_state <= T10_A;

      if (KEY(6) = '1') then
	 next_state <= ALMOST_THERE;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;

  -- - - - - - - - - - - - - - - - - - - - - - -
  -- V�tev pro kod "B"                         -
  -- - - - - - - - - - - - - - - - - - - - - - -

   when T2_B => 
      next_state <= T2_B;

      if (KEY(4) = '1') then
	 next_state <= T3_B;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;
  -- - - - - - - - - - - - - - - - - - - - - - -

   when T3_B => 
      next_state <= T3_B;

      if (KEY(5) = '1') then
	 next_state <= T4_B;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;
 -- - - - - - - - - - - - - - - - - - - - - - -

   when T4_B => 
      next_state <= T4_B;

      if (KEY(3) = '1') then
	 next_state <= T5_B;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;

  -- - - - - - - - - - - - - - - - - - - - - - -

   when T5_B => 
      next_state <= T5_B;

      if (KEY(0) = '1') then
	 next_state <= T6_B;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

   when T6_B => 
      next_state <= T6_B;

      if (KEY(4) = '1') then
	 next_state <= T7_B;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

   when T7_B => 
      next_state <= T7_B;

      if (KEY(2) = '1') then
	 next_state <= T8_B;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;

  -- - - - - - - - - - - - - - - - - - - - - - -

   when T8_B => 
      next_state <= T8_B;

      if (KEY(4) = '1') then
	 next_state <= T9_B;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;

 -- - - - - - - - - - - - - - - - - - - - - - -

   when T9_B => 
      next_state <= T9_B;

      if (KEY(4) = '1') then
	 next_state <= T10_B;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;
  -- - - - - - - - - - - - - - - - - - - - - - -

   when T10_B => 
      next_state <= T10_B;

      if (KEY(0) = '1') then
	 next_state <= ALMOST_THERE;

      elsif (KEY(14 downto 0) /= "000000000000000") then
	 next_state <= WAIT_FOR_HASH;

      elsif (KEY(15) = '1') then
	 next_state <= PRINT_DENIED;

      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -
   when ALMOST_THERE =>
	next_state <= ALMOST_THERE;

	  if (KEY(15) = '1') then
	    next_state <= PRINT_ALLOWED;

    	  elsif (KEY(14 downto 0) /= "000000000000000") then
	    next_state <= WAIT_FOR_HASH;
          end if;

   -- - - - - - - - - - - - - - - - - - - - - - -
   when WAIT_FOR_HASH =>
	next_state <= WAIT_FOR_HASH;
	  if (KEY(15) = '1') then
	    next_state <= PRINT_DENIED;
	  end if;

   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_DENIED =>
 	next_state <= PRINT_DENIED;
	if (CNT_OF = '1') then
	  next_state <= FINISH;
	end if;

   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_ALLOWED =>
 	next_state <= PRINT_ALLOWED;
	if (CNT_OF = '1') then
	  next_state <= FINISH;
	end if;

   -- - - - - - - - - - - - - - - - - - - - - - - 
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= T1_A_B; 
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= T1_A_B;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
 
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_DENIED =>

      FSM_MX_MEM     <= '0';	
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';

   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_ALLOWED =>

      FSM_MX_MEM     <= '1';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';

   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
	if (KEY(14 downto 0) /= "000000000000000") then
	  FSM_LCD_WR <= '1';
	end if;

	if (KEY(15) = '1') then
	  FSM_LCD_CLR <= '1';
	end if;

   end case;
end process output_logic;

end architecture behavioral;

