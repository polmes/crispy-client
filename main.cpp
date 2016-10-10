#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <tuple>
#include <regex>
#include <sys/stat.h>
#include "pw.hpp"

using std::string;

void curlUpFile(const string& userName, const string& fileName);
void curlUpVect(const string& userName,std::vector<string>& vect);
void curlDownFile(const string& userName, const string& fileName);
void curlDownVect(const string& userName,std::vector<string>& vect);
void synchronize(const string& userName);
time_t getModDate(const string& file);
std::pair<std::vector<string>,std::vector<string>> selectFilesToExchange();
void requestSyncedFilesInfoFromServer(const string& userName);
string getCommandOutput(const string& command);
string md5sum(const string& file);
void commandInfo();
string makeRel( string path);
std::vector<std::tuple<string,string,time_t>> parseInfo();
string removePath(const string& filename);
void curlDownApp(const string& userName,const string& app);
bool fileExists (const string& name);
void curlUpApp(string userName,string app);
string makeAbs(const string& st);
string gown(string file);
string gmod(string file);
std::pair<string,string> getPerm();
string getUser();

int main(int argc,char *argv[]){
	//system("sudo -E su");
	if(argc>1){
		string arg1=string(argv[1]);
		if(arg1=="sync"){
			if(argc==2){
				synchronize(getUser());
			} else {
				std::cout<<"Usage: crispy sync"<<std::endl;
			}
		} else if(arg1=="upload"){
			if(argc==3){
				curlUpFile(getUser(),argv[2]);
			} else {
				std::cout<<"Usage: crispy upload [filename]"<<std::endl;
			}
		} else if(arg1=="download"){
			if(argc==3){
				curlDownFile(getUser(),argv[2]);
			} else {
				std::cout<<"Usage: crispy download [filepath]"<<std::endl;
			}
		} else if(arg1=="fetchdata"){
			if(argc==2){
				requestSyncedFilesInfoFromServer(getUser());
			} else {
				std::cout<<"Usage: crispy fetchdata"<<std::endl;
			}
		} else if(arg1=="download-app"){
			if(argc==3){
				curlDownApp(getUser(),argv[2]);
			} else {
				std::cout<<"Usage: crispy download-app [app]"<<std::endl;
			}
		} else if(arg1=="upload-app"){
			if(argc==3){
				curlUpApp(getUser(),argv[2]);
			} else {
				std::cout<<"Usage: crispy upload-app [app]"<<std::endl;
			}
		} else if(arg1=="user"){
			if(argc==2){
				std::cout<<getUser();
			} else {
				std::cout<<"Usage: crispy user"<<std::endl;
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
	std::cout<<"\tupload [filepath]"<<std::endl;
	std::cout<<"\tdownload [filepath]"<<std::endl;
	std::cout<<"\tfetchdata"<<std::endl;
	std::cout<<"\tsync"<<std::endl;
	std::cout<<"\tdownload-app [app]"<<std::endl;
	std::cout<<"\tupload-app [app]"<<std::endl;
}

void curlUpFile(const string& userName,const string& fileName){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"file=@"<<makeAbs(fileName)<<"\" ";
	ss<<"-F \"filepath="<<makeRel(fileName)<<"\" ";
	ss<<"-F \"chown="<<gown(fileName)<<"\" ";
	ss<<"-F \"chmod="<<gmod(fileName)<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/uploader.php";
	system(ss.str().c_str());
}

void curlUpVect(const string& userName,std::vector<string>& vect){
	for(auto it=vect.begin();it!=vect.end();it++){
		curlUpFile(userName,*it);
	}
}

void curlDownFile(const string& userName, const string& fileName){
	std::stringstream ss;
	ss<<"cd ~/.crispy/tmp && rm * 2>/dev/null; rm .* 2>/dev/null; ";
	ss<<"curl -JOs -D ~/.crispy/ttmmpp/hheeaaddeerr.ttxxtt ";
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"filepath="<<makeRel(fileName)<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/downloader.php";
	system(ss.str().c_str());
	ss.str(std::string());
	auto servVec=parseInfo();
	for(auto it=servVec.begin();it!=servVec.end();it++){
		if (std::get<1>(*it)==fileName){//TODO:Check hash. If not equal, discard file
			if(std::get<0>(*it)==md5sum(fileName)){

			} else {
				return;
			}
		}
	}

	ss<<"sudo -E touch "<<fileName<<" && ";
	ss<<"sudo -E mv ~/.crispy/tmp/* "<<fileName;
	system(ss.str().c_str());
	ss.str(std::string());
	auto perm=getPerm();
	ss<<"sudo -E chmod "<<perm.first<<" "<<fileName<<" ; ";
	ss<<"sudo -E chown "<<perm.second<<" "<<fileName<<" 2> /dev/null || sudo -E chown root:root "<<fileName;
	system(ss.str().c_str());


}

std::pair<string,string> getPerm(){
	std::ifstream file(string(getenv("HOME"))+"/.crispy/ttmmpp/hheeaaddeerr.ttxxtt");
	std::stringstream ss;
	ss<<file.rdbuf();
	string s=ss.str();
	string a="crispy-mod: ";
	int p=s.find(a);
	string b="crispy-own: ";
	int q=s.find(b);
	int r=s.find("Content-Type: ");

	string mod=s.substr(p+a.length(),q-p-a.length()-2);
	string own=s.substr(q+b.length(),r-q-b.length()-2);
	return std::pair<string,string>{mod,own};
}

void curlDownApp(const string& userName,const string& app){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"app="<<app<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/app-fetcher.php > ~/.crispy/tmp/fetchapp.apahaca";
	system(ss.str().c_str());

	std::ifstream file(string(getenv("HOME"))+"/.crispy/tmp/fetchapp.apahaca");
	string s="";
	std::vector<string> v;
	while(!file.eof()){
		getline(file,s,'\n');
		if(s.size()!=0){
			v.push_back(s);
		}
	}
	for (auto it=v.begin();it!=v.end();it++){
		curlDownFile(userName,*it);
	}
}

void curlUpApp(string userName,string app){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"app="<<app<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/app-uploader.php > ~/.crispy/tmp/fetchapp.apahaca";
	system(ss.str().c_str());

	std::ifstream file(string(getenv("HOME"))+"/.crispy/tmp/fetchapp.apahaca");
	string s;
	std::vector<string> v;
	while(!file.eof()){
		getline(file,s,'\n');
		if(s.size()!=0){
			v.push_back(s);
		}
	}
	for (auto it=v.begin();it!=v.end();it++){
		//std::cout<<"File:"<<*it<<std::endl;
		if(fileExists(*it)){
			//std::cout<<"file exists"<<std::endl;
			curlUpFile(userName,*it);
		}
	}
}

string removePath(const string& filename){
	auto p=filename.find_last_of("/");
	return filename.substr(p+1);
}

void curlDownVect(const string& userName,std::vector<string>& vect){
	for(auto it=vect.begin();it!=vect.end();it++){
		curlDownFile(userName,*it);
	}
}

void changePath(const string& userName,const string& newPath,const string& oldPath){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"oldpath="<<oldPath<<"\" ";
	ss<<"-F \"newpath="<<newPath<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/uploader.php\n";//TODO:New page for this
}

void requestSyncedFilesInfoFromServer(const string& userName){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-u "+username+":"+password+" ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/fetcher.php > ~/.crispy/crispy_info";
	system(ss.str().c_str());
	ss.str(string());

	ss<<"killall -r \"(listener.sh)\" 2> /dev/null; killall inotifywait 2> /dev/null; ./listener.sh &";
	system(ss.str().c_str());
}

void synchronize(const string& userName){
	requestSyncedFilesInfoFromServer(userName);//Fetches up to date info
	auto queue=selectFilesToExchange();//Lists packages to be up/downloaded
	curlUpVect(userName,queue.first);
	curlDownVect(userName,queue.second);

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
		serverVec.push_back(std::tuple<string,string,time_t>(hash,filename,tm));
	}
	return serverVec;
}

std::pair<std::vector<string>,std::vector<string>> selectFilesToExchange(){
	auto serverVec = parseInfo();
	std::vector<string> newer;
	std::vector<string> older;
	for(auto it=serverVec.begin();it!=serverVec.end();it++){
		string hs=std::get<0>(*it);
		string fl=std::get<1>(*it);
		long dt=std::get<2>(*it);
		if(hs!=md5sum(fl)){
			//std::cout<<"moddate:"<<getModDate(fl)<<std::endl;
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

time_t getModDate(const string& file){
	return atoi(getCommandOutput(string("stat -c '%Y' ")+file).c_str());
}

string md5sum(const string& file){
	FILE *fpipe;
	char buff[256]={0};
	string command="md5sum "+file;
	fpipe=popen(command.c_str(),"r");
	fread(buff,256,1,fpipe);

	std::stringstream ss;
	ss<<buff;
	string r="";
	getline(ss,r,' ');
	return r;
}

string getCommandOutput(const string& command){
	FILE *fpipe;
	char buff[256]={0};
	fpipe=popen(command.c_str(),"r");
	fread(buff,256,1,fpipe);

	std::stringstream ss;
	ss<<buff;
	string r="";
	getline(ss,r,' ');
	return r;
}

string makeRel( string path){
	string hm="/home/";
	bool match=true;
	for(unsigned int i=0;i<hm.size()&&i<path.size();i++){
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
				if (r==3){
					match=true;break;
				}
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

string makeAbs(const string& st){
	string r=st;
	if(st[0]=='~'){
		if (st[1]=='/') {
			r.erase(r.begin());
			r.insert(0,string(getenv("HOME")));
		}
	}
	return r;
}

bool fileExists (const string& name) {
	string f=makeAbs(name);
	struct stat buf;
	auto r=(stat(f.c_str(), &buf) == 0);
    return r;
}

string gown(string file){
	string s="stat -c '%U' "+file;
	string t="stat -c '%G' "+file;
	string r="";
	r+=getCommandOutput(s);
	r.pop_back();
	r+=":";
	r+=getCommandOutput(t);
	return r;
}

string gmod(string file){
	string s="stat -c '%a' "+file;
	return getCommandOutput(s);
}

string getUser(){
	std::ifstream file(string(getenv("HOME"))+"/.crispy/crispy_client");
	string s;
	getline(file,s);
	return s;
}
