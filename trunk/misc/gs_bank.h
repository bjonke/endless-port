#ifndef GS_BANK_H
#define GS_BANK_H

struct gs_banks
{
	struct money
	{
		int gold;
		int silver;
		int copper;
	}vault;

	gs_banks()
	{
		vault.copper = 0;
		vault.silver = 0;
		vault.gold = 0;
	}

	void deposit(int i_gold,int i_silver,int i_copper)
	{
		vault.gold += i_gold;
		vault.silver += i_silver;
		vault.copper += i_copper;
	}

	void withdraw(int o_gold,int o_silver,int o_copper)
	{
		vault.gold -= o_gold;
		vault.silver -= o_silver;
		vault.copper -= o_copper;
	}
};

#endif