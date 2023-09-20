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

struct Text {
    char* path;
    char* buffer;
    int buffer_count;
    char** lines;
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
Errors quick_sort(Text *text);
int compare_lines(const void *a, const void *b);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("There is no text path in arguments! Bye!");
        return 0;
    }

    Text onegin = {};
    Text_ctor(&onegin);

    onegin.path = argv[1];

    if (error_manager(get_file_buf(&onegin)) != OK)
        return 0;

    if (error_manager(get_lines(&onegin)) != OK)
        return 0;

    printf("Before sorting:\n");
    print_lines(&onegin);

    if (error_manager(quick_sort(&onegin)) != OK)
        return 0;

    printf("After sorting:\n");
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

    //realloc(text->buffer, (text->buffer_count)*sizeof(char));
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

    //realloc(text->lines, (text->lines_count)*sizeof(char*));
    text->lines = (char**)calloc(text->lines_count, sizeof(char*));

    assert(text->lines);

    int temporary_lines_count = 0;

    text->lines[temporary_lines_count++] = text->buffer; //начало первой строки

    for (int i = 0; i < text->buffer_count; i++)
    {
        if (text->buffer[i] == '\n')
        {
            text->lines[temporary_lines_count++] = text->buffer + i + 1;
            text->buffer[i] = '\0';
        }
    }

    return OK;
}

Errors print_lines(Text* text)
{
    Text_verif(text);

    printf("lines = %p, lines_count = %d\n", text->lines, text->lines_count);
    for (int i = 0; i < text->lines_count; i++)
    {
        printf("lines[%d] = %p = %s\n", i, text->lines[i], text->lines[i]);
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
    text->buffer = NULL; //(char*)calloc(1, sizeof(char));
    text->buffer_count = 0;
    text->lines = NULL; //(char**)calloc(1, sizeof(char*));
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

Errors quick_sort(Text *text)
{
    Text_verif(text);

    qsort(text->lines, text->lines_count, sizeof(char*), compare_lines);

    return OK;
}

int compare_lines(const void *a, const void *b) //сравнение по первой букве строки
{
    int arg1 = **(const char**)a;
    int arg2 = **(const char**)b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}
