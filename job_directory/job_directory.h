#ifndef JOB_DIRECTORY_H
#define JOB_DIRECTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h> // For case-insensitive comparison
#include <openssl/evp.h>


// ANSI Color Codes for styling output
#define GREEN "\033[1;32m"   // Success messages
#define RED "\033[1;31m"     // Errors / Warnings
#define YELLOW "\033[1;33m"  // Prompts / Warnings
#define BLUE "\033[1;34m"    // Section Titles
#define RESET "\033[0m"      // Reset color to default



/* Maximum lengths for job listing fields */
#define MAX_TITLE_LENGTH 100
#define MAX_COMPANY_LENGTH 100
#define MAX_LOCATION_LENGTH 100
#define MAX_DESCRIPTION_LENGTH 500
#define HASH_LENGTH 64
#define DIFFICULTY 4
#define MAX_BLOCKS 1000
#define BLOCKCHAIN_FILE "job_directory.dat"

/* Job Listing Structure */
typedef struct Job {
    int index;
    time_t timestamp;
    char title[MAX_TITLE_LENGTH];
    char company[MAX_COMPANY_LENGTH];
    char location[MAX_LOCATION_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    char prev_hash[HASH_LENGTH + 1];
    char curr_hash[HASH_LENGTH + 1];
    int nonce;
    struct Job *next;
} Job;

/* Blockchain Structure */
typedef struct Blockchain {
    Job *head;
    int size;
} Blockchain;

/* Function Prototypes */
Blockchain *initialize_blockchain(void);
void generate_hash(const char *input, char *output);
void compute_hash(Job *job);
int add_job(Blockchain *chain, const char *title, const char *company, const char *location, const char *description);
Job *search_jobs(Blockchain *chain, const char *keyword);
int verify_blockchain(Blockchain *chain);
void free_blockchain(Blockchain *chain);
void print_job(Job *job);
void save_blockchain(Blockchain *chain);
void load_blockchain(Blockchain *chain);
void get_string_input(const char *prompt, char *buffer, size_t size);
void display_menu(void);
void to_lowercase(char *str);

#endif
