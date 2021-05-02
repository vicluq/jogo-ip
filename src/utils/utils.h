float distanceToEnemy(float pX, float pY, float eX, float eY);
void renderMenu(Texture2D MenuCape, Vector2 *mouse, int screenWidth, int screenHeight, Rectangle menuOptions[], int *menuMode, int *introMode, double *introStart, int *closeGame);
void enemyDynamic(Rectangle player, Rectangle *enemy, int speed, float *playerLifeBar);
void renderEnemies(float playerH, float playerW, float pX, float pY, Rectangle *enemies, int enemyAmount);
void renderEnemyLifeBar(Rectangle *enemies, Rectangle *lifebars, int enemyAmount);
char introText[400];
char instructions[90];