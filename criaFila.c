#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cthread.h"
#include "../include/cdata.h"

#define ERRO -9
#define OK 0

TCB_t *threadAtual;
PFILA2 fila_prioridade;
PFILA2 fila_block;

ucontext_t despachante;


void criaFila()
{
	fila_prioridade = malloc(sizeof(PFILA2));
	CreateFila2(fila_prioridade);

	fila_block = malloc(sizeof(PFILA2));
	CreateFila2(fila_block);

	TCB_t *main_thread = malloc(sizeof(TCB_t));
	main_thread->tid = 0;
	main_thread->state = PROCST_APTO;
	main_thread->prio = 0;
	getcontext(&(main_thread->context));

	main_thread->state = PROCST_EXEC;
	threadAtual = malloc(sizeof(TCB_t));
	threadAtual = main_thread;

	getcontext(&despachante);

	despachante.uc_link = 0;
	despachante.uc_stack.ss_sp = malloc(4096);
	despachante.uc_stack.ss_size = 4096;
	despachante.uc_stack.ss_flags = 0;

	makecontext(&despachante,(void(*)(void)) despachante,1,NULL);
}

int csem_init(csm_t *sem, int count)
{
	if (fila_prioridade == NULL)
	{
		criaFila();
	}

	PFILA2 novaFila = (PFILA2) malloc(sizeof(PFILA2));
	if(CreateFila2(novaFila) == 0)
	{
		sem->count = count;
		sem->fila = novaFila;
		return OK;
	}
	else
		return ERRO;

	return ERRO;
}

//-----------------------------------------------------------

TCB_t *retornaThread(int tid)
{
	if (FirstFila2(fila_prioridade) == 0)
	{
		while (GetAtIteratorFila2(fila_prioridade) != NULL)
		{
			TCB_t *temp = (TCB_t *) GetAtIteratorFila2(fila_prioridade);
			if (temp->tid == tid)
				return temp;
			NextFila2(fila_prioridade);
		}
	}

	if (FirstFila2(fila_block) == 0)
	{
		while (GetAtIteratorFila2(fila_block) != NULL)
		{
			TCB_t *temp = (TCB_t *) GetAtIteratorFila2(fila_block);
			if (temp->tid == tid)
				return temp;
			NextFila2(fila_block);
		}
	}

	return NULL;
}