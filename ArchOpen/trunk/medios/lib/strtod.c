/*
* kernel/lib/strtod.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <lib/string.h>
#include <sys_def/ctype.h>

double atof (char *str)
{
    return strtod(str,(char**)NULL);
}

double strtod(char * str,char ** ptr)
{
  char *p;
  
  if (ptr == (char **)0)
    return atof (str);
  
  p = str;
  
  while (isspace (*p))
    ++p;
  
  if (*p == '+' || *p == '-')
    ++p;

  /* INF or INFINITY.  */
  if ((p[0] == 'i' || p[0] == 'I')
      && (p[1] == 'n' || p[1] == 'N')
      && (p[2] == 'f' || p[2] == 'F'))
    {
      if ((p[3] == 'i' || p[3] == 'I')
      && (p[4] == 'n' || p[4] == 'N')
      && (p[5] == 'i' || p[5] == 'I')
      && (p[6] == 't' || p[6] == 'T')
      && (p[7] == 'y' || p[7] == 'Y'))
    {
      *ptr = p + 8;
      return atof (str);
    }
      else
    {
      *ptr = p + 3;
      return atof (str);
    }
    }

  /* NAN or NAN(foo).  */
  if ((p[0] == 'n' || p[0] == 'N')
      && (p[1] == 'a' || p[1] == 'A')
      && (p[2] == 'n' || p[2] == 'N'))
    {
      p += 3;
      if (*p == '(')
    {
      ++p;
      while (*p != '\0' && *p != ')')
        ++p;
      if (*p == ')')
        ++p;
    }
      *ptr = p;
      return atof (str);
    }

  /* digits, with 0 or 1 periods in it.  */
  if (isdigit (*p) || *p == '.')
    {
      int got_dot = 0;
      while (isdigit (*p) || (!got_dot && *p == '.'))
    {
      if (*p == '.')
        got_dot = 1;
      ++p;
    }

      /* Exponent.  */
      if (*p == 'e' || *p == 'E')
    {
      int i;
      i = 1;
      if (p[i] == '+' || p[i] == '-')
        ++i;
      if (isdigit (p[i]))
        {
          while (isdigit (p[i]))
        ++i;
          *ptr = p + i;
          return atof (str);
        }
    }
      *ptr = p;
      return atof (str);
    }
  /* Didn't find any digits.  Doesn't look like a number.  */
  *ptr = str;
  return 0.0;
}
