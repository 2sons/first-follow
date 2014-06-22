/**
*Copyright {2014} {Silvana Trindade,Maurício André Cinelli}
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define TAM_CARACTERE 80
#define TAM_ALFABETO 256
#define TAM_OPCOES 100
#define TAM_PRODUCOES 210
/* estruturas */

typedef struct __opcao
{
	char producao[TAM_PRODUCOES];
	short int ehEpsilon;
} Opcao;

typedef struct __estado
{
	char identificador[TAM_CARACTERE]; //caractere
	int nOpcoes;
	Opcao *opcoes[TAM_OPCOES];
	short int ehFinal;
	char first[TAM_ALFABETO];
	char follow[TAM_ALFABETO];

} Estado;

typedef struct __gramatica
{
	char alfabeto[TAM_ALFABETO];
	int numEstados;
	Estado *estados[TAM_OPCOES];
} Gramatica;

/**
 * vetor de caracteres permitidos para o estado simbolo
 */

char caracteres[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','Y','W','Z'} ;

/**
 * funções de inicialização e verificação da gramatica
 */
Gramatica *leGramatica();

/**
 * Identifica conjunto first
 * permite gramatica livre de contexto
 * uso de epsilon produção
 */
void conjuntoFirst(Gramatica *gramatica);

/**
 * Identifica conjunto follow
 * permite gramatica livre de contexto
 * uso de epsilon produção
 */
void conjuntoFollow(Gramatica *gramatica);

/**
 * Pertence ao alfabeto
 * Tem como dado de entrada uma produção e o alfabeto
 * Gera como dado de saída o número de caracteres que pertence ao alfabeto
 */
int pertenceAlfabeto(char *producao, char *alfabeto);

/**
 * Faz a leitura do alfabeto da linguagem
 * Tem como dado de entrada a gramatica
 */
void leAlfabeto(Gramatica *gramatica);

/**
 * Busco o estado identificador
 * tem como entrado uma gramatica e um estado não terminal
 * tem como saída o estado identificador
 */
Estado * buscaEstadoPorIdentificador(Gramatica *gramatica, char *estado);

/**
 * Limpa memória de gramatica
 * varre cada posição
 */
void freeGramatica(Gramatica *gramatica);


int main(int argc, char *argv[]) {

	Gramatica *gramatica = leGramatica();

	if (gramatica == NULL)
	{
		exit(1);
	}

	conjuntoFirst(gramatica);

	return 0;
}

/**
 * Le gramática
 * retorna vetor do tipo Estado 
 */
Gramatica *leGramatica(){


	Gramatica *gramatica = (Gramatica *) malloc (sizeof(Gramatica));


	if ( gramatica == NULL )
	{
		printf("Erro na Alocação de memória\n");
		return NULL;
	}

	leAlfabeto(gramatica);

	if (gramatica->alfabeto == NULL)
	{
		printf("Você não digitou o alfabeto.\n");
		exit(1);
	}


	gramatica->numEstados = 0;
	gramatica->estados[gramatica->numEstados] = (Estado *) malloc (sizeof(Estado));

	if (gramatica->estados[gramatica->numEstados] == NULL)
	{
		printf("Erro ao alocar memória\n");
		exit(1);
	}

	gramatica->estados[gramatica->numEstados]->nOpcoes = 0;

	char temporario[TAM_ALFABETO];


	while(fgets(temporario,TAM_ALFABETO,stdin) != NULL) {

		if (strcmp(temporario,"$$") == 0)
		{
			break;
		}

		/**
		 * identifica linha digitada pelo usuário
		 */
		char auxiliar[TAM_ALFABETO*2];
		strcpy(auxiliar,temporario);
		
		char *token = strtok(auxiliar,":|");
		int count = 0;
		
		while(token) {

			char auxToken[200];
			strcpy(auxToken, token);

			
			int tamanho = 0;

			tamanho = strlen(auxToken);

			if (auxToken[tamanho-1] == '\n')
			{
				auxToken[tamanho-1] = '\0';
				
			}

			//verificar identificador da gramatica
			if (count == 0)
			{
				strcpy(gramatica->estados[gramatica->numEstados]->identificador,auxToken);
			}
			else
			{

				Opcao *opcao = (Opcao*) malloc (sizeof(Opcao));

				if (opcao == NULL)
				{
					printf("Erro na alocação de memória\n");
					exit(1);
				}

				if (strcmp(auxToken,"epsilon") == 0)
				{
					gramatica->estados[gramatica->numEstados]->ehFinal = 1;
					opcao->ehEpsilon = 1;
				}
				else
				{
					gramatica->estados[gramatica->numEstados]->ehFinal = 0;
					opcao->ehEpsilon = 0;

					strcpy(opcao->producao,auxToken);
				}
				
				gramatica->estados[gramatica->numEstados]->opcoes[gramatica->estados[gramatica->numEstados]->nOpcoes] = opcao;
				gramatica->estados[gramatica->numEstados]->nOpcoes++;
			}

			token = strtok(NULL,":|");
		
			count++;
		}
		gramatica->numEstados++;
		gramatica->estados[gramatica->numEstados] = (Estado *) malloc(sizeof(Estado));

		if (gramatica->estados[gramatica->numEstados] == NULL)
		{
			printf("Erro na Alocação de memória\n");
			exit(1);
		}

		gramatica->estados[gramatica->numEstados]->nOpcoes = 0;
	}

	return gramatica;
}

/**
 * Faz a leitura do alfabeto
 * Recebe como parametro uma gramatica
 */
void leAlfabeto(Gramatica *gramatica) {

	char temporario[TAM_ALFABETO];

	if(fgets(temporario,TAM_ALFABETO,stdin) != NULL) {

		/**
		 * identifica linha digitada pelo usuário
		 */
		char *token;
		token = strtok(temporario,",");

		int count = 0;

		while(token != NULL) {

			gramatica->alfabeto[count] = token[0];
			count++;
			token = strtok(NULL,",");
		}
		gramatica->alfabeto[count] = '\0';
	}
}

/**
 * Pertence ao Alfabeto
 * Verica se um caractere passado como parametro pertence ao alfabeto
 */
int pertenceAlfabeto(char *producao, char *alfabeto) {

	int i,j,count = 0;

	char auxiliar[TAM_ALFABETO];
	strcpy(auxiliar,producao);

	for (i = 0; i < strlen(auxiliar); i++)
	{
		for (j = 0; j < strlen(alfabeto); j++)
		{
			if (alfabeto[j] == auxiliar[i])
			{
				count++;
			}
		}
	}

	return count;
}

/**
 * [pertenceAlfabeto description]
 * @param  producao [description]
 * @param  alfabeto [description]
 * @return          [description]
 */
int ehTerminal(char producao, char *alfabeto) {

	int j = 0;

	for (j = 0; j < strlen(alfabeto); j++)
	{
		if (alfabeto[j] == producao)
		{
			return 1;
		}
	}

	return 0;
}
/**
 *  busca estado -- se estiver contido na gramatica retorna o estado 
 *  do contrario NULL
 */

Estado * buscaEstadoPorIdentificador(Gramatica *gramatica, char *estado) {

	int i = 0;

	for (i = 0; i < gramatica->numEstados; i++)
	{
		if (strcmp(gramatica->estados[i]->identificador,estado) == 0	)
		{
			return gramatica->estados[i];
		}
	}

	return NULL;
}

void freeGramatica(Gramatica *gramatica)
{
	if (!gramatica) {
		return;
	}

	int i, j;

	for (i = 0; i < gramatica->numEstados; i++)
	{
		for (j = 0; j < gramatica->estados[i]->nOpcoes; j++)
		{
			free(gramatica->estados[i]->opcoes[j]->producao);
		}

		free(gramatica->estados[i]);
	}

	free(gramatica);
}

/**
 * [verificaEpsilon description]
 * @param  estado [description]
 * @return        [description]
 */
int verificaEpsilon(Estado *estado) {

	int i = 0;

	for (i = 0; i < strlen(estado->first); i++)
	{
		if (estado->first[i] == '\27')
		{
			return 1;
		}
	}
	return 0;
}
/**
 * 
 */
void conjuntoFirst(Gramatica *gramatica) {

	int i = 0,j = 0,k = 0,w = 0,aux;

	/**
	 * Identifica simbolos terminais
	 */
	for (i = 0; i < gramatica->numEstados; i++)
	{
		Estado *estado = gramatica->estados[i];

		for (j = 0; j < estado->nOpcoes; j++)
		{
			Opcao *opcao = estado->opcoes[j];

			int terminal = ehTerminal(opcao->producao[0], gramatica->alfabeto);

			aux = 0;

			for (k = 0; k < strlen(estado->first); k++)
			{
				if (estado->first[k] == opcao->producao[0])
				{
					aux = 1;
				}
			}
			if (!aux)
			{
				if (terminal > 0)
				{
					strncat(estado->first,&opcao->producao[0],1);
				}
				else if (opcao->ehEpsilon)
				{
					strncat(estado->first,"\27",1);
				}
			}
		}
	}


	int alterou = 0;
	do
	{
		alterou = 0;

		for (i = 0; i < gramatica->numEstados; i++)
		{
			Estado *estado = gramatica->estados[i];

			for (j = 0; j < estado->nOpcoes; j++)
			{
				Opcao *opcao = estado->opcoes[j];

				int terminal = ehTerminal(opcao->producao[0], gramatica->alfabeto);

				if (terminal <= 0)
				{
					if (opcao->producao[0] == estado->identificador[0] || opcao->ehEpsilon)
					{
						continue;
					}

					int temEpsilon = 0, contador = 0;

					do
					{
						if (ehTerminal(opcao->producao[contador],gramatica->alfabeto))
						{
							aux = 0;

							for (w = 0; w < strlen(estado->first); w++)
							{
								if (estado->first[w] == opcao->producao[contador])
								{
									aux = 1;
									break;
								}
							}
							if (!aux)
							{
								alterou = 1;

								strncat(estado->first,&opcao->producao[contador],1);
							}
							break;
						}

						char var[3];
						var[0] = opcao->producao[contador];
						var[1] = '\0';

						// printf("estado %s producao %s contador %d\n",estado->identificador,opcao->producao,contador );

						Estado *firstEstado = buscaEstadoPorIdentificador(gramatica,var);

						temEpsilon = 0;

						if (firstEstado == NULL)
						{
							printf("Erro ao buscar estado identificador.\n");
							exit(1);
						}
						for (k = 0; k < strlen(firstEstado->first); k++)
						{
							int achou = 0;

							
							for (w = 0; w < strlen(estado->first); w++)
							{
								if (estado->first[w] == firstEstado->first[k])
								{
									achou = 1;
									break;
								}
							}

							if (!achou)
							{
								if (firstEstado->first[k] != '\27')
								{
									alterou = 1;
									printf("%s --- %c\n",estado->first,firstEstado->first[k] );
									strncat(estado->first,&firstEstado->first[k],1);
								}
							}

						}
						if (verificaEpsilon(firstEstado))
						{
							contador++;
							temEpsilon = 1;
						}
						if (contador == strlen(opcao->producao))
						{
							break;
						}
					} while (temEpsilon);
				}
			}
			// printf("%s\n",estado->first );
		}

	} while (alterou);

	for (i = 0; i < gramatica->numEstados; i++)
	{
		printf("%s: %s\n",gramatica->estados[i]->identificador,gramatica->estados[i]->first );
	}
	return;
}
