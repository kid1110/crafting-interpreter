#ifndef clox_scanner_h
#define clox_scanner_h

typedef struct 
{
    const char* start;
    const char* current;
    int line;

}Scanner;


void initScanner(const char* source);

#endif