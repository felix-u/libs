#define log_info(s) log_internal("info: " s)
#define logf_info(format, ...) logf_internal("info: " format, __VA_ARGS__)

#define log_internal(s) log_internal_with_location(__FILE__, __LINE__, (char *)__func__, s)
static void log_internal_with_location(char *file, usize line, char *func, char *s);

#define logf_internal(format, ...) logf_internal_with_location(__FILE__, __LINE__, (char *)__func__, format, __VA_ARGS__)
static void logf_internal_with_location(char *file, usize line, char *func, char *format, ...);

static Str8 file_read_bytes_relative_path(Arena *arena, char *path, usize max_bytes);
