static FILE   *file_open(char *path, char *mode); 
static String8 file_read(Arena *arena, char *path, char *mode); 
static void    file_write(FILE *file, String8 memory); 

