//
// Created by avina on 5/16/2017.
//

//g++ -std=c++11 test.cpp
//#include <algorithm>
#include <iostream>
#include <cstring>
#include <ios>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "porter2_stemmer.h"


using  namespace std;

typedef vector<string> StringVector;
typedef std::map<std::string,int> orderedMap;
typedef std::unordered_map<std::string,unsigned int> StringUnIntMap;
typedef std::map<std::string,unsigned int> OStringUnIntMap;
typedef std::map<std::string,string*> stringOrderedMap;
StringUnIntMap addressmap;
//StringUnIntMap localmap=localmap;
string sourceDir,destDir;
stringOrderedMap tempIndexMap;
const string HASHFILE = "indexHash.txt";
const string DOCFILE = "DocHash.txt";
const string FILE_WORD_COUNT = "FileCount.txt";
const string CountIndex = "CountIndex";
typedef map<unsigned int,string> IntStringMap;
short countx=0;
OStringUnIntMap fileCountMap;
bool isConcept=0;
double concept_weight=0.0;
IntStringMap docContainer;

const char kPathSeparator =
#ifdef _WIN32
        '\\';
#else
'/';
#endif

inline char separator()
{
    return kPathSeparator;
}
unordered_set<std::string> stopwords = {"a", "about", "above", "after", "again", "against", "all", "am", "an", "and", "any", "are", "aren't", "as", "at", "be", "because", "been", "before", "being", "below", "between", "both", "but", "by", "can't", "cannot", "could", "couldn't", "did", "didn't", "do", "does", "doesn't", "doing", "don't", "down", "during", "each", "few", "for", "from", "further", "had", "hadn't", "has", "hasn't", "have", "haven't", "having", "he", "he'd", "he'll", "he's", "her", "here", "here's", "hers", "herself", "him", "himself", "his", "how", "how's", "i", "i'd", "i'll", "i'm", "i've", "if", "in", "into", "is", "isn't", "it", "it's", "its", "itself", "let's", "me", "more", "most", "mustn't", "my", "myself", "no", "nor", "not", "of", "off", "on", "once", "only", "or", "other", "ought", "our", "ours", "out", "over", "own", "same", "shan't", "she", "she'd", "she'll", "she's", "should", "shouldn't", "so", "some", "such", "than", "that", "that's", "the", "their", "theirs", "them", "themselves", "then", "there", "there's", "these", "they", "they'd", "they'll", "they're", "they've", "this", "those", "through", "to", "too", "under", "until", "up", "very", "was", "wasn't", "we", "we'd", "we'll", "we're", "we've", "were", "weren't", "what", "what's", "when", "when's", "where", "where's", "which", "while", "who", "who's", "whom", "why", "why's", "with", "won't", "would", "wouldn't", "you", "you'd", "you'll", "you're", "you've", "your", "yours", "yourself", "yourselves"};
orderedMap readFileinVector(string fileName);

string sequenceMerge(string string1, string string2);

void writeIndexHashInFile(string folder);

void loadIndexInMemory(string dest);

void writeMapinFileNew(string dest, const string &file);

void writeMapInMemory(orderedMap map, const string &file);

void createDirectory(string dir){
#if defined(_WIN32)
    _mkdir(dir.c_str());
#else
    mkdir(dir.c_str(), 0777);
#endif
}

StringVector listFileFromDir(string dir){
    DIR *pDIR;
    struct dirent *entry;
    StringVector listOfFile;
    if( pDIR=opendir(dir.c_str()) ){
        while(entry = readdir(pDIR)){
            if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 )
                listOfFile.push_back((basic_string<char, char_traits<char>, allocator<char>> &&) entry->d_name);
        }
        closedir(pDIR);
    }
    return listOfFile;
}

vector<string> split(string str, char delimiter) {
    vector<string> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;

    while(getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }

    return internal;
}

void createIndexFile(string source,string dest){
    unsigned int fileSize=0;
    DIR* dir = opendir(dest.c_str());
    if(dir)
        return;
    StringVector listOffiles = listFileFromDir(source);
    closedir(dir);
    string filename;
    ifstream in;
    string fileX;
    short i = 0;
    StringVector tmpFile;
    for(auto const &file : listOffiles){
        filename = source+separator()+file;
        in.open(filename,ios::binary|ios::ate);
        //cout << in.good()<<endl;
        //cout << in.tellg();
        fileSize = fileSize + in.tellg();
        in.close();
        docContainer[i]=file;
        orderedMap localmap =  readFileinVector(file);
        if(fileSize>50 &&tempIndexMap.size()>0 ){
            writeMapinFileNew(dest,fileX);
            tempIndexMap.clear();
            tmpFile.push_back(to_string(countx));
            countx++;
            fileSize=0;
        }
        writeMapInMemory(localmap,to_string(i));

        localmap.clear();
        i++;
    }
    writeMapinFileNew(dest,fileX);
    tmpFile.push_back(to_string(countx));
    string f1 = tmpFile[0];
    for(short i =1 ; i<tmpFile.size();i++){
        string x(dest+separator()+CountIndex+separator()+f1);
        string y(dest+separator()+CountIndex+separator()+tmpFile[i]);
        f1=sequenceMerge(x,y);
    }
    writeIndexHashInFile(dest);
}

void writeMapInMemory(orderedMap map, const string &file) {
    string data;
    for(auto const& ent : map){
        if(tempIndexMap.find(ent.first)==tempIndexMap.end()){
            data= file + ':' + to_string(ent.second);
            //delete ent.second;
            tempIndexMap.insert({ent.first,new string(data)});
        }
        else{
            data = *tempIndexMap[ent.first];
            data = data + '|' + file + ':' + to_string(ent.second);
            //delete ent.second;
            *tempIndexMap[ent.first]=data;
        }
    }

}

void writeMapinFileNew(string dir, const string &ogFile) {
    //mkdir(dir.c_str());
    DIR* dirx = opendir(dir.c_str());
    if(!dirx)
        createDirectory(dir);
    closedir(dirx);
    string direc = dir + separator() + CountIndex;
    dirx = opendir(direc.c_str());
    if(!dirx)
        createDirectory(direc);
    closedir(dirx);
    string fileName = direc + separator() + to_string(countx);
    //string fileName = dir + separator() + FILE_WORD_COUNT;
    ofstream out (fileName,ios::binary );
    for(auto const& ent : tempIndexMap){
        string data = ent.first+"="+*ent.second;
//        addressmap[ent.first]=out.tellp();
        out << data << endl;
        out.rdbuf();
        delete ent.second;
    }
    out.close();
}

void writeIndexHashInFile(string folder)
{
    string fileLocation = folder + separator() + HASHFILE;
    string input=destDir + separator()+CountIndex + separator() + FILE_WORD_COUNT;
    ifstream in(input,ios::binary);
    ofstream out(fileLocation, ios::binary);
    /*for(auto const &ent2 : addressmap){
        out << ent2.first << '=' << ent2.second<<endl;
        //delete *ent2.second;
    }*/
    string line;
    int x = in.tellg();
    while(getline(in,line)){
        if(line!=""){
            StringVector s = split(line,'=');
            out << s[0] << '=' << x<<endl;
            x=in.tellg();
        }

    }
    out.close();
    fileLocation = folder + separator() + DOCFILE;
    out.open(fileLocation, ios::binary);
    for(auto const &ent : docContainer){
        out << ent.first << '=' << ent.second<<endl;
    }
    out.close();

}
int searchKeyword(string keyword){
    std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
    Porter2Stemmer::stem(keyword);
    if(addressmap.find(keyword)==addressmap.end())
        return -1;
    unsigned int index = addressmap[keyword];
    string fileLocation=destDir+separator()+CountIndex+separator()+FILE_WORD_COUNT;
    ifstream in(fileLocation,ios::binary);
    string s;
    in.seekg(index);
    getline(in,s);
    vector<string> list = split(s,'=');
    vector<string> files = split(list[1],'|');
    vector<string> flCount;
    OStringUnIntMap tempIndexMap;
    unsigned  int count=0;
    for(string f : files){
        flCount = split(f,':');
        count = stoi(flCount[1]);
        if(fileCountMap.size()>0 && fileCountMap.find(flCount[0])!=fileCountMap.end()){
            count+=fileCountMap[flCount[0]];
            tempIndexMap.insert({flCount[0],count});
        } else if(fileCountMap.size()==0){
            tempIndexMap.insert({flCount[0],count});
        }
    }
    fileCountMap = tempIndexMap;
}

int main(int argc, char const *argv[]){
   // localmap.reserve(171000);
    //localmap.reserve(171000);
    sourceDir=argv[1];
    destDir = argv[2];
    createIndexFile(sourceDir,destDir);
    loadIndexInMemory(destDir);
    vector<string> keys;
    if(argv[3]=="-c"){
        isConcept=1;
        concept_weight=stod(argv[4]);
    } else{
        for(short i=3;i<argc;i++){
            string a(argv[i]);
            keys.push_back(a);
        }
    }
    for(string key:keys){
        int x = searchKeyword(key);
        if(x==-1)
            exit(0);
    }
    std::vector<std::pair<std::string,unsigned int>> finalRankMap(fileCountMap.begin(),fileCountMap.end());
    sort( finalRankMap.begin(), finalRankMap.end(),
          []( const std::pair<std::string,unsigned int>& lhs, const std::pair<std::string,unsigned int> & rhs )
          { return lhs.second > rhs.second; } );
    for(auto const& ent : finalRankMap){
        cout<<docContainer[stoi(ent.first)]<<endl;
    }
    return 0;
}

void loadIndexInMemory(string dest) {
    //localmap.clear();
    //docContainer.clear();
    string fileLocation = dest+separator()+HASHFILE;
    ifstream in(fileLocation,ios::binary);
    string s;
    vector<string> temp;
    if(addressmap.size()==0){
        while(getline(in,s)){
            temp=split(s,'=');
            addressmap[temp[0]]=stoi(temp[1]);
        }
    }
    in.close();
    fileLocation = dest+separator()+DOCFILE;
    in.open(fileLocation,ios::binary);
    if(docContainer.size()==0){
        while(getline(in,s)){
            temp=split(s,'=');
            docContainer[stoi(temp[0])]=temp[1];
        }
    }
    in.close();
}

/*
stringmap readFileinVector(string fileName) {
    fileName = sourceDir+separator()+fileName;
    ifstream in(fileName);
    string s;
    string s1;
    vector<string> sep;
    while (in >> s){

        if(stopwords.find(s)==stopwords.end()){
            Porter2Stemmer::trim(s);
            vector<string> sep = split(s,' ');
            for(string s : sep){
                s1=s;
                Porter2Stemmer::stem(s);
                if(stopwords.find(s)!=stopwords.end())
                    continue;
                if(s==""){
                    continue;
                }
                if(localmap.find(s)==localmap.end()){
                    localmap.insert({s,1});
                }
                else{
                    localmap[s]=localmap[s]+1;
                }
            }


        }
    }
    return localmap;
}*/

orderedMap readFileinVector(string fileName) {
    fileName = sourceDir+separator()+fileName;
    ifstream in(fileName);
    string s;
    string s1;
    vector<string> sep;
    std::vector<char> buffer (1024,0);
    orderedMap localmap;
    while (!in.eof()) {
        in.read(buffer.data(), buffer.size());
        bool isWord = false;
        s = "";
        for (short i = 0; i < buffer.size(); i++) {
            if ((buffer[i] >= 'a' ) && (buffer[i] <= 'z')) {
                s = s + buffer[i];
            }
            else if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
                s = s + buffer[i];
            }
            else{
                if(s!=""){
                    isWord=true;
                }
            }
        if(!isWord){
            continue;
        }
        if (stopwords.find(s) == stopwords.end() && s.size()>2) {
            //Porter2Stemmer::trim(s);
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            //vector<string> sep = split(s, ' ');
            //for (string s : sep) {
                s1 = s;
                Porter2Stemmer::stem(s);
                if (localmap.find(s) == localmap.end()) {
                    //auto const &ent = localmap.find(s);
                    unsigned int x=1;
                    localmap[s]=x;
                } else {
                    localmap[s] = localmap[s] + 1;
                }
            //}
        }
            s="";
            isWord= false;
    }
    }
    return localmap;
}

string sequenceMerge(string string1, string string2) {
    ifstream f1(string1,ios::binary);
    ifstream f2(string2,ios::binary);
    string z = destDir+separator()+CountIndex + separator() + "temp.txt";
    ofstream out(z, ios::binary);
    string line1,line2;
    string w1,w2;
    vector<string> x,y;
    if(f1){
        getline(f1,line1);
        x =split(line1,'=');
        w1=x[0];

    }
    if(f2){
        getline(f2,line2);
        y =split(line2,'=');
        w2=y[0];
    }
    while(f1 && f2){
        if(w1<w2){
            out << line1 <<endl;
            getline(f1,line1);
            if(line1!=""){
                x =split(line1,'=');
                w1=x[0];
            }

        } else if (w1>w2){
            out <<line2 << endl;
            getline(f2,line2);
            if(line2!=""){
                y =split(line2,'=');
                w2=y[0];
            }

        }
        else
        {
            out << w1 << '=' << x[1]<<'|'<<y[1]<<endl;
            getline(f1,line1);
            x =split(line1,'=');
            if(line1!="")
                w1=x[0];
            getline(f2,line2);
            y =split(line2,'=');
            if(line2!="")
                w2=y[0];

        }

    }
    if(f1){
        while(getline(f1,line1)){
            if(line1!="")
                out <<line1 << endl;
        }
    } else{
        while(getline(f2,line2)){
            if(line2!="")
                out <<line2 << endl;
        }

    }
    f1.close();
    f2.close();
    out.close();
    remove(string1.c_str());
    remove(string2.c_str());
    string Name = destDir + separator()+CountIndex + separator() + FILE_WORD_COUNT;
    rename(z.c_str(),Name.c_str());
    return FILE_WORD_COUNT;
}