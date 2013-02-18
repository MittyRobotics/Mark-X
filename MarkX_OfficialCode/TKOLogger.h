
#ifndef __TKOLogger_H
#define __TKOLogger_H

#include "stdio.h"
#include "Definitions.h"


class TKOLogger{
public:
//	void setMessage(char a);
//	void setMessageData(char a, float f);
	int getLength();
	float getData(int a);
	char getMessage(int a);
	void openFile();
	void closeFile();
	void log(char s[]);
	void logWithData(char s[], float f);
	TKOLogger();
	~TKOLogger();
	TKOLogger* GetInstance(TKOLogger*);
	float data[10240];
	char message[10240];
	int i;
private:
	TKOLogger* instance;
	bool opened;
	FILE* fp;
};
#endif
