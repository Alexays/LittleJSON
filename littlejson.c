/*
** parse.c for json in /home/rouill_a/Tek1/Graph/raytracer2/bonus/src/jparser
** 
** Made by Alexis Rouillard
** Login   <alexis.rouillard@epitech.eu>
** 
** Started on  Sun May  7 12:42:21 2017 Alexis Rouillard
** Last update Sun May  7 22:22:35 2017 Alexis Rouillard
*/

#include "littlejson.h"

int	j_parse(const char *str, t_j_val *s)
{
  char	*buff;

  buff = (char *)str;
  while (*buff == 32 || *buff == '\n')
    buff++;
  s->type = NONE;
  if (*buff == '{')
    s->type = JSON_OBJECT;
  if (*buff == '[')
    s->type = JSON_ARRAY;
  if (*buff == '\"')
    s->type = JSON_STRING;
  if (*buff == '-' || (*buff >= '0' && *buff <= '9'))
    s->type = JSON_NUMBER;
  if (*buff == 't')
    s->type = JSON_TRUE;
  if (*buff == 'f')
    s->type = JSON_FALSE;
  if (*buff == 'n')
    s->type = JSON_NULL;
  if (s->type != NONE)
    s->val = buff;
  return ((s->type != NONE) ? 1: 0);
}

int	parse_string(char *beg, char **end)
{
  char	*buff;

  if (*beg != '"')
    return (0);
  buff = beg;
  while (*(++buff) != '"')
    if (*buff == '\\')
      buff++;
  *end = buff;
  return (1);
}

int	skip_string(char **tmp)
{
  char	*buff;
  char	*beg;

  buff = *tmp;
  beg = *tmp;
  if (!parse_string(*tmp, &buff))
    {
      *tmp = beg;
      return (0);
    }
  *tmp = buff;
  return (1);
}

int	parse_object(char *beg, char **end)
{
  char	*buff;

  if ((buff = beg + 1) && *beg != '{')
    return (0);
  while (*buff != '}')
    {
      SKIPSPACE(buff);
      if (!skip_string(&buff))
	return (0);
      buff++;
      SKIPSPACE(buff);
      if (*buff != ':')
	return (0);
      buff++;
      SKIPSPACE(buff);
      if (!skip_val(&buff))
	return (0);
      buff++;
      SKIPSPACE(buff);
      if (*buff != ',' && *buff != '}')
	return (0);
      if (*buff == ',' && (buff += 1))
	SKIPSPACE(buff);
    }
  return (*end = buff, 1);
}

int	skip_object(char **tmp)
{
  char	*buff;
  char	*beg;

  beg = *tmp;
  if (!parse_object(*tmp, &buff))
    {
      *tmp = beg;
      return (0);
    }
  *tmp = buff;
  return (1);
}

int	parse_array(char *beg, char **end)
{
  char	*buff;

  if (*beg != '[')
    return (0);
  buff = beg + 1;
  while (*buff != ']')
    {
      while (*buff == 32 || *buff == '\n')
	buff++;
      if (!skip_val(&buff))
	return (0);
      buff++;
      while (*buff == 32 || *buff == '\n')
	buff++;
      if (*buff != ',' && *buff != ']')
	return (0);
      if (*buff == ',')
	{
	  buff++;
	  while (*buff == 32 || *buff == '\n')
	    buff++;
	}
    }
  *end = buff;
  return (1);
}

int	skip_array(char **tmp)
{
  char	*buff;
  char	*beg;

  beg = *tmp;
  if (!parse_array(*tmp, &buff))
    {
      *tmp = beg;
      return (0);
    }
  *tmp = buff;
  return (1);
}

int	parse_number(char *beg, char **end)
{
  char	*buff;

  buff = beg;
  while (('0' <= (*buff) && (*buff) <= '9') || *buff == '-')
    buff++;
  *end = buff - 1;
  if (beg > *end)
    return (0);
  return (1);
}

int	skip_number(char **tmp)
{
  char	*buff;
  char	*beg;

  beg = *tmp;
  if (!parse_number(*tmp, &buff))
    {
      *tmp = beg;
      return (0);
    }
  *tmp = buff;
  return (1);
}

int	skip_val(char **tmp)
{
  char	*beg;
  int	res;

  beg = *tmp;
  res = 1;
  if ((**tmp == '{' && !(res = skip_object(tmp))) ||
      (**tmp == '[' && !(res = skip_array(tmp))) ||
      (**tmp == '"' && !(res = skip_string(tmp))) ||
      ((**tmp == '-' || (**tmp >= '0' && **tmp <= '9')) &&
       (res = !skip_number(tmp))))
    *tmp = beg;
  if (**tmp == 't' && !strncmp(*tmp, "true", 4))
    *tmp += 3;
  else if (**tmp == 't')
    return (*tmp = beg, 0);
  if (**tmp == 'f' && !strncmp(*tmp, "false", 5))
    *tmp += 4;
  else if (**tmp == 'f')
    return (*tmp = beg, 0);
  if (**tmp == 'n' && !strncmp(*tmp, "null", 4))
    *tmp += 3;
  else if (**tmp == 'n')
    return (*tmp = beg, 0);
  return (res);
}

int	prse_obj(int len, char **buff, t_j_val *s, const char *key)
{
  char	*tmp;

  if (len > 0 && **buff == '\"' && **(buff + len + 1) == '\"' &&
      !strncmp(*(buff + 1), (char *)key, len))
    {
      tmp = *(buff + len + 2);
      SKIPSPACE(tmp);
      if (*tmp != ':')
	return (0);
      tmp++;
      SKIPSPACE(tmp);
      return (j_parse(tmp, s));
    }
  else if (len == 0 && **buff == '\"' && (tmp = *buff))
    {
      while (*tmp != '\"')
	tmp++;
      tmp++;
      if (*tmp != ':')
	return (0);
      tmp++;
      SKIPSPACE(tmp);
      return (j_parse(tmp, s));
    }
  return (-1);
}

int	j_get_obj(t_j_val json, const char *key, t_j_val *s)
{
  char	*buff;

  if (json.type != JSON_OBJECT)
    return (0);
  buff = json.val + 1;
  while (1)
    {
      SKIPSPACE(buff);
      if (*buff == '}' || !prse_obj(strlen((char *)key), &buff, s, key) ||
	  !skip_string(&buff))
	return (0);
      buff++;
      SKIPSPACE(buff);
      if (*buff != ':')
	return (0);
      buff++;
      SKIPSPACE(buff);
      if (!skip_val(&buff))
	return (0);
      buff++;
      SKIPSPACE(buff);
      if (*buff == ',')
	buff++;
      SKIPSPACE(buff);
    }
}

int	j_get_array(t_j_val json, int idx, t_j_val *s)
{
  char	*buff;

  if (json.type != JSON_ARRAY)
    return (0);
  buff = json.val;
  buff++;
  while (idx--)
    {
      while (*buff == 32 || *buff == '\n')
	buff++;
      if (!skip_val(&buff))
	return (0);
      buff++;
      while (*buff == 32 || *buff == '\n')
	buff++;
      if (*buff != ',')
	return (0);
      buff++;
    }
  while (*buff == 32 || *buff == '\n')
    buff++;
  return (j_parse(buff, s));
}

int	j_get_number(t_j_val json, int	*val)
{
  char	*end;

  if (json.type != JSON_NUMBER)
    return (0);
  if (!parse_number(json.val, &end))
    return (0);
  *val = atoi(json.val);
  return (1);
}

int	j_get_string(t_j_val json, char *val)
{
  char	*end;
  int	i;

  if (json.type != JSON_STRING)
    return (0);
  if (!parse_string(json.val, &end))
    return (0);
  i = -1;
  while (++i < end - json.val - 1)
    *(val + i) = *(json.val + i + 1);
  *(val + i) = 0;
  return (1);
}
