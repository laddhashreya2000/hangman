/****************************************************
HANGMAN GAME, in which the user
enters the word he'll "guess", and then the user gets
to start guessing letters. After each guess, the word
is printed out with *s instead of unguessed letters.
 ****************************************************/
#include <stdio.h>
#include <stdlib.h> //for rand function
//#include <string.h>
#include <at89c5131.h>
#include "lcd.h"	//C file contaninig LCD interfacing functions
#include "serial.c"	//C file with UART interfacing functions

#define NUMBER_OF_WORDS 100
#define MAX_WORD_SIZE 6

code unsigned char vocab[NUMBER_OF_WORDS][MAX_WORD_SIZE] = { "start" ,"accept", "about","above","across","action","admit","adult","affect","after","again","agency","agent","agree","ahead","allow","almost","alone","along","always","among","amount","animal","answer","anyone","appear","apply","area","argue","around","arrive","artist","assume","attack","author","avoid","away","back","ball","bank","base","beat","become","before","begin","behind","best","better","beyond","bill","black","blood","blue","board","book","born","both","break","bring","budget","build","call","camera","cancer","card","care","career","carry","case","catch","cause","cell","center","chair","chance","change","charge","check","child","choice","choose","church","city","civil","claim","class","clear","close","coach","cold","color","come","common","cost","could","couple","course","court", "system", "record"};

void main(void) {
	int N, show[MAX_WORD_SIZE], success, i, turn, incorrect;
	unsigned char ch, *word, guess, str_incorrect;
	lcd_init();
	uart_init();
	transmit_string("\r\n*******Hangman**********\r\n");

	lcd_cmd(0x84); //first row, 5th column
	lcd_write_string("Welcome"); //initial welcome msg
	lcd_cmd(0x0C1); //second row, second column
	lcd_write_string("Starting Game"); //initial starting game msg
	msdelay(1500); //1.5 sec delay
	
	while (1){ // Infinite loop so that game never ends
		lcd_cmd(0x01); // LCD clear at the start of every loop
		turn = rand()%100; // a random integer between 0 to 99 
		word = &vocab[turn]; // starting address of random word for every turn
		
		//transmit_char(*word); //check what the pointer is -- prints out first character of the word

		N=0; //length of the word initialised to 0
		// Get length of the word (max length = 6)
		while (*(word+N)!='\0'){ // while pointer does not point to empty string
			if(N==6){
				break;
			}
			transmit_char(*(word+N)); // to see the word in realterm
			N++;
		}
		//length = N+'0';
		//transmit_char(length); to check correct length
		transmit_string("\r\n");
		
		// show[i] is 1 if the character i has been guessed.
		for (i=0; i < N; ++i) {
			show[i] = 0;
		}
		incorrect=0; //initialise incorrect guesses to zero
		success=0; //success is 1 if word is guessed correctly
		
		// Loop over each round of guessing
		while (! success) {
			lcd_cmd(0x85); //first row, sixth column
			for(i=0; i < N; ++i) {
				if (show[i]) {
					lcd_write_char(*(word+i)); //display the characters which are guessed
				}
				else {
					lcd_write_string("_");
				}
			}
			
			lcd_cmd(0x0C1);
			lcd_write_string("Guess Letter ");
			// Get player's next guess
			ch = receive_char();
			lcd_write_char(ch); //show the guessed letter to player
			msdelay(500); // delay of 0.5 sec
			guess = 'F'; // True if user is successful in guess
			
			// show positions updated
			for(i=0; i < N; ++i) {
				if (*(word+i) == ch) {
					show[i] = 1;
					guess = 'T';
				}
			}
			lcd_cmd(0x01); //LCD clear
			
			if(guess=='F') { //incorrect guess case
				incorrect = incorrect+1; //incorrects incremented
				lcd_cmd(0x80);
				lcd_write_string("Incorrect Guess");
				lcd_cmd(0x0C2);
				lcd_write_string("Incorrects ");
				str_incorrect = incorrect+'0'; //converting int to char
				lcd_write_char(str_incorrect); //incorrect guesses displayed
				msdelay(1500);
				lcd_cmd(0x01);
				
				if(incorrect==6) { //Game over case
					lcd_cmd(0x82);
					lcd_write_string("Game Over !!");
					lcd_cmd(0x0C0);
					lcd_write_string("Word was ");
					for(i=0; i < N; ++i) {
						lcd_write_char(*(word+i)); //display correct word		
					}
					break;
				}
			}
			else {// Determine whether the player has won
				success = 1;
				for(i = 0; i < N; ++i) {
					if (!show[i]) {
						success = 0; //if any character unguessed, continue game
						break;
					}
				}
				
				if(success) { //player won case
					lcd_cmd(0x85);
					for(i=0; i < N; ++i) {
						lcd_write_char(*(word+i)); //display correct word		
					}
					lcd_cmd(0x0C0);
					lcd_write_string("Congratulations"); //victory msg
					break;
				}
			}
		} //game ends
		
		msdelay(1500); //delay 1.5 sec
		lcd_cmd(0x01); //LCD clear
		lcd_cmd(0x82); //first row, third column
		lcd_write_string("Starting New"); //start new game msg
		msdelay(1000); //delay 1 sec before starting new game
	}
}