#include <stdio.h>
#include <string.h>
#include "ezjson.h"

#if 0
#include <ctype.h>
#endif
#if 1
static inline bool isspace(int c) { return c == 0x20 ? true: false; }
#endif

/* json_parser.c */
enum awair_json_state {
	JSON_INIT = 0,
	JSON_KEY,
	JSON_KEY_IN_STR,
	JSON_KEY_END = 3,
	JSON_VALUE = 4,
	JSON_VALUE_IN_STR,
	JSON_VALUE_END,
	JSON_VALUE_IN_VAL = 7,
	JSON_CRASHED,
	JSON_COMPLETE,
};

bool json_parse(json_key_val_pair_t dst_pairs[], uint32_t pairs_max, char *string_buffer, uint32_t buffer_size) {
    int i = 0;
    int32_t brace_depth = 0;
    int state = JSON_INIT;
    uint32_t pairs_count = 0;

    char key_buffer[JSON_TREE_KEY_SIZE] = {0, };
    int32_t key_buffer_pos = 0;
    
    // while(state != JSON_COMPLETE && state != JSON_CRASHED) 
    for(i = 0; i < buffer_size; ++i)
    {
        char *pos = string_buffer + i;
        char c = *(pos);
        // printf("%c", c);
        
        if (isspace((int)c)) {
            continue;
        } else if (c == '{') {
            brace_depth++;
            if(brace_depth == 1) { continue; } // Beginning of json

            //save value as {
            dst_pairs[pairs_count].val_start_p = pos;
            state = JSON_INIT; //start obj again
            dst_pairs[pairs_count].depth = brace_depth;
            pairs_count++;

            
        } else if (c == '}') {
            switch(state) {
                case JSON_INIT:
                case JSON_VALUE_END:
                case JSON_VALUE_IN_VAL:
                    break;
                default:
                    return false; // fail.. especially JSON_KEY status means wrong JSON format!
            }
            brace_depth--;
            // OK start again
            state = JSON_INIT;
            if(brace_depth == 0) { 
                //end of parse
                printf("JSON complete\n");
                state = JSON_COMPLETE;
                return true;
            }
        } else if (c == ':') {
            //time to find value
            printf("\n : state=%d \n", state);
            switch(state) {
                case JSON_KEY_END: {
                    state = JSON_VALUE;
                    continue;
                }
                default: return false;
            }
        } else if (c == ',') {
            printf("\n , state=%d \n", state);
            switch(state) {
                case JSON_INIT:         // obj is done.. init again
                case JSON_VALUE_END: { 
                    state = JSON_INIT; // pair is done.. init again
                    continue;
                }
                default: return false;
            }
        } else if (c == '"') {
             printf("\n \" state=%d \n", state);
            switch(state) {
                case JSON_INIT:         { 
                    state = JSON_KEY; 
                    sprintf(key_buffer, "%s", ""); 
                    key_buffer_pos = 0;
                    continue;
                }
                case JSON_KEY:          { 
                    //key failed because no characters.. normally status should be JSON_KEY_IN_STR
                    return false;
                    continue; 
                } // key start
                case JSON_KEY_IN_STR:   { 
                    state = JSON_KEY_END; 
                    snprintf(dst_pairs[pairs_count].key, JSON_TREE_KEY_SIZE, "%s", key_buffer);
                    sprintf(key_buffer, "%s", ""); 
                    key_buffer_pos = 0;
                    continue; 
                } // key complete.
                case JSON_VALUE: { 
                    dst_pairs[pairs_count].val_start_p = pos;
                    state = JSON_VALUE_IN_STR;
                    dst_pairs[pairs_count].depth = brace_depth;
                    pairs_count++;
                    continue; 
                } // value start! go ahead
                case JSON_VALUE_IN_STR: { 
                    state = JSON_VALUE_END;
                    
                    continue; 
                }
                default: {
                    return false; //something is wrong
                }
            }
        } else {
            //normal characters..
             printf("\n normal state=%d \n", state);
            switch(state) {
                case JSON_KEY: {
                    key_buffer[key_buffer_pos] = c;
                    key_buffer_pos++;
                    state = JSON_KEY_IN_STR;
                    continue;
                }
                case JSON_VALUE: { 
                    dst_pairs[pairs_count].val_start_p = pos;
                    state = JSON_VALUE_IN_VAL;
                    dst_pairs[pairs_count].depth = brace_depth;
                    pairs_count++;
                    continue; 
                } //value which is not string
                case JSON_KEY_IN_STR:   { 
                    key_buffer[key_buffer_pos] = c;
                    key_buffer_pos++;
                    continue;
                } //store key!
                case JSON_VALUE_IN_STR: { continue; }
                default: { return false; }
            }
        }

    }

    return true;
}


void json_print_pairs(json_key_val_pair_t dst_pairs[], uint32_t pairs_max) {
    int i;
    for( i = 0; i < pairs_max; ++i) {
        char val_p = 0;
        if(dst_pairs[i].val_start_p == 0) {
            printf("empty\n"); continue;
        }
        
        val_p = *dst_pairs[i].val_start_p ;
        if (val_p == '{') {
            printf(" key:%s val_start_p: object  depth: %d \n", dst_pairs[i].key, dst_pairs[i].depth);
        } else if (val_p == '"'){
            printf(" key:%s val_start_p:%c depth : %d\n", dst_pairs[i].key,  val_p, dst_pairs[i].depth);
        } else {
            printf(" key:%s val_start_p:%c depth : %d\n", dst_pairs[i].key,  val_p, dst_pairs[i].depth);
        }
    }

}

json_type_t json_get_val_by_key(json_key_val_pair_t dst_pairs[], uint32_t pairs_max, char *buf, const char *key_name) {
    int i;
    for( i = 0; i < pairs_max; ++i) {
        
        if(strcmp(dst_pairs[i].key, key_name) == 0) {
            char val_p = 0;
            if(dst_pairs[i].val_start_p == 0) {
                printf("value empty\n");
                return JSON_TYPE_UNKNOWN;
            }
            val_p = *dst_pairs[i].val_start_p ;
            if (val_p == '{') {
                printf(" key:%s val_start_p: object \n", dst_pairs[i].key);
                return JSON_TYPE_OBJ;
            } else if (val_p == '"'){
                printf(" key:%s val_start_p:%c  str.. \n", dst_pairs[i].key,  val_p);
                return JSON_TYPE_VAL_STR;
            } else {
                printf(" key:%s val_start_p:%c  vall.. \n", dst_pairs[i].key, val_p);
                return JSON_TYPE_VAL;
            }
        }
        
    }
    
    return JSON_TYPE_UNKNOWN;
}
