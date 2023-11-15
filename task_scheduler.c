#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CATEGORY_ASSIGNMENT 1
#define CATEGORY_PROJECT 2
#define CATEGORY_EXAM 3

struct Task {
    int id;
    char name[100];
    time_t deadline;
    int category;
    int priority;
    struct Task* next;
};

struct Task* head = NULL;
int taskCount = 0;

int isValidDay(int year, int month, int day) {
    if (year < 1900 || month < 1 || month > 12 || day < 1)
        return 0;

    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
        daysInMonth[2] = 29;

    return day <= daysInMonth[month];
}

int isValidDate(int year, int month, int day) {
    if (!isValidDay(year, month, day))
        return 0;

    return (year >= 1900 && month >= 1 && month <= 12);
}

void addTask(char name[], int year, int month, int day, int category, int priority) {
    if (!isValidDate(year, month, day)) {
        printf("Invalid date. Please enter a valid date.\n");
        return;
    }

    struct Task* newTask = (struct Task*)malloc(sizeof(struct Task));
    if (newTask == NULL) {
        printf("Memory allocation failed. Task could not be added.\n");
        return;
    }
    newTask->id = ++taskCount;
    strcpy(newTask->name, name);

    struct tm dueDate = {0};
    dueDate.tm_year = year - 1900;
    dueDate.tm_mon = month - 1;
    dueDate.tm_mday = day;
    newTask->deadline = mktime(&dueDate);

    time_t currentTime;
    time(&currentTime);

    if (newTask->deadline < currentTime) {
        printf("Invalid deadline. The deadline cannot be earlier than the current date.\n");
        free(newTask);
        return;
    }

    newTask->category = category;
    newTask->priority = priority;
    newTask->next = head;
    head = newTask;

    printf("Task \"%s\" added with ID %d.\n", newTask->name, newTask->id);
}

void removeTask(int id) {
    struct Task* current = head;
    struct Task* prev = NULL;

    while (current != NULL) {
        if (current->id == id) {
            if (prev == NULL) {
                head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            printf("Task with ID %d removed.\n", id);
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("Task with ID %d not found.\n");
}

void listTasks() {
    struct Task* current = head;

    if (current == NULL) {
        printf("No tasks to display.\n");
        return;
    }

    time_t currentTime;
    time(&currentTime);

    printf("Task List:\n");
    while (current != NULL) {
        char date[20];
        strftime(date, sizeof(date), "%Y-%m-%d", localtime(&current->deadline));
        char categoryStr[20];
        switch (current->category) {
            case CATEGORY_ASSIGNMENT:
                strcpy(categoryStr, "Assignment");
                break;
            case CATEGORY_PROJECT:
                strcpy(categoryStr, "Project");
                break;
            case CATEGORY_EXAM:
                strcpy(categoryStr, "Exam");
                break;
            default:
                strcpy(categoryStr, "Unknown");
        }

        double timeLeftInSeconds = difftime(current->deadline, currentTime);
        int daysLeft = timeLeftInSeconds / (60 * 60 * 24);
        int hoursLeft = (timeLeftInSeconds - daysLeft * 60 * 60 * 24) / (60 * 60);
        int minutesLeft = (timeLeftInSeconds - daysLeft * 60 * 60 * 24 - hoursLeft * 60 * 60) / 60;

        if (current->deadline < currentTime) {
            printf("Task ID: %d, Name: %s, Deadline: %s, Category: %s, Priority: %d (Deadline passed)\n",
                   current->id, current->name, date, categoryStr, current->priority);
        } else {
            printf("Task ID: %d, Name: %s, Deadline: %s, Category: %s, Priority: %d, Time Left: %d days, %d hours, %d minutes\n",
                   current->id, current->name, date, categoryStr, current->priority, daysLeft, hoursLeft, minutesLeft);
        }

        current = current->next;
    }
}

void sortTasksByPriority() {
    struct Task* current = head;
    struct Task* newHead = NULL;

    while (current != NULL) {
        struct Task* next = current->next;

        if (newHead == NULL || current->priority < newHead->priority) {
            current->next = newHead;
            newHead = current;
        } else {
            struct Task* temp = newHead;
            while (temp->next != NULL && temp->next->priority <= current->priority) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }

        current = next;
    }

    head = newHead;
}

int main() {
    while (1) {
        printf("\nTask Scheduler Menu:\n");
        printf("1. Add Task\n");
        printf("2. Remove Task\n");
        printf("3. List Tasks\n");
        printf("4. Sort Tasks by Priority\n");
        printf("5. Quit\n");
        printf("Select an option: ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        int taskId;
        char taskName[100];
        int year, month, day, category, priority;

        switch (choice) {
            case 1:
                printf("Enter task name: ");
                scanf(" %[^\n]", taskName);
                printf("Enter task deadline (YYYY MM DD): ");
                if (scanf("%d %d %d", &year, &month, &day) != 3) {
                    printf("Invalid date format. Please enter the date in the format 'YYYY MM DD'.\n");
                    while (getchar() != '\n');
                    continue;
                }
                printf("Enter task category (1: Assignment, 2: Project, 3: Exam): ");
                if (scanf("%d", &category) != 1 || (category < 1 || category > 3)) {
                    printf("Invalid category. Please enter a valid category (1, 2, or 3).\n");
                    while (getchar() != '\n');
                    continue;
                }
                printf("Enter task priority (1 to 5, with 1 being the highest): ");
                if (scanf("%d", &priority) != 1 || (priority < 1 || priority > 5)) {
                    printf("Invalid priority. Please enter a valid priority (1 to 5).\n");
                    while (getchar() != '\n');
                    continue;
                }
                addTask(taskName, year, month, day, category, priority);
                break;
            case 2:
                printf("Enter task ID to remove: ");
                if (scanf("%d", &taskId) != 1) {
                    printf("Invalid input. Please enter a number.\n");
                    while (getchar() != '\n');
                    continue;
                }
                removeTask(taskId);
                break;
            case 3:
                listTasks();
                break;
            case 4:
                sortTasksByPriority();
                printf("Tasks sorted by priority.\n");
                break;
            case 5:
                while (head != NULL) {
                    struct Task* temp = head;
                    head = head->next;
                    free(temp);
                }
                exit(0);
            default:
                printf("Invalid choice. Please select a valid option.\n");
        }
    }

    return 0;
}
