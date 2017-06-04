#include <string.h>
#include <stdio.h>
#include "ezjson.h"

int main() {
    char *json = " {\
        \"key\": \"val\",\
        \"key2\": \"val2\",\
        \"key3\": {\
            \"child_key1\": \"child_val1\",\
            \"child_key2\": {\
                \"child_child_key3\": \"child_chlid_val3\"\
            }\
        },\
        \"key4\": 4.9999 , \
        \"key5\":  -5,\
        \"key6\": 6.00 ,\
    }";

    json_key_val_pair_t pairs[10] = {0, };

    if( !json_parse(pairs, 10, json, strlen(json)) )
    {
        printf("fail \n");
    } else {
        printf("success\n");
    }

    json_print_pairs(pairs, 10);

    char value[256] = {0,};

    json_type_t type = 0;
    type = json_get_val_by_key(pairs, 10, value, "key2");
    printf("value type : %d  value:%s \n", type, value);

    memset(value, 0, sizeof(value));
    type = json_get_val_by_key(pairs, 10, value, "key3");
    printf("value type : %d  value:%s \n", type, value);

    memset(value, 0, sizeof(value));
    type = json_get_val_by_key(pairs, 10, value, "key6");
    printf("value type : %d  value:%s \n", type, value);

    return 0;
}