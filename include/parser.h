/*
parser.c / parser.h: Manipulación de cadenas de texto (parse line, tokenización,
detección de operadores y pipes)
*/

#ifndef parser_h
#define parser_h

//Parsear la cadena
int parse_line(char *line, char **args,  int *in_background);

//Remover comillas de una cadena si las hay
void remover_comillas(char *args);

#endif