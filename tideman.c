#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
char candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count = 0;
int candidate_count;

// Function prototypes
bool vote(int rank, char name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

void exchange(int i, int max);
bool recursive_lock(int a, int b);

int main(int argc, char *argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1][0];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count;
    printf("Number of voters: ");
    scanf("%i", &voter_count);

    // Query for votes
    char name;
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            printf("Rank %i: ", j + 1);
            scanf(" %c", &name);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, char name, int ranks[])
{
    // Check if name is a match for a valid candidate
    bool found = false;
    for(int i = 0; i < candidate_count; i++)
    {
        if(candidates[i] == name)
        {
            found = true;
            break;
        }
    }
    if(!found)
    {
        return false;
    }
    // Check if the voter didn't choose already the candidate
    for(int i = 0; i < rank; i++)
    {
        if(ranks[i] == name)
        {
            return false;
        }
    }
    // Record the rank
    for(int i = 0; i < candidate_count; i++)
    {
        if(candidates[i] == name)
        {
            ranks[rank] = i;
            return true;
        }
    }
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for(int i = 0; i < candidate_count; i++)
    {
        for(int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]] += 1;
        }
    }
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    pair pr;
    for(int i = 0; i < candidate_count; i++)
    {
        for(int j = 0; j < candidate_count; j++)
        {
            if(i == j)
            {
                continue;
            }
            if(preferences[i][j] > preferences[j][i])
            {
                pr.winner = i;
                pr.loser = j;
                pairs[pair_count++] = pr;
            }
            else if(preferences[i][j] < preferences[j][i])
            {
                pr.winner = j;
                pr.loser = i;
                pairs[pair_count++] = pr;
            }
        }
    }
}

void exchange(int i, int max)
{
    pair tmp = pairs[i];
    pairs[i] = pairs[max];
    pairs[max] = tmp;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    for(int i = 0; i < pair_count; i++)
    {
        for(int j = i; j > 0; j--)
        {
            if(pairs[j].winner > pairs[j-1].winner)
            {
                exchange(j, j-1);
            }
            else
            {
                break;
            }
        }
    }
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for(int i = 0; i < pair_count; i++)
    {
        if (recursive_lock(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
}


// Print the winner of the election
void print_winner(void)
{
    bool has_won;
    for(int i = 0; i < candidate_count; i++)
    {
        has_won = true;
        for(int j = 0; j < candidate_count; j++)
        {
            if(i == j)
            {
                continue;
            }
            if(locked[j][i])
            {
                has_won = false;
                break;
            }
        }
        if(has_won)
        {
            printf("%c has won the election.", candidates[i]);
            break;
        }
    }
}


bool recursive_lock(int x, int y)
{
    if (x == y)
    {
        return false;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[i][x])
        {
            if (!recursive_lock(i, y))
            {
                return false;
            }
        }
    }

    return true;
}


