#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>

char current_path[PATH_MAX] = {0};
char* path_end = &current_path[0];
struct dirent* dirent_array = NULL;
size_t dirent_size = 0;
size_t dirent_alloc = 0;

void add_dirent(struct dirent* de) {
    if(dirent_size >= dirent_alloc) {
        dirent_alloc++;
        dirent_alloc *= 2;
        dirent_array = realloc(dirent_array, sizeof(*dirent_array) * dirent_alloc);
        assert(dirent_array != NULL);
    }

    dirent_array[dirent_size] = *de;
    dirent_size++;
}

void clear_dirent(void) {
    memset(dirent_array, 0, sizeof(*dirent_array) * dirent_alloc);
    dirent_alloc = dirent_size = 0;
}

struct dirent* get_dirent(size_t index) {
    if(index >= dirent_size) {
        printf("invalid index\n");
        return NULL;
    }

    return &dirent_array[index];
}

void read_directory(const char* path) {
    clear_dirent();
    DIR* dir = opendir(path);
    struct dirent* de = NULL;
    while((de = readdir(dir)))
        add_dirent(de);
    closedir(dir);
}

void print_directory(void) {
    static const char* type[] = {
        "DT_UNKNOWN",
        "DT_FIFO",
        "DT_CHR",
        "",
        "DT_DIR",
        "",
        "DT_BLK",
        "",
        "DT_REG",
        "",
        "DT_LNK",
        "",
        "DT_SOCK",
        "",
    };

    struct dirent* de; 
    for(size_t i = 0; i < dirent_size; i++) {
        de = get_dirent(i);
        printf("%zu : %s/%-20s", i + 1, current_path, de->d_name);
        printf("%s\n", type[de->d_type]);
    }
}

#define BUFFER_SIZE (512)
char* readline(int* size) {
    static char buffer[BUFFER_SIZE] = {0};

    fgets(buffer, BUFFER_SIZE - 1, stdin);
    for(*size = 0; *size < BUFFER_SIZE; (*size)++) {
        if(buffer[*size] == '\n' || buffer[*size] == '\r') {
            buffer[*size] = '\0';
            break;
        }
    }

    return buffer;
}

int get_str_length(char* str, int max) {
    int i = 0; 
    while(str[i] != '\0' && i < max)
        i++;
    return i;
}

char* get_file_directory(char* path, int max) {
    int length = get_str_length(path, max);
    for(char* it = path + length - 1; it != path - 1; it--) {
        if(*it == '/') {
            *it = '\0';
            return it;
        }
    }

    return NULL;
}

void append_path(struct dirent* de) {
    strncat(current_path, "/", sizeof(current_path) - 1);
    strncat(current_path, de->d_name, sizeof(current_path) - 1);
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP);
    
    strncpy(current_path, argv[0], PATH_MAX);
    path_end = get_file_directory(current_path, PATH_MAX);
    read_directory(current_path);

    while(1) {
        printf("DIRECTORY %s\n", current_path);
        print_directory();
        printf("option: "); 
        int size;
        char* input = readline(&size);

        if(strncmp("quit", input, size) == 0) {
            break;
        } 
        else if(strncmp("set", input, size) == 0) {
            printf("path: ");
            input = readline(&size);
            memcpy(current_path, input, size);
            read_directory(current_path);
        } 
        else if(strncmp("cd", input, size) == 0) {
            printf("cd option: ");
            input = readline(&size);
            char* endptr; 
            unsigned long index = strtoul(input, &endptr, 10);
            if(index != 0) {
                struct dirent* de = get_dirent(index - 1);
                if(strncmp("..", de->d_name, sizeof(de->d_name)) == 0) {
                    path_end = get_file_directory(current_path, PATH_MAX);
                    read_directory(current_path);
                }
                else if(strncmp(".", de->d_name, sizeof(de->d_name)) != 0)  {   
                    if(de->d_type == 4) { // 4 = DIR
                        append_path(de);
                        read_directory(current_path);
                    }
                }
            }
        } 
        else if (size > 0) {
            char* endptr; 
            unsigned long index = strtoul(input, &endptr, 10);
            if(index != 0) {
                struct dirent* de = get_dirent(index - 1);
                if(de->d_type == 8) {
                    append_path(de);
                    SDL_Surface* surface = IMG_Load(current_path);
                    if(surface == NULL) {
                        printf("error.\n");
                        path_end = get_file_directory(current_path, sizeof(current_path) - 1);
                        continue;
                    }
                    
                    SDL_Window* window = SDL_CreateWindow("viewer", 40, 40, surface->w, surface->h, 0);
                    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_Event event; 
                    int loop = 1;

                    while(loop) {
                        while(SDL_PollEvent(&event)) {
                            if(event.type == SDL_KEYUP) {
                                loop = 0; 
                            }
                        }
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, texture, NULL, NULL);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(500);
                    }

                    SDL_DestroyTexture(texture);
                    SDL_FreeSurface(surface);
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);

                    path_end = get_file_directory(current_path, sizeof(current_path) - 1);
                }
            }
        }
    }

    free(dirent_array);
    return 0;
}
