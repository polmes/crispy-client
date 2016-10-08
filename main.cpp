#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>

using std::string;

int curlFile(string userName,string clientName,string fileName);
void synchronize();
time_t getModDate(string file);

int main(int argc,char *argv[]){
	if(argc>1){
		if(argv[1]=="sync"){
			synchronize();
		}

	} else {
		//std::cout<<"Good job";
	}
	std::cout<<argv[0]<<std::endl;
	curlUpFile("usser","toshiba","/home/ramiro/syncTest/transmitterparams.config");
	time_t t1=getModDate("/home/ramiro/syncTest/transmitterparams.config");
	time_t t2=getModDate("/home/ramiro/syncTest/test1.txt");
	std::cout<<t2;
}


int curlUpFile(string userName,string clientName,string fileName){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientName="<<clientName<<"\" ";
	ss<<"-F \"file=@"<<fileName<<"\" ";
	ss<<"-F \"filePath="<<fileName<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/uploader.php\n";
	system(ss.str().c_str());
}

void requestSyncedFilesInfoToServer(string userName,string clientName){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientname="<<clientName<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/dbdata.php > ~/.crispy_info";
	system(ss.str().c_str());
}

void synchronize(){

}

time_t getModDate(string file){
	struct stat fStat;
	const char *f={file.c_str()};
	stat(f,&fStat);
	return fStat.st_mtime;
}
