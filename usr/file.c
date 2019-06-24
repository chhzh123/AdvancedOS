// Copyright (c) 2019 Hongzheng Chen
// chenhzh37@mail2.sysu.edu.cn

#include "stdio.h"
#include "file.h"

int main()
{
    // Declare the file pointer 
    FILE* fp;

    // Get the data to be written in file 
    char dataToBeWritten[50] = "This is a test message!"; 
  
    // Open the non-existing file with fopen()
    fp = fopen("testmsg.txt", "w");

    // Check if this fp is null 
    // which maybe if the file does not exist 
    if (fp == NULL)
    {
        printf("testmsg.txt file failed to open!"); 
    }
    else
    {

        printf("The file is now opened.\n");

        // Write the dataToBeWritten into the file 
        if (strlen(dataToBeWritten) > 0) 
        {
            // writing in the file using fputs()
            fputs(dataToBeWritten, fp);
            // fputs("\n", fp);
        }

        char dataToBeRead[50];
        fgets(dataToBeRead,strlen(dataToBeWritten),fp);
        dataToBeRead[strlen(dataToBeWritten)] = '\0';

        printf("========= Data read out =========\n\n");
        printf("%s\n\n", dataToBeRead);
        printf("=================================\n\n");

        // Closing the file using fclose()
        fclose(fp);

        printf("Data successfully written in file testmsg.txt\n"); 
        printf("The file is now closed.\n");
    }
    return 0;
}