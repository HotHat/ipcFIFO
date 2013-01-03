#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/stat.h>

#define		FIFO_START	"myfifo"
#define		FIFO_FINAL	"myfifo2"
#define		SZ			2

int main(int argc, char* argv[]) {
	FILE* 	fp;
	FILE*	fp2;

	int		numbuf[SZ];
	int		res;

	/* trata os argumentos passados */
	if (argc != 3) {
		fprintf(stderr,"USAGE %s [num1] [num2]\n",argv[0]);
		return 1;
	}
	
	/* abre o FIFO_START para escrever os argumentos passados */
	if ( (fp = fopen(FIFO_START,"w")) == NULL ) {
		perror("fopen");
		return 1;
	}

	/* converte char -> int */
	numbuf[0] = atoi(argv[1]);
	numbuf[1] = atoi(argv[2]);

	printf("valores enviados: %d e %d\n",numbuf[0],numbuf[1]);
	

	/* escreve na FIFO */
	fwrite(numbuf,sizeof(numbuf),1,fp);
	fclose(fp);

	/* Cria-se a outra FIFO e espera o server mandar a soma */
	umask(0);
	mknod(FIFO_FINAL, S_IFIFO|0666, 0);
	
	fp2 = fopen(FIFO_FINAL,"r");
	fread(&res, sizeof(res),1,fp2);
	fclose(fp2);

	printf("Numero recebido: %d \n",res);

	return 0;
}
