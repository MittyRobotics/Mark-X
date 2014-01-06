/*!!!!!!!
 * TO USE:
 * SEE HEADER FILE COMMENTS
 */
//Last edited by Vadim Korolik
//on 07/27/2013
#include "TKOLogger.h"
TKOLogger *_instance;
ofstream logFile;
vector<string> *messageBuffer;

/*!!!!!!!
 * TO USE:
 * SEE HEADER FILE COMMENTS
 */

TKOLogger::TKOLogger()
{
	printf("Constructing logger\n");
	logTask = new Task("Logging", (FUNCPTR) LogRunner);
	logFile.open("logT.txt", ios::app);
	if (_instance == NULL)
		_instance = this;
	printf("Done initializing logger\n");
	if (logFile.is_open())
	{
		printf("Logfile OPEN ON BOOT!!!!\n");
		logFile.close();
	}
//	else
//		printf("FILE CLOSED ON BOOT!!!!\n");
//	if (logFile.bad())
//		printf("LOGFILE REPORTED BAD ON BOOT!!!");
}
TKOLogger::~TKOLogger()
{
	delete logTask;
	messageBuffer->clear();
}
void TKOLogger::Start()
{
	logFile.open("logT.txt", ios::app);
	_ds = DriverStation::GetInstance();
	logTask->Start();
	if (logFile.is_open())
		printf("Logfile OPEN!!!!\n");
	else
		printf("FILE CLOSED!!!!\n");
	printf("Logger started\n");
}
void TKOLogger::Stop()
{
	if (logTask->Verify())
		logTask->Stop();
	if (logFile.is_open())
	{
		logFile.flush();
		logFile.close();
	}
	else
		printf("Logger stopping but log file already CLOSED!!!\n");
	printf("Logger stopped\n");
}
void TKOLogger::LogRunner()
{
	while(true)
	{
		_instance->writeBuffer();
		Wait(0.025);
	}
}
TKOLogger* TKOLogger::getInstance()
{
	if (_instance == NULL)
		_instance = this;
	return _instance;
}
void TKOLogger::writeBuffer()
{
	if (logFile.is_open())
	{
		if(messageBuffer->size() > 0)
		{
			printf("Need to write, buffer size is %i\n", messageBuffer->size());
			logFile << messageBuffer->back();
			logFile << "\n";
			messageBuffer->pop_back();
			printf("Wrote to file\n");
		}
	}
	else
	{
//		printf("Log file not open\n");
	}
	if (logFile.bad() or not logFile.is_open())
	{
		printf("LOG FILE BAD OR CLOSED WHILE WRITING\n");
		Stop();
	}
}
void TKOLogger::addMessage(string message)
{
	if (_ds == NULL)
		_ds = DriverStation::GetInstance();
	if (messageBuffer->size() >= messageBuffer->max_size())
	{
		printf("CRITICAL LOGGER BUFFER OVERFLOW \n");
		logFile << "CRITICAL LOGGER BUFFER OVERFLOW \n";
		printf("CURRENT BUFFER SIZE: %i", messageBuffer->size());
		printf("\n");
		printf("MAX BUFFER SIZE: %i", messageBuffer->max_size());
		printf("\n");
		return;
	}
	std::ostringstream newMess;
	newMess << "Time:   " << _ds->GetMatchTime() << "          Message: " << message;
	messageBuffer->push_back(newMess.str());
	printf("Buffer size: %i\n", messageBuffer->size());
}

void TKOLogger::addCMessage(string message, float val)
{
	ostringstream temp;
	temp << message << ":   " << val;
	addMessage(temp.str());
}
