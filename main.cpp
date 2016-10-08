#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>

using std::string;

int curlFile(string userName,string clientName,string fileName);
void synchronize();
string getModDate(string file);

int main(int argc,char *argv[]){
	if(argc>1){
		if(argv[1]=="sync"){
			synchronize();
		}

	} else {
		//std::cout<<"Good job";
	}
	std::cout<<argv[0]<<std::endl;
	curlFile("usser","toshiba","/home/ramiro/syncTest/transmitterparams.config");
}


int curlFile(string userName,string clientName,string fileName){
	std::stringstream ss;
	ss<<"curl ";
	ss<<"-F \"username="<<userName<<"\" ";
	ss<<"-F \"clientName="<<clientName<<"\" ";
	ss<<"-F \"file=@"<<fileName<<"\" ";
	ss<<"-F \"filePath="<<fileName<<"\" ";
	ss<<"https://dev.coderagora.com/crispy/uploader.php\n";
	system(ss.str().c_str());
}

void synchronize(){

}

string getModDate(string file){
	struct stat fStat;
	char[] f=file.c_str();
	stat(&f,fStat);
}
