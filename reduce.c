#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// // For time optimization only
// #include <time.h>

#define MAX_LENGTH 1024
#define MAX_FIELDS 55

#ifdef _WIN32
    #include <conio.h>  // Windows: _getch()
#else
    #include <termios.h>  // Linux/macOS: termios for raw input
    #include <unistd.h>

    // Must Define getch() to Match Windows Functionality
    char getch() 
    {
        struct termios oldt, newt;
        char ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif      

// Define the struct for AQS data
typedef struct {
    char state_code[3];
    char county_code[4];
    char site_num[5];
    char parameter_code[6];
    int poc;
    double latitude;
    double longitude;
    char datum[10];
    char parameter_name[200];
    char sample_duration[20];
    char pollutant_standard[50];
    char metric_used[150];
    char method_name[200];
    int year;
    char units_of_measure[20];
    char event_type[20];
    int observation_count;
    int observation_percent;
    char completeness_indicator;
    int valid_day_count;
    int required_day_count;
    int exceptional_data_count;
    int null_data_count;
    int primary_exceedance_count;
    int secondary_exceedance_count;
    char certification_indicator[20];
    int num_obs_below_mdl;
    double arithmetic_mean;
    double arithmetic_std_dev;
    double first_max_value;
    char first_max_datetime[20];
    double second_max_value;
    char second_max_datetime[20];
    double third_max_value;
    char third_max_datetime[20];
    double fourth_max_value;
    char fourth_max_datetime[20];
    double first_no_max_value; // May need to handle missing data
    char first_no_max_datetime[20];
    double second_no_max_value; // May need to handle missing data
    char second_no_max_datetime[20];
    double percentile_99;
    double percentile_98;
    double percentile_95;
    double percentile_90;
    double percentile_75;
    double percentile_50;
    double percentile_10;
    char local_site_name[50];
    char address[200];
    char state_name[20];
    char county_name[50];
    char city_name[50];
    char cbsa_name[50];
    char date_of_last_change[15];
} AQSData;

// * Functions * // 

// Read data from CSV file
AQSData *read_data(const char *filename, size_t *len);

// Function to Parse Each Line
char *parse_csv_line(char *line, int len);

// Function for Autocomplete
void autocomplete(char *buffer, int param_count, char **param_names);

// Function to compare nums and letters for qsort
int comp(const void *a, const void *b);

// * MAIN * //

int main(int argc, char *argv[]) {

    // clock_t start, end;
    // double cpu_time_used;

    // start = clock(); // Record start time

    const char *filename = argv[1];

    if (argc < 2)
    {
        printf("Error: Not enough arguments\nUsage: ./reduce input_file_path\n");
        return EXIT_FAILURE;
    }

    size_t aqs_len;
    // Populate structs
    AQSData *data = read_data(filename, &aqs_len);

    // Check for error first
    if(data == NULL)
    {
        fprintf(stderr, "Failed to read data\n");
        return EXIT_FAILURE;
    }

    // array for unique parameter names
    char **param_names = NULL;
    size_t size = 0;

    // Place a solid cap on params to start
    int capacity = MAX_LENGTH;

    // Allocate Array 
    param_names = malloc(capacity * sizeof(char *));

    // Check for Allocation Error
    if (!param_names)
    {
        perror("Failed to allocate param_names");
        return EXIT_FAILURE;
    }

    // Implement Loop for pushing unique params to param_names array from csv file

    // i = 1 to skip header
    for (int i = 1; i < aqs_len; i++)
    {
        if (size == capacity)
        {
            capacity *= 2;
            char **temp = realloc(param_names, capacity * sizeof(char *));

            if (!temp)
            {
                perror("Failed to allocate temp");

                // Free individual strings first 
                for (int j = 0; j < size; j++)
                {
                    free(param_names[j]);
                }

                free(param_names);
                free(data);

                return EXIT_FAILURE;
            }

            // if successful, set param_names = temp
            param_names = temp;
        }

        // Flag for continuing i loop
        bool match = false;

        for (int j = 0; j < size; j++)
        {
            if (!strcmp(param_names[j], data[i].parameter_name))
            {
                match = true;
                break;
            }
        }

        // If match true, 
        if (match) continue;

        // Allocate memory for string and copy 
        param_names[size] = malloc(strlen(data[i].parameter_name) + 1);

        // Allocation error 
        if (!param_names[size])
        {
            perror("Failed to allocate param_names[size]");

            // Free individual strings first 
            for (int j = 0; j < size; j++)
            {
                free(param_names[j]);
            }

            free(param_names);
            free(data);

            return EXIT_FAILURE;
        }

        strcpy(param_names[size], data[i].parameter_name);

        size++;
    }

    // Quick Sort param_names
    qsort(param_names, size, sizeof(param_names[0]), comp);

    // New Array for no_quotes_params
    char **no_quote_params = malloc(size * sizeof(char *));

    if (!no_quote_params)
    {
        perror("no_quote_params allocation failed");

        // Free individual strings first 
        for (int j = 0; j < size; j++)
        {
            free(param_names[j]);
        }

        free(param_names);
        free(data);

        return EXIT_FAILURE;
    }

    int copy_size = 0;
    for (int i = 0; i < size; i++) {
        int len = strlen(param_names[i]);

        // Allocate memory for new string (excluding quotes, +1 for null terminator)
        no_quote_params[i] = malloc(len + 1);  
        copy_size++;

        if (!no_quote_params[i]) {
            perror("no_quote_params[i] allocation failed");

            // Free allocated memory before exiting
            for (int j = 0; j < size; j++) {
                free(param_names[j]);
            }
            for (int j = 0; j < copy_size; j++) {
                free(no_quote_params[j]);
            }

            free(param_names);
            free(data);
            return EXIT_FAILURE;
        }

        int k = 0;  // Separate index for no_quote_params
        for (int j = 0; j < len; j++) {
            if (param_names[i][j] == '"') {
                continue;
            } 
            no_quote_params[i][k++] = param_names[i][j];
        }
        no_quote_params[i][k] = '\0';  // Null-terminate the string
    }

    // If all goes well, free the relevant pointers
    // Print the unique parameters
    printf("Unique parameters:\n");
    for (int i = 0; i < size; i++) 
    {
        printf("Parameter %i: %s\n", i + 1, no_quote_params[i]);
    }
    
    // Buffer for Autocomplete Function
    char buffer[200];
    
    // Get Parameter for In-Line Autocomplete
    autocomplete(buffer, size, no_quote_params);

    // TODO: Implement Param Selection to Reduce CSV Size for GIS Applications

    // Free data once passed down the pipeline
    for (int i = 0; i < size; i++) 
    {
        free(param_names[i]);
        free(no_quote_params[i]);
    }

    free(param_names);
    free(no_quote_params);
    free(data);

    // end = clock(); // Record end time

    // cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; // Calculate elapsed time in seconds

    // printf("Time taken: %f seconds\n", cpu_time_used);

    return EXIT_SUCCESS;
}

/* takes 2 params, the filename and a pointer
  to size_t where the number of data points is stored */

AQSData *read_data(const char *filename, size_t *len) 
{
    if (filename == NULL || len == NULL) return NULL;

    // Open File
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) 
    {
        fprintf(stderr, "Could not open %s: %s\n", filename, strerror(errno));
        return NULL;
    }

    // Set arr and tmp for data manip
    AQSData *arr = NULL, *tmp;
    *len = 0;

    // Assuming that no line will be longer than 1023 chars
    char line[MAX_LENGTH];

    // Read one line at a time
    while (fgets(line, sizeof(line), fp)) 
    {
        tmp = realloc(arr, (*len + 1) * sizeof(*arr));
        if (tmp == NULL) {
            fprintf(stderr, "could not parse the whole file %s\n", filename);
            // Free any previously parsed data
            if (*len == 0) {
                free(arr);
                arr = NULL;
            }
            return arr;
        }

        arr = tmp;

        // Parse the CSV line into the structure
        char *token = parse_csv_line(line, MAX_LENGTH);
        
        // Get token with Unit Separator as delimiter
        token = strtok(token, "\x1F");
        
        int field = 0;

        while (token != NULL && field < MAX_FIELDS) 
        {
            // Assign the token to the appropriate field in the struct
            switch (field)
            {
                case 0:
                    strncpy(arr[*len].state_code, token, sizeof(arr[*len].state_code) - 1);
                    arr[*len].state_code[sizeof(arr[*len].state_code) - 1] = '\0';
                    break;
                case 1:
                    strncpy(arr[*len].county_code, token, sizeof(arr[*len].county_code) - 1);
                    arr[*len].county_code[sizeof(arr[*len].county_code) - 1] = '\0';
                    break;
                case 2:
                    strncpy(arr[*len].site_num, token, sizeof(arr[*len].site_num) - 1);
                    arr[*len].site_num[sizeof(arr[*len].site_num) - 1] = '\0';
                    break;
                case 3:
                    strncpy(arr[*len].parameter_code, token, sizeof(arr[*len].parameter_code) - 1);
                    arr[*len].parameter_code[sizeof(arr[*len].parameter_code) - 1] = '\0';
                    break;
                case 4:
                    arr[*len].poc = atoi(token);
                    break;
                case 5:
                    arr[*len].latitude = atof(token);
                    break;
                case 6:
                    arr[*len].longitude = atof(token);
                    break;
                case 7:
                    strncpy(arr[*len].datum, token, sizeof(arr[*len].datum) - 1);
                    arr[*len].datum[sizeof(arr[*len].datum) - 1] = '\0';
                    break;
                case 8:
                    strncpy(arr[*len].parameter_name, token, sizeof(arr[*len].parameter_name) - 1);
                    arr[*len].parameter_name[sizeof(arr[*len].parameter_name) - 1] = '\0';
                    break;
                case 9:
                    strncpy(arr[*len].sample_duration, token, sizeof(arr[*len].sample_duration) - 1);
                    arr[*len].sample_duration[sizeof(arr[*len].sample_duration) - 1] = '\0';
                    break;
                case 10:
                    strncpy(arr[*len].pollutant_standard, token, sizeof(arr[*len].pollutant_standard) - 1);
                    arr[*len].pollutant_standard[sizeof(arr[*len].pollutant_standard) - 1] = '\0';
                    break;
                case 11:
                    strncpy(arr[*len].metric_used, token, sizeof(arr[*len].metric_used) - 1);
                    arr[*len].metric_used[sizeof(arr[*len].metric_used) - 1] = '\0';
                    break;
                case 12:
                    strncpy(arr[*len].method_name, token, sizeof(arr[*len].method_name) - 1);
                    arr[*len].method_name[sizeof(arr[*len].method_name) - 1] = '\0';
                    break;
                case 13:
                    arr[*len].year = atoi(token);
                    break;
                case 14:
                    strncpy(arr[*len].units_of_measure, token, sizeof(arr[*len].units_of_measure) - 1);
                    arr[*len].units_of_measure[sizeof(arr[*len].units_of_measure) - 1] = '\0';
                    break;
                case 15:
                    strncpy(arr[*len].event_type, token, sizeof(arr[*len].event_type) - 1);
                    arr[*len].event_type[sizeof(arr[*len].event_type) - 1] = '\0';
                    break;
                case 16:
                    arr[*len].observation_count = atoi(token);
                    break;
                case 17:
                    arr[*len].observation_percent = atoi(token);
                    break;
                case 18:
                    arr[*len].completeness_indicator = token[0];
                    break;
                case 19:
                    arr[*len].valid_day_count = atoi(token);
                    break;
                case 20:
                    arr[*len].required_day_count = atoi(token);
                    break;
                case 21:
                    arr[*len].exceptional_data_count = atoi(token);
                    break;
                case 22:
                    arr[*len].null_data_count = atoi(token);
                    break;
                case 23:
                    arr[*len].primary_exceedance_count = atoi(token);
                    break;
                case 24:
                    arr[*len].secondary_exceedance_count = atoi(token);
                    break;
                case 25:
                    strncpy(arr[*len].certification_indicator, token, sizeof(arr[*len].certification_indicator) - 1);
                    arr[*len].certification_indicator[sizeof(arr[*len].certification_indicator) - 1] = '\0';
                    break;
                case 26:
                    arr[*len].num_obs_below_mdl = atoi(token);
                    break;
                case 27:
                    arr[*len].arithmetic_mean = atof(token);
                    break;
                case 28:
                    arr[*len].arithmetic_std_dev = atof(token);
                    break;
                case 29:
                    arr[*len].first_max_value = atof(token);
                    break;
                case 30:
                    strncpy(arr[*len].first_max_datetime, token, sizeof(arr[*len].first_max_datetime) - 1);
                    arr[*len].first_max_datetime[sizeof(arr[*len].first_max_datetime) - 1] = '\0';
                    break;
                case 31:
                    arr[*len].second_max_value = atof(token);
                    break;
                case 32:
                    strncpy(arr[*len].second_max_datetime, token, sizeof(arr[*len].second_max_datetime) - 1);
                    arr[*len].second_max_datetime[sizeof(arr[*len].second_max_datetime) - 1] = '\0';
                    break;
                case 33:
                    arr[*len].third_max_value = atof(token);
                    break;
                case 34:
                    strncpy(arr[*len].third_max_datetime, token, sizeof(arr[*len].third_max_datetime) - 1);
                    arr[*len].third_max_datetime[sizeof(arr[*len].third_max_datetime) - 1] = '\0';
                    break;
                case 35:
                    arr[*len].fourth_max_value = atof(token);
                    break;
                case 36:
                    strncpy(arr[*len].fourth_max_datetime, token, sizeof(arr[*len].fourth_max_datetime) - 1);
                    arr[*len].fourth_max_datetime[sizeof(arr[*len].fourth_max_datetime) - 1] = '\0';
                    break;
                case 37:
                    arr[*len].first_no_max_value = atof(token);
                    break;
                case 38:
                    strncpy(arr[*len].first_no_max_datetime, token, sizeof(arr[*len].first_no_max_datetime) - 1);
                    arr[*len].first_no_max_datetime[sizeof(arr[*len].first_no_max_datetime) - 1] = '\0';
                    break;
                case 39:
                    arr[*len].second_no_max_value = atof(token);
                    break;
                case 40:
                    strncpy(arr[*len].second_no_max_datetime, token, sizeof(arr[*len].second_no_max_datetime) - 1);
                    arr[*len].second_no_max_datetime[sizeof(arr[*len].second_no_max_datetime) - 1] = '\0';
                    break;
                case 41:
                    arr[*len].percentile_99 = atof(token);
                    break;
                case 42:
                    arr[*len].percentile_98 = atof(token);
                    break;
                case 43:
                    arr[*len].percentile_95 = atof(token);
                    break;
                case 44:
                    arr[*len].percentile_90 = atof(token);
                    break;
                case 45:
                    arr[*len].percentile_75 = atof(token);
                    break;
                case 46:
                    arr[*len].percentile_50 = atof(token);
                    break;
                case 47:
                    arr[*len].percentile_10 = atof(token);
                    break;
                case 48:
                    strncpy(arr[*len].local_site_name, token, sizeof(arr[*len].local_site_name) - 1);
                    arr[*len].local_site_name[sizeof(arr[*len].local_site_name) - 1] = '\0';
                    break;
                case 49:
                    strncpy(arr[*len].address, token, sizeof(arr[*len].address) - 1);
                    arr[*len].address[sizeof(arr[*len].address) - 1] = '\0';
                    break;
                case 50:
                    strncpy(arr[*len].state_name, token, sizeof(arr[*len].state_name) - 1);
                    arr[*len].state_name[sizeof(arr[*len].state_name) - 1] = '\0';
                    break;
                case 51:
                    strncpy(arr[*len].county_name, token, sizeof(arr[*len].county_name) - 1);
                    arr[*len].county_name[sizeof(arr[*len].county_name) - 1] = '\0';
                    break;
                case 52:
                    strncpy(arr[*len].county_name, token, sizeof(arr[*len].county_name) - 1);
                    arr[*len].county_name[sizeof(arr[*len].county_name) - 1] = '\0';
                    break;
                case 53:
                    strncpy(arr[*len].city_name, token, sizeof(arr[*len].city_name) - 1);
                    arr[*len].city_name[sizeof(arr[*len].city_name) - 1] = '\0';
                    break;
                case 54:
                    strncpy(arr[*len].cbsa_name, token, sizeof(arr[*len].cbsa_name) - 1);
                    arr[*len].cbsa_name[sizeof(arr[*len].cbsa_name) - 1] = '\0';
                    break;
                case 55:
                    strncpy(arr[*len].date_of_last_change, token, sizeof(arr[*len].date_of_last_change) - 1);
                    arr[*len].date_of_last_change[sizeof(arr[*len].date_of_last_change) - 1] = '\0';
                    break;
            }
            field++;
            token = strtok(NULL, "\x1F");
        }
        (*len)++;
    }

    fclose(fp);
    return arr;
}

// Parse line, replace with more efficient delimiter, make all lowercase
char* parse_csv_line(char *line, int len) 
{
    bool in_quotes = false;
    char *ptr = line;

    for (int i = 0; i < len; i++)
    {        
        // Need lowercase for comprehensive qsort
        ptr[i] = tolower(ptr[i]);
        // if double quotes and comma not after, we're in a column
        if (ptr[i] == '"' && ptr[i + 1] != ',') 
        {
            in_quotes = true;
        // Else if , following ", we're leaving the column
        } else if (ptr[i] == '"' && ptr[i + 1] == ',')
        {
            in_quotes = false;
        }

        // Change to a nearly impossible single char delimiter for ease because I am so sick of commas, f*** you commas
        if (!in_quotes && ptr[i] == ',')
        {
            // Ascii unit separator
            ptr[i] = '\x1F';
        }

        // End if Null Terminator Encountered
        if (ptr[i] == '\0') break;
    }

    return ptr;
}

void autocomplete(char *buffer, int param_count, char **param_names) 
{
    int index = 0;
    int match_count = 0;
    int last_match_index = -1;
    char c;
    bool tabbed = false;
    printf("Enter parameter (Tab for autocomplete and Increment, Shift + Tab to Decrement):\n");

    while (1) {
        #ifdef _WIN32
            c = _getch();
        #else
            c = getch();
        #endif

        if (c == '\n' || c == '\r') 
        {
            buffer[index] = '\0';
            printf("\n");
            break;
        } else if (c == 127 || c == '\b') 
        {
            if (index > 0) {
                index--;
                printf("\b \b");
            }
        } else if (c == '\t') 
        {  // Tab key → Cycle through suggestions
            tabbed = true;
            match_count = 0;
            int first_match_index = -1;

            for (int i = 0; i < param_count; i++)
            {
                if (strncmp(param_names[i], buffer, index) == 0) 
                {
                    if (match_count == 0) 
                    {
                        first_match_index = i;
                    }
                    match_count++;

                    if (last_match_index == -1 || last_match_index < i) 
                    {
                        last_match_index = i;
                        strcpy(buffer, param_names[i]);
                        printf("\r%-50s", buffer);  // Overwrite the input line
                        index = 0;
                        break;
                    }
                }
            }
            
            if (match_count > 0 && last_match_index == param_count - 1) 
            {
                last_match_index = first_match_index - 1; // Restart cycling
            }
            
        } else if (c >= 32 && c <= 126) 
        {  // Printable ASCII characters
            buffer[index++] = c;
            printf("%c", c);
        } 

        // TODO : WHAT IS SHIFT
        else if (c == '\v')
        {
            printf("Okay");
        }
    }
}

int comp(const void *a, const void *b)
{
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;

    // Check if first characters are digits
    int is_digit1 = isdigit((unsigned char)str1[0]);
    int is_digit2 = isdigit((unsigned char)str2[0]);

    // If one is a number and the other is not, prioritize the number
    if (is_digit1 && !is_digit2) return -1;
    if (!is_digit1 && is_digit2) return 1;

    // If both are numbers or both are letters, use strcmp for alphanumeric sorting
    return strcmp(str1, str2);
}
