
//--------------------------------------------------------------
// hexdump.cpp : Dump a file in hex format
// Jonnie Gilmore, 28/07/2005 16:12:51
//--------------------------------------------------------------

#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <io.h>

#define TRUE  -1
#define FALSE 0

int ASCII_only;
int Add_Formfeed;

void usage(char *msg)
//--------------------------------------------------------------
// 
//--------------------------------------------------------------
{
  printf("\n");
  printf("Usage: dump filename.ext {f2.x...fn.x} {-[af]}\n");
  printf(  "       a = ASCII only\n");
  printf(  "       f = Add Formfeed between files.\n");
  printf(  "       \n");
  printf(  "   %s\n",msg);
}

int hexdump(char *file)
//--------------------------------------------------------------
// 
//--------------------------------------------------------------
{
   FILE *infile;
   unsigned int SizeOfFile;

   infile = fopen(file,"rb");
   if (!infile)
   {
      printf("\nCannot open %s.",file);
      return 1;
   } 

   SizeOfFile = filelength(fileno(infile));
   printf("\n");
   printf("HEXDUMP: File = \"%s\"\n",file); // Print file info
   printf("         Size = %lu (0x%X) bytes\n",SizeOfFile,SizeOfFile); // Print file info
   printf("         Switches = ASCII_Only=%s, Add_Formfeed=%s\n",ASCII_only?"Yes":"No",Add_Formfeed?"Yes":"No"); // Print file info

   if (ASCII_only)
   {
      int CharsProcessed;
      char ch;
      unsigned int i;
      int LineOpen;

      CharsProcessed = 0;
      LineOpen = 0;
      //while (!feof(infile))
      for (i=0; i<SizeOfFile; i++)
      {
         ch = (char)getc(infile);
         if (CharsProcessed%64 == 0)
         {
           printf("%08X: \"",CharsProcessed); // Print offset
           LineOpen = 1;
         }
         CharsProcessed++;

         printf("%c",isprint(ch)?ch:'.'); // Print byte in ascii 

         if (CharsProcessed%64 == 0)
         {
           printf("\"\n");
           LineOpen = 0;
         }
      }
      if (LineOpen)
      {
        printf("\"\n");
        LineOpen = 0;
      }
   }
   else
   {
      int StartAddress;
      int CharsProcessed;
      char ch;
      unsigned int i;
      char szAscii[17];

      CharsProcessed = 0;
      StartAddress = 0;
      memset(szAscii,0,17);

      //while (!feof(infile))
      for (i=0; i<SizeOfFile; i++)
      {
         ch = (char)getc(infile);
         szAscii[CharsProcessed  ] = ch;
         szAscii[CharsProcessed+1] = 0;

         CharsProcessed++;

         if (CharsProcessed==16) // i.e. we alread have chars 0..15
         {
            int j;

            printf("%08X: ",StartAddress); // Print offset
            StartAddress += CharsProcessed;
            for (j=0;j<16;j++)
            {
               printf("%02X ",szAscii[j]&0xFF); // Print byte in hex  (&0xff for annoying Tc hex bug)
               if (!isprint(szAscii[j]))
                  szAscii[j] = '.';
            }
            printf("\"%s\"\n",szAscii); // Print string in ascii 
            szAscii[0] = 0;
            CharsProcessed = 0;
         }
      }

      if (CharsProcessed>0) // Some still unreported
      {
         int i;

         printf("%08X: ",StartAddress); // Print offset

         for (i=0;i<16;i++)
         {
            if (i < CharsProcessed)
            {
               printf("%02X ",szAscii[i]&0xFF); // Print byte in hex 
               if (!isprint(szAscii[i]))
                  szAscii[i] = '.';
            } 
            else 
            {
               printf("   "); // Print filler
               szAscii[i]=' ';
            }
         }
         printf("\"%s\"\n",szAscii); // Print string in ascii 
      }
   }

   fclose(infile);

   if (Add_Formfeed)
   printf("\f");

   return 0;
}

int main(int argc, char* argv[])
//--------------------------------------------------------------
// 
//--------------------------------------------------------------
{
   int c;

   ASCII_only = FALSE;
   Add_Formfeed = FALSE;

   if (argc==1)
    usage("Too few parameters.");

   /////////////////////
   // Gather params
   for(c=argc-1;c>0;c--)
   {
      if (argv[c][0] == '-')
      {
         switch (tolower(argv[c][1])) 
         {
            case 'a': 
               ASCII_only = TRUE;
               break;
            case 'f': 
               Add_Formfeed = TRUE;
               break;
            default : 
               usage("Invalid switch");
               printf(" - Invalid switch \"-%c\"",argv[c][1]);
         }
      }
   }

   /////////////////////
   // Do the work
   for (c=1;c<argc;c++)
   {
      int rc;
      if (argv[c][0] != '-')
         rc = hexdump(argv[c]);
      //if (rc)
      //   break;
   }
   return 0;
}

