#include "TKOLogger.h"
TKOLogger::TKOLogger(){
	i = 0;
}
TKOLogger* TKOLogger::getInstance(){
	if(instance == NULL){
		instance = new TKOLogger();
	}
	return instance;
}
//void TKOLogger::setMessage(char a){
//	message[i++] = a;
//}
//void TKOLogger::setMessageData(char a, float f){
//	message[i] = a;
//	data[i] = f;
//	i++;
//}
void TKOLogger::openFile(){
	fp = fopen("log.txt","w");
	if(fp != NULL) opened = true;
}
void TKOLogger::closeFile(){
	fclose(fp);
}
void TKOLogger::log(char s[]){
	if(opened){
		fprintf(fp,"%s\n",s);
	}
}
void TKOLogger::logWithData(char s[], float f){
	if(opened){
		fprintf(fp,"%s %f\n",s,f);
	}
}
int TKOLogger::getLength(){
	return i;
}
char TKOLogger::getMessage(int a){
	return message[i];
}
float TKOLogger::getData(int a){
	return data[i];
}
TKOLogger::~TKOLogger(){
	//lol
}
