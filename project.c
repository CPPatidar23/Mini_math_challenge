#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ====== PREPROCESSOR MACROS & STRUCTURES ======
#define MAX_QUESTIONS 20
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

typedef struct {
    char name[50];
    int score;
    double percentage;
    int difficulty;
} Player;

typedef union {
    int intAns;
    float floatAns;
} Question;


// ===========================================================
// ========== SECTION 1 : INPUT HANDLING & BASIC UI ==========
// ===========================================================

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void banner() {
    printf(CYAN "\n***********************************\n");
    printf("*      MINI MATH CHALLENGE GAME   *\n");
    printf("***********************************\n\n" RESET);
}

void instructions() {
    printf(YELLOW "Instructions:\n" RESET);
    printf(" - Choose difficulty and number of questions.\n");
    printf(" - Answer math problems (+, -, *) quickly.\n");
    printf(" - Scores and time are saved.\n");
    printf(" - Try to beat your past records!\n\n");
}

int getMaxNumber(int difficulty) {
    switch (difficulty) {
        case 1: return 10;
        case 2: return 50;
        case 3: return 100;
        default: return 10;
    }
}


// ===========================================================
// ========== SECTION 2 : QUIZ LOGIC & EVALUATION ============
// ===========================================================

void playQuiz(Player *p, int totalQuestions) {
    Question *answers = (Question*)malloc(totalQuestions * sizeof(Question));
    int *userAns = (int*)malloc(totalQuestions * sizeof(int));
    int correct = 0;

    srand(time(0)); 

    for (int i = 0; i < totalQuestions; i++) {
        int max = getMaxNumber(p->difficulty);
        int a = rand() % max, b = rand() % max;
        int op = rand() % 3;
        char opChar;

        switch (op) {
            case 0: opChar = '+'; answers[i].intAns = a + b; break;
            case 1: opChar = '-'; answers[i].intAns = a - b; break;
            default: opChar = '*'; answers[i].intAns = a * b;
        }

        printf("Q%d: %d %c %d = ", i + 1, a, opChar, b);
        if (scanf("%d", &userAns[i]) != 1) {
            clearInputBuffer();
            printf(RED "Invalid! Skipped.\n" RESET);
            userAns[i] = -9999;
            continue;
        }

        if (userAns[i] == answers[i].intAns) {
            printf(GREEN "Correct!\n" RESET);
            correct++;
        } else {
            printf(RED "Wrong! Ans = %d\n" RESET, answers[i].intAns);
        }
    }

    p->score = correct;
    p->percentage = (correct * 100.0) / totalQuestions;

    printf("\n==== Result for %s ====\n", p->name);
    printf("Score: %d/%d (%.2f%%)\n", p->score, totalQuestions, p->percentage);

    if (p->percentage == 100)
        printf(GREEN "Excellent! Perfect score!\n" RESET);
    else if (p->percentage >= 70)
        printf(YELLOW "Good job! Keep practicing.\n" RESET);
    else
        printf(RED "Don't give up! Try again.\n" RESET);

    free(answers);
    free(userAns);
}


// ===========================================================
// ======== SECTION 3 : FILE HANDLING (SAVE & SHOW) ==========
// ===========================================================

void saveResult(Player p) {
    FILE *fp = fopen("scores.txt", "a");
    if (fp == NULL) {
        printf(RED "Error saving score!\n" RESET);
        return;
    }
    fprintf(fp, "%s %d %.2f Difficulty:%d\n",
            p.name, p.score, p.percentage, p.difficulty);
    fclose(fp);
}

void showPastScores() {
    FILE *fp = fopen("scores.txt", "r");
    char line[100];
    if (fp == NULL) {
        printf("No past scores found.\n");
        return;
    }
    printf("\n=== Past Scores ===\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}


// ===========================================================
// ============ SECTION 4 : MENU & MAIN FUNCTION =============
// ===========================================================

void menu(Player *p) {
    int choice, totalQuestions;

    banner();
    printf("1. Start Quiz\n2. Instructions\n3. Show Past Scores\n4. Exit\n");
    printf("Enter choice: ");
    if (scanf("%d", &choice) != 1) {
        clearInputBuffer();
        menu(p);
        return;
    }

    switch (choice) {
        case 1:
            printf("Select difficulty (1=Easy, 2=Medium, 3=Hard): ");
            scanf("%d", &p->difficulty);

            printf("How many questions (1-%d): ", MAX_QUESTIONS);
            scanf("%d", &totalQuestions);
            if (totalQuestions < 1 || totalQuestions > MAX_QUESTIONS)
                totalQuestions = 5;

            playQuiz(p, totalQuestions);
            saveResult(*p);

            printf("\nPlay again? (yes/no): ");
            char ans[10];
            scanf("%s", ans);
            if (strcmp(ans, "yes") == 0 || strcmp(ans, "y") == 0)
                menu(p);
            break;

        case 2:
            instructions();
            menu(p);
            break;

        case 3:
            showPastScores();
            menu(p);
            break;

        case 4:
            printf("Goodbye, %s!\n", p->name);
            exit(0);

        default:
            printf("Invalid choice!\n");
            menu(p);
    }
}

int main() {
    Player player;
    printf("Enter your name: ");
    scanf("%s", player.name);

    menu(&player);
    return 0;
}
