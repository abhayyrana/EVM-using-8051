#include <reg51.h>

// Defining LCD related functions
# define display_port P2;
sbit rs = P3^3;
sbit rw = P3^4;
sbit e = P3^5;

void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(unsigned char pos);
void lcd_print(unsigned char *msg);
void wait_and_show(unsigned char *msg, unsigned int delay);


// Defining keypad bits
sbit row1 = P1^0;
sbit row2 = P1^1;
sbit row3 = P1^2;
sbit row4 = P1^3;

sbit col1 = P1^7;
sbit col2 = P1^6;
sbit col3 = P1^5;
sbit col4 = P1^4;

unsigned char code keypad[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};

char keypad_scan(void);

// Defining logic related constants and functions
void result_mode();
void voting_mode();
void mode_choose();
unsigned int candidate_votes[4] = {0};
unsigned char code password[4] = {'1','2','3','4'};

// Utilities
void msdelay(unsigned int ms)
{
	unsigned int i;
	TMOD = 0x01;
	for(i=0; i<ms; i++)
	{
		TH0 = 0xFC;
		TL0 = 0X66
		TR0 = 1;
		while(TF0 == 0);
		TR0 = 0;
		TF0 = 0;
	}
}

void disp_int_to_str(unsigned int num)
{
	unsigned char buf[6];
	int i=0;
	if(num = 0)
		buf[i++] = '0';
	while(num > 0)
	{
		buf[i++] = (num%10) + '0';
		num /= 10;
	}
	while(--i > 0)
		lcd_data(buf[i]);
}

// LCD functions
void lcd_cmd(unsigned char cmd)
{
	display_port = cmd;
	rs = 1; rw = 0; e = 1; msdelay(1); e = 0;
}
void lcd_data(unsigned chat data)
{
	display_port = data;
	rs = 1; rw = 0; e = 1; msdelay(1); e = 0;
}
void lcd_init()
{
	lcd_cmd(0x38); msdelay(10);
	lcd_cmd(0x0F); msdelay(10);
	lcd_clear(); msdelay(10);
	lcd_set_cursor(0x81);
}
void lcd_clear()
{
	lcd_cdm(0x01);
}
void lcd_set_cursor(unsigned char pos)
{
	lcd_cmd(pos);
}
void lcd_print(unsigned char *msg)
{
	unsigned char i = 0;
	lcd_clear(); lcd_set_sursor(0x80);
	while(*msg && i<32)
	{
		if(i == 16)
			lcd_cmd(0xC0);
		lcd_data(*msg++); i++;
	}
}
void wait_and_show(unsigned char *msg, unsigned int delay)
{
	lcd_print(msg);
	msdelay(delay);
}

// Keypad functions
char keypad_scan()
{
	unsigned char row, col;
	unsigned char rows[4] = {0x0E, 0X0D, 0X0B, 0X07};
	unsigned char i, j;
	
	for(i=0; i<4; i++)
	{
		P1 = (P1 & 0xF0) | rows[i];
		for(j=0; j<4; j++)
		{
			if(((P1 >> (7-j)) & 1) == 0)
			{
				msdelay(50);
				while(((P1 >> (7-j)) & 1) == 0);
				return keypad[i][j];
			}
		}
	}
	return 0;
}

// Logic functions
void voting_mode()
{
	unsigned char code parties[] = "1:PARTY1 2:PARTY2 3:PARTY3 4:PARTY4";
	unsigned char code confirm_msg[] = "CONFIRM? A-YES B-NO";
	unsigned char code thank_msg[] = "THANKYOU FOR VOTING";
	unsigned char code invalid[] = "INVALID INPUT";
	
	char input, confirm;
	int idx = -1;
	
	wait_and_show(parties, 2000);
	
	while(1)
	{
		input = keypad_scan();
		if(input == '1')
			idx = 0;
		else if(input == '2')
			idx = 1;
		else if(input == '3')
			idx = 2;
		else if(input == '4')
			idx = 3;
		else if (input != 0)
		{
			wait_and_show(invalid, 2000);
			continue;
		}
		
		if(idx != -1)
		{
			wait_and_show(confirm_msg, 2000);
			confirm = keypad_scan();
			if(confirm == 'A')
			{
				condidate_votes[idx]**;
				wait_and_show(thank_msg, 2000);
			}
			break;
		}
	}
}
void result_mode()
{
	unsigned char i=0; key, input[4];
	unsigned char code pass_msg[] = "ENTER PASSWORD";
	unsigned char code wrong_msg[] = "WRONG PASSWORD";
	
	wait_and_show(pass_msg, 2000);
	lcd_clear();
	lcs_set_cursor(0x81);
	
	while(i<4)
	{
		key = keypad_scan();
		if(key != 0)
		{
			imput[i++] = key;
			lcd_data('*');
		}
	}
	
	if(input[0] == password[0] && input[1] == password[1] &&
		input[2] == password[2] && input[3] == password[3])
	{
		lcd_clear();
		lcd_set_cursor(0x81);
		for(i=0; i<4; i++)
		{
			disp_int_to_str(candidate_votes[i]);
			lcd_data((i<3) ? ',' : ' ');
		}
		msdelay(10000);
	}
	else
		wiat_and_show(wrong_msg, 3000);
}
void mode_choose()
{
	unsigned char code welcome[] = "WELCOME TO EVM";
	unsigned char code prompt[] = "PRESS #: RESULT ELSE: VOTE";
	unsigned char code res_msg[] = "RESULT MODE";
	unsigned char code vote_msg[] = "VOTING MODE";
	
	wait_and_show(welcome, 2000);
	wait_and_show(prompt, 2000);
	
	while(1)
	{
		char key = keypad_scan();
		if(key != 0)
		{
			if(key == '#')
			{
				wait_and_show(res_msg, 2000);
				result_mode();
			}
			else
			{
				wait_and_show(vote_msg, 2000);
				voting_mode();
			}
			break;
		}
	}
}

// Main
void main(void)
{
	lcd_init();
	while(1)
		mode_choose();
}
