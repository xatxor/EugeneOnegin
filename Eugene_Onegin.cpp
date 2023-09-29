#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\stat.h>
#include <cassert>

enum Errors
{
    OK = 0,
    ERR = 1,
    EMPTYF = 2,
    ENDOF = 3,
    NULLF = 4
};

struct Line {
    char* start_ptr;
    int length;
};

struct Text {
    char* path;
    char* buffer;
    int buffer_count;
    Line* lines;
    int lines_count;
};

Errors get_file_buf(Text* text); //записывает в буфер исходный текст из файла
Errors newline_count(Text* text); //считает количество строк в буфере
Errors get_lines(Text* text);
Errors print_lines(Text* text);
Errors print_buffer(Text* text);
Errors Text_ctor(Text* text);
Errors Text_dtor(Text* text);
Errors Text_verif(Text *text);
Errors error_manager(Errors err);

Errors quick_sort(Text *text, int (*comp)(const void *, const void *));

int compare_lines_strcmp(const void *a, const void *b);
int compare_lines(const void *a, const void *b);
int compare_lines_trimmed(const void *a, const void *b);
int compare_lines_trimmed_backwards(const void *a, const void *b);

int is_letter(char c);

int main(int argc, char *argv[])
{
    /*if (argc != 2)
    {
        printf("There is no text path in arguments! Bye!");
        return 0;
    }*/

    Text onegin = {};
    Text_ctor(&onegin);

    //onegin.path = argv[1];
    onegin.path = "onegin.txt";

    if (Errors err = get_file_buf(&onegin))
    {
        error_manager(err);
        return 0;
    }

    if (Errors err = get_lines(&onegin))
    {
        error_manager(err);
        return 0;
    }

    int a = 0;

    printf("Before:\n");
    print_lines(&onegin);

    printf("Trimmed sorting:\n");
    if (Errors err = quick_sort(&onegin, compare_lines_trimmed))
    {
        error_manager(err);
        return 0;
    }
    print_lines(&onegin);

    scanf("%d", &a); //пауза между выводами, ожидание ввода

    printf("Trimmed backwards sorting:\n");
    if (Errors err = quick_sort(&onegin, compare_lines_trimmed_backwards))
    {
        error_manager(err);
        return 0;
    }
    print_lines(&onegin);

    Text_dtor(&onegin);

    return 0;
}

Errors get_file_buf(Text* text)
{
    assert(text);
    assert(text->path);

    FILE* file = fopen(text->path, "r");
    if (file == NULL)
        return NULLF;

    struct stat buff;
    stat(text->path, &buff);

    text->buffer_count = buff.st_size + 1;

    if (text->buffer_count <= 1)
        return EMPTYF;

    text->buffer_count++; //+1 элемент

    text->buffer = (char*)calloc(text->buffer_count, sizeof(char));
    assert(text->buffer);

    fread(text->buffer, sizeof(char), text->buffer_count, file);

    if (fclose(file) == EOF)
    {
        return ENDOF;
    }
    return OK;
}

Errors newline_count(Text* text)
{
    assert(text);
    assert(text->buffer);
    assert(text->buffer_count > 0);

    int cnt = 1;

    for (int i = 0; i < text->buffer_count; i++) //сначала посчитаем кол-во строк
    {
        if (text->buffer[i] == '\n')
            cnt++;
    }

    text->lines_count = cnt;
    return OK;
}

Errors get_lines(Text* text)
{
    assert(text);
    assert(text->buffer);
    assert(text->buffer_count > 0);

    Errors err = newline_count(text);

    if (error_manager(err) != OK)
        return err;

    text->lines = (Line*)calloc(text->lines_count, sizeof(Line));

    assert(text->lines);

    int current_length = 0;
    int temporary_lines_count = 0;

    Line current_line = {NULL, 0};

    for (int i = 0; i < text->lines_count; i++)
    {
        text->lines[i] = current_line;
    }

    text->lines[temporary_lines_count].start_ptr = text->buffer;

    for (int i = 0; i < text->buffer_count; i++)
    {
        current_length++;

        if (text->buffer[i] == '\n')
        {
            text->lines[temporary_lines_count].length = current_length - 1;
            temporary_lines_count++;
            text->lines[temporary_lines_count].start_ptr = text->buffer + i + 1;
            current_length = 0;
            text->buffer[i] = '\0';
        }
    }
    text->lines[temporary_lines_count].length = strlen(text->lines[temporary_lines_count].start_ptr);

    return OK;
}

Errors print_lines(Text* text)
{
    Text_verif(text);

    printf("lines = %p, lines_count = %d\n", text->lines, text->lines_count);
    for (int i = 0; i < text->lines_count; i++)
    {
        printf("lines[%d] = length %d = %p = %s\n", i, text->lines[i].length, text->lines[i].start_ptr, text->lines[i].start_ptr);
    }
    return OK;
}

Errors print_buffer(Text* text)
{
    Text_verif(text);

    printf("buffer = %p, buffer_count = %d\n", text->buffer, text->buffer_count);
    for (int i = 0; i < text->buffer_count; i++)
    {
        printf("buffer[%d] = %p = %c\n", i, text->buffer+i, text->buffer[i]);
    }
    return OK;
}

Errors Text_ctor(Text* text)
{
    text->path = NULL;
    text->buffer = NULL;
    text->buffer_count = 0;
    text->lines = NULL;
    text->lines_count = 0;

    return OK;
}

Errors Text_dtor(Text* text)
{
    free(text->buffer);
    free(text->lines);
    text->path = NULL;
    text->buffer_count = -1;
    text->lines_count = -1;

    return OK;
}

Errors Text_verif(Text *text)
{
    assert(text);
    assert(text->path);
    assert(text->buffer);
    assert(text->buffer_count > 0);
    assert(text->lines);
    assert(text->lines_count > 0);

    return OK;
}

Errors error_manager(Errors err)
{
    switch (err)
    {
        case OK:
            return OK;
        case ERR:
            printf("Error! Code: ERR\n");
            return ERR;
        case EMPTYF:
            printf("Error! Code: EMPTYF\n");
            return EMPTYF;
        case ENDOF:
            printf("Error! Code: ENDOF\n");
            return ENDOF;
        case NULLF:
            printf("Error! Code: NULLF\n");
            return NULLF;
        default:
            return OK;
    }
}

Errors quick_sort(Text *text, int (*comp)(const void *, const void *))
{
    Text_verif(text);

    qsort(text->lines, text->lines_count, sizeof(Line), comp);

    return OK;
}

int compare_lines_strcmp(const void *a, const void *b)
{
    const char* arg1 = (const char*)(((Line*)a)->start_ptr);
    const char* arg2 = (const char*)(((Line*)b)->start_ptr);

    return strcmp(arg1, arg2);
}

int compare_lines(const void *a, const void *b)
{
    const char* arg1 = (const char*)(((Line*)a)->start_ptr);
    const char* arg2 = (const char*)(((Line*)b)->start_ptr);

    int i = 0;
    char c1 = arg1[i];
    char c2 = arg2[i];

    while (c1 != '\0' && c2 != '\0')
    {
        if (c1 < c2)
        {
            return -1;
        }
        else if (c1 > c2)
        {
            return 1;
        }

        i++;

        c1 = arg1[i];
        c2 = arg2[i];
    }

    if (c1 == '\0')
    {
        if (c2 == '\0')
            return 0;
        return -1;
    }
    return 1;
}

int compare_lines_trimmed(const void *a, const void *b)
{
    const char* arg1 = (const char*)(((Line*)a)->start_ptr);
    const char* arg2 = (const char*)(((Line*)b)->start_ptr);

    int i = 0;
    int j = 0;
    char c1 = arg1[i];
    char c2 = arg2[j];

    while (c1 != '\0' && c2 != '\0')
    {
        if (c1 < c2)
        {
            return -1;
        }
        else if (c1 > c2)\
        {
            return 1;
        }

        i++;
        j++;

        c1 = arg1[i];
        c2 = arg2[j];

        while (!is_letter(c1))
        {
            if (c1 == '\0') break;
            i++;
            c1 = arg1[i];
        }

        while (!is_letter(c2))
        {
            if (c2 == '\0') break;
            j++;
            c2 = arg2[j];
        }
    }

    if (c1 == '\0')
    {
        if (c2 == '\0')
            return 0;
        return -1;
    }
    return 1;
}

int compare_lines_trimmed_backwards(const void *a, const void *b)
{
    const char* arg1 = (const char*)(((Line*)a)->start_ptr);
    const char* arg2 = (const char*)(((Line*)b)->start_ptr);

    int i = (((Line*)a)->length)-1;
    int j = (((Line*)b)->length)-1;
    char c1 = arg1[i];
    char c2 = arg2[j];

    while (i >= 0 && j >= 0)
    {
        if (c1 < c2)
        {
            return -1;
        }
        else if (c1 > c2)
        {
            return 1;
        }

        i--;
        j--;

        c1 = arg1[i];
        c2 = arg2[j];

        while (!is_letter(c1))
        {
            if (i < 0) break;
            i--;
            c1 = arg1[i];
        }

        while (!is_letter(c2))
        {
            if (j < 0) break;
            j--;
            c2 = arg2[j];
        }
    }
    if (i < 0)
    {
        if (j < 0)
            return 0;
        return -1;
    }
    return 1;
}


int is_letter(char c)
{
    return ((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')));
}
