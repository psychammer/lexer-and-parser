#pragma once
#ifndef lexer_h
#define lexer_h

typedef struct lexer_struct{
    char current_char;
    int i;
    char* content;
    int current_line;
    int current_column;
}lexer;

token* determineToken(lexer* myLexer, token* myToken, char* valueToCheck, int tokenType);
void advance_lexer(lexer* myLexer);
void retreat_lexer(lexer* myLexer);
token* get_token_then_advance(lexer* myLexer, token* myToken);
char* char_to_string(char ch);

lexer* lexer_init(char* content){
    lexer* myLexer = malloc(sizeof(lexer));
    myLexer->content = content;
    myLexer->i = 0;
    myLexer->current_char = content[myLexer->i];
    myLexer->current_line = 0;
    myLexer->current_column = 0;

    return myLexer;
}

int is_comment(lexer* myLexer){
    // char next_char = myLexer->content[myLexer->i+1];
    // char next_two_char = myLexer->content[myLexer->i+2];

    if(myLexer->current_char == '/'){
        advance_lexer(myLexer);
        if(myLexer->current_char=='*'){
            advance_lexer(myLexer);
            return 2;
        }
        else{
            retreat_lexer(myLexer);
            return 0;
        }
    }
    else if(myLexer->current_char == '#'){
        advance_lexer(myLexer);
        return 1;
    }

    else if(myLexer->current_char == '"'){
        advance_lexer(myLexer);
        if(myLexer->current_char == '"'){
            advance_lexer(myLexer);
            if(myLexer->current_char == '"'){
                retreat_lexer(myLexer);
                retreat_lexer(myLexer);
                return 2;
            }
            else{
                retreat_lexer(myLexer);
                return 0;
            }
        }
        else{
            retreat_lexer(myLexer);
            return 0;
        }
    }
    else{
        return 0;
    }
    
}

int is_operator(char ch){
    // char next_char = myLexer->content[myLexer->i+1];
    switch(ch){
        case '+': return 1; break;
        case '-': return 1; break;
        case '*': return 1; break;
        case '/': return 1; break;
        case '%': return 1; break;

        case '=': return 1; break;
        case '>': return 1; break;
        case '<': return 1; break;
        case '!': return 1; break;

        default: return 0;
    }
}


void advance_lexer(lexer* myLexer){
    if(myLexer->i < strlen(myLexer->content)){
        myLexer->i += 1;
        myLexer->current_char = myLexer->content[myLexer->i];
    }
}

void retreat_lexer(lexer* myLexer){
    if(myLexer->i > 0){
        myLexer->i -= 1;
        myLexer->current_char = myLexer->content[myLexer->i];
    }
}

void skip_whitespace(lexer* myLexer){
    while(myLexer->current_char == ' ' || myLexer->current_char == 10 || myLexer->current_char == 13){
        advance_lexer(myLexer);
        if(myLexer->current_char == 10)
            myLexer->current_line+=1;
    }
}

void realloc_token_value_then_advance(token* myToken, lexer* myLexer){
    myToken->value = realloc(myToken->value, strlen(myToken->value) + 2);
    strcat(myToken->value, char_to_string(myLexer->current_char));
    advance_lexer(myLexer);
}

token* get_identifier_token(lexer* myLexer){


    token* myToken = malloc(sizeof(token));
    int state = 0;
    switch(myLexer->current_char){
        case 'a':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(isalnum(myLexer->current_char)!=0 && myLexer->current_char!='_' && myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for a's'
                        if(myLexer->current_char=='s'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                        }
                        else{
                            state=2; // if not s, then try n
                            break;
                        }
                    case 1: // check if there is as only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_RESERVEDWORDS, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }
                    
                    case 2: // check for a'n'
                        if(myLexer->current_char=='n'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=3; // now check for an'd'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 3: // check for an'd'
                        if(myLexer->current_char=='d'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check if it still has remaining charac
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 4: // check if there is and only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_OPERATOR, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }


                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'a'


        case 'b':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(isalnum(myLexer->current_char)!=0 && myLexer->current_char!='_' && myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for b'r'  
                        if(myLexer->current_char=='r'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                        }
                        else{
                            state=5; // if not r, then try o
                            break;
                        }
                    case 1: // check for br'e'
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check if it still has remaining charac
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 2: // check for bre'a'
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=3; // now check for brea'k'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 3: // check for brea'k'
                        if(myLexer->current_char=='k'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check if it still has remaining charac
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 4: // check if there is break only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_BREAK, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }



                    case 5: // check for b'o'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=6; // now check for bro'o'
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 6: // check for bo'o'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=7; // now check for boo'l'
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 7: // check for boo'l'
                        if(myLexer->current_char=='l'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=8; // now check if it still has remaining charac
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 8: // check if there is bool only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_DATATYPE, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }

                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'b'


        case 'c':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for c'a'  
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check for ca's'
                            break;
                        }
                        else{
                            state=7; // if not ca, then try ch
                            break;
                        }
                    case 1: // check for ca's'
                        if(myLexer->current_char=='s'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check for cas'e'
                            break;
                        }
                        else{
                            state=3; // check for 't' for catch if not 's' for case
                            break;
                        }
                    case 2: // check for cas'e'
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=6; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }    

                    case 3: // check for ca't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check for cat'c'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 4: // check for cat'c'
                        if(myLexer->current_char=='c'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=5; // // now check for catc'h'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 5: // check for catc'h'
                        if(myLexer->current_char=='h'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=15; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 6: // check if there is  case
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_CASE, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }
                    

                    case 7: // check for c'h'
                        if(myLexer->current_char=='h'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=8; // now check for ch'a'
                            break;
                        }
                        else{
                            state=16; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 8: // check for ch'a'
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=9; // now check for cha'r'
                            break;
                        }
                        else{
                            state=10; // if not ch'a', then check for ch'e'
                            break;
                        }
                    case 9: // check for cha'r'
                        if(myLexer->current_char=='r'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=13; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                        
                    case 10: // check for ch'e'
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=11; // now check for che'c'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 11: // check for che'c'
                        if(myLexer->current_char=='c'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=12; // now check for chec'k'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 12: // check for chec'k'
                        if(myLexer->current_char=='k'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=14; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 13: // check if there is char only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_DATATYPE, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }
                    case 14: // check if there is check only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_CHECK, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }
                    
                    case 15: // check if there is check only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_CATCH, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }

                    case 16:
                        // check for c'o'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=17; // now check for co'n'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }

                    case 17:
                        // check for co'n'
                        if(myLexer->current_char=='n'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=18; // now check for con's'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }

                    case 18:
                        // check for con's'
                        if(myLexer->current_char=='s'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=19; // now check for cons't'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }

                    case 19:
                        // check for cons't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=20; // now check for any remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    
                    case 20: // check if there is const only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_CONST, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }


                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'c'

        case 'd':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for d'o' or d'e'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                            break;
                        }
                        else if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=7; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 1: // check if there is only do
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_DO, myToken->value);
                        }
                        else{
                            state=2; // check for do'u' if there are more characs
                            break;
                        }
                    case 2: // check for do'u'
                        if(myLexer->current_char=='u'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=3; // now check for dou'b'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }    
                    case 3: // check for dou'b'
                        if(myLexer->current_char=='b'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check for doub'l'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 4: // check for doub'l'
                        if(myLexer->current_char=='l'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=5; // now check for doubl'e'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 5: // check for doubl'e'
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=6; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 6: // check if there is only double
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_DATATYPE, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }

                    case 7: // check for de'f'
                        if(myLexer->current_char=='f'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=8; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 8: // check if there is only double
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_RESERVEDWORDS, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }

                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'd'


        case 'e':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for e'l'  
                        if(myLexer->current_char=='l'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check for el's'
                            break;
                        }
                        else{
                            state=6; // if not el, then try en
                            break;
                        }
                    case 1: // check for el's'
                        if(myLexer->current_char=='s'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check for els'e'
                            break;
                        }
                        else{
                            state=3; // if not els, then try eli
                            break;
                        }   
                    case 2: // check for els'e'
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=5; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }

                    case 3: // check for el'i'
                        if(myLexer->current_char=='i'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check for eli'f'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 4: // check for eli'f'
                        if(myLexer->current_char=='f'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=13; // now check if it still has remaining charac
                            break; 
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 5: // check if there is only else
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_ELSE, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }



                    case 6: // check for e'n'
                        if(myLexer->current_char=='n'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=7; // now check for en'd'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 7: // check for en'd'
                        if(myLexer->current_char=='d'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=8; // now check for end'l'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        } 
                    case 8: // check for end'l'
                        if(myLexer->current_char=='l'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=9; // now check for endl'o'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        } 
                    case 9: // check for endl'o'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=10; // now check for endlo'o'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 10: // check for endlo'o'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=11; // now check for endloo'p'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        } 
                    case 11: // check for endloo'p'
                        if(myLexer->current_char=='p'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=12; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 12: // check if there is only endloop
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_ENDLOOP, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }

                    case 13: // check if there is only endloop
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_ELIF, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }
  

                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'e'


        case 'f':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for f'a'
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check for fa'l'
                            break;
                        }
                        else{
                            state=5; // if not fa, then try fu
                            break;
                        }
                    case 1: // check for fa'l'
                        if(myLexer->current_char=='l'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check for fal's'
                            break;
                        }
                        else{
                            state=3; // if not els, then try eli
                            break;
                        }   
                    case 2: // check for fal's'
                        if(myLexer->current_char=='s'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=3; // now check for fals'e'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 3: // check for fals'e'
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 4: // check if there is only false
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_BOOL, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }


                    case 5: // check for f'u'
                        if(myLexer->current_char=='u'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=6; // now check for fu'n'
                            break;
                        }
                        else{
                            state=13; // if not fu, then try fi
                            break;
                        }
                    case 6: // check for fu'n'
                        if(myLexer->current_char=='n'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=7; // now check for fun'c'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 7: // check for fun'c'
                        if(myLexer->current_char=='c'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=8; // now check for func't'
                            break;
                        }
                        else{
                            state=12; // no more keywords left, so maybe it's an id
                            break;
                        } 
                    case 8: // check for func't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=9; // now check for funct'i'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        } 
                    case 9: // check for funct'i'
                        if(myLexer->current_char=='i'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=10; // now check for functi'o'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 10: // check for functi'o'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=11; // now check for functio'n'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        } 
                    case 11: // check for functio'n'
                        if(myLexer->current_char=='n'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=12; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 12: // check if there is only function
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_FUNCTION, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }

                    
                    case 13: // check for f'i'
                        if(myLexer->current_char=='i'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=14; // now check for fi'n'
                            break;
                        }
                        else{
                            state=20 ; // if not fi, then try fl
                            break;
                        }
                    case 14: // check for fi'n'
                        if(myLexer->current_char=='n'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=15; // now check for fin'a'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 15: // check for fin'a'
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=16; // now check for fina'l'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        } 
                    case 16: // check for fina'l'
                        if(myLexer->current_char=='l'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=17; // now check for final'l'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        } 
                    case 17: // check for final'l'
                        if(myLexer->current_char=='l'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=18; // now check for finall'y'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 18: // check for finall'y'
                        if(myLexer->current_char=='y'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=19; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        } 
                    case 19: // check if there is only finally
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_RESERVEDWORDS, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }


                    case 20: // check for f'l'
                        if(myLexer->current_char=='l'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=21; // now check for fl'o'
                            break;
                        }
                        else{
                            state=25; // if not fl, then try fo
                            break;
                        }
                    case 21: // check for fl'o'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=22; // now check for flo'a'
                            break;
                        }
                        else{
                            state=-1; // if not els, then try eli
                            break;
                        }   
                    case 22: // check for flo'a'
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=23; // now check for floa't'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 23: // check for floa't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=24; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 24: // check there is only float
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_DATATYPE, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }


                    case 25: // check for f'o'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=26; // now check for fo'r'
                            break;
                        }
                        else{
                            state=-1; // if not fa, then try fu
                            break;
                        }
                    case 26: // check for fo'r'
                        if(myLexer->current_char=='r'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=27; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not els, then try eli
                            break;
                        } 
                    case 27: // check if there is only for
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_FOR, myToken->value);
                        }
                        else{
                            state=-1; // more characters ahead, so maybe it's an id
                            break;
                        }  


                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'f'



        case 'i':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for i'f'  
                        if(myLexer->current_char=='f'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=2; // if not if, then try is
                            break;
                        }
                    case 1: // check if there is only if
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_IF, myToken->value);
                        }
                        else{
                            state=-1; // check for do'u'
                            break;
                        }


                    case 2: // check for i's'
                        if(myLexer->current_char=='s'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=3; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=4; // if not is, try checking for in
                            break;
                        }    
                    case 3: 
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_RESERVEDWORDS, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        }


                    case 4: // check for i'n'
                        if(myLexer->current_char=='n'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=5; // now check for in't'
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 5: // 
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_RESERVEDWORDS, myToken->value);
                        }
                        else{
                            state=6; // if there is still more charac after in, try checking int
                            break;
                        }
                    case 6: // check for in't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=7; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 7: // check for int
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_DATATYPE, myToken->value);
                        }
                        else{
                            state=-1; // if there is still more charac after in, try checking int
                            break;
                        }
        

                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'i'


        case 'l':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for l'e'  
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=3; // if not l'e', try l'a'
                            break;
                        }
                    case 1: // check for le't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check for an'd'
                            break;
                        }
                        else{
                            state=-1; // if not is, try checking for in
                            break;
                        }
                    case 2: // check for let only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_RESERVEDWORDS, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        }    
                        
                    case 3: // check for l'a'
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=9; // if not la, try lo
                            break;
                        }
                    case 4: // check for la'm'
                        if(myLexer->current_char=='m'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=5; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 5: // 
                        if(myLexer->current_char=='b'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=6; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 6: // check for lamb'd'
                        if(myLexer->current_char=='d'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=7; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 7: // check for lambd'a'
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=8; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 8: // 
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_RESERVEDWORDS, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        }


                    case 9: // check for l'o'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=10; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 10: // check for lo'o'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=11; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 11: // check for loo'p'
                        if(myLexer->current_char=='p'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=12; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // no more keywords left, so maybe it's an id
                            break;
                        }
                    case 12: // check for loop only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_LOOP, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        }
                        
                    
        

                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'l'



        case 'n':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for n'o'  
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }
                    case 1: // check for no't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check for an'd'
                            break;
                        }
                        else{
                            state=-1; // if not is, try checking for in
                            break;
                        }
                    case 2: // check for not only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_OPERATOR, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        }    
                        

                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'n'


        case 'o':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for o'r'  
                        if(myLexer->current_char=='r'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=2; // if not o'r', try o't'
                            break;
                        }
                    case 1: // check if there is only or
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_OPERATOR, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        }


                    case 2: // check for o't'  
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=3; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }   
                    case 3: // check for ot'h'
                        if(myLexer->current_char=='h'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 4: // check for oth'e'
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=5; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 5: // check for othe'r'
                        if(myLexer->current_char=='r'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=6; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 6: // check for other'w'
                        if(myLexer->current_char=='w'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=7; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    case 7: // check for otherw'i'
                        if(myLexer->current_char=='i'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=8; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    case 8: // check for otherwi's'
                        if(myLexer->current_char=='s'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=9; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    case 9: // check for otherwis'e'
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=10; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    case 10: // check for otherwise only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_OTHERWISE, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    
                            
                        

                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'o'

        case 'p':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for p'r' 
                        if(myLexer->current_char=='r'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 1: // chechk for pr'i'
                        if(myLexer->current_char=='i'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 2: // check for pri'n'
                        if(myLexer->current_char=='n'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=3; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }   
                    case 3: // prin't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 4: // check for print'o'
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=5; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 5: // check for printo'u'
                        if(myLexer->current_char=='u'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=6; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 6: // printou't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=7; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    case 7: // check for printout
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_PRINTOUT, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    
                            
                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'p'


        case 'r':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for r'e' 
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=6; // if not r'e', try r'a'
                            break;
                        }
                    case 1: // check for re't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }
                    case 2: // check for ret'u' 
                        if(myLexer->current_char=='u'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=3; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }   
                    case 3: // check for retu'r'
                        if(myLexer->current_char=='r'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }
                    case 4: // check for retur'n'
                        if(myLexer->current_char=='n'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=5; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 5: // check for return only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_RETURN, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        } 


                    case 6: // check for r'a'  
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=7; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    case 7: // check for ra'i'
                        if(myLexer->current_char=='i'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=8; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 8: // check for rai's'
                        if(myLexer->current_char=='s'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=9; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1;
                            break;
                        }
                    case 9: // check for rais'e'
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=10; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 10: // check for raise only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_RESERVEDWORDS, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    
                            
                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'p'

        case 's':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for s't' 
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=6; // if not s't', try s'w'
                            break;
                        }
                    case 1: // check for st'r'
                        if(myLexer->current_char=='r'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=5; // check if it has only str 
                            break;
                        }
                    case 2: // check for str'i' 
                        if(myLexer->current_char=='i'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=3; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=5; 
                            break;
                        }   
                    case 3: // check for stri'n'
                        if(myLexer->current_char=='n'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }
                    case 4: // check for strin'g'
                        if(myLexer->current_char=='g'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=5; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 5: // check for string or str only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_DATATYPE, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        } 




                    case 6: // check for s'w'  
                        if(myLexer->current_char=='w'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=7; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    case 7: // check for sw'i' 
                        if(myLexer->current_char=='i'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=8; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }
                    case 8: // check for swi't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=9; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1;
                            break;
                        }
                    case 9: // check for swit'c'
                        if(myLexer->current_char=='c'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=10; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 10: // check for switc'h'
                        if(myLexer->current_char=='h'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=11; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1;
                            break;
                        }
                    case 11: // check for switch only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_SWITCH, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    
                            
                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 's'



        case 't':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for t'r' 
                        if(myLexer->current_char=='r'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=6; // if not t'r', try t'e'
                            break;
                        }
                    case 1: // check if there is only if
                        if(myLexer->current_char=='u'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=4; // if not tr'u', try tr'y'
                            break;
                        }
                    case 2: // check for o't'  
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=3; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }   
                    case 3: // check for tru'e'
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_BOOL, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        } 


                    case 4: // check for tr'y'  
                        if(myLexer->current_char=='y'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=5; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }
                    case 5: // check for try only  
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_TRY, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        }



                    case 6: // check for t'e'  
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=7; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        } 
                    case 7: // check for te'r' 
                        if(myLexer->current_char=='r'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=8; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 8: // check for ter'm'
                        if(myLexer->current_char=='m'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=9; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 9: // check for term'i' 
                        if(myLexer->current_char=='i'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=10; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 10: // check for termi'n'
                        if(myLexer->current_char=='n'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=11; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1;
                            break;
                        }
                    case 11: // check for termin'a'
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=12; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 12: // check for termina't'
                        if(myLexer->current_char=='t'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=13; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 13: // check for terminat'e'
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=14; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 14: // check for terminate only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_TERMINATE, myToken->value);
                        }
                        else{
                            state=15; // if not terminate, check for possiblity of terminateall 
                            break;
                        }
                    case 15: // check for terminate'a'
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=16; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 16: // check for terminatea'l'  
                        if(myLexer->current_char=='l'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=17; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }
                    case 17: // check for terminateal'l
                        if(myLexer->current_char=='l'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=18; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }
                    case 18: // check for terminateall only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_TERMINATEALL, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        }
                            
                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 't'



        case 'v':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for v'a' 
                        if(myLexer->current_char=='a'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=3; // if not v'a', try v'o'
                            break;
                        }
                    case 1: // check if for va'r'
                        if(myLexer->current_char=='r'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }
                    case 2: // check for var only
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_RESERVEDWORDS, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        }


                    case 3: // check for v'o'  
                        if(myLexer->current_char=='o'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }
                    case 4: // check for o't'  
                        if(myLexer->current_char=='i'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=5; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        }
                    case 5: // check for o't'  
                        if(myLexer->current_char=='d'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=6; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; // if not l'e', try l'a'
                            break;
                        } 
                    case 6: // check for   
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_DATATYPE, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        }

                    
                            
                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 's'


        case 'w':
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                break;
            }
            while(myLexer->current_char!=EOF && state!=-1){
                switch(state){
                    case 0: // check for w'h' 
                        if(myLexer->current_char=='h'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=1; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 1: // check for wh'i'
                        if(myLexer->current_char=='i'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=2; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }
                    case 2: // check for whi'l'
                        if(myLexer->current_char=='l'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=3; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1; 
                            break;
                        }   
                    case 3: // check for whil'e'
                        if(myLexer->current_char=='e'){
                            realloc_token_value_then_advance(myToken, myLexer);
                            state=4; // now check if it still has remaining charac
                            break;
                        }
                        else{
                            state=-1;
                            break;
                        }
                    case 4: // check for while only 
                        if(isalnum(myLexer->current_char)==0 && myLexer->current_char!='_'){
                            return token_init(TOKEN_WHILE, myToken->value);
                        }
                        else{
                            state=-1; 
                            break;
                        } 
                    
                            
                    default:
                        break;
                }
            }
        break; // endpoint for lexing for character 'w'


        default:
            myToken->value =  char_to_string(myLexer->current_char);
            advance_lexer(myLexer);
            break;
    }

    char* string_buffer = myToken->value;
    while(isalnum(myLexer->current_char) ||  myLexer->current_char=='_'){
        
        char* current_char_as_string = malloc(sizeof(char)+1);
        current_char_as_string[0] = myLexer->current_char;
        current_char_as_string[1] = '\0';

        string_buffer = realloc(string_buffer, strlen(string_buffer) + strlen(current_char_as_string) + 1);

        strcat(string_buffer, current_char_as_string);

            

        advance_lexer(myLexer);
    }
    
    myToken->value = string_buffer;
    myToken->type = TOKEN_ID;
    myToken->line = myLexer->current_line;
    
    // // printf("buffered: %s %d\n", string_buffer, myToken->type);
    return myToken;
}

token* get_number_token(lexer* myLexer){
    token* myToken = malloc(sizeof(token));

    char* string_buffer = malloc(sizeof(char));
    int point_counter = 0; // decimal point counter
    string_buffer[0] = '\0';
    while(isdigit(myLexer->current_char) ||  (myLexer->current_char=='.' && isdigit(myLexer->content[myLexer->i+1])&& point_counter<2)){
        if(myLexer->content[myLexer->i+1] == '.'){ // check if the next character is a decimal point.
            point_counter++;
        }
        char* current_char_as_string = malloc(sizeof(char)+1);
        current_char_as_string[0] = myLexer->current_char;
        current_char_as_string[1] = '\0';

        string_buffer = realloc(string_buffer, strlen(string_buffer) + strlen(current_char_as_string) + 1);

        strcat(string_buffer, current_char_as_string);
        advance_lexer(myLexer);
    }
    myToken->type = TOKEN_NUMBER;
    myToken->value = string_buffer;
    myToken->line = myLexer->current_line;
    
    // printf("buffered: %s %d\n", string_buffer, myToken->type);
    return myToken;
}

token* get_string_token(lexer* myLexer){
    char symbol = myLexer->current_char;
    advance_lexer(myLexer); // skip the "

    token* myToken = malloc(sizeof(token));

    char* string_buffer = malloc(sizeof(char));
    string_buffer[0] = '\0';
    while((myLexer->current_char!='"' && myLexer->current_char!=39) && myLexer->current_char != EOF && myLexer->i < strlen(myLexer->content) && myLexer->current_char != '\0'){ // isalnum(myLexer->current_char) ||  myLexer->current_char=='_'
        char* current_char_as_string = malloc(sizeof(char)+1);
        current_char_as_string[0] = myLexer->current_char;
        current_char_as_string[1] = '\0';

        string_buffer = realloc(string_buffer, strlen(string_buffer) + strlen(current_char_as_string) + 1);

        strcat(string_buffer, current_char_as_string);
        advance_lexer(myLexer);
    }
    advance_lexer(myLexer);


    if(symbol == '"'){
        myToken->type = TOKEN_STRING;
    }
    else if(symbol == '\''){
        myToken->type = TOKEN_CHAR;
    }
    
    myToken->value = string_buffer;
    myToken->line = myLexer->current_line;
    
    // printf("buffered: %s %d\n", string_buffer, myToken->type);
    return myToken;
}

char* get_single_line_string(lexer* myLexer){

    skip_whitespace(myLexer);
    char* string_buffer = malloc(sizeof(char));
    string_buffer[0] = '\0';
    while(myLexer->current_char!=10 && myLexer->current_char!=13  && myLexer->current_char!=EOF && myLexer->current_char!='\0'){
        char* current_char_as_string = malloc(sizeof(char)+1);
        current_char_as_string[0] = myLexer->current_char;
        current_char_as_string[1] = '\0';

        string_buffer = realloc(string_buffer, strlen(string_buffer) + 2);

        strcat(string_buffer, current_char_as_string);
        advance_lexer(myLexer);

    } 

    // printf("%s\n", string_buffer);
    return string_buffer;

}

char* get_multi_line_string(lexer* myLexer){
    char* string_buffer = malloc(sizeof(char));
    string_buffer[0] = '\0';
    while(myLexer->current_char!='\0' && myLexer->current_char!=EOF){
        if(myLexer->current_char=='*'){
            advance_lexer(myLexer);
            if(myLexer->current_char=='/'){
                break;
            }
            retreat_lexer(myLexer);
        }
        char* current_char_as_string = malloc(sizeof(char)+1);
        current_char_as_string[0] = myLexer->current_char;
        current_char_as_string[1] = '\0';
        string_buffer = realloc(string_buffer, strlen(string_buffer) + strlen(current_char_as_string) + 1);
        strcat(string_buffer, current_char_as_string);

        advance_lexer(myLexer);
        if(myLexer->current_char == 10 || myLexer->current_char == 13){
            skip_whitespace(myLexer);
            string_buffer = realloc(string_buffer, strlen(string_buffer) + 3);
            strcat(string_buffer, "\\n ");
        }

        free(current_char_as_string);
    } 
    advance_lexer(myLexer);
    if(myLexer->current_char == 10 || myLexer->current_char == 13){
        skip_whitespace(myLexer);
    }
    // advance_lexer(myLexer);

    return string_buffer;

}

token* get_comment_token(lexer* myLexer, int commentType){
    if(commentType==2){
        int current_line = myLexer->current_line;
        token* myToken = token_init(TOKEN_MULTICOMMENT, get_multi_line_string(myLexer));
        myToken->line = current_line;
        return myToken;
    }
    else if(commentType==1){
        token* myToken = token_init(TOKEN_SINGLECOMMENT, get_single_line_string(myLexer));
        myToken->line = myLexer->current_line;
        return myToken;
    }
}

token* get_token_then_advance(lexer* myLexer, token* myToken){
    advance_lexer(myLexer);
    myToken->line = myLexer->current_line;
    return myToken;
}

char* char_to_string(char ch){
    char* char_as_string = malloc(sizeof(char)+1);
    char_as_string[0] = ch;
    char_as_string[1] = '\0';
    return char_as_string;
}

token* token_buffer(lexer* myLexer){
    token* myToken = (void*)0;
    while(myLexer->current_char != EOF && myLexer->i < strlen(myLexer->content) || myLexer->current_char != '\0'){
        if(myLexer->current_char== ' ' || myLexer->current_char == 10 || myLexer->current_char == 13){
            skip_whitespace(myLexer);
        }

        if(isdigit(myLexer->current_char)){
            return get_number_token(myLexer);
        }

        if(isalnum(myLexer->current_char) || myLexer->current_char=='_'){
            myToken = get_identifier_token(myLexer);
            myToken->line = myLexer->current_line;
            return myToken;
        }

        if(myLexer->current_char == '"' || myLexer->current_char == '\''){
            return get_string_token(myLexer);
        }

        int commentType = is_comment(myLexer);
        if(commentType!=0){
            return get_comment_token(myLexer, commentType);
        }
        else if(is_operator(myLexer->current_char)){
            char operator = myLexer->current_char;
            advance_lexer(myLexer);
            char next_char = myLexer->current_char;
            char* symbol = malloc(sizeof(char)+2);
            symbol[0] = operator;
            symbol[1] = next_char;
            symbol[2] = '\0';

            // printf("%s\n", symbol);
            if(next_char==operator && (next_char=='*' || next_char=='+' || next_char=='-' || next_char=='/' || next_char=='=')){    
                 
                return get_token_then_advance(myLexer, token_init(TOKEN_OPERATOR, symbol));
            }
            else if((operator=='+' || operator=='-' || operator=='/' || operator=='*' || operator=='%' || operator=='<' || operator=='>' || operator=='!') && next_char=='='){ // check if it is an assignment or relational operator  
                return get_token_then_advance(myLexer, token_init(TOKEN_OPERATOR, symbol));
            }

            token* myToken = token_init(TOKEN_OPERATOR, char_to_string(operator));
            myToken->line = myLexer->current_line;
            return myToken;
        }

        if(myLexer->current_char == '\\'){
            char* backslash = char_to_string(myLexer->current_char);
            advance_lexer(myLexer);

            backslash = realloc(backslash, 3);
            strcat(backslash, char_to_string(myLexer->current_char));
            switch(myLexer->current_char){
                case 's': return get_token_then_advance(myLexer, token_init(TOKEN_BLANK, backslash));
                case 't': return get_token_then_advance(myLexer, token_init(TOKEN_BLANK, backslash));
                case 'n': return get_token_then_advance(myLexer, token_init(TOKEN_BLANK, backslash));
                default:
                    return get_token_then_advance(myLexer, token_init(TOKEN_UNKNOWN, backslash)); break;
            }
        }

        switch(myLexer->current_char){
            case '.': return get_token_then_advance(myLexer, token_init(TOKEN_DOT, char_to_string('.'))); break;
            case '@': return get_token_then_advance(myLexer, token_init(TOKEN_SYMBOL, char_to_string('@'))); break;
            case '^': return get_token_then_advance(myLexer, token_init(TOKEN_SYMBOL, char_to_string('^'))); break;
            case '~': return get_token_then_advance(myLexer, token_init(TOKEN_TILDE, char_to_string('~'))); break;
            case '?': return get_token_then_advance(myLexer, token_init(TOKEN_QUESTION, char_to_string('?'))); break;
            case '!': return get_token_then_advance(myLexer, token_init(TOKEN_OPERATOR, char_to_string('!'))); break;
            case '&': return get_token_then_advance(myLexer, token_init(TOKEN_AMPERSAND, char_to_string('&'))); break;
            case ':': return get_token_then_advance(myLexer, token_init(TOKEN_COLON, char_to_string(':'))); break;
            case ';': return get_token_then_advance(myLexer, token_init(TOKEN_SEMI, char_to_string(';'))); break;
            case '(': return get_token_then_advance(myLexer, token_init(TOKEN_LPAREN, char_to_string('('))); break;
            case ')': return get_token_then_advance(myLexer, token_init(TOKEN_RPAREN, char_to_string(')'))); break;
            case '{': return get_token_then_advance(myLexer, token_init(TOKEN_LBRACE, char_to_string('{'))); break;
            case '}': return get_token_then_advance(myLexer, token_init(TOKEN_RBRACE, char_to_string('}'))); break;
            case ',': return get_token_then_advance(myLexer, token_init(TOKEN_COMMA, char_to_string(','))); break;
            case '[': return get_token_then_advance(myLexer, token_init(TOKEN_LBRACKET, char_to_string('['))); break;
            case ']': return get_token_then_advance(myLexer, token_init(TOKEN_RBRACKET, char_to_string(']'))); break;


            default: 
                return get_token_then_advance(myLexer, token_init(TOKEN_UNKNOWN, char_to_string(myLexer->current_char))); break;
        }

        // printf("debug %d\n", myLexer->current_char);
        advance_lexer(myLexer);
        return token_init(404, NULL);
    }

    return NULL;
}

#endif 


token* determineToken(lexer* myLexer, token* myToken, char* valueToCheck, int tokenType){
    for(int i = 0; isalnum(myLexer->current_char) ||  myLexer->current_char=='_'; i++){

        char next_char = myLexer->content[myLexer->i+1];
        char* current_character = malloc(sizeof(char)+1);
        current_character[0] = myLexer->current_char;
        current_character[1] = '\0';
        myToken->value = realloc(myToken->value, strlen(myToken->value) + 2);

        strcat(myToken->value, current_character);
        if(valueToCheck[i] == myLexer->current_char &&  isalnum(next_char) == 0 &&  next_char != '_' && i < strlen(valueToCheck)){ // if the token is followed by a '_' at the last char it is considered to be an identifier.
            myToken->type = tokenType; 
            myToken->line = myLexer->current_line;
            return get_token_then_advance(myLexer, myToken);
        }
        advance_lexer(myLexer);
    }
    myToken->type =  TOKEN_ID;
    myToken->line = myLexer->current_line;
    return get_token_then_advance(myLexer, myToken);
}
