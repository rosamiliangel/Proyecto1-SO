/*
parser.c / parser.h: Manipulación de cadenas de texto (parse line, tokenización,
detección de operadores y pipes)
*/

#ifndef parser_h
#define parser_h

int parse_line(char *line, char **args);

#endif // parser_h