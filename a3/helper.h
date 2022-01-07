#ifndef _HELPER_H
#define _HELPER_H

#define SIZE 44

struct rec {
    int freq;
    char word[SIZE];
};

int get_file_size(char *filename);
int compare_freq(const void *rec1, const void *rec2);
void read_records(char *f_name, int num_records_before, int num_to_read, struct rec my_records[]);
void read_arguments(int argc, char **argv, int *num_process, char **inputfile, char **outputfile);
void set_num_process(int *number_process, int num_records);
void Pipe(int *pipe_fd);
FILE *Fopen(char **file_name, char *file_type);
void Fclose(FILE *file_to_close);
void WaitForChildren(int number_process);
void Merge(int num_records, int number_process, char **outfile, int pipe_fd[][2], int pipe_status[]);

#endif /* _HELPER_H */
