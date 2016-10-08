#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <sys/stat.h>

using std::string;

void curlUpFile(string userName,string clientName,string fileName);
void synchronize();
time_t getModDate(string file);
std::vector<string> selectFilesToUpload();
void requestSyncedFilesInfoToServer(string userName,string clientName);

int main(int argc,char *argv[]){
	if(argc>1){
		string arg1=string(argv[1]);
		if(arg1=="sync"){
			synchronize();
		} else if(arg1=="upload"){
			if(argc==5){
				curlUpFile(argv[2],argv[3],argv[4]);
			} else if (argc==2){
				std::cout<<"Usage: crispy upload [username] [clientname] [filename]"<<std::endl;
			} else {
				std::cout<<"Expected three parameters for upload"<<std::endl;
				std::cout<<"Usage: crispy upload [username] [clientname] [filename]"<<std::endl;
			}
		} else if(arg1=="fetchdata"){
			if(argc==4){
				requestSyncedFilesInfoToServer(argv[2],argv[3]);
			}
		} else {
			std::cout<<"Commands:"<<std::endl;
			std::cout<<"\tupload [username] [clientname] [filename]"<<std::endl;
			std::cout<<"\tfetchdata [username] [clientname]"<<std::endl;

		}
	} else {
		std::cout<<"Commands:"<<std::endl;
		std::cout<<"\tupload [username] [clientname] [filename]"<<std::endl;
		std::cout<<"\tfetchdata [username] [clientname]"<<std::endl;
	}
	//curlUpFile("crispy","toshiba","/home/ramiro/syncTest/transmitterparams.config");
	time_t t1=getModDate("/home/ramiro/syncTest/transmitterparams.config");
	time_t t2=getModDate("/home/ramiro/syncTest/test1.txt");
	//std::cout<<t1<<" "<<t2<<std::endl;
	//selectFilesToUpload();
}


void curlUpFile(string userName,string clientName,string fileName){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientname="<<clientName<<"\" ";
	ss<<"-F \"file=@"<<fileName<<"\" ";
	ss<<"-F \"filePath="<<fileName<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/uploader.php\n";
	system(ss.str().c_str());
}

void changePath(string userName, string clientName,string newPath,string oldPath){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientname="<<clientName<<"\" ";
	ss<<"-F \"oldpath="<<oldPath<<"\" ";
	ss<<"-F \"newpath="<<newPath<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/uploader.php\n";//TODO:New page for this

}

void requestSyncedFilesInfoToServer(string userName,string clientName){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientname="<<clientName<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/client_dates.php > ~/.crispy_info";
	system(ss.str().c_str());
}

void synchronize(){

}

std::vector<string> selectFilesToUpload(){
	std::ifstream file("/home/ramiro/.crispy_info");
	string filename;
	string dateS;
	long long date;
	time_t tm;
	std::vector<std::pair<string,time_t>> serverVec;
	while(!file.eof()){
		filename="";
		dateS="";
		getline(file,filename,'\t');
		if(file.eof()){
			break;
		}
		getline(file,dateS,'\n');
		date=stoi(dateS);
		tm=date;
		serverVec.push_back(std::pair<string,time_t>(filename,date));
	}
	std::vector<string> newer;
	for(auto it=serverVec.begin();it!=serverVec.end();it++){
		if (getModDate((*it).first)>(*it).second){
			newer.push_back((*it).first);
		}
	}
	for(auto it=newer.begin();it!=newer.end();it++){
		std::cout<<*it<<" ";
	}
	return newer;
}

time_t getModDate(string file){
	struct stat fStat;
	const char *f={file.c_str()};
	stat(f,&fStat);
	return fStat.st_mtime;
}
