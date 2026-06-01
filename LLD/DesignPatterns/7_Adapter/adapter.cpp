#include<bits/stdc++.h>
using namespace std;

class Ireports{
public:
    virtual string getJsonData() = 0;
    virtual ~Ireports() {};
};

class XMLData{
public:
    string getXMLData(){
        return "XML Data";
    }
};

class XMLDataAdapter : public Ireports{
private:
    XMLData *xml;
public:
    XMLDataAdapter(XMLData* x){
        this->xml = x;
    }

    string getJsonData() override{
        return xml->getXMLData() + " To JSON Data";
    }
};

int main(){
    XMLData* xml = new XMLData();
    Ireports *rep = new XMLDataAdapter(xml);
    cout<<rep->getJsonData();
}