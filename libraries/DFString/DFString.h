#ifndef DF_STRING_h
#define DF_STRING_h

#include "Arduino.h"


class DF_String
{
 public:
    DF_String();
    String substring(String str, int title1, int position1, int title2, int position2);
	int indexOf(String sonString,String parentString);
	int lastIndexOf(String sonString,String parentString);

    
    
 private:

};

extern DF_String dfstring;



#endif
