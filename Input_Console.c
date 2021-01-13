
void Input_Console(int treeAvailable)
{
    printf("\n┌─────────────────────────────────────────────────────────────┐");
    printf("\n│             Welcome to the world of B-Tree!!                │");
    printf("\n├─────────────────────────────────────────────────────────────┤");
    printf("\n│                   --- Select a menu ---                     │");
    if (treeAvailable == 0)
    {
        printf("\n│    Tree is   [ONLINE]                                       │");
    }
    else
    {
        printf("\n│    Tree is   [OFFLINE]                                      │");
    }
    printf("\n│   1. Make a Tree                                            │");
    printf("\n│   2. Insert a number in the Tree                            │");
    printf("\n│   3. Delete a number in the Tree                            │");
    printf("\n│   4. Search a number in the Tree                            │");
    printf("\n│   5. Print the Tree                                         │");
    printf("\n│   6. Delete the Tree                                        │");
    printf("\n│   7. Help                                                   │");
    printf("\n├─────────────────────────────────────────────────────────────┤");
    printf("\n│                                   Designed by TrueSunDragon │");
    printf("\n│                       copyright(c) 2021 All rights reserved │");
    printf("\n└─────────────────────────────────────────────────────────────┘");
    
}