/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: carlao
 * N USP 8901184
 * Created on 29 de Março de 2019, 13:03
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define TAMANHO_PAGINA 16000
#define TAMANHO_REGISTRO 80
#define NOME_ARQUIVO_WB_SAVE "arquivoTrab1si.bin"
#define REMOVIDO '*'
#define NAO_REMOVIDO '-'
#define TAG_CAMPO_CIDADE '4'
#define TAG_CAMPO_ESCOLA '5'
#define ARQUIVO_ABERTO_ESCRITA '0'
#define ARQUIVO_FECHADO_ESCRITA '1'

//nome dos campos para comparacao
#define NRO_INSCRICAO "nroInscricao"
#define NOTA "nota"
#define DATA "data"
#define CIDADE "cidade"
#define NOME_ESCOLA "nomeEscola"

#define NULO "NULO"

/*
//comandos de teste
1 csv.csv
2 arquivoTrab1si.bin
3 arquivoTrab1si.bin nroInscricao 332
3 arquivoTrab1si.bin nota 561.3
3 arquivoTrab1si.bin data 23/01/2017
3 arquivoTrab1si.bin cidade Recife
3 arquivoTrab1si.bin cidade Joao Pessoa    
3 arquivoTrab1si.bin nomeEscola FRANCISCO RIBEIRO CARRI
4 arquivoTrab1si.bin 4999
 */

/**
 * Recebe o path de um arquivo binario e mostra em tela
 * @param nomeArquivoBinario
 */
void escreverNaTela(char *nomeArquivoBinario) {

    /* Escolha essa função se você já fechou o ponteiro de arquivo 'FILE *'.
     *  Ela vai abrir de novo para leitura e depois fechar. */

    size_t fl;
    FILE *fs;
    char *mb;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
        fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela2): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar? Se você não fechou ele, pode usar a outra função, binarioNaTela1, ou pode fechar ele antes de chamar essa função!\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (char *) malloc(fl);
    fread(mb, 1, fl, fs);
    fwrite(mb, 1, fl, stdout);
    free(mb);
    fclose(fs);
}

/**
 * Recebe os dados de uma linha e imprime em tela
 * @param nroInscricao
 * @param nota
 * @param data
 * @param cidade
 * @param nomeEscola
 */
void imprimirLinhaEmTela(int nroInscricao, double nota, char * data, char * cidade, char * nomeEscola) {
    //imprime o número de inscrição
    printf("%d", nroInscricao);

    //se nota>=0 imprime
    if (nota >= 0) {
        printf(" %.1lf", nota);
    }

    //se tiver caracteres na data imprime
    if (strlen(data) > 0) {
        printf(" %s", data);
    }

    //verifica se tem cidade
    if (cidade != NULL && strlen(cidade) > 0) {
        printf(" %d %s", (int) strlen(cidade), cidade);
    }

    //verifica se tem escola
    if (nomeEscola != NULL && strlen(nomeEscola) > 0) {
        printf(" %d %s", (int) strlen(nomeEscola), nomeEscola);
    }

    printf("\n");
}

/**
 * Recebe o arquivo e RRN para ler o valor das variaveis
 * @param fileWb
 * @param RRN
 * @param removido
 * @param nroInscricao
 * @param nota
 * @param data
 * @param cidade
 * @param nomeEscola
 * @return nroInscricao caso seja lido ou 0 em caso de erro
 */
int lerLinha(FILE * fileWb, int RRN, char * removido, int * nroInscricao, double * nota, char * data, char * cidade, char * nomeEscola) {

    int encadeamento;

    int auxTamanho;
    char auxTagCampo;

    //posicao do proximo registro
    int pular = TAMANHO_PAGINA + RRN * TAMANHO_REGISTRO;

    //posicao atual do ponteiro no arquivo
    int posAtual = ftell(fileWb);

    //ajusta o tamanho do salto tirando o valor atual do ponteiro do registro a ser obtido
    pular -= posAtual;

    //tenta pular pra posição
    int seek = fseek(fileWb, pular, SEEK_CUR);

    //fssek retorna 0 caso seja lido sem erros
    if (!seek) {
        //pega o atributo para verificar se o registro esta excluido logicamente
        int read = fread(removido, sizeof (char), 1, fileWb);

        //verifica se o registro não esta excluido e imprime em tela
        if (read) {
            //le o valor de encadeamento
            fread(&encadeamento, sizeof (int), 1, fileWb);

            //le somente se o registro nao foi removido
            if (*removido == NAO_REMOVIDO) {

                //le o numero de inscricao
                fread(nroInscricao, sizeof (int), 1, fileWb);

                //le a nota
                fread(nota, sizeof (double), 1, fileWb);

                //le a data
                fread(data, 10, 1, fileWb);

                //tenta ler o tamanho da string
                read = fread(&auxTamanho, sizeof (int), 1, fileWb);

                if (read) {
                    //le a tag
                    fread(&auxTagCampo, sizeof (char), 1, fileWb);

                    auxTamanho--;

                    //verifica se é uma tagValida
                    if (auxTagCampo == TAG_CAMPO_CIDADE) {

                        //le o campo cidade
                        fread(cidade, auxTamanho, 1, fileWb);

                        read = fread(&auxTamanho, sizeof (int), 1, fileWb);

                        auxTamanho--;

                        if (read) {
                            //le a tag
                            fread(&auxTagCampo, sizeof (char), 1, fileWb);

                            //verifica se é uma tag validae
                            if (auxTagCampo == TAG_CAMPO_ESCOLA) {
                                //le o campo escolha
                                fread(nomeEscola, auxTamanho, 1, fileWb);
                            }

                        }

                    } else if (auxTagCampo == TAG_CAMPO_ESCOLA) {
                        //le o campo escolha
                        fread(nomeEscola, auxTamanho, 1, fileWb);
                    }
                }
            }
        }
    }

    //retorn o numero de inscricao ou 0
    return * nroInscricao;
}

/**
 * Abre um arquivo e verifica sua integridade
 * @param nomeArquivo
 * @return arquivo aberto ou nulo caso ocorra erro
 */
FILE * abrirArquivoBinarioLeitura(char * nomeArquivo) {
    FILE * file = fopen(nomeArquivo, "rb");

    if (file) {
        char status = ARQUIVO_ABERTO_ESCRITA;

        //le o primeiro char para verificar a integridade
        fread(&status, sizeof (char), 1, file);

        //se o arquivo estiver aberto, fecha e retorna nulo
        if (status == ARQUIVO_ABERTO_ESCRITA) {
            fclose(file);
            file = NULL;
        }

    }

    return file;
}

/**
 * Abre o arqvuio para escrita
 * Seta o status para ARQUIVO_ABERTO_ESCRITA
 * @param nomeArquivo
 * @return 
 */
FILE * abrirArquivoBinarioEscritra(char * nomeArquivo) {
    FILE * file = fopen(nomeArquivo, "r+b");

    if (file) {
        char status = ARQUIVO_ABERTO_ESCRITA;

        //le o primeiro char para verificar a integridade
        fread(&status, sizeof (char), 1, file);

        //se o arquivo estiver aberto, fecha e retorna nulo
        if (status == ARQUIVO_ABERTO_ESCRITA) {
            fclose(file);
            file = NULL;
        } else {
            //volta o curtos pro inicio do arquivo
            fseek(file, -1, SEEK_CUR);
            //seta o flag de arquivo aberto pra escrita
            char status = ARQUIVO_ABERTO_ESCRITA;
            fwrite(&status, sizeof (char), 1, file);
        }

    }

    return file;
}

/**
 * Seta o status para ARQUIVO_FECHADO_ESCRITA
 * Fechar o arquivo
 * @param file
 */
void fecharArquivoBinarioEscrita(FILE * file) {
    if (file != NULL) {
        //move o ponteiro pro inicio do arquivo

        //atualiza o status para ARQUIVO_FECHADO_ESCRITA 1
        //posiciona o cursor pro inicio do arquivo
        fseek(file, 0, SEEK_SET);
        char status = ARQUIVO_FECHADO_ESCRITA;
        fwrite(&status, sizeof (char), 1, file);

        //fecha o arquivo binario
        fclose(file);
    }
}

/**
 * Le uma nova linha de comando
 * @return nova linha de comando
 */
char * lerComando() {
    //realoca memoria pra variavel comando
    char * comando = calloc(100, sizeof (char));
    fgets(comando, 100, stdin);

    //pega o tamnho do comando lido
    size_t ln = strlen(comando) - 1;

    //elimina o \n caso houver para ajudar e evitar erros no processamento
    if (comando[ln] == '\n') {
        comando[ln] = '\0';
    }

    return comando;
}

/**
 * Le um arquivo csv e mostra em tela
 * Entrada Modelo: 1 arquivo.csv
 * @param comando
 */
void opc1(char * comando) {

    char * nomeArquivo = strsep(&comando, "\0");

    //tenta abrir o arquivo csv em modo leitura
    FILE * file = fopen(nomeArquivo, "r");

    //se conseguiu abrir o arquivo
    if (file) {

        //cria o arquivo para salvar os dados em modo binario
        FILE * wbFile = fopen(NOME_ARQUIVO_WB_SAVE, "wb");

        char buff[200];

        //vez = 1 lendo dados do cacecalho
        int vez = 1;

        while (!feof(file)) {
            // Lê uma linha (inclusive com o '\n')
            char * result = fgets(buff, 200, file); // o 'fgets' lê até 199 caracteres ou até o '\n'

            //verifica se e uma linha valida
            if (result != NULL) {
                //printf("%s\n", result);

                size_t ln = strlen(result) - 1;
                //remove o enter e o carriage return
                if (result[ln] == '\n') {
                    result[ln] = '\0';

                    if (result[ln - 1] == '\r') {
                        result[ln - 1] = '\0';
                    }
                }

                if (vez == 1) {

                    //escreve os dados do cabecalho

                    //grava o status como arquivo aberto pra escrita
                    char status = ARQUIVO_ABERTO_ESCRITA;
                    fwrite(&status, sizeof (status), 1, wbFile);

                    //grava o topo da pilha
                    int topoPilha = -1;
                    fwrite(&topoPilha, sizeof (topoPilha), 1, wbFile);

                    //grava os dados do campo 1
                    char tagCampo1 = '1';
                    fwrite(&tagCampo1, sizeof (tagCampo1), 1, wbFile);

                    char desCampo1[55] = "numero de inscricao do participante do ENEM\0@@@@@@@@@@@";
                    fwrite(&desCampo1, sizeof (desCampo1), 1, wbFile);

                    //grava os dados do campo 2
                    char tagCampo2 = '2';
                    fwrite(&tagCampo2, sizeof (tagCampo2), 1, wbFile);

                    char desCampo2[55] = "nota do participante do ENEM na prova de matematica\0@@@";
                    fwrite(&desCampo2, sizeof (desCampo2), 1, wbFile);

                    //grava os dados do campo 3
                    char tagCampo3 = '3';
                    fwrite(&tagCampo3, sizeof (tagCampo3), 1, wbFile);

                    char desCampo3[55] = "data\0@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
                    fwrite(&desCampo3, sizeof (desCampo3), 1, wbFile);

                    //grava os dados do campo 4
                    char tagCampo4 = '4';
                    fwrite(&tagCampo4, sizeof (tagCampo4), 1, wbFile);

                    char desCampo4[55] = "cidade na qual o participante do ENEM mora\0@@@@@@@@@@@@";
                    fwrite(&desCampo4, sizeof (desCampo4), 1, wbFile);


                    //grava os dados do campo 4
                    char tagCampo5 = '5';
                    fwrite(&tagCampo5, sizeof (tagCampo5), 1, wbFile);

                    char desCampo5[55] = "nome da escola de ensino medio\0@@@@@@@@@@@@@@@@@@@@@@@@";
                    fwrite(&desCampo5, sizeof (desCampo5), 1, wbFile);

                    //for para completar com @ e deixar o cabeçalho em uma pagina só
                    int i;

                    int total = 13000;
                    char * lixo = calloc(total, sizeof (char));

                    // por algum motivo o fwrite so esta escrevendo ate 13000 caracteres por vez
                    for (i = 0; i < total; i++) {
                        lixo[i] = '@';
                    }

                    fwrite(&lixo, total, 1, wbFile);

                    //termina de escrever o lixo
                    total = TAMANHO_PAGINA - total - 285;
                    fwrite(&lixo, total, 1, wbFile);

                    //libera memoria do lixo
                    free(lixo);
                    lixo = NULL;

                    //break;

                } else {
                    //grava o valor de removido
                    char removido = NAO_REMOVIDO;
                    fwrite(&removido, sizeof (removido), 1, wbFile);

                    //grava o encadeamento
                    int encadeamento = -1;
                    fwrite(&encadeamento, sizeof (encadeamento), 1, wbFile);

                    //começa a ler os dados
                    char * tmp = strsep(&result, ",");

                    //convert a string pra inteiro
                    int nroInscricao = atoi(tmp);

                    //grava no arquivo binario
                    fwrite(&nroInscricao, sizeof (int), 1, wbFile);

                    //pega  a nota
                    tmp = strsep(&result, ",");

                    double nota = -1;

                    if (strlen(tmp)) {
                        //caso haja texto, converte o mesmo pra double
                        nota = strtod(tmp, NULL);
                    }

                    //grava no arquivo binario
                    fwrite(&nota, sizeof (nota), 1, wbFile);

                    //seta data nula padrao
                    tmp = strsep(&result, ",");
                    char data[10] = "\0@@@@@@@@@";

                    //caso conseguiui ler campo data do arquivo copia para variavel
                    if (strlen(tmp)) {
                        strncpy(data, tmp, sizeof (data));
                    }

                    //grava a data no arquivo binario
                    fwrite(&data, sizeof (data), 1, wbFile);

                    //pega o tamanho dos campos fixo
                    size_t totalBytes = 27;

                    //tenta ler o campo cidade
                    char * cidade = strsep(&result, ",");

                    //add 1 para o \0
                    int tamanhoCidade = strlen(cidade) + 1;
                    if (tamanhoCidade > 1) {
                        //concatena com \0 no final d string
                        cidade = strncat(cidade, "\0", tamanhoCidade);

                        //add o char tagCampoCidade no tamanho do campo
                        tamanhoCidade++;

                        //salva o tamanho do campo
                        fwrite(&tamanhoCidade, sizeof (tamanhoCidade), 1, wbFile);

                        //remove o char tagCampoCidade para salvar a string cidade
                        tamanhoCidade--;

                        //escreve a tag do campo
                        char tagCampoCidade = TAG_CAMPO_CIDADE;
                        fwrite(&tagCampoCidade, sizeof (char), 1, wbFile);

                        //escreve a string cidade no arquivo
                        fwrite(cidade, tamanhoCidade, 1, wbFile);

                        // 5 = int (4) + tagCampoCIdade(1)
                        totalBytes += 5 + tamanhoCidade;
                    }

                    //tenta ler o nomeEscola
                    char * nomeEscola = strsep(&result, ",");

                    //soma 1 do \0
                    int tamanhoEscola = strlen(nomeEscola) + 1;

                    if (tamanhoEscola > 1) {

                        //concatena com \0
                        nomeEscola = strncat(nomeEscola, "\0", tamanhoEscola);

                        //soma 1 do char tagCampoEscola
                        tamanhoEscola++;

                        //salva o tamanho do campo
                        fwrite(&tamanhoEscola, sizeof (tamanhoEscola), 1, wbFile);

                        //remove 1 do tagCampoEscola
                        tamanhoEscola--;

                        //salva a tag do campo
                        char tagCampoEscola = TAG_CAMPO_ESCOLA;
                        fwrite(&tagCampoEscola, sizeof (char), 1, wbFile);

                        fwrite(nomeEscola, tamanhoEscola, 1, wbFile);

                        //5 = int tamanho + char tag
                        totalBytes += 5 + tamanhoEscola;
                    }


                    //escreve o lixo para completar o registro
                    int i, total = TAMANHO_REGISTRO - totalBytes;
                    //cria a variavel com tamanho que falta
                    char * lixo = calloc(total, 1);
                    //for para setar @ nos bytes faltantes
                    for (i = 0; i < total; i++) {
                        lixo[i] = '@';
                    }
                    //escreve em arquivo o lixo
                    fwrite(&lixo, total, 1, wbFile);
                    free(lixo);
                    lixo = NULL;
                }
            }


            result = NULL;
            vez++;



        }

        //atualiza o status para ARQUIVO_FECHADO_ESCRITA 1
        //posiciona o cursor pro inicio do arquivo
        fseek(wbFile, 0, SEEK_SET);
        char status = ARQUIVO_FECHADO_ESCRITA;
        fwrite(&status, sizeof (char), 1, wbFile);

        //fecha o arquivo binario
        fclose(wbFile);

        //fecha o arquivo csv
        fclose(file);

        //mostra o nome do arquivo salvo
        printf(NOME_ARQUIVO_WB_SAVE);

    } else {
        printf("Falha no carregamento do arquivo.");
    }
}

/**
 * Le um arquivo binario e mostra em tela
 * Entrada Modelo: 2 arquivoTrablsi.bin
 * @param comando
 */
void opc2(char * comando) {

    char * nomeArquivo = strsep(&comando, "\0");

    //numero do registro que esta sendo lido ou RRN
    int vez = 0;

    //tenta abrir o arquivo pra modo leitura binario
    FILE *fileWb = abrirArquivoBinarioLeitura(nomeArquivo);

    if (fileWb) {

        while (!feof(fileWb)) {
            char removido;
            //int encadeamento;
            int nroInscricao = 0;
            double nota = -1;
            char data[11] = "\0";
            //data[10] = '\0';

            char cidade[100] = "\0"; // = NULL;
            char nomeEscola[100] = "\0"; // = NULL;

            //se conseguiu ler a linha
            if (lerLinha(fileWb, vez, &removido, &nroInscricao, &nota, data, cidade, nomeEscola)) {
                //se o registro não esta removido logicamente
                if (removido == NAO_REMOVIDO) {
                    imprimirLinhaEmTela(nroInscricao, nota, data, cidade, nomeEscola);
                }
            }

            vez++;
        }

        //fecha o arquivo
        fclose(fileWb);


        //Calcula qtas paginas foram acessadas
        int totalBytes = TAMANHO_REGISTRO * (vez);

        int totalPaginasAcessadas = totalBytes / TAMANHO_PAGINA;

        int diff = totalBytes % TAMANHO_PAGINA;

        totalPaginasAcessadas += (diff > 0) ? 1 : 0;

        printf("Número de páginas de disco acessadas: %d", totalPaginasAcessadas);
    } else {
        printf("Falha no processamento do arquivo.");
    }
}

/**
 * Pesquisa dentro de um arquivo binario conforme parametros e mostra em tela
 * Entrada Modelo: 3 arquivoTrab1si.bin nroInscricao 332
 * Entrada Modelo: 3 arquivoTrab1si.bin nota 561.3
 * Entrada Modelo: 3 arquivoTrab1si.bin data 23/01/2017
 * Entrada Modelo: 3 arquivoTrab1si.bin cidade Recife
 * Entrada Modelo: 3 arquivoTrab1si.bin cidade Joao Pessoa    
 * Entrada Modelo: 3 arquivoTrab1si.bin nomeEscola FRANCISCO RIBEIRO CARRI
 * @param comando
 */
void opc3(char * comando) {

    char * nomeArquivo = strsep(&comando, " ");

    char * parametroNome = strsep(&comando, " ");

    char * parametroValor = strsep(&comando, "\0");

    int erro = 0;
    int imprimiu = 0;

    //verifica se é um parametro válido
    if (strcmp(parametroNome, NRO_INSCRICAO) == 0 || strcmp(parametroNome, NOTA) == 0 || strcmp(parametroNome, DATA) == 0 || strcmp(parametroNome, CIDADE) == 0 || strcmp(parametroNome, NOME_ESCOLA) == 0) {
        //printf("ok");

        int vez = 0;

        FILE *fileWb = abrirArquivoBinarioLeitura(nomeArquivo);

        if (fileWb) {

            while (!feof(fileWb)) {
                char removido;
                //int encadeamento;
                int nroInscricao = 0;
                double nota = -1;
                char data[11] = "\0";
                //data[10] = '\0';

                char cidade[100] = "\0"; // = NULL;
                char nomeEscola[100] = "\0"; // = NULL;


                if (lerLinha(fileWb, vez, &removido, &nroInscricao, &nota, data, cidade, nomeEscola)) {

                    if (removido == NAO_REMOVIDO) {


                        //verificar se o valor corresponde ao parametro
                        if (strcmp(parametroNome, NRO_INSCRICAO) == 0) {
                            int nroAux = atoi(parametroValor);

                            if (nroInscricao == nroAux) {
                                imprimirLinhaEmTela(nroInscricao, nota, data, cidade, nomeEscola);
                                imprimiu = 1;
                                vez += 2;
                                break;
                            }
                        }

                        if (strcmp(parametroNome, NOTA) == 0) {
                            double notaAux = strtod(parametroValor, NULL);

                            if (notaAux >= 0 && notaAux == nota) {
                                imprimirLinhaEmTela(nroInscricao, nota, data, cidade, nomeEscola);
                                imprimiu = 1;
                            }
                        }

                        if (strcmp(parametroNome, DATA) == 0) {
                            if (strcmp(parametroValor, data) == 0) {
                                imprimirLinhaEmTela(nroInscricao, nota, data, cidade, nomeEscola);
                                imprimiu = 1;
                            }
                        }

                        if (strcmp(parametroNome, CIDADE) == 0) {
                            if (strcmp(parametroValor, cidade) == 0) {
                                imprimirLinhaEmTela(nroInscricao, nota, data, cidade, nomeEscola);
                                imprimiu = 1;
                            }
                        }

                        if (strcmp(parametroNome, NOME_ESCOLA) == 0) {
                            if (strcmp(parametroValor, nomeEscola) == 0) {
                                imprimirLinhaEmTela(nroInscricao, nota, data, cidade, nomeEscola);
                                imprimiu = 1;
                            }
                        }

                    }
                }

                vez++;
            }


            if (imprimiu) {
                //Calcula qtas paginas foram acessadas
                //soma uma pagina para contar o cabecallho
                int totalBytes = TAMANHO_REGISTRO * (vez - 1) + TAMANHO_PAGINA;

                int totalPaginasAcessadas = totalBytes / TAMANHO_PAGINA;

                int diff = totalBytes % TAMANHO_PAGINA;

                totalPaginasAcessadas += (diff > 0) ? 1 : 0;

                printf("Número de páginas de disco acessadas: %d", totalPaginasAcessadas);
            } else {
                printf("Registro inexistente.");
            }


        } else {
            erro == 1;
        }

    } else {
        erro = 1;
    }


    if (erro) {
        printf("Falha no processamento do arquivo");
    }
}

/**
 * Pesquisa um registro pelo RRN e mostra em tela
 * Entrada Modelo: 4 arquivoTrab1si.bin 4999
 * @param comando
 */
void opc4(char * comando) {
    char * nomeArquivo = strsep(&comando, " ");

    int RRN = -1;
    RRN = atoi(strsep(&comando, "\0"));

    int erro = 0;


    if (RRN >= 0) {

        FILE *fileWb = abrirArquivoBinarioLeitura(nomeArquivo);

        if (fileWb) {

            char removido;
            //int encadeamento;
            int nroInscricao = 0;
            double nota = -1;
            char data[11] = "\0";
            //data[10] = '\0';

            char cidade[100] = "\0"; // = NULL;
            char nomeEscola[100] = "\0"; // = NULL;

            if (lerLinha(fileWb, RRN, &removido, &nroInscricao, &nota, data, cidade, nomeEscola)) {

                if (removido == NAO_REMOVIDO) {
                    imprimirLinhaEmTela(nroInscricao, nota, data, cidade, nomeEscola);
                    //2 cabcalho + seek direto pro registro
                    printf("Número de páginas de disco acessadas: 2");
                } else {
                    printf("Registro inexistente.");
                }
            } else {
                printf("Registro inexistente.");
            }



        } else {
            erro = 1;
        }



    } else {
        erro = 1;
    }

    if (erro) {
        printf("Falha no processamento do arquivo");
    }

}

/**
 * Esclui registros conforme paramestro informados
 * Entrada Modelo: 

5 arquivoTrab1si.bin 2
nroInscricao 13893
cidade "Coimbra"

 * Entrada Modelo: 

5 arquivoTrab1si.bin 1
nomeEscola "DONIZETTI TAVARES DE LI"

 * @param comando
 */
void opc5(char * comando) {

    char * nomeArquivo = strsep(&comando, " ");

    int erro = 0;

    int numeroIteracoes = 0;
    numeroIteracoes = atoi(strsep(&comando, "\0"));

    FILE * fileWb = abrirArquivoBinarioEscritra(nomeArquivo);

    if (fileWb) {

        //pega a posicao do topo para poder ser atualizada posteriormente
        int posTopoPilha = ftell(fileWb);

        //pega o topo da pilha de removidos
        int topoPilha = 0;
        fread(&topoPilha, sizeof (int), 1, fileWb);

        int vez;
        //for para ler os comandos a serem executados
        for (vez = 0; vez < numeroIteracoes; vez++) {

            //le uma nova linha
            comando = lerComando();

            //pega o campo a ser comparado
            char * parametroNome = strsep(&comando, " ");
            //pega o valor do campo a ser comparado
            char * parametroValor = strsep(&comando, "\"");

            //remove " do valor do campo
            if (strcasecmp(parametroNome, NOME_ESCOLA) == 0 || strcasecmp(parametroNome, CIDADE) == 0 || strcasecmp(parametroNome, DATA) == 0) {
                parametroValor = strsep(&comando, "\"");
            }

            //verifica se é um parametro válido
            if (strcmp(parametroNome, NRO_INSCRICAO) == 0 || strcmp(parametroNome, NOTA) == 0 || strcmp(parametroNome, DATA) == 0 || strcmp(parametroNome, CIDADE) == 0 || strcmp(parametroNome, NOME_ESCOLA) == 0) {
                //printf("ok");

                int RRN = 0;


                while (!feof(fileWb)) {
                    char removido;
                    //int encadeamento;
                    int nroInscricao = 0;
                    double nota = -1;
                    char data[11] = "\0";
                    //data[10] = '\0';

                    char cidade[100] = "\0"; // = NULL;
                    char nomeEscola[100] = "\0"; // = NULL;

                    int excluir = 0;
                    int parar = 0;


                    if (lerLinha(fileWb, RRN, &removido, &nroInscricao, &nota, data, cidade, nomeEscola)) {

                        if (removido == NAO_REMOVIDO) {


                            //verificar se o valor corresponde ao parametro
                            if (strcmp(parametroNome, NRO_INSCRICAO) == 0) {
                                int nroAux = atoi(parametroValor);

                                if (nroInscricao == nroAux) {
                                    excluir = 1;
                                    parar = 1;
                                }
                            }

                            if (strcmp(parametroNome, NOTA) == 0) {
                                double notaAux = strtod(parametroValor, NULL);

                                if (notaAux >= 0 && notaAux == nota) {
                                    excluir = 1;
                                }
                            }

                            if (strcmp(parametroNome, DATA) == 0) {
                                if (strcmp(parametroValor, data) == 0) {
                                    excluir = 1;
                                }
                            }

                            if (strcmp(parametroNome, CIDADE) == 0) {
                                if (strcmp(parametroValor, cidade) == 0) {
                                    excluir = 1;
                                }
                            }

                            if (strcmp(parametroNome, NOME_ESCOLA) == 0) {
                                if (strcmp(parametroValor, nomeEscola) == 0) {
                                    excluir = 1;
                                }
                            }

                        }
                    }


                    //faz a esclusao do resgistro
                    if (excluir) {
                        //remove o registro logicamente

                        //posicao do proximo registro
                        int pular = TAMANHO_PAGINA + RRN * TAMANHO_REGISTRO;

                        //posicao atual do ponteiro no arquivo
                        int posAtual = ftell(fileWb);

                        //ajusta o tamanho do salto tirando o valor atual do ponteiro do registro a ser obtido
                        pular -= posAtual;

                        //tenta pular pra posição
                        int seek = fseek(fileWb, pular, SEEK_CUR);

                        //escreve o registro como nulo
                        char status = REMOVIDO;
                        fwrite(&status, sizeof (char), 1, fileWb);

                        //escreve o encadeamento
                        fwrite(&topoPilha, sizeof (int), 1, fileWb);

                        //escreve @ nos campos restantes
                        int i;
                        //status 1 + encadeamento 4 = 5
                        for (i = 5; i < TAMANHO_REGISTRO; i++) {
                            char arr = '@';
                            fwrite(&arr, sizeof (char), 1, fileWb);
                        }

                        //atualiza o topo da pilha com o RRN atual
                        topoPilha = RRN;

                        //move o ponteiro pra posição de cabeçalho topo da lista
                        fseek(fileWb, posTopoPilha, SEEK_SET);
                        fwrite(&topoPilha, sizeof (int), 1, fileWb);

                        //verifica se deve parar
                        if (parar) {
                            break;
                        }
                    }



                    RRN++;
                }


            } else {
                erro = 1;
            }



        }



        fecharArquivoBinarioEscrita(fileWb);



    } else {
        erro = 1;
    }

    if (erro) {
        printf("Falha no processamento do arquivo");
    } else {
        escreverNaTela(nomeArquivo);
    }
}

/**
 * Inser um novo registro conforme valores informados
 * Entrada Modelo:
 
6 arquivoTrab1si.bin 5
78 408.02 "01/08/2016" "CAMPINA GRANDE" NULO
79 109.98 NULO NULO "ESCOLA DE ESTUDO PRIMARIO"
80 NULO NULO "Paradise City" NULO
81 NULO NULO NULO NULO
82 NULO "10/10/2010" "CID CID" "ESC ESC"

6 arquivoTrab1si.bin 4
11 50.10 "10/10/2010" "C D" "E N"
12 50.10 "10/10/2010" "C D" "E N" 
13 50.10 "10/10/2010" "C D" "E N" 
14 50.10 "10/10/2010" "C D" "E N" 

 * @param comando
 */
void opc6(char * comando) {

    char * nomeArquivo = strsep(&comando, " ");

    int erro = 0;

    int numeroIteracoes = 0;
    numeroIteracoes = atoi(strsep(&comando, "\0"));

    FILE * fileWb = abrirArquivoBinarioEscritra(nomeArquivo);

    if (fileWb) {

        //pega  a posicao do topo
        int posTopoPilha = ftell(fileWb);
        //pega o topo da pilha de removidos
        int topoPilha = -10;
        fread(&topoPilha, sizeof (int), 1, fileWb);


        int vez;
        //for para ler os comandos a serem executados
        for (vez = 0; vez < numeroIteracoes; vez++) {


            //posiciona o ponteiro no local correo para a nova insercao

            //insere no final
            if (topoPilha == -1) {
                //posiciona o ponteiro pro final do arquivo
                fseek(fileWb, 0, SEEK_END);
            } else {
                //pega a posicao do registro removido
                int posicao = topoPilha * TAMANHO_REGISTRO + TAMANHO_PAGINA;

                //soma 1 do byte de statys
                fseek(fileWb, posicao + 1, SEEK_SET);

                //pega o topo atual
                fread(&topoPilha, sizeof (int), 1, fileWb);

                //volta 5 bytes que foram andados e deixa o ponteiro na posição correta para inseção
                fseek(fileWb, -5, SEEK_CUR);

                //int pos = ftell(fileWb);
                //int a = 10;
            }

            //começa a escreve os dados no arquivo
            comando = lerComando();

            //grava o valor de removido
            char removido = NAO_REMOVIDO;
            fwrite(&removido, sizeof (char), 1, fileWb);

            //grava o encadeamento
            int encadeamento = -1;
            fwrite(&encadeamento, sizeof (int), 1, fileWb);


            //começa a ler os dados
            char * tmp = strsep(&comando, " ");
            //convert a string pra inteiro
            int nroInscricao = atoi(tmp);
            //grava no arquivo binario
            fwrite(&nroInscricao, sizeof (int), 1, fileWb);


            //pega  a nota
            tmp = strsep(&comando, " ");

            double nota = -1;


            if (strcmp(tmp, NULO) != 0) {
                //caso haja texto, converte o mesmo pra double
                nota = strtod(tmp, NULL);
            }

            //grava no arquivo binario
            fwrite(&nota, sizeof (nota), 1, fileWb);

            //separa o comando por " ou espaco vazio
            tmp = strsep(&comando, "\" ");
            //seta data nula padrao
            char data[10] = "\0@@@@@@@@@";

            //caso conseguiui ler campo data do arquivo copia para variavel
            if (strcmp(tmp, NULO) != 0) {
                tmp = strsep(&comando, "\"");
                strncpy(data, tmp, sizeof (data));
            }

            //grava a data no arquivo binario
            fwrite(&data, sizeof (data), 1, fileWb);

            //pega o tamanho dos campos fixo
            size_t totalBytes = 27;


            //tenta ler o campo cidade
            char * cidade = strsep(&comando, " \"");


            /*if (cidade[strlen(cidade) - 1] == ' ') {
                cidade[strlen(cidade) - 1] = '\0';
            }*/

            if (strcmp(cidade, NULO) != 0) {
                //remove a primeira aspas
                if(cidade[0]=='\0' && data[0]!='\0'){
                    strsep(&comando, "\"");
                }
                
                cidade = strsep(&comando, "\"");
                //removeAspas(cidade);

                //add 1 para o \0
                int tamanhoCidade = strlen(cidade) + 1;

                //concatena com \0 no final d string
                cidade = strncat(cidade, "\0", tamanhoCidade);

                //add o char tagCampoCidade no tamanho do campo
                tamanhoCidade++;

                //salva o tamanho do campo
                fwrite(&tamanhoCidade, sizeof (int), 1, fileWb);

                //remove o char tagCampoCidade para salvar a string cidade
                tamanhoCidade--;

                //escreve a tag do campo
                char tagCampoCidade = TAG_CAMPO_CIDADE;
                fwrite(&tagCampoCidade, sizeof (char), 1, fileWb);

                //escreve a string cidade no arquivo
                fwrite(cidade, tamanhoCidade, 1, fileWb);

                // 5 = int (4) + tagCampoCIdade(1)
                totalBytes += 5 + tamanhoCidade;

                strsep(&comando, " ");
            }



            //tenta ler o nomeEscola
            char * nomeEscola = strsep(&comando, "\"");



            if (strcmp(nomeEscola, NULO) != 0) {
                //pega stering restante
                nomeEscola = strsep(&comando, "\"");


                //soma 1 do \0
                int tamanhoEscola = strlen(nomeEscola) + 1;

                //concatena com \0
                nomeEscola = strncat(nomeEscola, "\0", tamanhoEscola);

                //soma 1 do char tagCampoEscola
                tamanhoEscola++;

                //salva o tamanho do campo
                fwrite(&tamanhoEscola, sizeof (tamanhoEscola), 1, fileWb);

                //remove 1 do tagCampoEscola
                tamanhoEscola--;

                //salva a tag do campo
                char tagCampoEscola = TAG_CAMPO_ESCOLA;
                fwrite(&tagCampoEscola, sizeof (char), 1, fileWb);

                fwrite(nomeEscola, tamanhoEscola, 1, fileWb);

                //5 = int tamanho + char tag
                totalBytes += 5 + tamanhoEscola;
            }




            //escreve o lixo para completar o registro
            int i, total = TAMANHO_REGISTRO - totalBytes;
            //cria a variavel com tamanho que falta
            char * lixo = calloc(total, 1);
            //for para setar @ nos bytes faltantes
            for (i = 0; i < total; i++) {
                lixo[i] = '@';
            }
            //escreve em arquivo o lixo
            fwrite(&lixo, total, 1, fileWb);
            free(lixo);
            lixo = NULL;





            //salva o topo atual dos escluidos
            fseek(fileWb, posTopoPilha, SEEK_SET);
            fwrite(&topoPilha, sizeof (int), 1, fileWb);
        }



        fecharArquivoBinarioEscrita(fileWb);
    } else {
        erro = 1;
    }









    if (erro) {
        printf("Falha no processamento do arquivo");
    } else {
        escreverNaTela(nomeArquivo);
    }
}

/**
 * Atualiza um campo de um registro conforme seu RRN
 * Entrada Modelo: 
 * 
7 arquivoTrab1si.bin 2
1 nomeEscola "ESCOLA DE ENSINO"
5 data "07/07/2007"

 * @param comando
 */
void opc7(char * comando) {
    int erro = 0;
    
    
    
    if (erro) {
        printf("Falha no processamento do arquivo");
    } else {
        escreverNaTela(nomeArquivo);
    }
}

/*
 * Função Principal
 */
int main() {

    //comando a ser lido
    char * comando = calloc(100, sizeof (char));
    //strcpy(comando,"3 arquivoTrab1si.bin nomeEscola FRANCISCO RIBEIRO CARRI\0");

    //varicavel que guarda a opcao selecioanda
    int opc = 0;

    //leitura do comando a ser executado
    fgets(comando, 100, stdin);

    //pega o tamnho do comando lido
    size_t ln = strlen(comando) - 1;

    //elimina o \n caso houver para ajudar e evitar erros no processamento
    if (comando[ln] == '\n') {
        comando[ln] = '\0';
    }


    //verifica o inteiro digitado no comando
    opc = atoi(strsep(&comando, " "));


    switch (opc) {
        case 1:
        {
            opc1(comando);
            break;
        }
        case 2:
        {
            opc2(comando);
            break;
        }
        case 3:
        {
            opc3(comando);
            break;
        }
        case 4:
        {
            opc4(comando);
            break;
        }
        case 5:
        {
            opc5(comando);
            break;
        }
        case 6:
        {
            opc6(comando);
            break;
        }
        case 7:
        {
            opc7(comando);
            break;
        }
        default:
        {
            printf("Opcao Invalida");
            break;
        }
    }

    //libera memoria do comando
    free(comando);
    comando = NULL;

    return (0);
}
