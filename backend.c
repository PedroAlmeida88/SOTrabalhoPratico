#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

//typedef struct promocao Promocao;
typedef struct promocao {
    char categoria[50];
    int desconto;
    int duracao;
}Promocao;


int numArgumentos(char str[]) {
    int tam = 0;
    char aux[128];
    strcpy(aux, str);
    char *token = strtok(aux, " ");

    while (token != NULL) {
        tam++;
        token = strtok(NULL, " ");
    }
    return tam;
} //TODO:Por nos utils.

Promocao lancaPromotor(char *nomePromotor) {
    Promocao prom;
    char frase[100],categoria[50];
    int res,canal[2],bytes=0,desconto,duracao;
    if(pipe(canal) == -1){
        fprintf(stderr,"Erro ao criar o pipe");
        exit(1);
    }
    res = fork();
    if(res == -1)
        exit(1);
    if(res == 0){ //Processo filho
        close(1);           //redirecionamento
        dup(canal[1]);
        close(canal[0]);
        close(canal[1]);

        if(execl(nomePromotor,nomePromotor,NULL) == -1){
            fprintf(stderr,"ERRO");
        }
        exit(2);
    }else{//Processo pai
        bytes = read(canal[0],frase, sizeof (frase));
        if(bytes == -1){
            fprintf(stderr,"Erro na leitura");
            exit(1);
        }
        //printf("Li: %s\n",frase);
        sscanf(frase,"%s %d %d",categoria,&desconto,&duracao);
        strcpy(prom.categoria,categoria);
        prom.desconto = desconto;
        prom.duracao = duracao;
    }
    close(canal[1]);
    close(canal[0]);

    return prom;
}

int main() {
    Promocao prom;
    int numArgumento;
    char str[128];

    prom = lancaPromotor("black_friday");
    printf("Categoria: %s\n",prom.categoria);
    printf("Desconto: %d\n",prom.desconto);
    printf("Duracao: %d\n",prom.duracao);

    printf("Bem vindo Administrador\n");

    while (1) {
        printf("Comando:");
        fflush(stdout);
        fgets(str, 128, stdin);
        str[strcspn(str, "\n")] = 0;

        printf("String: %s\n", str);

        numArgumento = numArgumentos(str);
        printf("Num de Argumetos: %d\n", numArgumento);
        char *token = strtok(str, " ");

        if (strcmp(token, "users") == 0) {///FEITO
            if (numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        } else if (strcmp(token, "list") == 0) {    ///FEITO
            if (numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        } else if (strcmp(token, "kick") == 0) {    ///TODO:COMPARAR COM TODOS OS CLIENTES LOGADOS
            if (numArgumento != 2)
                printf("Nao Valido\n");
            else {
                token = strtok(NULL, " ");
                printf("User: %s\n", token);
                printf("Valido\n");
            }
        } else if (
                strcmp(token,"prom") == 0) {    ///Feito
            if (numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        } else if (
                strcmp(token,"reprom") == 0) {  ///Feito
            if (numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        } else if (
                strcmp(token,"cancel") == 0) { ///
            if (numArgumento != 2)
                printf("Nao Valido\n");
            else {
                token = strtok(NULL, " ");
                printf("Nome do executavel: %s\n", token);
                printf("Valido\n");
            }
        } else if (
                strcmp(token,"close") == 0) {
            if (numArgumento != 1)
                printf("Nao Valido\n");
            else {
                printf("Valido\n");
                break;
            }
        } else {
            printf("Comando nao Valido\n");
        }

    }

    printf("A avisar os clientes que ira fechar\n");
    printf("Fechou\n");
    return 0;

}