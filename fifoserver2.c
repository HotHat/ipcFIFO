#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/stat.h>

#define		FIFO_FILE	"myfifo"
#define		FIFO_FINAL	"myfifo2"
#define		SZBUF		80
#define		SZ 			2
#define		QUIT		"bye"

int main(void) {
	FILE* fp;
	FILE* fp2;
	char readbuf[SZBUF];
	int  numbuf[SZ];
	int	 res;

	
	/*create the fifo*/
	umask(0);
	mknod(FIFO_FILE, S_IFIFO|0666, 0);
	
	printf("Running server... To quit, send via the client the msg 'bye'.\n");
	
	while(1) {

		/* Lê um vetor do FIFO_FILE */
		fp = fopen(FIFO_FILE,"r");
		fread(numbuf, sizeof(numbuf),1,fp);

		printf("Received numbers: %d %d\n",numbuf[0],numbuf[1]);

		fclose(fp);

		/*calcula a soma para retornar através de outra fifo*/
		res = numbuf[0]	+ numbuf[1];
		printf("resultado da soma: %d\n",res);

		/* abre a outra fifo para escrita */
		if ( (fp2 = fopen(FIFO_FINAL,"w")) == NULL ) {
			perror("fopen");
			return 1;
		}	

		/* escreve na fifo */
		fwrite(&res,sizeof(res),1,fp2);
		fclose(fp2);
	}
	
	return 0;
}
