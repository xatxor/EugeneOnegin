#include <stdio.h>
#include <stdlib.h>
#include <sys\stat.h>

int main(void)
{
    char path[] = "text.txt";

    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        printf("ERR: file == NULL");
        return 0;
    }

    struct stat buff;
    stat(path, &buff);

    const int buf_length = buff.st_size + 1;

    char *buf = (char*)calloc(buf_length + 1, sizeof(char));

    if (fread(buf, sizeof(char), buf_length+1, file) == 0)
    {
        printf("Error occured or file is empty. Bye!");
        return 0;
    }

    int lines_count = 1;

    for (int i = 0; i < buf_length; i++) //������� ��������� ���-�� �����
    {
        if (buf[i] == '\n')
            lines_count++;
    }

    char **text = (char**)calloc(lines_count, sizeof(char*)); //�������� ������ ��� ����� ��������� ��������� �� ������ �����

    int temporary_lines_count = 0;
    text[temporary_lines_count++] = buf; //������ ������ ������

    for (int i = 0; i < buf_length; i++)
    {
        if (buf[i] == '\n')
        {
            text[temporary_lines_count++] = buf + i + 1;
            buf[i] = '\0';
        }
    }

    for (int i = 0; i < lines_count; i++)
    {
        printf("text[%d] = %p = %s\n", i, text[i], text[i]);
    }

    free(buf);
    for (int i = 0; i < lines_count; i++)
    {
        free(text[i]);
    }

    fclose(file);

    return 0;
}
