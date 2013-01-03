#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/stat.h>

#define		FIFO_START	"myfifo"
#define		FIFO_FINAL	"myfifo2"
#define		SZ 			2

int main(int argc, char* argv[]) {
	pid_t   childpid;

	FILE* 	fp;
	FILE*	fp2;

	int		numbuf[SZ];
	int		res;	

	/* trata os argumentos passados */
	if (argc != 3) {
		fprintf(stderr,"USAGE %s [num1] [num2]\n",argv[0]);
		return 1;
	}

	/* converte char -> int */
	numbuf[0] = atoi(argv[1]);
	numbuf[1] = atoi(argv[2]);	

	/*create the fifo's*/
	umask(0);
	mknod(FIFO_START, S_IFIFO|0666, 0);		
	mknod(FIFO_FINAL, S_IFIFO|0666, 0);	

	/* forka o processo */
	if ( (childpid = fork()) == -1 ) {
		perror("fork");
		return 1;
	}

	if (!childpid) {
		/* filho */


		
		/* Lê um vetor do FIFO_START */
		fp = fopen(FIFO_START,"r");
		fread(numbuf, sizeof(numbuf),1,fp);

		printf("[filho] numeros recebidos: %d %d\n",numbuf[0],numbuf[1]);

		fclose(fp);

		/*calcula a soma para retornar através de outra fifo*/
		res = numbuf[0]	+ numbuf[1];
		printf("[filho] valor a ser enviado: %d\n",res);



		/* abre a outra fifo para escrita */
		if ( (fp2 = fopen(FIFO_FINAL,"w")) == NULL ) {
			perror("fopen");
			return 1;
		}
			

		/* escreve na fifo */
		fwrite(&res,sizeof(res),1,fp2);
		fclose(fp2);
		
			

		return 0;

	} else {
		/* pai */

			/* abre o FIFO_START para escrever os argumentos passados */
		if ( (fp = fopen(FIFO_START,"w")) == NULL ) {
			perror("fopen");
			return 1;
		}

		printf("[pai] numeros enviados: %d %d\n",numbuf[0],numbuf[1]);
		

		/* escreve na FIFO */
		fwrite(numbuf,sizeof(numbuf),1,fp);
		fclose(fp);


		if ( (fp2 = fopen(FIFO_FINAL,"r")) == NULL ) {
			perror("fopen");
		}

		fread(&res, sizeof(res),1,fp2);
		fclose(fp2);

		printf("[pai] soma recebida: %d \n",res);
		return 0;

	}

	return 0;
}