static FILE *file_open(char *path, char *mode) {
    if (path == 0 || mode == 0) return 0;
    FILE *file = fopen(path, mode);
    if (file == 0) errf("failed to open file '%s'", path);
    return file;
}

static void file_open_write_close(char *path, char *mode, String8 mem_to_write) {
    FILE *file = file_open(path, mode);
    file_write(file, mem_to_write);
    fclose(file);
}

static String8 file_read(Arena *arena, char *path, char *mode, usize max_bytes) {
    String8 bytes = {0};
    if (path == 0 || mode == 0) return bytes;

    FILE *file = file_open(path, mode);
    if (file == 0) return (String8){0};

    fseek(file, 0L, SEEK_END);
    usize filesize = ftell(file);
    if (filesize + 1 > max_bytes) {
        fclose(file);
        errf("file '%s' is too large: %zu/%zu bytes", path, filesize + 1, max_bytes);
        return (String8){0};
    }

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
    if (file == 0) return;
    fwrite(memory.ptr, memory.len, 1, file);
}
