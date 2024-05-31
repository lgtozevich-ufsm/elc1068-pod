#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "common/buffered_io.h"

BufferedReader open_reader(char *filename, int capacity)
{
    BufferedReader reader;
    reader.file = fopen(filename, "r");
    reader.buffer = (char *)malloc(capacity * sizeof(char));
    reader.index = 0;
    reader.size = 0;
    reader.capacity = capacity;

    return reader;
}

void close_reader(BufferedReader *reader)
{
    fclose(reader->file);
    free(reader->buffer);
}

int read_number(BufferedReader *reader)
{
    int number = 0;

    while (true)
    {

        char c = read_char(reader);

        if (!isdigit(c))
        {
            if (c == '\n')
            {
                break;
            }

            fprintf(stderr, "ERROR: unexpected end of number. Expected: '\\n', Received: %c.\n", c);
            exit(EXIT_FAILURE);
        }

        number = number * 10 + (c - '0');
    }

    return number;
}

void refill_reader_buffer(BufferedReader *reader)
{
    reader->size = fread(reader->buffer, sizeof(char), reader->capacity, reader->file);
    reader->index = 0;
}

char read_char(BufferedReader *reader)
{
    if (has_reader_ended(reader))
    {
        fprintf(stderr, "ERROR: unexpected end of file when trying to read character.\n");
        exit(EXIT_FAILURE);
    }

    if (reader->index == reader->size)
    {
        refill_reader_buffer(reader);
    }

    return reader->buffer[reader->index++];
}

bool has_reader_ended(BufferedReader *reader)
{
    return reader->index == reader->size && feof(reader->file);
}

BufferedWriter open_writer(char *filename, int capacity)
{
    BufferedWriter writer;
    writer.file = fopen(filename, "w");
    writer.buffer = (char *)malloc(capacity * sizeof(char));
    writer.size = 0;
    writer.capacity = capacity;

    return writer;
}

void close_writer(BufferedWriter *writer)
{
    flush_writer(writer);
    fclose(writer->file);
    free(writer->buffer);
}

void write_number(BufferedWriter *writer, int number)
{
    char *buffer = NULL;
    int buffer_size = 0;
    int index = 0;

    do
    {

        if (index >= buffer_size)
        {
            buffer_size += 10;
            buffer = (char *)realloc(buffer, buffer_size * sizeof(char));
            if (buffer == NULL)
            {
                fprintf(stderr, "ERROR: Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }
        }

        buffer[index++] = number % 10 + '0';
        number /= 10;
    } while (number > 0);

    for (int i = index - 1; i >= 0; i--)
    {
        write_char(writer, buffer[i]);
    }

    free(buffer);

    write_char(writer, '\n');
}

void write_char(BufferedWriter *writer, char c)
{
    if (writer->size == writer->capacity)
    {
        flush_writer(writer);
    }

    writer->buffer[writer->size++] = c;
}

void flush_writer(BufferedWriter *writer)
{
    fwrite(writer->buffer, sizeof(char), writer->size, writer->file);
    writer->size = 0;
}