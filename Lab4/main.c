// This code is implemented for fourth lab of EECS 678 Lab - University of Kansas

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>

// Recursive file counter
int getNumOfFilesRec(char *path);

int main(int argc,char* argv[]) {
    int numOfFiles=0;

    if(argc != 2){
        fprintf(stderr, "Usage: %s absolute_path_to_directory\n", argv[0]);
        return EXIT_FAILURE;
    }

#ifdef LAB_CODE
    // Create a pipe,fork, and call the function. The child will write the returned
    // value(number of files) to the parent. Parent should read it and wait for child
    // to finish.
  int fds[2];
	pipe(fds);
	pid_t process_id;
	process_id = fork();

	switch(process_id)
	{
		case -1:
			perror("fork");
			exit(1);
		case 0:
			close(fds[0]);
			numOfFiles = getNumOfFilesRec(argv[1]);
			write(fds[1], &numOfFiles, sizeof(numOfFiles));
			close(fds[1]);
			exit(0);
		default:
			close(fds[1]);
			read(fds[0], &numOfFiles, sizeof(numOfFiles));
			wait(0);

	}


#else
    numOfFiles = getNumOfFilesRec(argv[1]);
#endif

    if(numOfFiles < 0){
        printf("An error occurred.\n");
        return EXIT_FAILURE;
    }

    printf("Number of files: %d\n", numOfFiles);
    return EXIT_SUCCESS;
}

int getNumOfFilesRec(char path[PATH_MAX]){
    DIR* directoryPtr; /*DIR* of opened directory*/
    struct dirent* direntPtr;/*Reading a directory*/
    struct stat statBuf; /*Get status information of a file*/
    int numOfFiles=0;
    char targetPath[PATH_MAX];
#ifdef LAB_CODE
    // Create a pipe for all children. No need to create an external
    // pipe for each children.
    int fds[2];
	  pipe(fds);

#endif

    printf("Process %u counting files in path: %s\n", getpid(), path);

    /*Open given directory*/
    if((directoryPtr = opendir(path)) == NULL){
        perror("Failed to open file directory");
        return -1;
    }

    // the direntPtr can point to a file or directory
    while((direntPtr = readdir(directoryPtr)) != NULL){
        if(strcmp(".",direntPtr->d_name) == 0 || // skip these directories
           strcmp("..",direntPtr->d_name) == 0)
            continue;

        sprintf(targetPath, "%s/%s", path, direntPtr->d_name);
        if(stat(targetPath, &statBuf) == -1){ /*Get file mode (file or directory)*/
            perror("Failed to get file status");
            goto finish;
        }

        // check whether it is directory or not
        if(S_ISDIR(statBuf.st_mode)){
            /*Directory found*/
#ifdef LAB_CODE
            // Create a child process to get into the directory
            // to count files. Write the returned number of files
            // to parent via pipe
      pid_t process_id;
			process_id = fork();
			int returnValue;

			switch(process_id)
			{
				case -1:
					perror("fork");
					exit(1);
				case 0:
					close(fds[0]);
					returnValue = getNumOfFilesRec(targetPath);
					write(fds[1], &returnValue, sizeof(returnValue));
					close(fds[1]);
					exit(0);
				default:
					break;
			}

#else
            int returnVal = getNumOfFilesRec(targetPath);
            if (returnVal == -1) {
                numOfFiles = -1;
                goto finish;
            }
            numOfFiles += returnVal;
#endif
        }
        else{ /*File found*/
#ifdef DEBUG
            printf("File found: %s\n",targetPath);
#endif
            ++numOfFiles;
        }
    }

#ifdef LAB_CODE
    // read the number of files integer coming from children and
    // wait all of them to finish execution
  close(fds[1]);
	int returnValue;

	while(read(fds[0], &returnValue, sizeof(returnValue)) >= 1){
		numOfFiles = numOfFiles + returnValue;
  }
	wait(0);
	goto finish;

#endif

finish:
    /*Close directory*/
    if(closedir(directoryPtr) == -1){
        perror("Failed to close directory");
        return -1;
    }

    return numOfFiles;
}
