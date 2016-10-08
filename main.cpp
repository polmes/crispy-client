#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <tuple>
#include <regex>
#include <sys/stat.h>
#include "../pw.hpp"

using std::string;

void curlUpFile(string userName,string clientName,string fileName);
void curlUpVect(string userName, string clientName,std::vector<string> vect);
void curlDownFile(string userName, string clientName, string fileName);
void curlDownVect(string userName, string clientName,std::vector<string> vect);
void synchronize(string userName, string clientName);
time_t getModDate(string file);
std::pair<std::vector<string>,std::vector<string>> selectFilesToExchange();
void requestSyncedFilesInfoFromServer(string userName,string clientName);
string md5sum(string file);
void commandInfo();
string makeRel(string path);
std::vector<std::tuple<string,string,time_t>> parseInfo();
string removePath(string filename);



int main(int argc,char *argv[]){
	if(argc>1){
		string arg1=string(argv[1]);
		if(arg1=="sync"){
			if(argc==4){
				synchronize(argv[2],argv[3]);
			} else {
				std::cout<<"Usage: crispy sync [username] [clientname]"<<std::endl;
			}
		} else if(arg1=="upload"){
			if(argc==5){
				curlUpFile(argv[2],argv[3],argv[4]);
			} else {
				std::cout<<"Usage: crispy upload [username] [clientname] [filename]"<<std::endl;
			}
		} else if(arg1=="fetchdata"){
			if(argc==4){
				requestSyncedFilesInfoFromServer(argv[2],argv[3]);
			} else {
				std::cout<<"Usage: crispy fetchdata [username] [clientname]"<<std::endl;
			}
		} else if(arg1=="md5sum"){
			if(argc==3){
				std::cout<<md5sum(argv[2]);
			} else {
				std::cout<<"Usage: crispy md5sum [filename]"<<std::endl;
			}
		} else if(arg1=="download"){
			if(argc==5){
				curlDownFile(argv[2],argv[3],argv[4]);
			} else {
				std::cout<<"Usage: crispy download [username] [clientname] [filepath]"<<std::endl;
			}
		} else if(arg1=="makerel"){
			if(argc==3){
				makeRel(argv[2]);
			} else {
				std::cout<<"Usage: crispy makerel [username] [clientname] [filepath]"<<std::endl;
			}
		} else {
			commandInfo();
		}
	} else {
		commandInfo();
	}
}

void commandInfo(){
	std::cout<<"Commands:"<<std::endl;
	std::cout<<"\tupload [username] [clientname] [filepath]"<<std::endl;
	std::cout<<"\tdownload [username] [clientname] [filepath]"<<std::endl;
	std::cout<<"\tfetchdata [username] [clientname]"<<std::endl;
	std::cout<<"\tsync"<<std::endl;
	std::cout<<"\tmd5sum [filepath]"<<std::endl;
}

void curlUpFile(string userName,string clientName,string fileName){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientname="<<clientName<<"\" ";
	ss<<"-F \"file=@"<<fileName<<"\" ";
	ss<<"-F \"filepath="<<makeRel(fileName)<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/uploader.php";
	system(ss.str().c_str());
}

void curlUpVect(string userName, string clientName,std::vector<string> vect){
	for(auto it=vect.begin();it!=vect.end();it++){
		curlUpFile(userName,clientName,*it);
	}
}

void curlDownFile(string userName, string clientName, string fileName){
	std::stringstream ss;
	ss<<"cd ~/.crispy/tmp &&  ";
	ss<<"curl -JO ";
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientname="<<clientName<<"\" ";
	ss<<"-F \"filepath="<<makeRel(fileName)<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/downloader.php";
	system(ss.str().c_str());
	ss.str(std::string());
	auto servVec=parseInfo();
	for(auto it=servVec.begin();it!=servVec.end();it++){
		if (std::get<1>(*it)==fileName){std::cout<<"check";}//TODO:Check hash
	}


	ss<<"mv ~/.crispy/tmp/* "<<fileName;
	system(ss.str().c_str());
}

string removePath(string filename){
	auto p=filename.find_last_of("/");
	return filename.substr(p+1);
}

void curlDownVect(string userName, string clientName,std::vector<string> vect){
	for(auto it=vect.begin();it!=vect.end();it++){
		curlDownFile(userName,clientName,*it);
	}
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

void requestSyncedFilesInfoFromServer(string userName,string clientName){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientname="<<clientName<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/db_list.php > ~/.crispy/crispy_info";
	system(ss.str().c_str());
}

void synchronize(string userName, string clientName){
	requestSyncedFilesInfoFromServer(userName,clientName);//Fetches up to date info
	auto queue=selectFilesToExchange();//Lists packages to be up/downloaded
	curlUpVect(userName,clientName,queue.first);
	curlDownVect(userName,clientName,queue.second);

}

std::vector<std::tuple<string,string,time_t>> parseInfo(){
	std::ifstream file(string(getenv("HOME"))+"/.crispy/crispy_info");
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
	return serverVec;
}

std::pair<std::vector<string>,std::vector<string>> selectFilesToExchange(){
	auto serverVec= parseInfo();
	std::vector<string> newer;
	std::vector<string> older;
	for(auto it=serverVec.begin();it!=serverVec.end();it++){
		string hs=std::get<0>(*it);
		string fl=std::get<1>(*it);
		long dt=std::get<2>(*it);
		if(hs!=md5sum(fl)){
			if (getModDate(fl)>dt){
				newer.push_back(fl);
			} else if (getModDate(fl)<dt){
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

string makeRel(string path){
	string hm="/home/";
	bool match=true;
	for(int i=0;i<hm.size()&&i<path.size();i++){
		if(path[i]!=hm[i]){
			match=false;
			break;
		}
	}
	int i=0;
	if(match){
		int r=0;
		match=false;
		for(i=0;i<path.size();i++){
			if(path[i]=='/'){
				r++;
				if (r==3){match=true;break;}
			}
		}
	}
	string t=path;
	if(match){
		for(;i>=0;i--){
			t.erase(t.begin());
		}
		t.insert(0,"~/");
	}
	return t;
}
