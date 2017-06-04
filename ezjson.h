//Author : Steve (Seunghwa) Song <sshtel@gmail.com>

#include <stdbool.h>
#include <stdint.h>

#define JSON_TREE_KEY_SIZE 32
#define JSON_TREE_VAL_SIZE 256



// typedef struct {
//     char *obj_begin;
//     char *obj_end;
//     int32_t brace_depth;
//     json_key_val_pair_t **pairs;
// } json_obj_t;


typedef struct {
    char key[JSON_TREE_KEY_SIZE];
    char *val_start_p;
    char *val_end_p;
    int32_t depth;
} json_key_val_pair_t;


typedef enum {
    JSON_TYPE_UNKNOWN = 0,
    JSON_TYPE_KEY,
    JSON_TYPE_VAL,
    JSON_TYPE_VAL_STR,
    JSON_TYPE_OBJ
} json_type_t;

bool json_parse(json_key_val_pair_t dst_pairs[], uint32_t pairs_max, char *string_buffer, uint32_t buffer_size);

void json_print_pairs();

json_type_t json_get_val_by_key(json_key_val_pair_t dst_pairs[], uint32_t pairs_max, char *buf, const char *key_name);


// uint32_t json_obj_child_count(json_obj_t *obj);

/*
{
    "key": "val", // no braces and comma! complete
    "key2": "val2",
    "key3": {
        "child_key1": "child_val1",
        "child_key2": {
            "child_child_key3": "child_chlid_val3"
        }
    },
    "key4": "val4"
}
*/