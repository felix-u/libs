static FILE *file_open(char *path, char *mode) {
    if (path == 0 || mode == 0) return 0;
    FILE *file = fopen(path, mode);
    if (file == 0) errf("failed to open file '%s'", path);
    return file;
}

static String8 file_read(Arena *arena, char *path, char *mode) {
    String8 bytes = {0};
    if (path == 0 || mode == 0) return bytes;

    FILE *file = file_open(path, mode);

    fseek(file, 0L, SEEK_END);
    usize filesize = ftell(file);
    bytes.ptr = arena_alloc(arena, filesize + 1, sizeof(u8));

    fseek(file, 0L, SEEK_SET);
    bytes.len = fread(bytes.ptr, sizeof(u8), filesize, file);
    bytes.ptr[bytes.len] = '\0';

    if (ferror(file)) {
        fclose(file);
        errf("error reading file '%s'", path);
        return (String8){0};
    }

    fclose(file);
    return bytes;
}

static void file_write(FILE *file, String8 memory) {
    fwrite(memory.ptr, memory.len, 1, file);
}
