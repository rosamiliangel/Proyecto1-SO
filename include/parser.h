/*
parser.c / parser.h: Manipulación de cadenas de texto (parse line, tokenización,
detección de operadores y pipes)
*/

#ifndef parser_h
#define parser_h

//Se agrego el nuevo parametro para invocar la funcion varias veces en una linea
int parse_line(char *line, char **args,  int *in_background);

#endif // parser_h