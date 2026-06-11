/*
parser.c / parser.h: Manipulación de cadenas de texto (parse line, tokenización,
detección de operadores y pipes)
*/

#ifndef parser_h
#define parser_h

//Se agrego el nuevo parametro para invocar la funcion varias veces en una linea
int parse_line(char *line, char **args,  int *in_background);

//Remover comillas de una cadena (si las hay)
void remover_comillas(char *args);

#endif // parser_h