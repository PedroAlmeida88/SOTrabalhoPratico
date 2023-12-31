#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "utils.h"
char CLIENT_FIFO_FINAL[100];
int servPid=-1;
char nome[32];
int id=1;
pid_t pid;

typedef struct {
    int continua;
    //int *clientes;//tudo o que é partilhado, é um ponteiro
    int fdResposta;
    pthread_mutex_t *ptrinco;
}TDATA;

void funcSair(){
    printf("\nA encerrar o cliente...\n");
    unlink(CLIENT_FIFO_FINAL);
    exit(1);
}

void colocaLeilao(char *nome,Item a){
    Comando comando;
    comando.item=a;
    comando.user.pid = pid;
    comando.comando=1;//enviar um item
    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
        funcSair();
    }
    int size = write(fdEnvia, &comando, sizeof(Comando));
    if (size == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvia);
}

void list(){
    Comando comando;
    comando.user.pid = pid;
    comando.comando=2;//lista todos os itens

    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
    }

    int size = write(fdEnvia, &comando, sizeof(Comando));
    if (size == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvia);

}

void listCat(char *categoria){
    Comando comando;
    comando.user.pid = pid;
    strcpy(comando.item.categoria,categoria);
    comando.comando=3;//lista todos os itens de uma categoria

    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
    }

    int size = write(fdEnvia, &comando, sizeof(Comando));
    if (size == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvia);

}

void listVen(char *vendedor){
    Comando comando;
    comando.user.pid = pid;
    strcpy(comando.item.usernameVendedor,vendedor);
    comando.comando=4;//lista todos os itens de uma categoria

    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
    }

    int size = write(fdEnvia, &comando, sizeof(Comando));
    if (size == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvia);

}

void listVal(int val){
    Comando comando;
    comando.user.pid = pid;
    comando.item.valAtual=val;
    comando.comando=5;//lista todos os itens de uma categoria

    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
        funcSair();
    }

    int size = write(fdEnvia, &comando, sizeof(Comando));
    if (size == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvia);

}
void listTemp(int tempo){
    Comando comando;
    comando.user.pid = pid;
    comando.item.duracao=tempo;
    comando.comando=6;//lista todos os itens de uma categoria

    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
    }

    int size = write(fdEnvia, &comando, sizeof(Comando));
    if (size == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvia);

}

void pedeHora(){
    Comando comando;
    comando.user.pid = pid;
    comando.comando=7;//lista todos os itens de uma categoria

    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
    }

    int size = write(fdEnvia, &comando, sizeof(Comando));
    if (size == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvia);

}

void licita(int id,int valor){
    //valor tem de ser mais alto do que a licitacao atual
    //tem ter o dinheiro~
    Comando comando;
    comando.user.pid = pid;
    comando.item.id = id;
    comando.item.valAtual = valor;
    comando.comando=8;//enviar um item
    strcpy(comando.user.nome,nome);

    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
    }

    int size = write(fdEnvia, &comando, sizeof(Comando));
    if (size == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvia);

}

void pedeSaldo(char *nome){
    Comando comando;
    comando.user.pid = pid;
    strcpy(comando.user.nome,nome);
    comando.comando=9;//lista todos os itens de uma categoria

    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
    }

    int size = write(fdEnvia, &comando, sizeof(Comando));
    if (size == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvia);

}

void addSaldo(int saldo){
    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
    }
    Comando comando;
    comando.user.pid = pid;
    comando.item.valAtual = saldo;
    strcpy(comando.user.nome,nome);
    comando.comando=10;//lista todos os itens de uma categoria
    int size = write(fdEnvia, &comando, sizeof(Comando));
    if (size == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvia);
}

char* pedeComandos(){
    while(1){
        int numArgumento;
        char buffer[50],nomeItem[50],categoria[50],aux[128];
        int precoBase,precoCompreJa,duracao,id,valor;
        char str[128];

        sleep((unsigned int) 0.1);
        printf("Comando:");
        fflush(stdout);
        fgets(str, 128, stdin);
        str[strcspn(str, "\n")] = 0;

        //printf("String: %s\n", str);
        strcpy(aux,str);
        numArgumento = numArgumentos(str);
        //printf("Num de Argumetos: %d\n", numArgumento);
        char *token = strtok(str, " ");

        if(strcmp(token, "sell") == 0){
            //printf("String dentro: %s\n", str);

            if(numArgumento != 6)
                printf("Nao Valido\n");
            else if (sscanf(aux,"%s %s %s %d %d %d",buffer,nomeItem,categoria,&precoBase,&precoCompreJa,&duracao) != 6)
                printf("Nao Valido\n");
            else {
                Item a;
                strcpy(a.nome,nomeItem);a.duracao=duracao;strcpy(a.categoria,categoria);
                strcpy(a.usernameVendedor,nome);strcpy(a.usernameLicitador,"-");
                a.valAtual=precoBase;a.valCompreJa=precoCompreJa;
                colocaLeilao(nome,a);
                printf("Item colocado para venda\n");
            }
        }
        else if(strcmp(token, "list") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                list();
        }
        else if(strcmp(token, "licat") == 0){       //TODO:Ver se a categoria existe
            if(numArgumento != 2)
                printf("Nao Valido\n");
            else {
                token = strtok(NULL, " ");
                //printf("Nome da categoria: %s\n", token);
                listCat(token);
            }
        }
        else if(strcmp(token, "lisel") == 0){       //TODO:Ver se o vendedor existe
            if(numArgumento != 2)
                printf("Nao Valido\n");
            else{
                token = strtok(NULL, " ");
                //printf("Nome do vendedor: %s\n", token);
                listVen(token);
            }

        }
        else if(strcmp(token, "lival") == 0){       //TODO:Ver se o vendedor existe
            if(numArgumento != 2)
                printf("Nao Valido\n");
            else{
                token = strtok(NULL, " ");
                //printf("Nome do vendedor: %s\n", token);
                listVal(atoi(token));
            }

        }
        else if(strcmp(token, "litime") == 0){
            if(numArgumento != 2)
                printf("Nao Valido\n");
            else{
                token = strtok(NULL, " ");
                duracao = atoi(token);
                //printf("Hora em segundos: %d\n", duracao);
                listTemp(duracao);
            }

        }
        else if(strcmp(token, "time") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                pedeHora();
        }
        else if(strcmp(token, "buy") == 0){
            if(numArgumento != 3)
                printf("Nao Valido1\n");
            else if(sscanf(aux,"%s %d %d",buffer,&id,&valor) != 3)
                printf("Nao Valido2\n");
            else {
                licita(id,valor);
            }
        }
        else if(strcmp(token, "cash") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                pedeSaldo(nome);
        }
        else if(strcmp(token, "add") == 0){
            if(numArgumento != 2)
                printf("Nao Valido\n");
            else{
                token = strtok(NULL, " ");
                valor = atoi(token);
                addSaldo(valor);
            }

        }
        else if(strcmp(token, "exit") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else {
                return token;
            }
        }
        else {
            printf("Comando nao Valido\n");
        }
    }
}

void handle_alarm(){
    union sigval info;
    struct sigaction sa;
    info.sival_int = getpid();                          //enviar o PID ao backend
    sigqueue(servPid,SIGUSR1,info);
    alarm(atoi(getenv("HEARTBEAT")));
}

void handle_sig_servidorEncerrou(){
    printf("\nO servidor foi encerrado...\n");
    printf("A terminar...\n");
    unlink(CLIENT_FIFO_FINAL);
    funcSair();
}

void handle_sig_servidorExpulsou(){
    printf("\nVoce foi expulso pelo servidor...\n A sair...\n");
    unlink(CLIENT_FIFO_FINAL);
    funcSair();
}


void *trata_pipe(void *dados){
    TDATA *pd = dados;
    Resposta resposta;
    do{
        ///Le a informacao do cliente
        int size2 = read(pd->fdResposta, &resposta, sizeof(Resposta));
        if(size2 > 0){
            if(resposta.comando == 1){
                if(resposta.num == 1){
                    printf("Bem vindo ao SOBay, %s!\n",nome);
                    if(pthread_mutex_lock(pd->ptrinco) != 0){
                        printf("Erro com o lock\n");
                        funcSair();
                    }
                    servPid = resposta.pid;
                    if(pthread_mutex_unlock(pd->ptrinco) != 0){
                        printf("Erro com o lock\n");
                        funcSair();
                    }
                }else if(resposta.num == 2){
                    printf("Erro com o ficheiro!\n");
                    funcSair();
                }else if(resposta.num == 3){
                    printf("Utilizador desconecido ou password errada!\n");
                    funcSair();
                }else if(resposta.num == 4){
                    printf("Utilizador ja esta ligado!\n");
                    funcSair();
                }
            }else if(resposta.comando == 0){
                printf("O item adicionado tem o id %d\n",resposta.item.id);
            }
            else if(resposta.comando == 2 || resposta.comando == 3 || resposta.comando == 4 || resposta.comando == 5 || resposta.comando == 6){
                Item item;
                item = resposta.item;
                //printf("\nId:%d Nome:%s Cat:%s  VAtual:%d VComprar:%d Duracao:%d NVendedor:%s NLicitador:%s", item.id, item.nome, item.categoria, item.valAtual,
                //          item.valCompreJa, item.duracao, item.usernameVendedor, item.usernameLicitador);
                printf("\nId:%d Nome:%s %s %d %d %d %s %s\n", item.id, item.nome, item.categoria, item.valAtual,
                       item.valCompreJa, item.duracao, item.usernameVendedor, item.usernameLicitador);

            }else if(resposta.comando == 7){
                printf("\nHora atual: %d\n",resposta.num);
            }else if(resposta.comando == 8){
                printf("%s\n",resposta.item.categoria);
            }else if(resposta.comando == 9){
                printf("\nSaldo atual: %d\n",resposta.num);
            }else if(resposta.comando == 10){
                printf("\nSaldo adicionado!\nSaldo Atual: %d\n",resposta.num);
            }else if(resposta.comando == 11){
                printf("\nItem %s [id=%d]adquirido!\n",resposta.item.nome,resposta.item.id);
            }

        }else{
            fprintf(stderr,"Erro na leitura");
            funcSair();
        }


    }while(pd->continua);
    pthread_exit(NULL);
}

int main(int argc, char **argv, char **envp){
    if(argc != 3) {
        printf("[ERRO] Numero invalido de argumentos");
        funcSair();
    }

    Comando a;
    strcpy(a.user.nome,argv[1]);
    strcpy(a.user.password,argv[2]);
    a.user.pid = getpid();
    a.comando= 0;//validacao
    strcpy(nome,argv[1]);
    pid = getpid();

    pthread_mutex_t trinco;
    pthread_t tid;
    TDATA data;

    ///Garantir que o servidor está ativo
    if(access(FIFO_SERVIDOR, F_OK) != 0){
        printf("O servidor nao esta ativo!!!(Pipe nao exite)");
        exit(1);
    }

    sprintf(CLIENT_FIFO_FINAL, FIFO_CLIENTE, getpid());
    //pid = getpid();

    if(mkfifo(CLIENT_FIFO_FINAL,0666) == -1){
        if(errno == EEXIST){
            printf("Fifo já está a correr");
        }
        printf("Erro abrir fifo");
        exit(1);
    }

    ///Envia o nome,password e PID do cliente do frontend para o backend
    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
        funcSair();
    }
    int size = write(fdEnvia, &a, sizeof(Comando));
    if (size == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }

    close(fdEnvia);
    ///Receber a resposta do servidor
    int fdResposta = open(CLIENT_FIFO_FINAL, O_RDWR);
    if(fdResposta == -1){
        printf("Erro ao abrir o fifo");
        funcSair();
    }

    if(pthread_mutex_init(&trinco,NULL) != 0){
        printf("Erro no pthread_mutex_init\n");
        funcSair();
    }
    //T1
    data.continua = 1;
    data.ptrinco = &trinco;
    data.fdResposta = fdResposta;
    if(pthread_create(&tid,NULL, trata_pipe,&data) != 0){
        printf("Erro ao criar a thread\n");
        funcSair();
    }


    //caso o servidor seja encerrado
    struct sigaction sa3;
    sa3.sa_sigaction = handle_sig_servidorEncerrou;
    sa3.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa3, NULL);

    //caso o cliente seja expulso pelo servidor
    struct sigaction sa4;
    sa4.sa_sigaction = handle_sig_servidorExpulsou;
    sa4.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &sa4, NULL);

    //sinal(quando o cliente termina á força (ctrl + c))
    struct sigaction sa2;
    sa2.sa_sigaction = funcSair;
    sa2.sa_flags = SA_SIGINFO;
    sigaction(SIGINT,&sa2,NULL);


    struct sigaction saAlarm;
    saAlarm.sa_handler = handle_alarm;
    saAlarm.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGALRM,&saAlarm,NULL);
    alarm(atoi(getenv("HEARTBEAT")));

    char str[128];
    do{//T0
        strcpy(str,pedeComandos());
    } while (strcmp(str,"exit")!=0);

    data.continua=0;
    if(pthread_cancel(tid) != 0){
        printf("Erro com o pthread_cancel\n");
        funcSair();
    }
    if(pthread_join(tid,NULL) != 0 ){
        printf("Erro com o pthread_join\n");
        funcSair();
    }

    if(pthread_mutex_destroy(&trinco) !=  0){
        printf("Erro com o mutex_destry\n");
        funcSair();
    }

    close(fdResposta);
    unlink(CLIENT_FIFO_FINAL);
    printf("A avisar o servidor que irei sair\n");
    //avisar o servidor que vou encerrar

    union sigval info;
    struct sigaction sa;
    info.sival_int = getpid();  //enviar o PID ao backend
    sigqueue(servPid,SIGUSR2,info);


    printf("Adeus\n");


    exit(0);
}