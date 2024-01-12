#include "TextGame.h"
// using namespace std;
// take argument from teminal
int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    TextGame TextGame(argv[1]);
    return 0;
}