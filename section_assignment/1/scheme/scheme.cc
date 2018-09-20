/**
 * Traverses a properly structured list, and returns the ordered
 * concatenation of all strings, including those in nested sublists.
 *
 * When applied to the two lists drawn above, the following strings
 * would be returned:
 *
 * ConcatAll(gameThree) would return "YankeesDiamondbacks"
 * ConcatAll(nestedNumbers) would return "onethreesix"
 */
typedef enum {
 Integer, String, List, Nil
} nodeType;

char *ConcatAll(nodeType *list){
    switch(*list){
        case nodeType.Integer:
        case nodeType.Nil:
            return strdup("");
        case nodeType.String:
            return strdup((char *)(list + 1));
        case nodeType.List:
            nodeType **p = (nodeType**)(list + 1);
            char *first = ConcatAll(*p);
            char *second = ConcatAll(*(p + 1));

            char *ret = Concat(first, second);
            free(first);
            free(second);
            return ret;
    }
}

char *Concat(char* str1, char* str2){
    char* ret = malloc((strlen(str1) + strlen(str2) + 1) * sizeof(char));
    strcpy(ret, str1);
    strcat(ret, str2);
    return ret;
}
