#include <stdio.h>
#include <string.h>
int main() {
  char countryNames[5][20] = {"china","america","japan","india","france"};
  char st[20];

  for(int i=0;i<5-1;i++) {
    for (int j=0;j<5-1;j++)
    {
      if (strcmp(countryNames[j], countryNames[j+1]) > 0)
      {
        strcpy(st, countryNames[j]);
        strcpy(countryNames[j], countryNames[j+1]);
        strcpy(countryNames[j+1], st);
      }
    }
  }
  for (int i=0; i<5;i++)
  {
    printf("%s\n",countryNames[i]);
  }

}
