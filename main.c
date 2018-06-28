#include <stdlib.h>
#include <stdio.h>

#define NUMBER_OF_DECKS 3
#define CARDS_IN_DECK 52
#define MAX_INDEX NUMBER_OF_DECKS*CARDS_IN_DECK

struct card
{
	int suit;
	int value;
	int priority;
};

static char *suits[4] = {"hearts", "diamonds", "clubs", "spades"};
static char *values[14] = {0, "Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King"};

void heapify( struct card *array, int index );
void extract_max( struct card *array, struct card *card );
void print_hand( struct card* hand, int num_cards );
void deal( struct card *pile, struct card* hand, int *num_cards);
int best_hand_value( struct card *hand, int num_cards );

int heap_count = 0;

int main (int argc, char **argv)
{
	struct card *pile;
	int i, j, d;
	FILE *rand;
	
	printf("Basic blackjack\n" );

	/* generate the deck */
	pile = malloc( sizeof(struct card) * NUMBER_OF_DECKS * CARDS_IN_DECK );

	/* suit and value structure;
	 * 0 = hearts
	 * 1 = diamonds
	 * 2 = clubs
	 * 3 = spades
	 *
	 * 2-10 = value cards
	 * 1    = Ace
	 * 11   = Jack
	 * 12   = Queen
	 * 13   = King
	 */
	rand = fopen("/dev/random", "r");

	for (d = 0; d < NUMBER_OF_DECKS; d++)
	{
		/* Loop through values */
		for (i = 1; i < 14; i++)
		{
			/* Loop through suits */
			for (j = 0; j < 4; j++)
			{
				struct card *c_ptr;
				int pri;
				pri = random();
				c_ptr = pile + d*CARDS_IN_DECK+(i-1)*4+j;
				c_ptr->value = i;
				c_ptr->suit  = j;
				c_ptr->priority = pri;
				heap_count++;
			}
		}
	}

	fclose(rand);

	for (i = (int)(MAX_INDEX/2); i >= 0; i--)
	{
		heapify (pile, i);
	}

	while (1)
	{
		struct card card;
		int dealer_cards = 0;
		int player_cards = 0;
		char key = '\0';
		int value = 0;
		int d_value = 0;

		struct card player_hand[10];
		struct card dealer_hand[10];

		/* Initial deal */
		deal( pile, player_hand, &player_cards );
		deal( pile, dealer_hand, &dealer_cards );
		deal( pile, player_hand, &player_cards );
		deal( pile, dealer_hand, &dealer_cards );
		
		print_hand( player_hand, player_cards );
		printf( "Dealer showing: %s of %s\n", values[dealer_hand[1].value], suits[dealer_hand[1].suit] );
		value = best_hand_value( player_hand, player_cards );
		printf( "Best value: %d\n", value);

		while (key != 's' && value < 21)
		{
			while (key != 'h' && key != 's')
			{
				char buf[1024];
				printf( "Hit `h' to hit, `s' to stay.\n" );
				key = getchar();
				getchar();
			}
			if (key == 'h') 
			{
				deal( pile, player_hand, &player_cards );
				print_hand( player_hand, player_cards );
				printf( "Dealer showing: %s of %s\n", values[dealer_hand[1].value], suits[dealer_hand[1].suit] );
				value = best_hand_value( player_hand, player_cards );
				printf( "Best value: %d\n", value);
				key = '\0';
			}
			else 
			{
				printf( "Standing.\n" );
			}
		}

		if (value > 21)
		{
			printf( "BUST!\n" );
		}
		else 
		{
			printf( "Dealer showing: %s of %s\n", values[dealer_hand[1].value], suits[dealer_hand[1].suit] );
			printf("Dealer's hole card is: %s of %s\n", values[dealer_hand[0].value], suits[dealer_hand[0].suit] );
			d_value = best_hand_value( dealer_hand, dealer_cards );

			while (d_value < 17)
			{
				deal( pile, dealer_hand, &dealer_cards );
				printf("Dealer drew %s of %s\n", values[dealer_hand[dealer_cards-1].value], suits[dealer_hand[dealer_cards-1].suit]);
				d_value = best_hand_value( dealer_hand, dealer_cards );
			}

			if (d_value > 21)
			{
				printf("dealer busts; you win!\n");
			}
			else if (d_value > value || d_value == 21)
			{
				printf("dealer wins!\n");
			}
			else if (value == d_value)
			{
				printf("Push\n");
			}
			else
			{
				printf("You beat the dealer!\n");
			}
			printf("\n\n\n");
		}
	}
	return 0;
}

int best_hand_value( struct card *hand, int num_cards )
{
	int value = 0;

	int i;
	int aces = 0;

	for (i = 0; i < num_cards; i++)
	{
		int c = hand[i].value;
		
		if (c == 1)
		{
			aces++;
			value += 11;
		}
		else if (c > 1 && c < 11)
		{
			value += c;
		}
		else
		{
			value += 10;
		}
	}

	while (value > 21 && aces > 0)
	{
		value -= 10;
		aces--;
	}

	return value;
}

void print_hand( struct card* hand, int num_cards )
{
	int i;
	printf( "Your hand: \n" );
	for (i = 0; i < num_cards; i++)
	{
		printf( "%s of %s\n", values[hand[i].value], suits[hand[i].suit] );
	}
}
				

void deal( struct card *pile, struct card* hand, int *num_cards)
{
	struct card card;

	extract_max (pile, &card);
	hand[*num_cards] = card;
	(*num_cards)++;
}
	

/* heapify operation */
void heapify( struct card *array, int index )
{
	int node_value;
	int left_value;
	int right_value;

	int left_index;
	int right_index;

	left_index = 2*index + 1;
	right_index = 2*index + 2;

	if (left_index >= heap_count)
	{
		left_value = 0;
	}
	else 
	{
		left_value = array[left_index].priority;
	}
	if (right_index >= heap_count)
	{
		right_value = 0;
	}
	else
	{
		right_value = array[right_index].priority;
	}

	node_value = array[index].priority;

	/* Max heap property */
	if (node_value < left_value || node_value < right_value)
	{
		int sw_index;
		struct card c;
		if (left_value > right_value)
		{
			sw_index = left_index;
		}
		else 
		{
			sw_index = right_index;
		}
		c = array[index];
		array[index] = array[sw_index];
		array[sw_index] = c;
		heapify( array, sw_index );
	}

}

void extract_max( struct card *array, struct card *card )
{
	*card = array[0];
	array[0] = array[heap_count-1];
	heap_count--;
	heapify (array, 0);
	return;
}
