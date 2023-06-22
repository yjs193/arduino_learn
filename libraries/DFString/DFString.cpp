#include "DFString.h"

DF_String::DF_String()
{

}


String DF_String::substring(String str, int title1, int position1, int title2, int position2)
{
	if(title1 == 0 && title2 == 0){//正 正
		if(position1 <= position2){
		  return str.substring(position1-1,position2);  
		}else{
		  return str.substring(position2-1,position1);  
		}
	}else if(title1 == 0 && title2 == 1){//正 倒
		if(position1 <= (str.length()-position2)){
		  return str.substring(position1-1,str.length()-position2+1);
		}else{
		  return str.substring(str.length()-position2,position1);
		}
	}else if(title1 == 1 && title2 == 0){//倒 正
		if((str.length()-position1)<position2){
		  return str.substring(str.length()-position1,position2);  
		}else{
		  return str.substring(position2-1,str.length()-position1+1);  
		}
	}else{//倒 倒
		if((str.length()-position1)<=(str.length()-position2)){
		  return str.substring(str.length()-position1,str.length()-position2+1);  
		}else{
		  return str.substring(str.length()-position2,str.length()-position1+1);
		}
	}
}

int DF_String::indexOf(String sonString,String parentString)
{
	return parentString.indexOf(sonString)+1;
}

int DF_String::lastIndexOf(String sonString,String parentString)
{
	return parentString.lastIndexOf(sonString)+1;
}

DF_String dfstring = DF_String();
