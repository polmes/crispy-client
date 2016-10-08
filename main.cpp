#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <tuple>
#include <sys/stat.h>
#include "../pw.hpp"

using std::string;

void curlUpFile(string userName,string clientName,string fileName);
void synchronize();
time_t getModDate(string file);
std::pair<std::vector<string>,std::vector<string>> selectFilesToExchange();
void requestSyncedFilesInfoToServer(string userName,string clientName);
string md5sum(string file);

int main(int argc,char *argv[]){
	if(argc>1){
		string arg1=string(argv[1]);
		if(arg1=="sync"){
			selectFilesToExchange();
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
			} else if (argc==2){
				std::cout<<"Usage: crispy fetchdata [username] [clientname]"<<std::endl;
			} else {
				std::cout<<"Expected three parameters for upload"<<std::endl;
				std::cout<<"Usage: crispy fetchdata [username] [clientname]"<<std::endl;
			}
		} else if(arg1=="md5sum"){
			if(argc==3){
				std::cout<<md5sum(argv[2]);
			} else {
				std::cout<<"Expected one parameter for md5sum"<<std::endl;
				std::cout<<"Usage: crispy fetchdata [username] [clientname]"<<std::endl;
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
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientname="<<clientName<<"\" ";
	ss<<"-F \"file=@"<<fileName<<"\" ";
	ss<<"-F \"filepath="<<fileName<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/uploader.php\n";
	system(ss.str().c_str());
}

void changePath(string userName, string clientName,string newPath,string oldPath){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientname="<<clientName<<"\" ";
	ss<<"-F \"oldpath="<<oldPath<<"\" ";
	ss<<"-F \"newpath="<<newPath<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/uploader.php\n";//TODO:New page for this

}

void requestSyncedFilesInfoToServer(string userName,string clientName){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientname="<<clientName<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/db_list.php > ~/.crispy_info";
	system(ss.str().c_str());
}

void synchronize(){

}

std::pair<std::vector<string>,std::vector<string>> selectFilesToExchange(){
	std::ifstream file(string(getenv("HOME"))+"/.crispy_info");
	string filename;
	string hash;
	string dateS;
	long long date;
	time_t tm;
	std::vector<std::tuple<string,string,time_t>> serverVec;
	while(!file.eof()){
		filename="";
		hash="";
		dateS="";
		getline(file,hash,'\t');
		if(file.eof()){
			break;
		}
		getline(file,filename,'\t');
		if(file.eof()){
			break;
		}
		getline(file,dateS,'\n');
		date=stoi(dateS);
		tm=date;
		serverVec.push_back(std::tuple<string,string,time_t>(hash,filename,date));
	}
	std::vector<string> newer;
	std::vector<string> older;
	for(auto it=serverVec.begin();it!=serverVec.end();it++){
		string hs=std::get<0>(*it);
		string fl=std::get<1>(*it);
		long dt=std::get<2>(*it);
		if(hs!=md5sum(fl)){
			if (getModDate(fl)>dt){
				newer.push_back(fl);
			}
			if (getModDate(fl)<dt){
				older.push_back(fl);
			}
		} else {
			std::cout<<fl<<" is synced"<<std::endl;
		}
	}
	std::cout<<"To Upload:"<<std::endl;
	for(auto it=newer.begin();it!=newer.end();it++){
		std::cout<<*it<<std::endl;
	}
	std::cout<<"To Download:"<<std::endl;
	for(auto it=older.begin();it!=older.end();it++){
		std::cout<<*it<<std::endl;
	}
	return std::pair<std::vector<string>,std::vector<string>> {newer,older};
}

time_t getModDate(string file){
	struct stat fStat;
	const char *f={file.c_str()};
	stat(f,&fStat);
	return fStat.st_mtime;
}

string md5sum(string file){
	FILE *fpipe;
	char buff[256];
	string command="md5sum "+file;
	fpipe=popen(command.c_str(),"r");
	fread(buff,256,1,fpipe);

	std::stringstream ss;
	ss<<buff;
	string r="";
	getline(ss,r,' ');
	return r;
}
