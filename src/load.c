#include "load.h"
#include "question.h"
#include "answer.h"
#include "dateTime.h"

#include <string.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>

#define POSTS_FILE "/Posts.xml"
#define USERS_FILE "/Users.xml"
#define TAGS_FILE "/Tags.xml"

/**
 * \enum Post_attr
 * \brief Define os atributos de um post
 */
enum Post_attr{
    POST_ID,            /**< Id */
    OWNER_USER_ID,      /**< Owner Id */
    SCORE,              /**< Score */
    COMMENT_COUNT,      /**< Contagem de commentarios */
    CREATION_DATE,      /**< Data */
    OWNER_DISPLAY_NAME, /**< Owner Display Name */
    TITLE,              /**< Titulo do post (exclusivo de questões) */
    TAGS,               /**< Tags do post (exclusivo de questões) */
    ANSWER_COUNT,       /**< Numero de respostas (exclusivo de questões) */
    PARENT_ID,          /**< Id do questao a que o post responde (exclusivo de respostas) */
    POST_TYPE,          /**< Tipo do post */
    POST_NONE           /**< Nenhuma das anteriores */
};

/**
 * \enum User_attr
 * \brief Define os atributos de um user
 */
enum User_attr{
    USER_ID,            /**< Id */
    REPUTATION,         /**< Reputação */
    DISPLAY_NAME,       /**< User Name */
    ABOUT_ME,           /**< Biografia */
    USER_NONE           /**< Nenhuma das anteriores */
};

/**
 * \enum Tag_atr
 * \brief Define os atributos de uma tag
 */
enum Tag_attr{
    TAG_ID,     /**< Id */
    TAG_NAME,   /**< Nome */
    TAG_NONE    /**< Nenhuma das anteriores */
};

/**
 * Retorna o atributo de post associado à string dada
 * @param attribute Atributo a testar
 * @return O enum do atributo correspondente
 */
static inline enum Post_attr postStrcmp(const xmlChar *attribute);

/**
 * Retorna o atributo de user associado à string dada
 * @param attribute Atributo a testar
 * @return O enum do atributo correspondente
 */
static inline enum User_attr userStrcmp(const xmlChar *attribute);

/**
 * Retorna o atributo de tag associado à string dada
 * @param attribute Atributo a testar
 * @return O enum do atributo correspondente
 */
static inline enum Tag_attr tagStrcmp(const xmlChar *attribute);

/**
 * Converte uma string num DATETIME
 * @param dateStr string a converter
 * @returns A data convertida
 */
static inline DATETIME parseDate(const xmlChar *dateStr);

/**
 * Função passada ao saxHandler para fazer parse do ficheiro de posts
 * @param user_data Apontador generico usado para passar a instancia da estrutura
 * @param name Nome do elemento de xml encontrado
 * @param attrs Array de strings onde se encontram os atributos e respetivos valores
 */
void start_post_element(void *user_data, const xmlChar *name, const xmlChar **attrs){
    if(name[0] == 'p') return;
    TAD_community com = (TAD_community) user_data;
    int numAttr = 9;
    long id = -2, owner_id = -2;
    int score = 0, comment_count = 0;
    DATETIME date = NULL;
    xmlChar *owner_name = NULL;
    //question attr
    xmlChar *title = NULL;
    xmlChar *tags = NULL;
    int answer_count = 0;
    //answer attr
    long parentId = -2;
    //boolean
    int postType = 0;
    for(;postType < 3 && numAttr > 0 && attrs!=NULL && attrs[0]!=NULL;attrs += 2){
        int atrType = postStrcmp(attrs[0]);
        switch(atrType){
            case USER_ID:
            /*long*/id = strtol((char *) attrs[1],NULL,10);
                    numAttr--;
                    break;
            case OWNER_USER_ID:
            /*long*/owner_id = strtol((char *) attrs[1],NULL,10);
                    numAttr--;
                    break;
            case SCORE:
            /*int*/ score = atoi((char *) attrs[1]);
                    numAttr--;
                    break;
            case COMMENT_COUNT:
            /*int*/ comment_count = atoi((char *) attrs[1]);
                    numAttr--;
                    break;
            case CREATION_DATE:
       /*DATETIME*/ date = parseDate(attrs[1]);
                    numAttr--;
                    break;
            case OWNER_DISPLAY_NAME:
       /*xmlChar* */owner_name = xmlStrdup(attrs[1]);
                    numAttr--;
                    break;
            case TITLE:
       /*xmlChar* */title = xmlStrdup(attrs[1]);
                    numAttr--;
                    break;
            case TAGS:
       /*xmlChar* */tags = xmlStrdup(attrs[1]);
                    numAttr--;
                    break;
            case ANSWER_COUNT:
            /*int*/ answer_count = atoi((char *) attrs[1]);
                    numAttr--;
                    break;
            case PARENT_ID:
            /*long*/parentId = strtol((char *) attrs[1],NULL,10);
                    numAttr-=3;
                    break;
            case POST_TYPE:
            /*int*/ postType = atoi((char *) attrs[1]);
            default: break;
        }
    }
    if(postType == 1){
        community_add_question(
                com,
                question_create(id,date,score,owner_id,title,tags,answer_count,owner_name));
    }else if(postType == 2){
        community_add_answer(
                com,
                answer_create(id,date,score,owner_id,parentId,owner_name,comment_count));
    }else if(date){
        dateTime_destroy(date);
    }
    if(owner_name) xmlFree(owner_name);
    if(title)      xmlFree(title);
    if(tags)       xmlFree(tags);
}

/**
 * Função passada ao saxHandler para fazer parse do ficheiro de users
 * @param user_data Apontador generico usado para passar a instancia da estrutura
 * @param name Nome do elemento de xml encontrado
 * @param attrs Array de strings onde se encontram os atributos e respetivos valores
 */
void start_user_element(void *user_data, const xmlChar *name, const xmlChar **attrs){
    if(name[0] == 'u') return;
    TAD_community com = (TAD_community) user_data;
    int numAttr = 4;
    long id = -2;
    int reputation = 0;
    xmlChar *displayName = NULL;
    xmlChar *bio = NULL;
    for(;numAttr > 0 && attrs!=NULL && attrs[0]!=NULL;attrs += 2){
        int atrType = userStrcmp(attrs[0]);
        switch(atrType){
            case USER_ID:
                    id = strtol((char *) attrs[1],NULL,10);
                    numAttr--;
                    break;
            case REPUTATION:
                    reputation = atoi((char *) attrs[1]);
                    numAttr--;
                    break;
            case DISPLAY_NAME:
                    displayName = xmlStrdup(attrs[1]);
                    numAttr--;
                    break;
            case ABOUT_ME:
                    bio = xmlStrdup(attrs[1]);
                    numAttr--;
                    break;
            default: break;
        }
    }
    community_add_user(com,so_user_create(id,reputation,displayName,bio));
    if(displayName) xmlFree(displayName);
    if(bio) xmlFree(bio);
}

/**
 * Função passada ao saxHandler para fazer parse do ficheiro de tags
 * @param user_data Apontador generico usado para passar a instancia da estrutura
 * @param name Nome do elemento de xml encontrado
 * @param attrs Array de strings onde se encontram os atributos e respetivos valores
 */
void start_tag_element(void* user_data, const xmlChar* name, const xmlChar** attrs){
    if(name[0] == 't') return;
    TAD_community com = (TAD_community) user_data;
    long id = -2;
    xmlChar* tag = NULL;
    int numAttr = 2;
    for(;numAttr > 0 && attrs!=NULL && attrs[0] != NULL; attrs += 2){
        int atrType = tagStrcmp(attrs[0]);
        switch(atrType){
            case TAG_ID:
                id = strtol((char *) attrs[1], NULL, 10);
                numAttr--;
                break;
            case TAG_NAME:
                tag = xmlStrdup(attrs[1]);
                numAttr--;
                break;
        }
    }
    community_add_tag(com, id, tag);
}

/**
 * Funcão que é chamada caso o xml estaja mal formatado
 * @param user_data Apontador generico usado para passar a instancia da estrutura
 * @param msg Mensagem de erro
 */
void error_handler(void *user_data, const char *msg, ...) {
    user_data=0;
    va_list args;
    va_start(args, msg);
    g_logv("XML", G_LOG_LEVEL_CRITICAL, msg, args);
    va_end(args);
}

/**
 * Carrega os dados dos ficheiros para um instancia da estrutura
 * @param com Instancia da estrutura
 * @param dump_path Caminho para a diretoria dos ficheiros
 * @returns A instancia da estrutura
 */
TAD_community load(TAD_community com, char *dump_path){
    int n;
    char xmlPath[strlen(dump_path)+10];
    // sax handler
    xmlSAXHandler saxH;
    memset(&saxH,0,sizeof(xmlSAXHandler));
    saxH.error = error_handler;

    // parse users
    sprintf(xmlPath,"%s%s",dump_path,USERS_FILE);

    saxH.startElement = start_user_element;

    if((n=xmlSAXUserParseFile(&saxH,com,xmlPath))){
        fprintf(stderr,"Couldn't parse xml for file %s. Error: %d",USERS_FILE,n);
        return com;
    }

    // parse posts
    sprintf(xmlPath,"%s%s",dump_path,POSTS_FILE);

    saxH.startElement = start_post_element;

    if((n=xmlSAXUserParseFile(&saxH,com,xmlPath))){
        fprintf(stderr,"Couldn't parse xml for file %s. Error: %d",POSTS_FILE,n);
        return com;
    }

    // parse tags
    sprintf(xmlPath,"%s%s",dump_path,TAGS_FILE);

    saxH.startElement = start_tag_element;

    if((n=xmlSAXUserParseFile(&saxH,com,xmlPath))){
        fprintf(stderr,"Couldn't parse xml for file %s. Error: %d",TAGS_FILE,n);
        return com;
    }

    xmlCleanupParser();
    return com;
}

static inline enum Post_attr postStrcmp(const xmlChar *attribute){
    switch(attribute[0]){
        case 'I': return POST_ID;
        case 'O': switch(attribute[5]){
                      case 'U': return OWNER_USER_ID;
                      case 'D': return OWNER_DISPLAY_NAME;
                  };
        case 'S': return SCORE;
        case 'C': switch(attribute[6]){
                      case 't': return COMMENT_COUNT;
                      case 'o': return CREATION_DATE;
                  };
        case 'T': switch(attribute[1]){
                      case 'i': return TITLE;
                      case 'a': return TAGS;
                  };
        case 'A': if(attribute[1] == 'n') return ANSWER_COUNT;
        case 'P': switch(attribute[1]){
                      case 'a': return PARENT_ID;
                      case 'o': return POST_TYPE;
                  };
        default: return POST_NONE;
    }
}

static inline enum User_attr userStrcmp(const xmlChar *attribute){
    switch(attribute[0]){
        case 'I': return USER_ID;
        case 'R': return REPUTATION;
        case 'D': if(attribute[1]=='i') return DISPLAY_NAME;
        case 'A': if(attribute[1]=='b') return ABOUT_ME;

        default: return USER_NONE;
    }
}

static inline enum Tag_attr tagStrcmp(const xmlChar *attribute){
    switch(attribute[0]){
        case 'I': return TAG_ID;
        case 'T': return TAG_NAME;

        default: return TAG_NONE;
    }
}

static inline DATETIME parseDate(const xmlChar *dateStr){
    int year, month, day, hour, minute, seconds, milisseconds;
    sscanf((char *) dateStr,"%d-%d-%dT%d:%d:%d.%d",&year,&month,&day,&hour,&minute,&seconds,&milisseconds);
    return dateTime_create(year,month-1,day-1,hour,minute,seconds,milisseconds);
}

