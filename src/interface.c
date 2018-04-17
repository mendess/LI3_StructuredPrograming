#include "interface.h"
#include "soUser.h"
#include "community.h"
#include "question.h"
#include "answer.h"
#include "q4_helper.h"
#include "q6_helper.h"
#include "q7_helper.h"
#include "q8_helper.h"
#include "q9_helper.h"

// query 1
STR_pair info_from_post(TAD_community com, long id){
    QUESTION question = community_get_question(com, id);
    if(question){
        char *name = (char *) question_get_owner_name(question);
        if(name == NULL){
            SO_USER user = community_get_user(
                    com,
                    question_get_owner_id(question));
            name = (char *) so_user_get_name(user);
        }
        return create_str_pair((char *) question_get_title(question), name);
    }
    ANSWER answer = community_get_answer(com, id);
    if(answer){
        long qId = answer_get_parent_id(answer);
        return info_from_post(com, qId);
    }
    return NULL;
}
// query 2
LONG_list top_most_active(TAD_community com, int N){
    return NULL;
}

// query 3
LONG_pair total_posts(TAD_community com, Date begin, Date end){
    return NULL;
}

// query 4
LONG_list questions_with_tag(TAD_community com, char* tag, Date begin, Date end){
    return questions_with_tag_helper(com, tag, begin, end);
}

// query 5
USER get_user_info(TAD_community com, long id){
    SO_USER user = community_get_user(com, id);
    if(user==NULL) return NULL;
    char* bio = (char*) so_user_get_bio(user);
    POSTS posts = so_user_get_posts(user);
    long list[10];
    int i;
    for(i=0; i<10 && posts!=NULL; i++){
        POST post = (POST) posts->data;
        long id = post_get_id(post);
        list[i] = id;
        posts = posts->next;
    }
    while(i<10)
        list[i++] = -1;

    return create_user(bio, list);
}

// query 6
LONG_list most_voted_answers(TAD_community com, int N, Date begin, Date end){
    return most_voted_answers_helper(com, N, begin, end);
}

// query 7
LONG_list most_answered_questions(TAD_community com, int N, Date begin, Date end){
    return most_answered_questions_helper(com, N, begin, end);
}

// query 8
LONG_list contains_word(TAD_community com, char* word, int N){
    return contains_word_helper(com, word, N);
}

// query 9
LONG_list both_participated(TAD_community com, long id1, long id2, int N){
    LONG_list list = create_list(N);
    for(int i=0; i<N; i++) set_list(list, i, 0);
    SO_USER user1 = community_get_user(com, id1);
    if(!user1) return list;
    SO_USER user2 = community_get_user(com, id2);
    if(!user2) return list;

    POSTS posts = so_user_get_posts(user1);
    int i = 0;
    for(; posts && i<N; posts = posts->next){
        long qId = searchThread((POST) posts->data, id2);
        if(qId != -2){
            set_list(list, i++, qId);
        }
    }
    return list;
}

// query 10
long better_answer(TAD_community com, long id){
    QUESTION question = community_get_question(com,id);
    if(question == NULL) return 0;
    ANSWERS answers = question_get_answers(question);
    int bestP = 0;
    int idBest = 0;
    for(ANSWERS cur = answers;cur != NULL;cur = cur->next){
        int score = answer_get_score(cur->data);
        long idAnswer = answer_get_id(cur->data);
        long idUser = answer_get_owner_id(cur->data);
        SO_USER user = community_get_user(com,idUser);
        int rep = so_user_get_reputation(user);
        int nrCom = answer_get_comment_count(cur->data);
        int testScore = (score*0.65)+(rep*0.25)+(nrCom*0.1);
        if(testScore>bestP){
            bestP = testScore;
            idBest = idAnswer;
        }
    }
    return idBest;
}

// query 11
LONG_list most_used_best_rep(TAD_community com, int N, Date begin, Date end){
    return NULL;
}

TAD_community clean(TAD_community com){
    community_destroy(com);
    return NULL;
}
