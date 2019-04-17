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

#define TAMANHO_PAGINA 16000
#define TAMANHO_REGISTRO 80
#define NOME_ARQUIVO_WB_SAVE "arquivoTrab1si.bin"
#define REMOVIDO '*'
#define NAO_REMOVIDO '-'
#define TAG_CAMPO_CIDADE '4'
#define TAG_CAMPO_ESCOLA '5'
#define ARQUIVO_ABERTO_ESCRITA '0'
#define ARQUIVO_FECHADO_ESCRITA '1'

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
3 arquivoTrab1si.bin nomeEscola FRANCISCO RIBEIRO CARRI
4 arquivoTrab1si.bin 4999
 */

/**
 * Recebe o path de um arquivo binario e mostra em tela
 * @param nomeArquivoBinario
 */
void binarioNaTela(char *nomeArquivoBinario) {

	/* Escolha essa função se você já fechou o ponteiro de arquivo 'FILE *'.
	*  Ela vai abrir de novo para leitura e depois fechar. */

	size_t fl;
	FILE *fs;
	char *mb;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
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
int lerLinha(FILE * fileWb, long RRN, char * removido, int * nroInscricao, double * nota, char * data, char * cidade, char * nomeEscola) {

    int encadeamento;

    int auxTamanho;
    char auxTagCampo;

    //posicao do proximo registro
    long pular = TAMANHO_PAGINA + RRN * TAMANHO_REGISTRO;

    //posicao atual do ponteiro no arquivo
    long posAtual = ftell(fileWb);

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

    //retorn o numero de inscricao ou 0
    return * nroInscricao;
}

/**
 * Abre um arquivo e verifica sua integridade
 * @param nomeArquivo
 * @param modo
 * @return arquivo aberto ou nulo caso ocorra erro
 */
FILE * abrirArquivoLeitura(char * nomeArquivo, char * modo) {
    FILE * file = fopen(nomeArquivo, modo);

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


    if (opc == 1) {
        // exemplo de comando
        // 1 arquivo.csv 

        char * nomeArquivo = strsep(&comando, "\0");

        //tenta abrir o arquivo csv em modo leitura
        FILE * file = fopen(nomeArquivo, "r");

        //se conseguiu abrir o arquivo
        if (file) {

            //cria o arquivo para salvar os dados em modo binario
            FILE * wbFile = fopen(NOME_ARQUIVO_WB_SAVE, "wb");

            char buff[200];

            //vez = 1 lendo dados do cacecalho
            long vez = 1;

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
                        fwrite(&nroInscricao, sizeof (nroInscricao), 1, wbFile);

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
    } else if (opc == 2) {
        // exemplo de comando
        // 2 arquivoTrab1si.bin 

        char * nomeArquivo = strsep(&comando, "\0");

        //numero do registro que esta sendo lido ou RRN
        long vez = 0;

        //tenta abrir o arquivo pra modo leitura binario
        FILE *fileWb = abrirArquivoLeitura(nomeArquivo, "rb");

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
            long totalBytes = TAMANHO_REGISTRO * (vez);

            int totalPaginasAcessadas = totalBytes / TAMANHO_PAGINA;

            int diff = totalBytes % TAMANHO_PAGINA;

            totalPaginasAcessadas += (diff > 0) ? 1 : 0;

            printf("Número de páginas de disco acessadas: %d", totalPaginasAcessadas);
        } else {
            printf("Falha no processamento do arquivo.");
        }



        //printf("Acabou");
    } else if (opc == 3) {
        //3 arquivo.bin NomeDoCampo valor
        //3 arquivo.bin nroInscricao 332
        char * nomeArquivo = strsep(&comando, " ");

        char * parametroNome = strsep(&comando, " ");

        char * parametroValor = strsep(&comando, "\0");

        //nome dos campos para comparacao
        char NRO_INSCRICAO[] = "nroInscricao";
        char NOTA[] = "nota";
        char DATA[] = "data";
        char CIDADE[] = "cidade";
        char NOME_ESCOLA[] = "nomeEscola";


        int erro = 0;
        int imprimiu = 0;

        //verifica se é um parametro válido
        if (strcmp(parametroNome, NRO_INSCRICAO) == 0 || strcmp(parametroNome, NOTA) == 0 || strcmp(parametroNome, DATA) == 0 || strcmp(parametroNome, CIDADE) == 0 || strcmp(parametroNome, NOME_ESCOLA) == 0) {
            //printf("ok");

            long vez = 0;

            FILE *fileWb = abrirArquivoLeitura(nomeArquivo, "rb");

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
                    long totalBytes = TAMANHO_REGISTRO * (vez - 1) + TAMANHO_PAGINA;

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


        //printf("ok");
    } else if (opc == 4) {
        char * nomeArquivo = strsep(&comando, " ");

        long RRN = -1;
        RRN = atoi(strsep(&comando, "\0"));

        int erro = 0;
        //printf("%u",RRN);


        if (RRN >= 0) {

            FILE *fileWb = abrirArquivoLeitura(nomeArquivo, "rb");

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


    } else {
        printf("Opcao Invalida");
    }

    //libera memoria do comando
    free(comando);
    comando = NULL;

    return (0);
}
