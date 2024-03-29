/* unimelb comp10002 assignment */
/* a basic text formatting program that can process some simple commands */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEFMARGIN 4     /* default margin size */
#define DEFWIDTH 50     /* default line width */
#define MAXSIZE 999     /* max line size */

/* command signals */

#define NOCOMMAND 0     /* no command needs to be processed */
#define NEWLINE 1       /* command signal to break lines */
#define NEWPARA 2       /* command signal for new paragraphs */
#define INITIAL 3       /* setting when we are allready in a new para */

/* number constants for commands ...sizes ect.. */

#define COMDIGSIZE 2    /* number of digits in commands */
#define NCOMSIZE 5      /* number of chars in commands containing nums */
#define SCOMSIZE 2      /* num of chars in break, para, center comms */
#define COMID 1         /* position of command identifier */
#define HEADDEPTH 5     /* default depth for headings */

/*  function prototypes */

int read_line(char line[],int size,int *lastchar);
int read_next_word(char word[],char line[],int *pos);
void process_word(char word[],int width,int margin,int *count);

void process_commands(char line[],int *comstatus,int *width,
									int *margin, int section[]);
void process_variables(char line[],int *width,int *margin);
void process_heading(char line[],int section[],int width,int margin);
void process_center(char line[],int width,int margin);

int	mygetchar(void);
int get_num(char line[]);
void print_newlines(int *comstatus);
void print_whitespace(int num);

/**************************************************************************/
/* algorithms are fun */
/* main program that does everything */
int
main(int argc, char *argv[]) {
	char word[MAXSIZE+1], line[MAXSIZE+1];    /* char arrays for word/line */
	int count=0,pos=0,lastchar;     /* variables used to store status info */
	int width = DEFWIDTH, margin = DEFMARGIN;      /* default width/margin */
	int section[HEADDEPTH]={ 0 }; /* initialise all values in section to 0 */
	int comstatus=INITIAL;         /* we are initialy at the start of para */
	
	
	while (read_line(line,MAXSIZE,&lastchar)) {
		
		/* newline implementation 
		 * skip implementing commands if line is empty */
		if ((line[0] != '.') && (strlen(line)>0)) {
			print_newlines(&comstatus);
		}				
		/* processing commands here */
		if (line[0] == '.') {
			process_commands(line,&comstatus,&width,&margin,section);						
			/* write over lines which contained the command signal '.' 
			 * and reset word count */
			line[0] = '\0';
			count = 0;			
		}
		
		while(read_next_word(word,line,&pos)) {
			process_word(word,width,margin,&count);			
		}				
	}
	
	/* all lines have been processed, go home */
	printf("\n");			
	return 0;
	/* algorithms are fun */
}

/**************************************************************************/
/* function that controls all the commands from being processed */
void process_commands(char line[],int *comstatus,int *width,
	int *margin, int section[]) {
	
	int pos=0; /* used for the read_next_word() function */
	char word[MAXSIZE+1]; /* word array for read_next_word function */
	
	if (line[COMID] == 'c') {
		/* need to process commands before centering line
		 * if there isnt a command we need a newline */
		if (*comstatus == NOCOMMAND) {
			*comstatus = NEWLINE;
		}
		print_newlines(comstatus);
		process_center(line,*width,*margin);
		*comstatus = NEWLINE;
		
	} else if (line[COMID] == 'h') {
		/* need to start a heading on a new para */
		/* no new para if there is no para yet */
		if (*comstatus!=INITIAL) {
			 *comstatus = NEWPARA;
		}
		print_newlines(comstatus);
		
		/* print out section and heading */
		process_heading(line,section,*width,*margin);
		while (read_next_word(word,line,&pos)) {
			printf(" %s",word);
		}
		
		/* need a new para after headings */
		*comstatus = NEWPARA;
		
	} else if (line[COMID] == 'b') {
		/* if last line needed a NEWPARA it has precedence */
		if ((*comstatus != NEWPARA) && (*comstatus!=INITIAL)) {
			*comstatus = NEWLINE;
		}
		
	} else if (line[COMID] == 'p') {
		/* newpara commands */
		/* no new para if there is no para yet */
		if (*comstatus!=INITIAL) {
			 *comstatus = NEWPARA;
		}
		
	} else if ((line[COMID] == 'w') || (line[COMID] == 'l')) {
		/* width and margin commands */
		process_variables(line,width,margin);
		
		/* no new para if there is no para yet */
		 if (*comstatus!=INITIAL) {
			 *comstatus = NEWPARA;
		 }
	}
			
}

/**************************************************************************/
/* function that prints a centered line based on the current 
 * width and margin values */
void process_center(char line[],int width,int margin) {
	int textdist = SCOMSIZE+1; /* number of garbage chars due to command */
	
	/* test if valid centered command */
	if (strlen(line)<textdist) {
		printf("SYNTAXERROR: CENTERED COMMAND NEEDS CONTENT\n");
		exit(EXIT_FAILURE);
	}
	
	/* we need a margin */	
	print_whitespace(margin);
	
	/* check if line overflows max line width 
	 * skip over garbage chars */
	if (strlen(line+textdist) >= width) { 
		printf("%s",line+textdist);
	} else {
		/* dividing by int 2 to put the lower half of spaces on the left */
		print_whitespace((width-strlen(line+textdist))/2);
		printf("%s",line+textdist);
		
	}
}

/**************************************************************************/
/* a function that changes width and margin values when these commands 
 * are presented */
void process_variables(char line[],int *width,int *margin) {
	int num;
	
	/* getting number from command using function get_num */
	num = get_num(line);
	
	if (line[COMID] == 'w') {
		*width = num;
	} else {
		*margin = num;
	}
	
}

/**************************************************************************/
/* a function that processes the heading commands 
 * prints out the heading numbers and saves the current section*/
void process_heading(char line[],int section[],int width,int margin) {
	int i,next;
	
	/* getting heading number from line using getnum function */
	next = get_num(line);

	/* if heading level is 1 we need to print dashed line */
	if (next == 1) {
		print_whitespace(margin);
		for (i=0;i<width;i++) {
			printf("-");
		}
		printf("\n");
	}
	
	/* now print the title of the heading */
	if ((next<=HEADDEPTH) && (next>0)) {
		
		/* need a margin and to increase heading number by 1 */
		print_whitespace(margin);
		section[next-1]++;
		
		/* reseting sections above heading num */
		for (i=next;i<HEADDEPTH;i++) {
			section[i] = 0;
		}
		/* print heading nums 
		 * only print periods if guard will be true on next loop */
		for (i=0;i<next;i++) {
			printf("%d",section[i]);
			if ((i+1) < next) {
				printf(".");
			}
		}
		
	} else {
		/* number was invalid for heading */
		printf("ERROR: INVALID HEADING SECTION\n");
		printf("given %d! outside valid range 1-%d\n",next,HEADDEPTH);
		exit(EXIT_FAILURE);
	}
	
	/* now write over the 5 chars in the command line so we can process 
	 * the words only */
	line[0] = line[1] = line[2] = line[3] = line[4] = ' ';
}

/**************************************************************************/
/* a function that puts newlines in based on the comstatus  */
void print_newlines(int *comstatus) {
	if (*comstatus == NEWPARA) {
		printf("\n\n");
	} else if (*comstatus == NEWLINE) {
		printf("\n");
	}
	/* reset command */	
	*comstatus = NOCOMMAND;	

}

/**************************************************************************/	
/* function that reads lines from an input using mygetchar
 * returns 1 if there are more lines to be read
 * returns 0 if we are at EOF and there are no more chars to
 * exits the program if there is a line exeeding 999 chars
*/

int read_line(char line[],int size,int *lastchar) {
	int len=0,c;
	
	/* test if there are more lines to be read */
	if (*lastchar==EOF) {
		return 0;
	}
	
	/* keep accepting chars unless we get EOF or newline chars */
	while ( ((c=mygetchar())!=EOF) && (len<size) && (c!='\n') ) {
		line[len] = c;
		len++;
	}
	
	/* keep last char used to determine if at end of line */	 
	*lastchar = c;
	
	if ((c == '\n') || (c == EOF)) {
		/* close of the line and report line read sucess */
		line[len] = '\0';
		return 1;	
	} else {
		/* max line length limit exceeded */
		printf("ERROR: LINE LIMIT EXCEEDED\n");
		exit(EXIT_FAILURE);	
	}
}

/**************************************************************************/

/* function that reads words from the line input
 * returns 0 if there are no more words to be read in the line
 * returns 1 if word is read
 */
 
int read_next_word(char word[],char line[],int *pos) {
	int len=0;
	
	/* check if pos is at end of line */
	if (line[*pos] == '\0') {		
		word[0] = '\0';
		*pos = 0;
		return 0;
	}
	
	/* skip over any whitespace +\t moving our pos along */
	while (((line[*pos]==' ') || (line[*pos]=='\t')) && (line[*pos] != '\0')) {
		*pos=*pos+1;
	}
	
	/* now we got a word dont lose track of line poisition */
	while((line[*pos]!= ' ') && (line[*pos]!='\t') && (line[*pos] != '\0') ) {
		word[len] = line[*pos];
		len++;
		*pos=*pos+1;
	}

	/* pos is location of whitespace \t or \n 
	 * seal of word and ship it */
	word[len] = '\0';	
	return 1;
}

/**************************************************************************/
/* function that takes a word as input and process it 
 * keeps count of character length and prints margins
 */
void process_word(char word[],int width,int margin,int *count) {

	/* if at start of new line add margin */
	if (*count == 0) {
		print_whitespace(margin);
	}
	
	/* test if we can fit next word in line */
	if (*count+strlen(word)+1 <= width ) {				
		
		if (*count == 0) {
			/*dont include space between margin and next word */
			printf("%s",word);
			*count+=strlen(word);
			
		} else if (strlen(word) != 0) {
			/* include space between words */
			printf(" %s",word);
			*count+=strlen(word)+1;
		}
		
	} else {		
		/* must add word to a new line */
		printf("\n");				
		*count = 0;
		print_whitespace(margin);
		printf("%s",word);
		*count+=strlen(word);
	}
}

/**************************************************************************/
/* a getchar function that skips over \r chars
 * function sourced from the ass1 information and resources page */
int	mygetchar(void) {
	int c;
	while ((c=getchar())=='\r') {
	}
	return c;
}

/**************************************************************************/
/* prints the required ammount of whitespace indicated by the argument num
 */
void print_whitespace(int num) {
	int i;
	for (i = 0;i<num;i++)
		printf(" ");
}

/**************************************************************************/
/* function that returns the number in a command of the form .c dd */
int get_num(char line[]) {
	char numarray[COMDIGSIZE+1];
	
	/* get the first digit at position 3 in the command */
	if (isdigit(line[3])) {
		numarray[0] = line[3];
	} else {
		printf("SYNTAX ERROR: COMMAND NEEDS NUMBER ARGUMENT.\n");
		exit(EXIT_FAILURE);
	}
	
	/* if there is one get the second digit at position 4 in the command
	 * close of the array */
	if (isdigit(line[4])) {
		numarray[1] = line[4];
		numarray[2] = '\0';
	} else {
		numarray[1] = '\0';
	}
	
	return atoi(numarray);
}

/**************************************************************************/