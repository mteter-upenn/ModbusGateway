


void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}


// searches for the string sfind in the string str
// returns 0 if string not found
//byte StrContains(char *str, const char *sfind){  // returns the index of the end of the string in str  DO NOT USE WITH 1 LENGTH sfind!!!! 
//    uint8_t found = 0;
//    uint8_t index = 0;
//    uint8_t len, lenf;
//    
//    len = strlen(str);
//    lenf = strlen(sfind);
//    
//    if (lenf > len) {
//        return 0;
//    }
//    while (index < len) {
//        if (str[index] == sfind[found]) {
//            found++;
//            if (lenf == found) {
//                return index;
//            }
//        }
//        else {
//            found = 0;
//        }
//        index++;
//    }
//
//    return 0;
//}
