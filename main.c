#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <ctype.h>
#include <libgen.h>
#define INPUT_SIZE (512)

struct
{
    struct dirent *list;
    size_t size;
    size_t alloc;
} entries = {0};

struct
{
    char dirpath[PATH_MAX];
    char input[INPUT_SIZE];
} program;

void load_directory(DIR *dir)
{
    void *pentry = NULL;
    entries.size = 0;

    do
    {
        struct dirent *entry = readdir(dir);
        pentry = entry;
        if (entry && entries.size >= entries.alloc)
        {
            entries.alloc = (entries.alloc + 1) * 2;
            entries.list = realloc(entries.list, sizeof(*entry) * entries.alloc);
            assert(entries.list != NULL);
        }
        if (entry)
        {
            entries.list[entries.size++] = *entry;
        }
    } while (pentry);
}

int open_directory(const char *path)
{
    static char temp[PATH_MAX] = {0};
    memset(temp, 0, sizeof(temp));
    strncpy(temp, program.dirpath, PATH_MAX);
    strncat(temp, "/", PATH_MAX - 1);
    strncat(temp, path, PATH_MAX - 1);
    realpath(temp, program.dirpath);
    DIR *dir = opendir(program.dirpath);
    if (dir == NULL)
    {
        return 1;
    }
    load_directory(dir);
    closedir(dir);
    return 0;
}

struct dirent *get_directory(int index)
{
    return (index <= entries.size && index >= 1) ? &entries.list[index - 1] : NULL;
}

void show_directory(void)
{
    const char *dirent_type[] = {
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

    for (size_t i = 0; i < entries.size; i++)
    {
        int type_int = entries.list[i].d_type;
        const char *type_str = dirent_type[type_int];
        printf("%-5zu %-10s ", i + 1, type_str);
        printf("%s\n", entries.list[i].d_name);
    }
}

void show_directory_path(void)
{
    printf("Directory: %s\n", program.dirpath);
}

size_t program_prompt(const char *msg)
{
    printf("%s > ", msg);
    memset(program.input, 0, sizeof(program.input));
    char *result = fgets(program.input, INPUT_SIZE, stdin);
    if (result == NULL)
    {
        memcpy(program.input, "", sizeof(""));
        return 0;
    }
    else
    {
        size_t len = strcspn(program.input, "\r\n");
        program.input[len] = '\0';
        return len;
    }
}

int program_cmd_set(void)
{
    static char temp[PATH_MAX];
    memset(temp, 0, sizeof(temp));
    if (strcmp(program.input, "set") != 0)
    {
        return 0;
    }

    memcpy(temp, program.dirpath, PATH_MAX - 1);
    memcpy(program.dirpath, "", sizeof(""));
    program_prompt("absolute path");
    if (open_directory(program.input))
    {
        printf("can't load directory.\n");
        memcpy(program.dirpath, temp, PATH_MAX - 1);
        return 1;
    }

    return 1;
}

int program_cmd_cd(void)
{
    static char temp[PATH_MAX];
    memset(temp, 0, sizeof(temp));
    if (strcmp(program.input, "cd") != 0)
    {
        return 0;
    }

    memcpy(temp, program.dirpath, PATH_MAX - 1);
    program_prompt("directory #");
    char *end;
    size_t index = strtoul(program.input, &end, 10);
    struct dirent *entry = get_directory(index);
    if (entry == NULL)
    {
        printf("invalid number\n");
        memcpy(program.dirpath, temp, PATH_MAX - 1);
        return 1;
    }

    if (open_directory(entry->d_name))
    {
        printf("can't open directory '%s'\n", entry->d_name);
        memcpy(program.dirpath, temp, PATH_MAX - 1);
        return 1;
    }

    return 1;
}

int program_cmd_open(void)
{
    static char temp[PATH_MAX];
    memset(temp, 0, sizeof(temp));
    char *end;
    size_t index = strtoul(program.input, &end, 10);
    struct dirent *entry = get_directory(index);

    if (entry == NULL)
    {
        printf("invalid number\n");
        return 0;
    }

    strncpy(temp, program.dirpath, PATH_MAX);
    strncat(program.dirpath, "/", PATH_MAX - 1);
    strncat(program.dirpath, entry->d_name, PATH_MAX - 1);

    SDL_Surface *surface = IMG_Load(program.dirpath);
    if (surface == NULL)
    {
        printf("image loading error\n");
        memcpy(program.dirpath, temp, PATH_MAX - 1);
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("viewer", 40, 40, surface->w, surface->h, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Event event;
    int loop = 1;

    while (loop)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYUP)
            {
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

    memcpy(program.dirpath, temp, PATH_MAX - 1);
    return 1;
}

int program_cmd_quit(void)
{
    if (strcmp(program.input, "quit") != 0)
    {
        return 0;
    }

    free(entries.list);
    memset(&entries, 0, sizeof(entries));
    memset(&program, 0, sizeof(program));
    return 1;
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP);

    if(open_directory("/")) {
        printf("error loading initial folder\n");
    }

    while (1)
    {
        show_directory_path();
        show_directory();
        program_prompt("command");

        if (program_cmd_quit())
        {
            break;
        }

        if (program_cmd_set())
        {
            continue;
        }

        if (program_cmd_cd())
        {
            continue;
        }

        if (program_cmd_open())
        {
            continue;
        }
    }
    return 0;
}
