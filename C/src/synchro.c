#include "ensitheora.h"
#include "synchro.h"
#include <pthread.h>

/* les variables pour la synchro, ici */
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t tailleC = PTHREAD_COND_INITIALIZER;
static pthread_cond_t textureC = PTHREAD_COND_INITIALIZER;
static bool tailleSent = false;
static bool textureReady = false;

static pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t consommateurC = PTHREAD_COND_INITIALIZER;
static pthread_cond_t producteurC = PTHREAD_COND_INITIALIZER;
int textureNb;

/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer)
{
    pthread_mutex_lock(&m);
    windowsx = buffer->width;
    windowsy = buffer->height;
    tailleSent = true;
    pthread_cond_signal(&tailleC);
    pthread_mutex_unlock(&m);
    attendreFenetreTexture();
}

void attendreFenetreTexture()
{
    pthread_mutex_lock(&m);
    while (!textureReady)
    {
        pthread_cond_wait(&textureC, &m);
    }
    textureReady = false;
    pthread_mutex_unlock(&m);
}

void attendreTailleFenetre()
{
    pthread_mutex_lock(&m);
    while (!tailleSent)
    {
        pthread_cond_wait(&tailleC, &m);
    }
    tailleSent = false;
    pthread_mutex_unlock(&m);
    signalerFenetreEtTexturePrete();
}

void signalerFenetreEtTexturePrete()
{
    pthread_mutex_lock(&m);
    textureReady = true;
    pthread_cond_signal(&textureC);
    pthread_mutex_unlock(&m);
}

void debutConsommerTexture()
{
    pthread_mutex_lock(&m1);
    while (textureNb == 0)
    {
        pthread_cond_wait(&consommateurC, &m1);
    }
    pthread_mutex_unlock(&m1);
}

void finConsommerTexture()
{
    pthread_mutex_lock(&m1);
    textureNb--;
    pthread_cond_signal(&producteurC);
    pthread_mutex_unlock(&m1);
}

void debutDeposerTexture()
{
    pthread_mutex_lock(&m2);
    while (textureNb == NBTEX)
    {
        pthread_cond_wait(&producteurC, &m2);
    }
    pthread_mutex_unlock(&m2);
}

void finDeposerTexture()
{
    pthread_mutex_lock(&m2);
    textureNb++;
    pthread_cond_signal(&consommateurC);
    pthread_mutex_unlock(&m2);
}
