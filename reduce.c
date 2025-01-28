#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

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
    char parameter_name[50];
    char sample_duration[20];
    char pollutant_standard[50];
    char metric_used[150];
    char method_name[50];
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
    char address[100];
    char state_name[20];
    char county_name[50];
    char city_name[50];
    char cbsa_name[50];
    char date_of_last_change[15];
} AQSData;

// Read data from CSV file
AQSData *read_data(const char *filename, size_t *len);

int main() {
    char fp[4096];

    printf("Enter a file name to parse: ");
    fgets(fp, sizeof(fp), stdin);

    // Remove the trailing newline character that fgets includes
    fp[strcspn(fp, "\n")] = '\0';

    printf("You entered: %s\n", fp);

    size_t aqs_len;
    AQSData *data = read_data(fp, &aqs_len);

    // Check for error first
    if(data == NULL) {
        fprintf(stderr, "Failed to read data\n");
        return 1;
    }

    // array for unique parameter names
    char **param_names = NULL;
    size_t size = 0;
    size_t capacity = 1;

    param_names = malloc(capacity * sizeof(char *));

    if (!param_names)
    {
        perror("Failed to allocate memory");
        return 1;
    }

    // Implement Loop for pushing unique params to param_names array from csv file

    free(data);

    // Print the unique parameters
    printf("Unique parameters:\n");
    for (int i = 0; i < size; i++) {
        printf("%s\n", param_names[i]);
        free(param_names[i]); // Free each string
    }

    free(param_names);
    
    return 0;
}

// takes 2 params, the filename and a pointer
// to size_t where the number of data points is stored
AQSData *read_data(const char *filename, size_t *len)
{
    if(filename == NULL || len == NULL)
        return NULL;

    FILE *fp = fopen(filename, "r");
    if(fp == NULL)
    {
        fprintf(stderr, "Could not open %s: %s\n", filename, strerror(errno));
        return NULL;
    }

    AQSData *arr = NULL, *tmp;
    *len = 0;

    // assuming that no line will be longer than 1023 chars long
    char line[1024];

    // Read one line at a time
    while(fgets(line, sizeof line, fp))
    {
        tmp = realloc(arr, (*len + 1) * sizeof *arr);
        if(tmp == NULL)
        {
            fprintf(stderr, "could not parse the whole file %s\n", filename);
            // returning all parsed cities so far

            if(*len == 0)
            {
                free(arr);
                arr = NULL;
            }

            return arr;
        }

        // arr = allocated tmp string
        arr = tmp;

        // Parse the CSV line into the structure
        char *token = strtok(line, ",");
        int field = 0;
        
        // while loop for line only
        while(token != NULL && field < 53) {
            switch(field) {
                case 0:
                    strncpy(arr[*len].state_code, token, sizeof(arr[*len].state_code)-1);
                    arr[*len].state_code[sizeof(arr[*len].state_code)-1] = '\0';
                    break;
                case 1:
                    strncpy(arr[*len].county_code, token, sizeof(arr[*len].county_code)-1);
                    arr[*len].county_code[sizeof(arr[*len].county_code)-1] = '\0';
                    break;
                case 2:
                    strncpy(arr[*len].site_num, token, sizeof(arr[*len].site_num)-1);
                    arr[*len].site_num[sizeof(arr[*len].site_num)-1] = '\0';
                    break;
                case 3:
                    strncpy(arr[*len].parameter_code, token, sizeof(arr[*len].parameter_code)-1);
                    arr[*len].parameter_code[sizeof(arr[*len].parameter_code)-1] = '\0';
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
                    strncpy(arr[*len].datum, token, sizeof(arr[*len].datum)-1);
                    arr[*len].datum[sizeof(arr[*len].datum)-1] = '\0';
                    break;
                case 8:
                    strncpy(arr[*len].parameter_name, token, sizeof(arr[*len].parameter_name)-1);
                    arr[*len].parameter_name[sizeof(arr[*len].parameter_name)-1] = '\0';
                    break;
                case 9:
                    strncpy(arr[*len].sample_duration, token, sizeof(arr[*len].sample_duration)-1);
                    arr[*len].sample_duration[sizeof(arr[*len].sample_duration)-1] = '\0';
                    break;
                case 10:
                    strncpy(arr[*len].pollutant_standard, token, sizeof(arr[*len].pollutant_standard)-1);
                    arr[*len].pollutant_standard[sizeof(arr[*len].pollutant_standard)-1] = '\0';
                    break;
                case 11:
                    strncpy(arr[*len].metric_used, token, sizeof(arr[*len].metric_used)-1);
                    arr[*len].metric_used[sizeof(arr[*len].metric_used)-1] = '\0';
                    break;
                case 12:
                    strncpy(arr[*len].method_name, token, sizeof(arr[*len].method_name)-1);
                    arr[*len].method_name[sizeof(arr[*len].method_name)-1] = '\0';
                    break;
                case 13:
                    arr[*len].year = atoi(token);
                    break;
                case 14:
                    strncpy(arr[*len].units_of_measure, token, sizeof(arr[*len].units_of_measure)-1);
                    arr[*len].units_of_measure[sizeof(arr[*len].units_of_measure)-1] = '\0';
                    break;
                case 15:
                    strncpy(arr[*len].event_type, token, sizeof(arr[*len].event_type)-1);
                    arr[*len].event_type[sizeof(arr[*len].event_type)-1] = '\0';
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
                    strncpy(arr[*len].certification_indicator, token, sizeof(arr[*len].certification_indicator)-1);
                    arr[*len].certification_indicator[sizeof(arr[*len].certification_indicator)-1] = '\0';
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
                    strncpy(arr[*len].first_max_datetime, token, sizeof(arr[*len].first_max_datetime)-1);
                    arr[*len].first_max_datetime[sizeof(arr[*len].first_max_datetime)-1] = '\0';
                    break;
                case 31:
                    arr[*len].second_max_value = atof(token);
                    break;
                case 32:
                    strncpy(arr[*len].second_max_datetime, token, sizeof(arr[*len].second_max_datetime)-1);
                    arr[*len].second_max_datetime[sizeof(arr[*len].second_max_datetime)-1] = '\0';
                    break;
                case 33:
                    arr[*len].third_max_value = atof(token);
                    break;
                case 34:
                    strncpy(arr[*len].third_max_datetime, token, sizeof(arr[*len].third_max_datetime)-1);
                    arr[*len].third_max_datetime[sizeof(arr[*len].third_max_datetime)-1] = '\0';
                    break;
                case 35:
                    arr[*len].fourth_max_value = atof(token);
                    break;
                case 36:
                    strncpy(arr[*len].fourth_max_datetime, token, sizeof(arr[*len].fourth_max_datetime)-1);
                    arr[*len].fourth_max_datetime[sizeof(arr[*len].fourth_max_datetime)-1] = '\0';
                    break;
                case 37:
                    arr[*len].first_no_max_value = atof(token);
                    break;
                case 38:
                    strncpy(arr[*len].first_no_max_datetime, token, sizeof(arr[*len].first_no_max_datetime)-1);
                    arr[*len].first_no_max_datetime[sizeof(arr[*len].first_no_max_datetime)-1] = '\0';
                    break;
                case 39:
                    arr[*len].second_no_max_value = atof(token);
                    break;
                case 40:
                    strncpy(arr[*len].second_no_max_datetime, token, sizeof(arr[*len].second_no_max_datetime)-1);
                    arr[*len].second_no_max_datetime[sizeof(arr[*len].second_no_max_datetime)-1] = '\0';
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
                    strncpy(arr[*len].local_site_name, token, sizeof(arr[*len].local_site_name)-1);
                    arr[*len].local_site_name[sizeof(arr[*len].local_site_name)-1] = '\0';
                    break;
                case 49:
                    strncpy(arr[*len].address, token, sizeof(arr[*len].address)-1);
                    arr[*len].address[sizeof(arr[*len].address)-1] = '\0';
                    break;
                case 50:
                    strncpy(arr[*len].state_name, token, sizeof(arr[*len].state_name)-1);
                    arr[*len].state_name[sizeof(arr[*len].state_name)-1] = '\0';
                    break;
                case 51:
                    strncpy(arr[*len].county_name, token, sizeof(arr[*len].county_name)-1);
                    arr[*len].county_name[sizeof(arr[*len].county_name)-1] = '\0';
                    break;
                case 52:
                    strncpy(arr[*len].city_name, token, sizeof(arr[*len].city_name)-1);
                    arr[*len].city_name[sizeof(arr[*len].city_name)-1] = '\0';
                    break;
                case 53:
                    strncpy(arr[*len].cbsa_name, token, sizeof(arr[*len].cbsa_name)-1);
                    arr[*len].cbsa_name[sizeof(arr[*len].cbsa_name)-1] = '\0';
                    break;
                case 54:
                    strncpy(arr[*len].date_of_last_change, token, sizeof(arr[*len].date_of_last_change)-1);
                    arr[*len].date_of_last_change[sizeof(arr[*len].date_of_last_change)-1] = '\0';
                    break;
            }
            token = strtok(NULL, ",");
            field++;
        }
        // incrementing only when parsing of line was OK
        (*len)++;
    }
    fclose(fp);
    return arr;
}