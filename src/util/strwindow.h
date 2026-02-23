#ifndef STRWINDOW
#define STRWINDOW

/**
 * @brief A window into a source string.
 * 
 * @warning Will result in a memory error if the source string is freed.
 */
typedef struct StrWindow {
    char *ptr;
    int size;
} StrWindow;

/** 
 * @brief Create a StrWindow from the src pointer of the given size.
 * 
 * If size of the src string is smaller than the given size,
 * create a StrWindow of the size of the src string.
 * 
 * @param src The start of the window in the source string.
 * @param size The size of the window to be created.
 * @returns The created StrWindow.
 */
StrWindow create_str_window(char *src, int size);

#endif