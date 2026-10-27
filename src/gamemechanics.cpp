#include "gamemechanics.h"
#include <sstream>

// static initialization
std::string Game_mec::wordsSelect = ""; // injected at startup
std::string Game_mec::wordsValid = "";  // injected at startup
std::string Game_mec::hidword = "";
int Game_mec::random = 0;
int Game_mec::a = 0;
int Game_mec::status = 0;
int Game_mec::posn[5] = {0};
bool Game_mec::game_started = false;
int Game_mec::no_of_guesses = 0;

// void Game_mec::gethidword(std::string s)
// {
//     a = 1;
//     hidword = s;
// }

void Game_mec::gethidword()
{
    a = 1;
    std::istringstream fptr(wordsSelect);
    randnum(); // function generates random number
    // cout<<random<<endl;
    if (random < 2)
    {
        fptr.seekg((random * 5), std::ios::beg); // places file pointer to 1st word of file if random number=0 and 2nd word if random number =1
    }
    else
    {
        fptr.seekg((random * 5) + (2 * (random - 1)), std::ios::beg); // places file pointer based on random number (eg. if num=4 place in 5th word)
    }
    fptr >> hidword;
    Game_mec::game_started = true;
}

std::string Game_mec::puthidword() // use this function to display hiddenword
{
    if (a == 0)
    {
        a = 1;
        gethidword();
    }
    return hidword;
}

void Game_mec::randnum() // generates random number between 0 to 5757 .i.e. no of lines in text file
{
    srand(time(NULL));
    random = (rand() % 1000);
}

bool Game_mec::val_checker(std::string s) // checks if word is valid or not and return 0 if invalid return 1 if valid
{
    std::string q;
    std::istringstream fptr(wordsValid);
    while (fptr.peek() != EOF)
    {
        fptr >> q;
        if (s == q)
        {
            return true;
        }
    }
    return false;
}

bool Game_mec::len_checker(std::string s) // check if length of hidword and entered word match or not return 1 if match if not match return 0
{
    if (hidword.length() != s.length())
    {
        return false;
    }
    return true;
}

void Game_mec::let_checker(std::string s) // check if word is correct or incorrect or correct letter in wrong position

/*
    if status =1 entered guess is correct
    if status =0 entered guess is incorrect
    if status =2 entered guess has some correct letters(position may not be correct)
*/
/*
    posn[0]=1st letter; posn[1]=2nd letter and so on
    if posn[x]=0 wrong letter
    if posn[x]=1 right letter in right position
    if posn[x]=2 right letter in wrong position
*/
{
    const int n = (int)hidword.length();

    // Count each letter in the answer; greens consume from this first so the
    // remaining counts decide which non-green letters can be yellow.
    int remaining[26] = {0};
    for (int i = 0; i < n; i++)
        remaining[hidword[i] - 'a']++;

    // Pass 1: correct letter in correct position.
    for (int i = 0; i < n; i++)
    {
        if (s[i] == hidword[i])
        {
            posn[i] = 1;
            remaining[s[i] - 'a']--;
        }
        else
        {
            posn[i] = 0;
        }
    }

    // Pass 2: correct letter in wrong position, while that letter is still
    // available in the answer (so duplicates are not over-counted).
    for (int i = 0; i < n; i++)
    {
        if (posn[i] != 1 && remaining[s[i] - 'a'] > 0)
        {
            posn[i] = 2;
            remaining[s[i] - 'a']--;
        }
    }

    // status: 1 all correct, 2 some correct letters, 0 none.
    status = 0;
    bool allCorrect = true;
    for (int i = 0; i < n; i++)
    {
        if (posn[i] != 0)
            status = 2;
        if (posn[i] != 1)
            allCorrect = false;
    }
    if (allCorrect)
        status = 1;
}