#include "Respuesta.h"
#include "control.h"
#include <vector>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include<bits/stdc++.h>  
#include <regex>

const int ALPHABET_SIZE = 1000; 
  
// trie node 
struct TrieNode { 
    struct TrieNode *children[ALPHABET_SIZE]; 
  
    // isEndOfWord is true if the node represents 
    // end of a word 
    bool isEndOfWord; 
}; 
  
// Returns new trie node (initialized to NULLs) 
struct TrieNode *getNode(void) { 
    struct TrieNode *pNode =  new TrieNode; 
  
    pNode->isEndOfWord = false; 
  
    for (int i = 0; i < ALPHABET_SIZE; i++) 
        pNode->children[i] = NULL; 
  
    return pNode; 
} 
  
// If not present, inserts key into trie 
// If the key is prefix of trie node, just 
// marks leaf node 
void insert(struct TrieNode *root, std::string key) { 
    struct TrieNode *pCrawl = root; 
  
    for (int i = 0; i < key.length(); i++) 
    { 
        int index = key[i] - 'a'; 
        if (!pCrawl->children[index]) 
            pCrawl->children[index] = getNode(); 
  
        pCrawl = pCrawl->children[index]; 
    } 
  
    // mark last node as leaf 
    pCrawl->isEndOfWord = true; 
} 
  
// Returns true if key presents in trie, else 
// false 
bool search(struct TrieNode *root, std::string key) { 
    struct TrieNode *pCrawl = root; 
  
    for (int i = 0; i < key.length(); i++) 
    { 
        int index = key[i] - 'a'; 
        if (!pCrawl->children[index]) 
            return false; 
  
        pCrawl = pCrawl->children[index]; 
    } 
  
    return (pCrawl != NULL && pCrawl->isEndOfWord); 
} 

std::string convertToString(char* a, int size) 
{ 
    int i; 
    std::string s = ""; 
    for (i = 0; i < size; i++) { 
        s = s + a[i]; 
    } 
    return s; 
} 

std::string Filter(const std::string &to) //EDIT: removed const std::string &remove
{
    std::string final;
    for(std::string::const_iterator it = to.begin(); it != to.end(); ++it)
    {
        if((*it >= 97 && *it <= 122  ) || (*it >= 65 && *it <= 90) || *it<0 )
            final += *it;
        else{
            //final += *it;
        }
    }
    return final;
}  

int main(int argc, char const *argv[]){
    
    //Se inicia el servidor
    Respuesta respuesta(7200);
    int contadorComplejidad = 0;

    std::cout << "Servidor Iniciado...\n" << std::endl;
    //std::vector<long int> agenda;
    int* op,opOperation;
    mensaje resp;
    int nbd = 0, aux2, id = 0, idAux;  
    char b[25],number[11];
    long int phone;
    char cc[50];
    std::regex purga("[A-Za-z]$");
    std::smatch matchfiltroP;

    std::string palabra, dato, palabra2, palabra3;
    std::ifstream destino;
    destino.open("palabras.txt");
    struct TrieNode *root = getNode();
    std::regex letras("[[A-Za-z]]*");
    std::regex tabu("\t");
    std::regex vacio(" ");
    std::smatch matchfiltro;

    while(getline(destino,dato)){
        regex_search(dato,matchfiltro,letras);
        std::string aux = dato.substr(matchfiltro.position(0),dato.length());
        regex_search(aux,matchfiltro,tabu);
        std::string aux2 = aux.substr(0,matchfiltro.position(0));
        regex_search(aux2,matchfiltro,vacio);
        std::string aux3 = aux2.substr(0,matchfiltro.position(0));
        
        insert(root,aux3);
    }

    destino.close();

    
    while (1)
    {
        struct timeval timestamp;
        control res;
        //Recibe la respuesta del cliente
        memcpy(&resp,(respuesta.getRequest()),sizeof(mensaje));

        std::cout << "Cliente: \n" <<  "IP:" << inet_ntoa(respuesta.getSocket()->getDirForanea().sin_addr);
        std::cout << ", Puerto: " << ntohs(respuesta.getSocket()->getDirForanea().sin_port) << "\n" << std::endl;

        char buffer[resp.tambuffer];  

        std::cout << "ENENENE: " << resp.tambuffer << std::endl; 

        memcpy(&idAux,&resp.requestId,sizeof(unsigned int));
        memcpy(&opOperation,&resp.operationId,sizeof(int));
        memcpy(&buffer,&resp.arguments,sizeof(buffer));

        if ( strcmp(buffer, "s") == 0 ) {

            std::cout << "----------------Sirvio------------" << std::endl;
            res.perdidaComunicacion = 1;

        } else {

            int b_size = sizeof(buffer) / sizeof(char); 
  
            std::string s_a = convertToString(buffer, b_size);
        
            // Vector of string to save tokens 
            std::vector <std::string> tokens; 
        
            // stringstream class check1 
            std::stringstream check1(s_a); 
        
            std::string intermediate; 
        
            // Tokenizing w.r.t. space ' ' 
            while(getline(check1, intermediate, ' ')) 
            { 
                tokens.push_back(intermediate); 
            }

            

            for(int i = 0; i < tokens.size(); i++) {
               std::string line=tokens[i];
              /*  for(int j = 0; j < tokens[i].size(); ++j) {
        
                    if (!((line[j] >= 'a' && line[j]<='z') || (line[j] >= 'A' && line[j]<='Z'))) {
                        line[j] = '\0';
                    }
                }
               */ 
                std::cout << line << '\n';
                /*
                if(!search(root,line)) 
                    contadorComplejidad++;
                */
            }

            std::cout << "-----------------------------" << std::endl;

            res.PalabrasLeidas = tokens.size();
            res.PorcentajeComplejidad = contadorComplejidad;
            res.perdidaComunicacion = 0;

        }
   
        memcpy(cc, (char*)&res,sizeof(res));
        respuesta.sendReply((char*)cc); 
        
    }

    
    return 0;
}

