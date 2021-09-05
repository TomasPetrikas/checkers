#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define True        1
#define False       0
#define ROWS        8
#define COLS        8
#define BLACK_ROWS  3
#define WHITE_ROWS  5
#define MAX_STR_LEN 100
#define SAVE_SIZE   83  // Visi issaugoti zaidimai uzima lygiai tiek baitu

/*
https://i.redd.it/bl1y2bwj3h021.jpg

Sioje implementacijoje chain'ingo nera (negalima kirsti daugiau nei 1 saske) ir
kirsti nebutina. Atgal kirsti gali tik damos.

Chain'inga galbut pabandysiu 2-am namu darbui.
*/

// Langelis
typedef struct Square {
    char side;    // 'w' jei balta, 'b' jei juoda, ' ' jei tuscias
                  // kai dama, 'w' tampa 'W' ir 'b' tampa 'B'
} Square;

// Lenta
typedef struct Board {
    Square array[COLS][ROWS];
} Board;

/*****************************************************************************/

void printTitle() {
    printf("   _____ _    _ ______ _____ _  ________ _____   _____ \n");
    printf("  / ____| |  | |  ____/ ____| |/ /  ____|  __ \\ / ____|\n");
    printf(" | |    | |__| | |__ | |    | ' /| |__  | |__) | (___  \n");
    printf(" | |    |  __  |  __|| |    |  < |  __| |  _  / \\___ \\ \n");
    printf(" | |____| |  | | |___| |____| . \\| |____| | \\ \\ ____) |\n");
    printf("  \\_____|_|  |_|______\\_____|_|\\_\\______|_|  \\_\\_____/ \n");
    printf("Tomas Petrikas 2018, All Rights Reserved\n\n");
}

// Si funkcija paimta is 7 pratybu, 6 uzduoties
// Meniu galetu buti ir struktura, bet tingiu perrasyti
int showMenu(char *menuTitle, char *menuOptions[], int menuSize, char *inputMsg) {
    int chosenOption;
    while (True) {
        printf("\n%s\n", menuTitle);
        for (int i = 0; i < menuSize; i++) {
            printf("%d: %s\n", i, menuOptions[i]);
        }

        printf("\n%s", inputMsg);
        if ((scanf("%d", &chosenOption) == 1) && (getchar() == '\n')) {
            if (chosenOption >= 0 && chosenOption < menuSize) {
                break;
            }
            else {
                continue;
            }
        }
        else {
            while (getchar() != '\n')
                ;
            continue;
        }
    }

    return chosenOption;
}

void initializeBoard(Board *board) {
    // Pirma visus langelius nustatome tusciais
    for (int col = 0; col < COLS; col++) {
        for (int row = 0; row < ROWS; row++) {
            board->array[col][row].side = ' ';
        }
    }

    // Inicializuojame juodasias saskes
    for (int col = 0; col < COLS; col++) {
        for (int row = 0; row < BLACK_ROWS; row++) {
            // Neat triukas pastatyti saskes kur reikia
            if ((col + row) % 2 == 1) {
                board->array[col][row].side = 'b';
            }
        }
    }

    // Inicializuojame baltasias saskes
    for (int col = 0; col < COLS; col++) {
        for (int row = WHITE_ROWS; row < ROWS; row++) {
            if ((col + row) % 2 == 1) {
                board->array[col][row].side = 'w';
            }
        }
    }
}

void printBoard(Board board) {
    printf(" A B C D E F G H\n");
    for (int row = 0; row < ROWS; row++) {
        printf("+-+-+-+-+-+-+-+-+\n");
        for (int col = 0; col < COLS; col++) {
            printf("|");
            printf("%c", board.array[col][row].side);
        }
        printf("| %d\n", row);
    }
    printf("+-+-+-+-+-+-+-+-+\n");
    printf(" A B C D E F G H\n");
}

// Kartojasi kol neduodamas korektiskas failo vardas
void filenameInputValidation(char *filename) {
    while (True) {
        printf("Iveskite .txt failo pavadinima: ");
        if ((scanf("%s", filename) == 1) && (getchar() == '\n')) {
            if (strlen(filename) < MAX_STR_LEN) {
                int isAlphaNumeric = True;
                for (int i = 0; i < strlen(filename); i++) {
                    if (!(isalnum(filename[i]))) {
                        isAlphaNumeric = False;
                    }
                }
                if (isAlphaNumeric) {
                    break;
                }
                else {
                    printf("Error: not alphanumeric\n");
                }
            }
            else {
                printf("Error: name too long\n");
            }
        }
        else {
            while (getchar() != '\n')
                ;
            printf("Error: no spaces please\n");
        }
    }
}

// Paimta is 7 pratybu, 5 uzduoties
// Grazina failo dydi baitais ir -1 jei failo nera
int getFileSize(char *fileName) {
    FILE *f_source;
    f_source = fopen(fileName, "r");
    if (f_source == NULL) {
        return -1;
    }
    else {
        fseek(f_source, 0, SEEK_END);
    }
    return ftell(f_source);
}

// Dalinai paimta is 6 pratybu, 3 uzduoties
// Issaugoja dabartine lenta ir kieno ejimas i .txt faila
// Geba issaugoti tik dirbamajame aplanke
void saveBoard(Board board, char turn) {
    char f_out[MAX_STR_LEN];

    filenameInputValidation(f_out);

    int len = strlen(f_out);

    // Pridedame .txt galune
    f_out[len] = '.';
    f_out[len+1] = 't';
    f_out[len+2] = 'x';
    f_out[len+3] = 't';
    f_out[len+4] = '\0';

    FILE *fp = fopen(f_out, "w");
    fprintf(fp, "%c\n", turn);

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            // Jei langelis tuscias, rasome 'E' kad nebutu whitespace'as
            if (board.array[col][row].side == ' ') {
                fprintf(fp, "E");
            }
            else {
                fprintf(fp, "%c", board.array[col][row].side);
            }
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

// Uzkrauna lenta is failo
void loadBoard(Board *board, char *turn) {
    char f_in[MAX_STR_LEN];

    while (True) {
        filenameInputValidation(f_in);

        int len = strlen(f_in);

        // Pridedame .txt galune
        f_in[len] = '.';
        f_in[len+1] = 't';
        f_in[len+2] = 'x';
        f_in[len+3] = 't';
        f_in[len+4] = '\0';

        // Patikriname ar failas korektiskas
        // Jis privalo a) egzistuoti ir b) uzimti konstantini kieki baitu
        if (getFileSize(f_in) == SAVE_SIZE) {
            break;
        }
        else {
            printf("Error: bad file\n");
        }
    }

    FILE *fp = fopen(f_in, "r");
    *turn = fgetc(fp);
    //printf("%c\n", turn);
    fgetc(fp);  // Praleidziame tiketina '\n'
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            char readInput = fgetc(fp);
            if (readInput == 'E') {
                board->array[col][row].side = ' ';
            }
            else {
                board->array[col][row].side = readInput;
            }
        }
        fgetc(fp);  // Cia taip pat praleidziame '\n'
    }
    fclose(fp);
}

// ABCDEFGH -> 01234567
// Crash'ina jei kazkas blogai
int columnToNumber(char column) {
    switch(column) {
        case 'A':
            return 0;
        case 'B':
            return 1;
        case 'C':
            return 2;
        case 'D':
            return 3;
        case 'E':
            return 4;
        case 'F':
            return 5;
        case 'G':
            return 6;
        case 'H':
            return 7;
        default:
            assert(False);
    }
}

// Grazina 1 jei ejimas legalus, 0 jei ne
// Kadangi eina po validacijos, visada gauna korektiskus duomenis (teoriskai)
int isLegalMove(Board board, char turn, char *move) {
    //printf("%s\n", move);

    // '0' yra magiska konstanta konvertuojanti 'c' -> c, kur c = 0..9
    // Garantuota C99 standarto
    int colFrom = columnToNumber(move[0]);
    int rowFrom = move[1] - '0';
    int colTo = columnToNumber(move[3]);
    int rowTo = move[4] - '0';

    //printf("%d %d %d %d\n", colFrom, rowFrom, colTo, rowTo);

    // Tikriname ar turn sutampa su saskes spalva
    if (toupper(board.array[colFrom][rowFrom].side) != turn) {
        return False;
    }

    // Tikriname ar norimas langelis tuscias
    if (board.array[colTo][rowTo].side != ' ') {
        return False;
    }

    // Tikriname ar pozicijos yra toje pacioje istrizaineje (/ arba \)
    if (abs(colFrom - colTo) != abs(rowFrom - rowTo)) {
        return False;
    }

    // Baltos (paprastos) saskes ejimai
    // Column +- 1; Row - 1
    if (board.array[colFrom][rowFrom].side == 'w') {
        if ((colTo == colFrom + 1 || colTo == colFrom - 1) && rowTo == rowFrom - 1 &&
        board.array[colTo][rowTo].side == ' ') {
            return True;
        }

        // Jei persokama juoda saske
        // This is where Tom knew, he fucked up
        if ((colTo == colFrom + 2 || colTo == colFrom - 2) && rowTo == rowFrom - 2 &&
        tolower(board.array[(colTo+colFrom)/2][rowTo + 1].side) == 'b' &&
        board.array[colTo][rowTo].side == ' ') {
            return True;
        }
    }

    // Juodos (paprastos) saskes ejimai
    // Column +- 1; Row + 1
    if (board.array[colFrom][rowFrom].side == 'b') {
        if ((colTo == colFrom + 1 || colTo == colFrom - 1) && rowTo == rowFrom + 1 &&
        board.array[colTo][rowTo].side == ' ') {
            return True;
        }

        // Jei persokama balta saske
        if ((colTo == colFrom + 2 || colTo == colFrom - 2) && rowTo == rowFrom + 2 &&
        tolower(board.array[(colTo+colFrom)/2][rowTo - 1].side) == 'w' &&
        board.array[colTo][rowTo].side == ' ') {
            return True;
        }
    }

    // Damu ejimai

    // Suzinome i kuria puse judeti
    int colAdvance, rowAdvance;
    if (colTo > colFrom) {
        colAdvance = 1;
    }
    else {
        colAdvance = -1;
    }

    if (rowTo > rowFrom) {
        rowAdvance = 1;
    }
    else {
        rowAdvance = -1;
    }

    int colCurrent = colFrom;
    int rowCurrent = rowFrom;
    int opponentHopped = False;

    // Baltos damos ejimai
    // TODO: Galima sita ir kita bloka sujungti, reikia f-jos kuri 'W' pavercia
    // i 'B' ir atvirksciai
    if (board.array[colFrom][rowFrom].side == 'W') {
        while (colCurrent != colTo) {
            colCurrent += colAdvance;
            rowCurrent += rowAdvance;

            // Tikriname ar bandoma persokti tos pacios spalves saske
            if (toupper(board.array[colCurrent][rowCurrent].side) == 'W') {
                return False;
            }

            // Tikriname ar praeitame langelyje buvo persokta juoda saske
            // Jei viena jau buvo persokta, graziname 0
            if (toupper(board.array[colCurrent-colAdvance][rowCurrent-rowAdvance].side) == 'B') {
                if (opponentHopped) {
                    return False;
                }
                opponentHopped = True;
            }
        }

        return True;
    }

    // Juodos damos ejimai
    if (board.array[colFrom][rowFrom].side == 'B') {
        while (colCurrent != colTo) {
            colCurrent += colAdvance;
            rowCurrent += rowAdvance;

            // Tikriname ar bandoma persokti tos pacios spalves saske
            if (toupper(board.array[colCurrent][rowCurrent].side) == 'B') {
                return False;
            }

            // Tikriname ar praeitame langelyje buvo persokta balta saske
            // Jei viena jau buvo persokta, graziname 0
            if (toupper(board.array[colCurrent-colAdvance][rowCurrent-rowAdvance].side) == 'W') {
                if (opponentHopped) {
                    return False;
                }
                opponentHopped = True;
            }
        }

        return True;
    }

    return False;
}

// Grazina 1 jei input'as geras, 0 jei blogas
int moveInputValidation(Board board, char turn, char *move) {
    const char letters[] = "ABCDEFGH";
    const char numbers[] = "01234567";

    // Pirma tikriname ar norima issaugoti
    if (tolower(move[0]) == 's' && tolower(move[1]) == 'a' &&
    tolower(move[2]) == 'v' && tolower(move[3]) == 'e' && move[4] == '\0') {
        printf("Saving...\n\n");
        saveBoard(board, turn);
        // Tada graziname 0, kad butu galima zaisti toliau
        return False;
    }

    // Niekada neturetu virsyti 5 simboliu
    if (strlen(move) > 5) {
        printf("Error: string too long\n\n");
        return False;
    }

    // 1-as ir 3-as simboliai raides, 2-as ir 4-as skaiciai
    // 3-as privalo buti '-'
    if (!(strchr(letters, toupper(move[0])) && strchr(numbers, move[1]) &&
    move[2] == '-' && strchr(letters, toupper(move[3])) && strchr(numbers, move[4]))) {
        printf("Error: bad formatting\n\n");
        return False;
    }

    // Konvertuojame mazasias raides i didziasias kad nebutu problemu veliau
    move[0] = toupper(move[0]);
    move[3] = toupper(move[3]);

    // Tikriname ar ejimas legalus
    if (isLegalMove(board, turn, move) == False) {
        printf("Error: illegal move\n\n");
        return False;
    }

    return True;
}

// Pakeicia lenta pagal nurodyta ejima
// Naudojamas po validacijos, tikimasi korektisku duomenu
void makeMove(Board *board, char *move) {
    // Kodas pavogtas is isLegalMove, galetu buti savo funkcija?
    int colFrom = columnToNumber(move[0]);
    int rowFrom = move[1] - '0';
    int colTo = columnToNumber(move[3]);
    int rowTo = move[4] - '0';

    char movedPiece = board->array[colFrom][rowFrom].side;
    // Istriname saske is senos pozicijos
    board->array[colFrom][rowFrom].side = ' ';

    // Darome ejima, ieskodami kirstamos saskes, jei tokia yra
    // Tinka paprastoms saskems ir damoms
    // 4 skirtingi atvejai kur keiciasi zenklai
    // Galima geriau padaryti?
    int max = abs(rowFrom - rowTo);
    int min = 1;
    if (rowTo > rowFrom && colTo > colFrom) {
        for (int i = min; i <= max; i++) {
            board->array[colFrom+i][rowFrom+i].side = ' ';
        }
    }
    if (rowTo > rowFrom && colTo < colFrom) {
        for (int i = min; i <= max; i++) {
            board->array[colFrom-i][rowFrom+i].side = ' ';
        }
    }
    if (rowTo < rowFrom && colTo > colFrom) {
        for (int i = min; i <= max; i++) {
            board->array[colFrom+i][rowFrom-i].side = ' ';
        }
    }
    if (rowTo < rowFrom && colTo < colFrom) {
        for (int i = min; i <= max; i++) {
            board->array[colFrom-i][rowFrom-i].side = ' ';
        }
    }

    // Pajudiname saske
    board->array[colTo][rowTo].side = movedPiece;

    // Paverciame saske dama, jei to reikia
    if (rowTo == 0 && movedPiece == 'w') {
        board->array[colTo][rowTo].side = 'W';
    }
    else if (rowTo == 7 && movedPiece == 'b') {
        board->array[colTo][rowTo].side = 'B';
    }

}

// Gauna saske ir tikrina ar ta saske turi bent viena legalu ejima
// Grazina 1 jei turi, 0 jei ne
// NOTE: col turi buti skaicius tarp [0; 7], ne raide
int hasLegalMove(Board board, int col, int row) {
    // Keli patikrinimai isitikinti kad duomenis geri, crash'ina jei ne
    char piece = board.array[col][row].side;
    if (col < 0 || col > COLS - 1 || row < 0 || row > ROWS - 1) {
        assert(False);
    }
    if (piece == ' ') {
        assert(False);
    }

    const char columns[] = "ABCDEFGH";
    char move[5];
    move[0] = columns[col];
    move[1] = row + '0';
    move[2] = '-';
    //printf("Move[0] -- %c\n", move[0]);
    //printf("Move[1] -- %c\n", move[1]);

    // Paprastos saskes
    // 8 atvejai (4 ejimo ir 4 kirtimo) kur keiciasi zenklai
    // I'm sorry for the PTSD
    if (col + 1 < COLS && row + 1 < ROWS) {
        move[3] = columns[col+1];
        move[4] = (row+1) + '0';
        if (isLegalMove(board, toupper(piece), move)) {
            return True;
        }

        if (col + 2 < COLS && row + 2 < ROWS &&
        tolower(board.array[col+1][row+1].side) != piece &&
        tolower(board.array[col+1][row+1].side) != ' ') {
            move[3] = columns[col+2];
            move[4] = (row+2) + '0';
            if (isLegalMove(board, toupper(piece), move)) {
                return True;
            }
        }
    }

    if (col + 1 < COLS && row - 1 >= 0) {
        move[3] = columns[col+1];
        move[4] = (row-1) + '0';
        if (isLegalMove(board, toupper(piece), move)) {
            return True;
        }

        if (col + 2 < COLS && row - 2 >= 0 &&
        tolower(board.array[col+1][row-1].side) != piece &&
        tolower(board.array[col+1][row-1].side) != ' ') {
            move[3] = columns[col+2];
            move[4] = (row-2) + '0';
            if (isLegalMove(board, toupper(piece), move)) {
                return True;
            }
        }
    }

    if (col - 1 >= 0 && row + 1 < ROWS) {
        move[3] = columns[col-1];
        move[4] = (row+1) + '0';
        if (isLegalMove(board, toupper(piece), move)) {
            return True;
        }

        if (col - 2 >= 0 && row + 2 < ROWS &&
        tolower(board.array[col-1][row+1].side) != piece &&
        tolower(board.array[col-1][row+1].side) != ' ') {
            move[3] = columns[col-2];
            move[4] = (row+2) + '0';
            if (isLegalMove(board, toupper(piece), move)) {
                return True;
            }
        }
    }

    if (col - 1 >= 0 && row - 1 >= 0) {
        move[3] = columns[col-1];
        move[4] = (row-1) + '0';
        if (isLegalMove(board, toupper(piece), move)) {
            return True;
        }

        if (col - 2 >= 0 && row - 2 >= 0 &&
        tolower(board.array[col-1][row-1].side) != piece &&
        tolower(board.array[col-1][row-1].side) != ' ') {
            move[3] = columns[col-2];
            move[4] = (row-2) + '0';
            if (isLegalMove(board, toupper(piece), move)) {
                return True;
            }
        }
    }

    return False;
}

// Grazina 'W' arba 'B' jei kazkas laimejo
// Grazina ' ' jei niekas nelaimejo
char gameIsWon(Board board) {
    // Tikriname ar is likusiu saskiu, nors viena turi legalu ejima
    // Jei ju neliko, tai legaliu ejimu irgi nera
    int whiteHasMoves = False, blackHasMoves = False;

    for (int col = 0; col < COLS; col++) {
        for (int row = 0; row < ROWS; row++) {
            if (toupper(board.array[col][row].side) == 'W') {
                if (hasLegalMove(board, col, row)) {
                    //printf("White at %d %d has moves\n", col, row);
                    whiteHasMoves = True;
                }
            }
            if (toupper(board.array[col][row].side) == 'B') {
                if (hasLegalMove(board, col, row)) {
                    //printf("Black at %d %d has moves\n", col, row);
                    blackHasMoves = True;
                }
            }
        }
    }

    if (whiteHasMoves == False) {
        return 'B';
    }
    if (blackHasMoves == False) {
        return 'W';
    }

    return ' ';
}

// Pagrindinis zaidimo loop'as
void play(Board *board, char turn) {
    char move[MAX_STR_LEN];
    while (True) {
        // Turn privalo buti vienas is dvieju
        assert(turn == 'W' || turn == 'B');

        printBoard(*board);

        // Tikriname kiekviena karta ar kazkas laimejo
        char winner = gameIsWon(*board);
        if (winner == 'W') {
            printf("\nWHITE has won!\n");
            // Laimejus is programos iseinama
            exit(0);
        }
        else if (winner == 'B') {
            printf("\nBLACK has won!\n");
            exit(0);
        }

        if (turn == 'W') printf("\nWHITE'S turn.\n");
        else printf("\nBLACK's turn.\n");
        printf("Please enter your move: ");

        scanf("%s", move);
        if (!moveInputValidation(*board, turn, move)) {
            continue;
        }
        makeMove(board, move);

        if (turn == 'W') turn = 'B';
        else turn = 'W';

        //break;
    }
}

/*****************************************************************************/

int main() {
    Board board;
    initializeBoard(&board);
    printTitle();

    char mainMenuTitle[] = "Welcome to Checkers!";
    int mainMenuSize = 2;
    char *mainMenuOptions[mainMenuSize];
    mainMenuOptions[0] = "Play a game.";
    mainMenuOptions[1] = "Load from file.";
    char mainInputMsg[] = "Please make a selection: ";

    int selection = showMenu(mainMenuTitle, mainMenuOptions, mainMenuSize, mainInputMsg);
    char turn = 'W';  // Pirmas ejimas balto zaidejo

    if (selection == 0) {
        printf("\n--------------------\n");
        printf("NOTE! All moves must be entered in the following format:\n");
        printf("'A5-B4' or 'h2-g3'\n");
        printf("Enter 'save' to save the current board position\n");
        printf("--------------------\n\n");
        play(&board, turn);
    }
    else if (selection == 1) {
        printf("\n");
        loadBoard(&board, &turn);
        play(&board, turn);
    }

    return 0;
}
