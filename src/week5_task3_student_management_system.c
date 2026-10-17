
/*
 * week5_task3_student_management_system.c
 * Name: Ziya  Surname: Pirizade  Student ID: 251ADB128
 *
 * Task 3: Student management system with file persistence
 * Week 5 – Files & Modular Programming
 *
 * Build: make lab5
 * Run: ./bin/week5_task3_student_management_system
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define MAX_STUDENTS 100
#define NAME_LEN 50
#define LINE_LEN 256
#define DATA_FILE "students.txt"

typedef struct Student {
    char name[NAME_LEN];
    int id;
    float gpa;
} Student;

/* Function prototypes required by the autograder. */
int load_students(Student arr[], int max);
int save_students(const Student arr[], int count);
void add_student(Student arr[], int *count);
void list_students(const Student arr[], int count);
int read_line(const char *prompt, char *buf, size_t size);

int main(void) {
    Student students[MAX_STUDENTS];
    int count = 0;
    char input[LINE_LEN];
    int running = 1;

    count = load_students(students, MAX_STUDENTS);
    printf("Loaded %d student(s) from %s\n", count, DATA_FILE);

    while (running) {
        printf("\n=== Student Management System ===\n");
        printf("1. List students\n");
        printf("2. Add student\n");
        printf("3. Save and Exit\n");

        int choice;

        if (read_line("Select an option: ", input, sizeof input) != 0) {
            choice = 3;
        } else {
            char *end;
            long value = strtol(input, &end, 10);
            choice = (end != input && *end == '\0' &&
                      value >= INT_MIN && value <= INT_MAX)
                         ? (int)value : 0;
        }

        switch (choice) {
            case 1:
                list_students(students, count);
                break;

            case 2:
                add_student(students, &count);
                break;

            case 3:
                if (save_students(students, count) != 0) {
                    perror(DATA_FILE);
                } else {
                    printf("Data saved. Goodbye!\n");
                }
                running = 0;
                break;

            default:
                printf("Invalid option. Try again.\n");
                break;
        }
    }

    return EXIT_SUCCESS;
}

/* Reads a line and removes its trailing newline. */
int read_line(const char *prompt, char *buf, size_t size) {
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buf, (int)size, stdin) == NULL) {
        return -1;
    }

    size_t len = strcspn(buf, "\r\n");

    if (buf[len] == '\0' && len == size - 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }

    buf[len] = '\0';
    return 0;
}

/* Loads valid comma-separated records from the data file. */
int load_students(Student arr[], int max) {
    if (arr == NULL || max <= 0) {
        return 0;
    }

    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    char line[LINE_LEN];
    int count = 0;

    while (count < max && fgets(line, sizeof line, fp) != NULL) {
        Student current = {0};

        if (sscanf(line, "%49[^,],%d,%f",
                   current.name, &current.id, &current.gpa) == 3) {
            if (current.name[0] != '\0') {
                arr[count] = current;
                count++;
            }
        }
    }

    fclose(fp);
    return count;
}

/* Saves all records, replacing the existing data file. */
int save_students(const Student arr[], int count) {
    if (count < 0 || (count > 0 && arr == NULL)) {
        return -1;
    }

    FILE *fp = fopen(DATA_FILE, "w");
    if (fp == NULL) {
        return -1;
    }

    int result = 0;

    for (int i = 0; i < count; i++) {
        if (fprintf(fp, "%s,%d,%.2f\n",
                    arr[i].name, arr[i].id, arr[i].gpa) < 0) {
            result = -1;
            break;
        }
    }

    if (fclose(fp) != 0) {
        result = -1;
    }

    return result;
}

/* Reads, validates and adds one student. */
void add_student(Student arr[], int *count) {
    if (arr == NULL || count == NULL) {
        return;
    }

    if (*count >= MAX_STUDENTS) {
        printf("Cannot add more students (limit reached).\n");
        return;
    }

    char name[LINE_LEN];
    char id_input[LINE_LEN];
    char gpa_input[LINE_LEN];

    if (read_line("Enter name: ", name, sizeof name) != 0) {
        return;
    }

    if (read_line("Enter ID: ", id_input, sizeof id_input) != 0) {
        return;
    }

    if (read_line("Enter GPA: ", gpa_input, sizeof gpa_input) != 0) {
        return;
    }

    if (name[0] == '\0' ||
        strlen(name) >= NAME_LEN ||
        strchr(name, ',') != NULL) {
        printf("Invalid input. Student not added.\n");
        return;
    }

    errno = 0;
    char *id_end;
    long id_value = strtol(id_input, &id_end, 10);

    while (*id_end == ' ' || *id_end == '\t') {
        id_end++;
    }

    if (id_end == id_input ||
        *id_end != '\0' ||
        errno == ERANGE ||
        id_value < INT_MIN ||
        id_value > INT_MAX) {
        printf("Invalid input. Student not added.\n");
        return;
    }

    errno = 0;
    char *gpa_end;
    float gpa_value = strtof(gpa_input, &gpa_end);

    while (*gpa_end == ' ' || *gpa_end == '\t') {
        gpa_end++;
    }

    if (gpa_end == gpa_input ||
        *gpa_end != '\0' ||
        errno == ERANGE ||
        !(gpa_value >= 0.0f && gpa_value <= 4.0f)) {
        printf("Invalid input. Student not added.\n");
        return;
    }

    strcpy(arr[*count].name, name);
    arr[*count].id = (int)id_value;
    arr[*count].gpa = gpa_value;

    (*count)++;

    printf("Student added successfully!\n");
}

/* Lists all students currently stored in memory. */
void list_students(const Student arr[], int count) {
    if (count == 0) {
        printf("No students found.\n");
        return;
    }

    printf("%-5s %-20s %s\n", "ID", "Name", "GPA");

    for (int i = 0; i < count; i++) {
        printf("%-5d %-20s %.2f\n",
               arr[i].id, arr[i].name, arr[i].gpa);
    }
}
