static FILE *file_open(char *path, char *mode); 
static void  file_open_write_close(char *path, char *mode, Str8 mem_to_write);
static Str8  file_read(Arena *arena, char *path, char *mode, usize max_bytes); 
static void  file_write(FILE *file, Str8 memory); 

