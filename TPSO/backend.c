#include "utils.h"
#include "backend.h"
#include "users_lib.h"
char CLIENT_FIFO_FINAL[100];
User ListaClientes[20];
int nUsers=0;
void initLista(){
    User a;
    a.pid = -1;
    strcpy(a.nome," ");
    for(int i=0;i<20;i++){
        ListaClientes[i] = a;
    }
}

void printLista(){
    printf("Lista de Utilizadores:\n");
    for(int i=0;i<20;i++){
        if(ListaClientes[i].pid != -1)
            printf("%s\n",ListaClientes[i].nome);
    }
}

void adicionaUserLista(User a){
    int flag = 0;
    //Verificar se o utlizadores ja esta na lista
    for(int i=0;i<20;i++){
        if(strcmp(ListaClientes[i].nome,a.nome) == 0){
            printf("Utilizador %s ja esta logado!\n",a.nome);
            flag = 1;
        }
    }
    if(nUsers >= 20)
        flag = 1;
    if(flag == 0){
        ListaClientes[nUsers++] = a;
        printf("O utilizador [%s] esta ligado!\n",a.nome);
    }
}

void executaPromotores(char* filename){
    FILE *f;Promocao prom;
    char nomePromotor[50];
    f= fopen(filename,"rt");
    if(f == NULL) {
        printf("Erro ao abrir o ficheiro");
        return;
    }else{
        while (fscanf(f,"%s",nomePromotor) != EOF){
            prom = lancaPromotor(nomePromotor);
            printf("Promotor: %s\n",nomePromotor);
            printf("Categoria: %s\n",prom.categoria);
            printf("Desconto: %d\n",prom.desconto);
            printf("Duracao: %d\n\n",prom.duracao);
        }
    }
}

void pedeComando(char *str){
    Promocao prom;
    int numArgumento;

    str[strcspn(str, " \n")] = 0;

    numArgumento = numArgumentos(str);
    char *token = strtok(str, " ");

    if (strcmp(token, "users") == 0) {///FEITO
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else
            printLista();
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
    } else if (strcmp(token,"prom") == 0) {    ///Feito
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else
            printf("Valido\n");
    } else if (strcmp(token,"reprom") == 0) {  ///Feito
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else
            printf("Valido\n");
    } else if (strcmp(token,"cancel") == 0) { ///
        if (numArgumento != 2)
            printf("Nao Valido\n");
        else {
            token = strtok(NULL, " ");
            printf("Nome do executavel: %s\n", token);
            printf("Valido\n");
        }
    } else if (strcmp(token,"close") == 0) {
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else {
            printf("Valido\n");
            exit(0);
        }
    } else if(strcmp(token,"promotores") == 0){
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else {
            executaPromotores(getenv("FPROMOTERS"));
        }
    } else if(strcmp(token,"utilizadores") == 0){
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else {
            loadUsersFile(getenv("FUSERS"));
            printf("Saldo do utilizador TESTE: %d\n", getUserBalance("TESTE"));
            updateUserBalance("TESTE", getUserBalance("TESTE") - 1);
            saveUsersFile(getenv("FUSERS"));
        }
    } else {
        printf("Comando nao Valido\n");
    }
}

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
        sscanf(frase,"%s %d %d",categoria,&desconto,&duracao);
        strcpy(prom.categoria,categoria);
        prom.desconto = desconto;
        prom.duracao = duracao;
    }
    close(canal[1]);
    close(canal[0]);


    union sigval a;
    if(sigqueue(res,SIGUSR1,a) == -1)
        fprintf(stderr,"Erro ao enviar o sinal");
    wait(&res);

    return prom;
}
void handle_sig(int sig,siginfo_t *info,void *old){
    printf("Eliminar da lista o cliente com o PID [%d]\n",info->si_value.sival_int);
}

int main(int argc,char *argv[],char *envp[]) {
    printf("Bem vindo Administrador\n");
    User a;
    fd_set fds;
    struct timeval tv;

    struct sigaction sa;
    sa.sa_sigaction = handle_sig;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);

    initLista();

    //TODO:ACCESS
    if(mkfifo(FIFO_SERVIDOR, 0666) == -1){
        if(errno == EEXIST){
            printf("Servidor em execução ou fifo ja existe");
        }
        printf("Erro abrir fifo");
        exit(1);
    }
    int fdRecebe = open(FIFO_SERVIDOR, O_RDWR);
    if(fdRecebe == -1){
        printf("Erro ao abrir o fifo");
        exit(1);
    }

    do{
        printf("Comando:\n");
        fflush(stdout);
        FD_ZERO(&fds);
        FD_SET(0,&fds);//tomar atenção ao 0(scanf)
        FD_SET(fdRecebe,&fds);//tomar atenção ao fdReceber -> read
        //tv.tv_sec = 5;
        //tv.tv_usec = 0; // esperar no maximo 5 segundos
        int res = select(fdRecebe+1,&fds,NULL,NULL,/*&tv*/NULL);//colocar o ultimo + 1 |
        if(res == 0){


        }else if(res >0 && FD_ISSET(0,&fds)){
            char str[128];
            //printf("Comando1:");
            fflush(stdout);
            fgets(str, 128, stdin);
            pedeComando(str);
        }else if (res >0 && FD_ISSET(fdRecebe,&fds)) {
            ///Le a informacao do cliente
            int size = read(fdRecebe, &a, sizeof(User));
            if (size > 0) {
                //printf("[BACKEND] Recebi do frontend %d:%s %s\n", a.pid, a.nome, a.password);
                sprintf(CLIENT_FIFO_FINAL, FIFO_CLIENTE, a.pid);
                ///Tratar info
                Resposta resposta;
                loadUsersFile(getenv("FUSERS"));
                int aux = isUserValid(a.nome, a.password);
                if (aux == -1) {
                    resposta.num = 0;
                    fprintf(stderr, "Erro(funcao isUserValid)!");
                    exit(1);
                } else if (aux == 0) {
                    resposta.num = 0;
                    printf("Utilizador nao existe ou password errada!\n");
                } else {
                    resposta.num = 1;
                    adicionaUserLista(a);
                    printLista();
                }
                ///Enviar resposta ao cliente
                resposta.pid = getpid();
                int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
                int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
                if (size2 == -1) {
                    fprintf(stderr, "Erro a escrever");
                    exit(1);
                }
                close(fdEnvio);
            } else {
                printf("Erro ao na leitura\n");
                exit(1);
            }
        }
    } while (1);

    unlink(FIFO_SERVIDOR);



    printf("A avisar os clientes que ira fechar\n");
    printf("Fechou\n");
    return 0;

}