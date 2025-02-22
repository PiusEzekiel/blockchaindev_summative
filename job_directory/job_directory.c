#include "job_directory.h"
#include "ansi_colors.h"

/**
 * generate_hash - Generate SHA-256 hash from input string
 */
void generate_hash(const char *input, char *output) {
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha256();
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, input, strlen(input));
    EVP_DigestFinal_ex(mdctx, hash, &hash_len);

    for (unsigned int i = 0; i < hash_len; i++)
        sprintf(output + (i * 2), "%02x", hash[i]);

    output[HASH_LENGTH] = '\0';
    EVP_MD_CTX_free(mdctx);
}

/**
 * initialize_blockchain - Creates a new blockchain
 */
Blockchain *initialize_blockchain(void) {
    Blockchain *chain = malloc(sizeof(Blockchain));
    if (!chain) return NULL;
    chain->head = NULL;
    chain->size = 0;
    return chain;
}

/**
 * compute_hash - Compute hash for a job listing
 */
void compute_hash(Job *job) {
    char buffer[MAX_TITLE_LENGTH + MAX_COMPANY_LENGTH +
                MAX_LOCATION_LENGTH + MAX_DESCRIPTION_LENGTH +
                HASH_LENGTH + 100];

    sprintf(buffer, "%d%s%s%s%s%ld%s", job->index, job->title,
            job->company, job->location, job->description,
            job->timestamp, job->prev_hash);

    generate_hash(buffer, job->curr_hash);
}

/**
 * add_job - Add a new job to the blockchain
 */
int add_job(Blockchain *chain, const char *title,
            const char *company, const char *location,
            const char *description) {
    Job *new_job = malloc(sizeof(Job));
    if (!new_job) return 0;

    new_job->index = chain->size;
    strncpy(new_job->title, title, MAX_TITLE_LENGTH - 1);
    strncpy(new_job->company, company, MAX_COMPANY_LENGTH - 1);
    strncpy(new_job->location, location, MAX_LOCATION_LENGTH - 1);
    strncpy(new_job->description, description, MAX_DESCRIPTION_LENGTH - 1);
    new_job->timestamp = time(NULL);

    if (chain->head)
        strcpy(new_job->prev_hash, chain->head->curr_hash);
    else
        strcpy(new_job->prev_hash, "0000000000000000000000000000000000000000000000000000000000000000");

    compute_hash(new_job);
    new_job->next = chain->head;
    chain->head = new_job;
    chain->size++;

    save_blockchain(chain);
    printf(GREEN "✅ Job added successfully and mined as Block %d!\n" RESET, new_job->index);
    return 1;
}

/**
 * verify_blockchain - Check blockchain integrity
 */
int verify_blockchain(Blockchain *chain) {
    Job *current = chain->head;
    char temp_hash[HASH_LENGTH + 1];

    while (current && current->next) {
        compute_hash(current);
        strcpy(temp_hash, current->curr_hash);

        if (strcmp(temp_hash, current->curr_hash) != 0) {
            printf(RED "⚠️ Integrity violation detected at Block %d!\n" RESET, current->index);
            return 0;
        }

        if (strcmp(current->prev_hash, current->next->curr_hash) != 0) {
            printf(RED "⚠️ Blockchain broken between Blocks %d and %d!\n" RESET, current->index, current->next->index);
            return 0;
        }

        current = current->next;
    }
    printf(GREEN "✅ Blockchain integrity verified. No issues detected.\n" RESET);
    return 1;
}


/**
 * search_jobs - Searches for jobs by keyword.
 */
Job *search_jobs(Blockchain *chain, const char *keyword) {
    Job *current = chain->head;
    Job *results = NULL;
    Job *last_result = NULL;

    while (current) {
        if (strstr(current->title, keyword) || strstr(current->company, keyword) || strstr(current->location, keyword)) {
            Job *match = (Job *)malloc(sizeof(Job));
            if (!match) {
                current = current->next;
                continue;
            }
            memcpy(match, current, sizeof(Job));
            match->next = NULL;

            if (!results) {
                results = match;
                last_result = match;
            } else {
                last_result->next = match;
                last_result = match;
            }
        }
        current = current->next;
    }

    return results;
}


/**
 * save_blockchain - Saves the blockchain to a file.
 */
void save_blockchain(Blockchain *chain) {
    FILE *file = fopen(BLOCKCHAIN_FILE, "wb");
    if (!file) {
        printf(RED "❌ Error: Unable to save blockchain.\n" RESET);
        return;
    }

    Job *current = chain->head;
    while (current) {
        fwrite(current, sizeof(Job), 1, file);
        current = current->next;
    }

    fclose(file);
    printf(GREEN "✅ Blockchain saved successfully!\n" RESET);
}

/**
 * load_blockchain - Loads blockchain from a file.
 */
void load_blockchain(Blockchain *chain) {
    FILE *file = fopen(BLOCKCHAIN_FILE, "rb");
    if (!file) {
        printf(YELLOW "⚠️ No existing blockchain found. Starting fresh.\n" RESET);
        return;
    }

    // Clear existing blockchain before loading
    while (chain->head) {
        Job *temp = chain->head;
        chain->head = chain->head->next;
        free(temp);
    }
    chain->size = 0;

    Job *last_job = NULL;

    while (1) {
        Job *job = (Job *)malloc(sizeof(Job));
        if (!job) {
            printf(RED "❌ Memory allocation failed during loading!\n" RESET);
            break;
        }

        if (fread(job, sizeof(Job), 1, file) != 1) {
            free(job);
            break;
        }

        job->next = chain->head;
        chain->head = job;

        if (last_job) {
            strcpy(last_job->prev_hash, job->curr_hash);
        }
        last_job = job;
        chain->size++;
    }

    fclose(file);
    printf(GREEN "✅ Blockchain loaded successfully! Last Block Index: %d\n" RESET, chain->size - 1);
}


/**
 * get_string_input - Gets string input safely
 */
void get_string_input(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
    }
}

/**
 * print_job - Display job listing information in a formatted table.
 */
void print_job(Job *job) {
    printf(GREEN "\n╔═════════════════════════════════════════════════════════════════════════════════════╗\n" RESET);
    printf(GREEN "║                                        Block %d                                      ║\n" RESET, job->index);
    printf(GREEN "╠═════════════════════════════════════════════════════════════════════════════════════╣\n" RESET);
    printf(BLUE "║ 🏢 Company: %-72s║\n" RESET, job->company);
    printf(BLUE "║ 📍 Location: %-71s║\n" RESET, job->location);
    printf(BLUE "║ 🔖 Title: %-74s║\n" RESET, job->title);
    
    
    printf(BLUE "║ 📝 Description: %.65s...║\n" RESET, job->description);
    printf(BLUE "╠═════════════════════════════════════════════════════════════════════════════════════╣\n" RESET);
    printf(BLUE "║ ⏳ List Date: %.24s                                              ║\n" RESET, ctime(&job->timestamp));                                  
    printf(BLUE "║ 🔗 Prev Hash: %-70s║\n" RESET, job->prev_hash);
    printf(BLUE "║ 🔗 Current Hash: %s   ║\n" RESET, job->curr_hash);
    printf(BLUE "╚═════════════════════════════════════════════════════════════════════════════════════╝\n" RESET);
}


/**
 * free_blockchain - Free all allocated memory
 */
void free_blockchain(Blockchain *chain) {
    Job *current = chain->head, *temp;
    while (current) {
        temp = current;
        current = current->next;
        free(temp);
    }
    free(chain);
}

/**
 * display_menu - Prints the menu
 */
void display_menu(void) {
    printf(BLUE "\n=== Job Directory Blockchain ===\n" RESET);
    printf(GREEN "1. List new jobs\n" RESET);
    printf(GREEN "2. View all job listings\n" RESET);
    printf(GREEN "3. Search jobs\n" RESET);
    printf(GREEN "4. Verify blockchain integrity\n" RESET);
    printf(GREEN "5. Save Blockchain\n" RESET);
    printf(GREEN "6. Exit\n" RESET);
    printf(YELLOW "Enter your choice: " RESET);
}
/**
 * main - Entry point
 */
int main(void) {
    Blockchain *chain = initialize_blockchain();
    load_blockchain(chain);

    int choice;
    char title[MAX_TITLE_LENGTH], company[MAX_COMPANY_LENGTH], location[MAX_LOCATION_LENGTH], description[MAX_DESCRIPTION_LENGTH], keyword[50];

    while (1) {
        display_menu();
        scanf("%d", &choice);
        getchar();

        switch (choice) {
        case 1:
            get_string_input(YELLOW "Enter job title: " RESET, title, MAX_TITLE_LENGTH);
            get_string_input(YELLOW "Enter company name: " RESET, company, MAX_COMPANY_LENGTH);
            get_string_input(YELLOW "Enter location: " RESET, location, MAX_LOCATION_LENGTH);
            get_string_input(YELLOW "Enter job description: " RESET, description, MAX_DESCRIPTION_LENGTH);
            if (add_job(chain, title, company, location, description))
                printf(GREEN "✅ Job listing added successfully!\n" RESET);
            else
                printf(RED "❌ Failed to add job listing.\n" RESET);
            break;

        case 2:
            printf(GREEN "\n                                === All Job Listings ===\n" RESET);
            Job *current = chain->head;
            while (current) {
                print_job(current);
                current = current->next;
            }
            break;

        case 3:
            get_string_input(YELLOW "Enter search keyword: " RESET, keyword, sizeof(keyword));
            Job *results = search_jobs(chain, keyword);
            if (!results)
                printf(RED "❌ No matching job found.\n" RESET);
            else
                while (results) {
                    print_job(results);
                    results = results->next;
                }
            break;

        case 4:
            verify_blockchain(chain);
            break;

        case 5:
            save_blockchain(chain);
            break;

        case 6:
            free_blockchain(chain);
            printf(GREEN "\n✅ Exiting program. Goodbye!\n" RESET);
            return 0;

        default:
            printf(RED "Invalid choice! Please try again.\n" RESET);
        }
    }
}